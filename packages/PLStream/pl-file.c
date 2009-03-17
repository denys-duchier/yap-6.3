/*  $Id$

    Part of SWI-Prolog

    Author:        Jan Wielemaker
    E-mail:        wielemak@science.uva.nl
    WWW:           http://www.swi-prolog.org
    Copyright (C): 1985-2008, University of Amsterdam

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
This module is far too big.  It defines a layer around open(), etc.   to
get  opening  and  closing  of  files to the symbolic level required for
Prolog.  It also defines basic I/O  predicates,  stream  based  I/O  and
finally  a  bundle  of  operations  on  files,  such  as name expansion,
renaming, deleting, etc.  Most of this module is rather straightforward.

If time is there I will have a look at all this to  clean  it.   Notably
handling times must be cleaned, but that not only holds for this module.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*#define O_DEBUG 1*/
/*#define O_DEBUG_MT 1*/

#define EXPERIMENT 1

#include "pl-incl.h"
#include "pl-ctype.h"
#include "pl-utf8.h"
#include <errno.h>

#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#ifdef HAVE_SYS_FILE_H
#include <sys/file.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_FILE_H
#include <sys/file.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_BSTRING_H
#include <bstring.h>
#endif

#define LOCK()   PL_LOCK(L_FILE)	/* MT locking */
#define UNLOCK() PL_UNLOCK(L_FILE)

static int	bad_encoding(atom_t name);

static bool streamStatus(IOSTREAM *s);

INIT_DEF(atom_t, standardStreams, 6)
  ADD_STDSTREAM(ATOM_user_input)			/* 0 */
  ADD_STDSTREAM(ATOM_user_output)			/* 1 */
  ADD_STDSTREAM(ATOM_user_error)			/* 2 */
  ADD_STDSTREAM(ATOM_current_input)			/* 3 */
  ADD_STDSTREAM(ATOM_current_output)			/* 4 */
  ADD_STDSTREAM(ATOM_protocol)			/* 5 */
END_STDSTREAMS(NULL_ATOM)

 

static int
standardStreamIndexFromName(atom_t name)
{ const atom_t *ap;

  for(ap=standardStreams; *ap; ap++)
  { if ( *ap == name )
      return (int)(ap - standardStreams);
  }

  return -1;
}


static int
standardStreamIndexFromStream(IOSTREAM *s)
{ GET_LD
  IOSTREAM **sp = LD->IO.streams;
  int i = 0;

  for( ; i<6; i++, sp++ )
  { if ( *sp == s )
      return i;
  }

  return -1;
}


		 /*******************************
		 *	   BOOKKEEPING		*
		 *******************************/

static void aliasStream(IOSTREAM *s, atom_t alias);
static void unaliasStream(IOSTREAM *s, atom_t name);

static Table streamAliases;		/* alias --> stream */
static Table streamContext;		/* stream --> extra data */

typedef struct _alias
{ struct _alias *next;
  atom_t name;
} alias;


#define IO_TELL	0x001			/* opened by tell/1 */
#define IO_SEE  0x002			/* opened by see/1 */

typedef struct
{ alias *alias_head;
  alias *alias_tail;
  atom_t filename;			/* associated filename */
  unsigned flags;
} stream_context;


static stream_context *
getStreamContext(IOSTREAM *s)
{ Symbol symb;

  if ( !(symb = lookupHTable(streamContext, s)) )
  { GET_LD
    stream_context *ctx = allocHeap(sizeof(*ctx));

    DEBUG(1, Sdprintf("Created ctx=%p for stream %p\n", ctx, s));

    ctx->alias_head = ctx->alias_tail = NULL;
    ctx->filename = NULL_ATOM;
    ctx->flags = 0;
    addHTable(streamContext, s, ctx);

    return ctx;
  }

  return symb->value;
}


void
aliasStream(IOSTREAM *s, atom_t name)
{ GET_LD
  stream_context *ctx;
  Symbol symb;
  alias *a;

					/* ensure name is free (error?) */
  if ( (symb = lookupHTable(streamAliases, (void *)name)) )
    unaliasStream(symb->value, name);

  ctx = getStreamContext(s);
  addHTable(streamAliases, (void *)name, s);
  PL_register_atom(name);

  a = allocHeap(sizeof(*a));
  a->next = NULL;
  a->name = name;

  if ( ctx->alias_tail )
  { ctx->alias_tail->next = a;
    ctx->alias_tail = a;
  } else
  { ctx->alias_head = ctx->alias_tail = a;
  }
}

/* MT: Locked by freeStream()
*/

static void
unaliasStream(IOSTREAM *s, atom_t name)
{ GET_LD
  Symbol symb;

  if ( name )
  { if ( (symb = lookupHTable(streamAliases, (void *)name)) )
    { deleteSymbolHTable(streamAliases, symb);

      if ( (symb=lookupHTable(streamContext, s)) )
      { stream_context *ctx = symb->value;
	alias **a;
      
	for(a = &ctx->alias_head; *a; a = &(*a)->next)
	{ if ( (*a)->name == name )
	  { alias *tmp = *a;

	    *a = tmp->next;
	    freeHeap(tmp, sizeof(*tmp));
	    if ( tmp == ctx->alias_tail )
	      ctx->alias_tail = NULL;

	    break;
	  }
	}
      }

      PL_unregister_atom(name);
    }
  } else				/* delete them all */
  { if ( (symb=lookupHTable(streamContext, s)) )
    { stream_context *ctx = symb->value;
      alias *a, *n;

      for(a = ctx->alias_head; a; a=n)
      { Symbol s2;

	n = a->next;

	if ( (s2 = lookupHTable(streamAliases, (void *)a->name)) )
	{ deleteSymbolHTable(streamAliases, s2);
	  PL_unregister_atom(a->name);
	}

	freeHeap(a, sizeof(*a));
      }

      ctx->alias_head = ctx->alias_tail = NULL;
    }
  }
}


static void
freeStream(IOSTREAM *s)
{ GET_LD
  Symbol symb;
  int i;
  IOSTREAM **sp;

  DEBUG(1, Sdprintf("freeStream(%p)\n", s));

  LOCK();
  unaliasStream(s, NULL_ATOM);
  if ( (symb=lookupHTable(streamContext, s)) )
  { stream_context *ctx = symb->value;

    if ( ctx->filename == source_file_name )
    { source_file_name = NULL_ATOM;	/* TBD: pop? */
      source_line_no = -1;
    }

    freeHeap(ctx, sizeof(*ctx));
    deleteSymbolHTable(streamContext, symb);
  }
					/* if we are a standard stream */
					/* reassociate with standard I/O */
					/* NOTE: there may be more! */
  for(i=0, sp = LD->IO.streams; i<6; i++, sp++)
  { if ( *sp == s )
    { if ( s->flags & SIO_INPUT )
	*sp = Sinput;
      else if ( sp == &Suser_error )
	*sp = Serror;
      else if ( sp == &Sprotocol )
	*sp = NULL;
      else
	*sp = Soutput;
    }
  }
  UNLOCK();
}


/* MT: locked by caller (openStream()) */

static void
setFileNameStream(IOSTREAM *s, atom_t name)
{ getStreamContext(s)->filename = name;
}


static atom_t
fileNameStream(IOSTREAM *s)
{ atom_t name;

  LOCK();
  name = getStreamContext(s)->filename;
  UNLOCK();

  return name;
}


		 /*******************************
		 *	     GET HANDLES	*
		 *******************************/

#ifdef O_PLMT

static inline IOSTREAM *
getStream(IOSTREAM *s)
{ if ( s && s->magic == SIO_MAGIC )	/* TBD: ensure visibility? */
  { Slock(s);
    return s;
  }

  return NULL;
}

static inline IOSTREAM *
tryGetStream(IOSTREAM *s)
{ if ( s && s->magic == SIO_MAGIC && StryLock(s) == 0 )
    return s;

  return NULL;
}

static inline void
releaseStream(IOSTREAM *s)
{ if ( s->magic == SIO_MAGIC )
    Sunlock(s);
}

#else /*O_PLMT*/

#define getStream(s)	(s)
#define tryGetStream(s) (s)
#define releaseStream(s)

#endif /*O_PLMT*/

int
PL_release_stream(IOSTREAM *s)
{ if ( Sferror(s) )
    return streamStatus(s);

  releaseStream(s);
  return TRUE;
}


#define SH_ERRORS   0x01		/* generate errors */
#define SH_ALIAS    0x02		/* allow alias */
#define SH_UNLOCKED 0x04		/* don't lock the stream */
#define SH_SAFE	    0x08		/* Lookup in table */

static int
get_stream_handle__LD(term_t t, IOSTREAM **s, int flags ARG_LD)
{ atom_t alias;

  if ( PL_is_functor(t, FUNCTOR_dstream1) )
  { void *p;
    term_t a = PL_new_term_ref();

    _PL_get_arg(1, t, a);
    if ( PL_get_pointer(a, &p) )
    { if ( flags & SH_SAFE )
      { Symbol symb;

	LOCK();
	symb = lookupHTable(streamContext, p);
	UNLOCK();
	
	if ( !symb )
	  goto noent;
      }

      if ( flags & SH_UNLOCKED )
      { if ( ((IOSTREAM *)p)->magic == SIO_MAGIC )
	{ *s = p;
	  return TRUE;
	}
	goto noent;
      }

      if ( (*s = getStream(p)) )
	return TRUE;

      goto noent;
    }
  } else if ( PL_get_atom(t, &alias) )
  { Symbol symb;

    if ( !(flags & SH_UNLOCKED) )
      LOCK();
    if ( (symb=lookupHTable(streamAliases, (void *)alias)) )
    { IOSTREAM *stream;
      uintptr_t n = (uintptr_t)symb->value;

      if ( n < 6 )			/* standard stream! */
      { stream = LD->IO.streams[n];
      } else
	stream = symb->value;
	
      if ( !(flags & SH_UNLOCKED) )
	UNLOCK();
      
      if ( stream )
      { if ( (flags & SH_UNLOCKED) )
	{ if ( stream->magic == SIO_MAGIC )
	  { *s = stream;
	    return TRUE;
	  }
	} else if ( (*s = getStream(stream)) )
	  return TRUE;
	goto noent;
      }
    }
    if ( !(flags & SH_UNLOCKED) )
      UNLOCK();

    goto noent;
  }
      
  if ( flags & SH_ERRORS )
    return PL_error(NULL, 0, NULL, ERR_DOMAIN,
		    (flags&SH_ALIAS) ? ATOM_stream_or_alias : ATOM_stream, t);

  fail;

noent:
  if ( flags & SH_ERRORS )
    PL_error(NULL, 0, NULL, ERR_EXISTENCE, ATOM_stream, t);
  fail;
}

#define get_stream_handle(t, sp, flags) \
	get_stream_handle__LD(t, sp, flags PASS_LD)

int
PL_get_stream_handle(term_t t, IOSTREAM **s)
{ GET_LD
  return get_stream_handle(t, s, SH_ERRORS|SH_ALIAS);
}


int
PL_unify_stream_or_alias(term_t t, IOSTREAM *s)
{ GET_LD
  int rval;
  stream_context *ctx;
  int i;

  if ( (i=standardStreamIndexFromStream(s)) >= 0 && i < 3 )
    return PL_unify_atom(t, standardStreams[i]);

  LOCK();
  ctx = getStreamContext(s);
  if ( ctx->alias_head )
  { rval = PL_unify_atom(t, ctx->alias_head->name);
  } else
  { term_t a = PL_new_term_ref();
    
    PL_put_pointer(a, s);
    PL_cons_functor(a, FUNCTOR_dstream1, a);

    rval = PL_unify(t, a);
  }
  UNLOCK();

  return rval;
}


int
PL_unify_stream(term_t t, IOSTREAM *s)
{ GET_LD
  stream_context *ctx;
  term_t a = PL_new_term_ref();

  LOCK();
  ctx = getStreamContext(s);
  UNLOCK();

  PL_put_pointer(a, s);
  PL_cons_functor(a, FUNCTOR_dstream1, a);

  if ( PL_unify(t, a) )
    succeed;
  if ( PL_is_functor(t, FUNCTOR_dstream1) )
    fail;

  return PL_error(NULL, 0, NULL, ERR_DOMAIN, ATOM_stream, t);
}


bool					/* old FLI name (compatibility) */
PL_open_stream(term_t handle, IOSTREAM *s)
{ return PL_unify_stream(handle, s);
}


IOSTREAM **				/* provide access to Suser_input, */
_PL_streams(void)			/* Suser_output and Suser_error */
{ GET_LD
  return &Suser_input;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
getInputStream(term_t t, IOSTREAM **s)
getOutputStream(term_t t, IOSTREAM **s)
    These functions are the basis used by all Prolog predicates to get
    a input or output stream handle.  If t = 0, current input/output is
    returned.  This allows us to define the standard-stream based version
    simply by calling the explicit stream-based version with 0 for the
    stream argument.

    MT: The returned stream is always locked and should be returned
    using releaseStream() or streamStatus().
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


static bool
getOutputStream(term_t t, IOSTREAM **stream)
{ GET_LD
  atom_t a;
  IOSTREAM *s;

  if ( t == 0 )
  { *stream = getStream(Scurout);
    return TRUE;
  } else if ( PL_get_atom(t, &a) && a == ATOM_user )
  { *stream = getStream(Suser_output);
    return TRUE;
  } else
  { *stream = NULL;			/* make compiler happy */
  }

  if ( !PL_get_stream_handle(t, &s) )
    fail;
  
  if ( !(s->flags &SIO_OUTPUT) )
  { releaseStream(s);
    return PL_error(NULL, 0, NULL, ERR_PERMISSION,
		    ATOM_output, ATOM_stream, t);
  }

  *stream = s;
  succeed;
}


static bool
getInputStream__LD(term_t t, IOSTREAM **stream ARG_LD)
{ atom_t a;
  IOSTREAM *s;

  if ( t == 0 )
  { *stream = getStream(Scurin);
    return TRUE;
  } else if ( PL_get_atom(t, &a) && a == ATOM_user )
  { *stream = getStream(Suser_input);
    return TRUE;
  } else
  { *stream = NULL;			/* make compiler happy */
  }

  if ( !get_stream_handle(t, &s, SH_ERRORS|SH_ALIAS) )
    fail;

  if ( !(s->flags &SIO_INPUT) )
  { releaseStream(s);
    return PL_error(NULL, 0, NULL, ERR_PERMISSION,
		    ATOM_input, ATOM_stream, t);
  }

  *stream = s;
  succeed;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
In windows GUI applications, the IO-streams  are   not  bound. We do not
wish to generate an error on the  stream   errors  that may be caused by
this. It is a bit of a hack, but   the alternative is to define a stream
that ignores the error. This might get hairy if the user is playing with
these streams too.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifdef __WINDOWS__
static int
isConsoleStream(IOSTREAM *s)
{ int i = standardStreamIndexFromStream(s);

  return i >= 0 && i < 3;
}
#else
#define isConsoleStream(s) FALSE
#endif


static bool
reportStreamError(IOSTREAM *s)
{ if ( GD->cleaning == CLN_NORMAL &&
       !isConsoleStream(s) &&
       (s->flags & (SIO_FERR|SIO_WARN)) )
  { GET_LD
    atom_t op;
    term_t stream = PL_new_term_ref();
    char *msg;

    PL_unify_stream_or_alias(stream, s);

    if ( (s->flags & SIO_FERR) )
    { if ( s->exception )
      { fid_t fid = PL_open_foreign_frame();
	term_t ex = PL_new_term_ref();
	PL_recorded(s->exception, ex);
	PL_erase(s->exception);
	s->exception = NULL;
	PL_raise_exception(ex);
	PL_close_foreign_frame(fid);
	fail;
      }

      if ( s->flags & SIO_INPUT )
      { if ( Sfpasteof(s) )
	{ return PL_error(NULL, 0, NULL, ERR_PERMISSION,
			  ATOM_input, ATOM_past_end_of_stream, stream);
	} else if ( (s->flags & SIO_TIMEOUT) )
	{ PL_error(NULL, 0, NULL, ERR_TIMEOUT,
		   ATOM_read, stream);
	  Sclearerr(s);
	  fail;
	} else
	  op = ATOM_read;
      } else
	op = ATOM_write;
  
      msg = s->message ? s->message : MSG_ERRNO;

      PL_error(NULL, 0, msg, ERR_STREAM_OP, op, stream);
      
      if ( (s->flags & SIO_CLEARERR) )
	Sseterr(s, SIO_FERR, NULL);

      fail;
    } else
    { printMessage(ATOM_warning,
		   PL_FUNCTOR_CHARS, "io_warning", 2,
		   PL_TERM, stream,
		   PL_CHARS, s->message);

      Sseterr(s, SIO_WARN, NULL);
    }
  }
  
  succeed;
}


bool
streamStatus(IOSTREAM *s)
{ if ( (s->flags & (SIO_FERR|SIO_WARN)) )
  { releaseStream(s);
    return reportStreamError(s);
  }

  releaseStream(s);
  succeed;
}


		 /*******************************
		 *	     TTY MODES		*
		 *******************************/

ttybuf	ttytab;				/* saved terminal status on entry */
int	ttymode;			/* Current tty mode */

typedef struct input_context * InputContext;
typedef struct output_context * OutputContext;

struct input_context
{ IOSTREAM *    stream;                 /* pushed input */
  atom_t        term_file;              /* old term_position file */
  int           term_line;              /* old term_position line */
  InputContext  previous;               /* previous context */
};


struct output_context
{ IOSTREAM *    stream;                 /* pushed output */
  OutputContext previous;               /* previous context */
};

#define input_context_stack  (LD->IO.input_stack)
#define output_context_stack (LD->IO.output_stack)

static IOSTREAM *openStream(term_t file, term_t mode, term_t options);

void
dieIO(void)
{ if ( GD->io_initialised )
  { pl_noprotocol();
    closeFiles(TRUE);
    PopTty(Sinput, &ttytab);
  }
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
closeStream() performs Prolog-level closing. Most important right now is
to to avoid closing the user-streams. If a stream cannot be flushed (due
to a write-error), an exception is  generated.

MT: We assume the stream is locked and will unlock it here.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

static bool
closeStream(IOSTREAM *s)
{ if ( s == Sinput )
  { Sclearerr(s);
    releaseStream(s);
  } else if ( s == Soutput || s == Serror )
  { if ( Sflush(s) < 0 )
      return streamStatus(s);
    releaseStream(s);
  } else
  { if ( !Sferror(s) && Sflush(s) < 0 )
    { streamStatus(s);
      Sclose(s);
      return FALSE;
    }
    if ( Sclose(s) < 0 )		/* will unlock as well */
      fail;
  }

  succeed;
}


void
closeFiles(int all)
{ GET_LD
  TableEnum e;
  Symbol symb;

  e = newTableEnum(streamContext);
  while( (symb=advanceTableEnum(e)) )
  { IOSTREAM *s = symb->name;

    if ( all || !(s->flags & SIO_NOCLOSE) )
    { IOSTREAM *s2 = tryGetStream(s);

      if ( s2 )
      { if ( !all )
	{ term_t t = PL_new_term_ref();

	  PL_unify_stream_or_alias(t, s2);
	  printMessage(ATOM_informational,
		       PL_FUNCTOR, FUNCTOR_close_on_abort1,
		         PL_TERM, t);
	  PL_reset_term_refs(t);
	}

	closeStream(s2);
      }
    }
  }
  freeTableEnum(e);
}


void
PL_cleanup_fork(void)
{ TableEnum e;
  Symbol symb;

  e = newTableEnum(streamContext);
  while( (symb=advanceTableEnum(e)) )
  { IOSTREAM *s = symb->name;
    int fd;

    if ( (fd=Sfileno(s)) >= 3 )
      close(fd);
  }
  freeTableEnum(e);

  stopItimer();
}


void
protocol(const char *str, size_t n)
{ GET_LD
  IOSTREAM *s;

  if ( LD && (s = getStream(Sprotocol)) )
  { while( n-- > 0 )
      Sputcode(*str++&0xff, s);
    Sflush(s);
    releaseStream(s);			/* we don not check errors */
  }
}


		 /*******************************
		 *	  TEMPORARY I/O		*
		 *******************************/


static word
pl_push_input_context(void)
{ GET_LD
  InputContext c = allocHeap(sizeof(struct input_context));

  c->stream           = Scurin;
  c->term_file        = source_file_name;
  c->term_line        = source_line_no;
  c->previous         = input_context_stack;
  input_context_stack = c;

  succeed;
}


static word
pl_pop_input_context(void)
{ GET_LD
  InputContext c = input_context_stack;

  if ( c )
  { Scurin              = c->stream;
    source_file_name    = c->term_file;
    source_line_no      = c->term_line;
    input_context_stack = c->previous;
    freeHeap(c, sizeof(struct input_context));

    succeed;
  } else
  { Scurin		= Sinput;
    fail;
  }
}


static void
pushOutputContext(void)
{ GET_LD
  OutputContext c = allocHeap(sizeof(struct output_context));

  c->stream            = Scurout;
  c->previous          = output_context_stack;
  output_context_stack = c;
}


static void
popOutputContext(void)
{ GET_LD
  OutputContext c = output_context_stack;

  if ( c )
  { if ( c->stream->magic == SIO_MAGIC )
      Scurout = c->stream;
    else
    { Sdprintf("Oops, current stream closed?");
      Scurout = Soutput;
    }
    output_context_stack = c->previous;
    freeHeap(c, sizeof(struct output_context));
  } else
    Scurout = Soutput;
}


int
setupOutputRedirect(term_t to, redir_context *ctx, int redir)
{ GET_LD
  atom_t a;

  ctx->term = to;
  ctx->redirected = redir;

  if ( to == 0 )
  { ctx->stream = getStream(Scurout);
    ctx->is_stream = TRUE;
  } else if ( PL_get_atom(to, &a) && a == ATOM_user )
  { ctx->stream = getStream(Suser_output);
    ctx->is_stream = TRUE;
  } else if ( get_stream_handle(to, &ctx->stream, SH_SAFE) )
  { if ( !(ctx->stream->flags &SIO_OUTPUT) )
    { releaseStream(ctx->stream);
      return PL_error(NULL, 0, NULL, ERR_PERMISSION,
		      ATOM_output, ATOM_stream, to);
    }

    ctx->is_stream = TRUE;
  } else
  { if ( PL_is_functor(to, FUNCTOR_codes2) )
    { ctx->out_format = PL_CODE_LIST;
      ctx->out_arity = 2;
    } else if ( PL_is_functor(to, FUNCTOR_codes1) )
    { ctx->out_format = PL_CODE_LIST;
      ctx->out_arity = 1;
    } else if ( PL_is_functor(to, FUNCTOR_chars2) )
    { ctx->out_format = PL_CHAR_LIST;
      ctx->out_arity = 2;
    } else if ( PL_is_functor(to, FUNCTOR_chars1) )
    { ctx->out_format = PL_CHAR_LIST;
      ctx->out_arity = 1;
    } else if ( PL_is_functor(to, FUNCTOR_string1) )
    { ctx->out_format = PL_STRING;
      ctx->out_arity = 1;
    } else if ( PL_is_functor(to, FUNCTOR_atom1) )
    { ctx->out_format = PL_ATOM;
      ctx->out_arity = 1;
    } else
    { return PL_error(NULL, 0, NULL, ERR_TYPE, ATOM_output, to);
    }
    
    ctx->is_stream = FALSE;
    ctx->data = ctx->buffer;
    ctx->size = sizeof(ctx->buffer);
    ctx->stream = Sopenmem(&ctx->data, &ctx->size, "w");
    ctx->stream->encoding = ENC_WCHAR;
  }

  ctx->magic = REDIR_MAGIC;

  if ( redir )
  { pushOutputContext();
    Scurout = ctx->stream;
  }

  succeed;
}


int
closeOutputRedirect(redir_context *ctx)
{ int rval = TRUE;

  if ( ctx->magic != REDIR_MAGIC )
    return rval;			/* already done */
  ctx->magic = 0;

  if ( ctx->redirected )
    popOutputContext();

  if ( ctx->is_stream )
  { rval = streamStatus(ctx->stream);
  } else
  { GET_LD
    term_t out  = PL_new_term_ref();
    term_t diff, tail;

    closeStream(ctx->stream);
    _PL_get_arg(1, ctx->term, out);
    if ( ctx->out_arity == 2 )
    { diff = PL_new_term_ref();
      _PL_get_arg(2, ctx->term, diff);
      tail = PL_new_term_ref();
    } else
    { diff = tail = 0;
    }
 
    rval = PL_unify_wchars_diff(out, tail, ctx->out_format,
				ctx->size/sizeof(wchar_t),
				(wchar_t*)ctx->data);
    if ( tail )
      rval = PL_unify(tail, diff);

    if ( ctx->data != ctx->buffer )
      free(ctx->data);
  }

  return rval;
}


void
discardOutputRedirect(redir_context *ctx)
{ if ( ctx->magic != REDIR_MAGIC )
    return;				/* already done */

  ctx->magic = 0;

  if ( ctx->redirected )
    popOutputContext();

  if ( ctx->is_stream )
  { releaseStream(ctx->stream);
  } else
  { closeStream(ctx->stream);
    if ( ctx->data != ctx->buffer )
      free(ctx->data);
  }
}


static
PRED_IMPL("with_output_to", 2, with_output_to, PL_FA_TRANSPARENT)
{ redir_context outctx;

  if ( setupOutputRedirect(A1, &outctx, TRUE) )
  { term_t ex = 0;
    int rval;

    if ( (rval = callProlog(NULL, A2, PL_Q_CATCH_EXCEPTION, &ex)) )
      return closeOutputRedirect(&outctx);
    discardOutputRedirect(&outctx);
    if ( ex )
      return PL_raise_exception(ex);
  }

  fail;
}



void
PL_write_prompt(int dowrite)
{ GET_LD
  IOSTREAM *s = getStream(Suser_output);

  if ( s )
  { if ( dowrite )
    { atom_t a = PrologPrompt();

      if ( a )
	writeAtomToStream(s, a);
    }

    Sflush(s);
    releaseStream(s);
  }

  LD->prompt.next = FALSE;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Get a single character from Sinput  without   waiting  for a return. The
character should not be echoed.  If   TTY_CONTROL_FEATURE  is false this
function will read the first character and  then skip all character upto
and including the newline.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

static int
Sgetcode_intr(IOSTREAM *s, int signals)
{ int c;

#ifdef __WINDOWS__
  int newline = s->newline;
  s->newline = SIO_NL_POSIX;		/* avoid blocking \r */
#endif
  do
  { Sclearerr(s);
    c = Sgetcode(s);
  } while ( c == -1 && 
	    errno == EINTR &&
	    (!signals || PL_handle_signals() >= 0)
	  );
#ifdef __WINDOWS__
  s->newline = newline;
#endif

  return c;
}


static int
getSingleChar(IOSTREAM *stream, int signals)
{ GET_LD
  int c;
  ttybuf buf;
    
  //  debugstatus.suspendTrace++; WARNING: suspendTrace
  Slock(stream);
  Sflush(stream);
  PushTty(stream, &buf, TTY_RAW);	/* just donot prompt */
  
  if ( !trueFeature(TTY_CONTROL_FEATURE) )
  { int c2;

    c2 = Sgetcode_intr(stream, signals);
    while( c2 == ' ' || c2 == '\t' )	/* skip blanks */
      c2 = Sgetcode_intr(stream, signals);
    c = c2;
    while( c2 != EOF && c2 != '\n' )	/* read upto newline */
      c2 = Sgetcode_intr(stream, signals);
  } else
  { if ( stream->position )
    { IOPOS oldpos = *stream->position;
      c = Sgetcode_intr(stream, signals);
      *stream->position = oldpos;
    } else
      c = Sgetcode_intr(stream, signals);
  }

  if ( c == 4 || c == 26 )		/* should ask the terminal! */
    c = -1;

  PopTty(stream, &buf);
  // debugstatus.suspendTrace--; WARNING: suspendTrace
  Sunlock(stream);

  return c;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
readLine() reads a line from the terminal.  It is used only by the tracer.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifndef DEL
#define DEL 127
#endif

bool
readLine(IOSTREAM *in, IOSTREAM *out, char *buffer)
{ GET_LD
  int c;
  char *buf = &buffer[strlen(buffer)];
  ttybuf tbuf;

  Slock(in);
  Slock(out);

  PushTty(in, &tbuf, TTY_RAW);		/* just donot prompt */

  for(;;)
  { Sflush(out);

    switch( (c=Sgetc(in)) )
    { case '\n':
      case '\r':
      case EOF:
        *buf++ = EOS;
        PopTty(in, &tbuf);
	Sunlock(in);
	Sunlock(out);

	return c == EOF ? FALSE : TRUE;
      case '\b':
      case DEL:
	if ( trueFeature(TTY_CONTROL_FEATURE) && buf > buffer )
	{ Sfputs("\b \b", out);
	  buf--;
	}
      default:
	if ( trueFeature(TTY_CONTROL_FEATURE) )
	  Sputc(c, out);
	*buf++ = c;
    }
  }
}


IOSTREAM *
PL_current_input()
{ GET_LD
  return getStream(Scurin);
}


IOSTREAM *
PL_current_output()
{ GET_LD
  return getStream(Scurout);
}


static word
openProtocol(term_t f, bool appnd)
{ GET_LD
  IOSTREAM *s;
  term_t mode = PL_new_term_ref();

  pl_noprotocol();

  PL_put_atom(mode, appnd ? ATOM_append : ATOM_write);
  if ( (s = openStream(f, mode, 0)) )
  { s->flags |= SIO_NOCLOSE;		/* do not close on abort */

    Sprotocol = s;
    Suser_input->tee = s;
    Suser_output->tee = s;
    Suser_error->tee = s;

    return TRUE;
  }

  return FALSE;
}


word
pl_noprotocol(void)
{ GET_LD
  IOSTREAM *s;

  if ( (s = getStream(Sprotocol)) )
  { TableEnum e;
    Symbol symb;

    e = newTableEnum(streamContext);
    while( (symb=advanceTableEnum(e)) )
    { IOSTREAM *p = symb->name;

      if ( p->tee == s )
	p->tee = NULL;
    }
    freeTableEnum(e);

    closeStream(s);
    Sprotocol = NULL;
  }

  succeed;
}


		 /*******************************
		 *	 STREAM ATTRIBUTES	*
		 *******************************/


static foreign_t
pl_set_stream(term_t stream, term_t attr)
{ GET_LD
  IOSTREAM *s;
  atom_t aname;
  int arity;

  if ( !PL_get_stream_handle(stream, &s) )
    fail;

  if ( PL_get_name_arity(attr, &aname, &arity) )
  { if ( arity == 1 )
    { term_t a = PL_new_term_ref();

      _PL_get_arg(1, attr, a);

      if ( aname == ATOM_alias )	/* alias(name) */
      { atom_t alias;
	int i;
  
	if ( !PL_get_atom_ex(a, &alias) )
	  goto error;
	
	if ( (i=standardStreamIndexFromName(alias)) >= 0 )
	{ LD->IO.streams[i] = s;
	  if ( i == 0 )
	    LD->prompt.next = TRUE;	/* changed standard input: prompt! */
	  goto ok;
	}
  
	LOCK();
	aliasStream(s, alias);
	UNLOCK();
	goto ok;
      } else if ( aname == ATOM_buffer ) /* buffer(Buffering) */
      { atom_t b;

#define SIO_ABUF (SIO_FBUF|SIO_LBUF|SIO_NBUF)
	if ( !PL_get_atom_ex(a, &b) )
	  goto error;
	if ( b == ATOM_full )
	{ s->flags &= ~SIO_ABUF;
	  s->flags |= SIO_FBUF;
	} else if ( b == ATOM_line )
	{ s->flags &= ~SIO_ABUF;
	  s->flags |= SIO_LBUF;
	} else if ( b == ATOM_false )
	{ Sflush(s);
	  s->flags &= ~SIO_ABUF;
	  s->flags |= SIO_NBUF;
	} else
	{ PL_error("set_stream", 2, NULL, ERR_DOMAIN,
		   ATOM_buffer, a);
	  goto error;
	}
	goto ok;
      } else if ( aname == ATOM_buffer_size )
      { int size;
	
	if ( !PL_get_integer_ex(a, &size) )
	  goto error;
	if ( size < 1 )
	{ PL_error(NULL, 0, NULL, ERR_DOMAIN, ATOM_not_less_than_one, a);
	  goto error;
	}
	Ssetbuffer(s, NULL, size);
	goto ok;
      } else if ( aname == ATOM_eof_action ) /* eof_action(Action) */
      { atom_t action;

	if ( !PL_get_atom_ex(a, &action) )
	  fail;
	if ( action == ATOM_eof_code )
	{ s->flags &= ~(SIO_NOFEOF|SIO_FEOF2ERR);
	} else if ( action == ATOM_reset )
	{ s->flags &= ~SIO_FEOF2ERR;
	  s->flags |= SIO_NOFEOF;
	} else if ( action == ATOM_error )
	{ s->flags &= ~SIO_NOFEOF;
	  s->flags |= SIO_FEOF2ERR;
	} else
	{ PL_error("set_stream", 2, NULL, ERR_DOMAIN,
		   ATOM_eof_action, a);
	  goto error;
	}

	goto ok;
      } else if ( aname == ATOM_close_on_abort ) /* close_on_abort(Bool) */
      { int close;

	if ( !PL_get_bool_ex(a, &close) )
	  goto error;

	if ( close )
	  s->flags &= ~SIO_NOCLOSE;
	else
	  s->flags |= SIO_NOCLOSE;

	goto ok;
      } else if ( aname == ATOM_record_position )
      { int rec;

	if ( !PL_get_bool_ex(a, &rec) )
	  goto error;

	if ( rec )
	  s->position = &s->posbuf;
	else
	  s->position = NULL;

	goto ok;
      } else if ( aname == ATOM_file_name ) /* file_name(Atom) */
      {	atom_t fn;

	if ( !PL_get_atom_ex(a, &fn) )
	  goto error;

	LOCK();
	setFileNameStream(s, fn);
	UNLOCK();

	goto ok;
      } else if ( aname == ATOM_timeout )
      { double f;
	atom_t v;
	
	if ( PL_get_atom(a, &v) && v == ATOM_infinite )
	{ s->timeout = -1;
	  goto ok;
	}
	if ( !PL_get_float_ex(a, &f) )
	  goto error;

	s->timeout = (int)(f*1000.0);
	if ( s->timeout < 0 )
	  s->timeout = 0;
	goto ok;
      } else if ( aname == ATOM_tty )	/* tty(bool) */
      {	int val;

	if ( !PL_get_bool_ex(a, &val) )
	  goto error;

	if ( val )
	  set(s, SIO_ISATTY);
	else
	  clear(s, SIO_ISATTY);

	goto ok;
      } else if ( aname == ATOM_encoding )	/* encoding(atom) */
      {	atom_t val;
	IOENC enc;

	if ( !PL_get_atom_ex(a, &val) )
	  goto error;
	if ( (enc = atom_to_encoding(val)) == ENC_UNKNOWN )
	{ bad_encoding(val);
	  goto error;
	}

	if ( Ssetenc(s, enc, NULL) == 0 )
	  goto ok;

	PL_error(NULL, 0, NULL, ERR_PERMISSION,
		 ATOM_encoding, ATOM_stream, stream);
	goto error;
      } else if ( aname == ATOM_representation_errors )
      { atom_t val;

	if ( !PL_get_atom_ex(a, &val) )
	  goto error;
	clear(s, SIO_REPXML|SIO_REPPL);
	if ( val == ATOM_error )
	  ;
	else if ( val == ATOM_xml )
	  set(s, SIO_REPXML);
	else if ( val == ATOM_prolog )
	  set(s, SIO_REPPL);
	else
	{ PL_error(NULL, 0, NULL, ERR_DOMAIN,
		   ATOM_representation_errors, a);
	  goto error;
	}
	goto ok;
      } else if ( aname == ATOM_newline )
      { atom_t val;

	if ( !PL_get_atom_ex(a, &val) )
	  goto error;
	if ( val == ATOM_posix )
	  s->newline = SIO_NL_POSIX;
	else if ( val == ATOM_dos )
	  s->newline = SIO_NL_DOS;
	else if ( val == ATOM_detect )
	{ if ( false(s, SIO_INPUT) )
	  { PL_error(NULL, 0, "detect only allowed for input streams",
		     ERR_DOMAIN, ATOM_newline, a);
	    goto error;
	  }
	  s->newline = SIO_NL_DETECT;
	} else
	{ PL_error(NULL, 0, NULL, ERR_DOMAIN, ATOM_newline, a);
	  goto error;
	}
	goto ok;
      }
    }
  }

  PL_error("set_stream", 2, NULL, ERR_TYPE,
	   PL_new_atom("stream_attribute"), attr);
  goto error;

ok:
  releaseStream(s);
  succeed;
error:
  releaseStream(s);
  fail;
}


		/********************************
		*          STRING I/O           *
		*********************************/

extern IOFUNCTIONS Smemfunctions;

bool
tellString(char **s, size_t *size, IOENC enc)
{ GET_LD
  IOSTREAM *stream;
  
  stream = Sopenmem(s, size, "w");
  stream->encoding = enc;
  pushOutputContext();
  Scurout = stream;

  return TRUE;
}


bool
toldString(void)
{ GET_LD
  IOSTREAM *s = getStream(Scurout);

  if ( !s )
    succeed;

  if ( s->functions == &Smemfunctions )
  { closeStream(s);
    popOutputContext();
  } else
    releaseStream(s);

  succeed;
}


		/********************************
		*       WAITING FOR INPUT	*
		********************************/

#ifndef HAVE_SELECT

word
pl_wait_for_input(term_t streams, term_t available,
		  term_t timeout)
{ GET_LD
  return notImplemented("wait_for_input", 3);
}

#else

typedef struct fdentry
{ int fd;
  term_t stream;
  struct fdentry *next;
} fdentry;


static inline term_t
findmap(fdentry *map, int fd)
{ for( ; map; map = map->next )
  { if ( map->fd == fd )
      return map->stream;
  }
  assert(0);
  return 0;
}


static word
pl_wait_for_input(term_t Streams, term_t Available,
		  term_t timeout)
{ GET_LD
  fd_set fds;
  struct timeval t, *to;
  double time;
  int n, max = 0, ret, min = 1 << (INTBITSIZE-2);
  fdentry *map     = NULL;
  term_t head      = PL_new_term_ref();
  term_t streams   = PL_copy_term_ref(Streams);
  term_t available = PL_copy_term_ref(Available);
  term_t ahead     = PL_new_term_ref();
  int from_buffer  = 0;
  atom_t a;

  FD_ZERO(&fds);
  while( PL_get_list(streams, head, streams) )
  { IOSTREAM *s;
    int fd;
    fdentry *e;

    if ( !PL_get_stream_handle(head, &s) )
      fail;
    if ( (fd=Sfileno(s)) < 0 )
    { releaseStream(s);
      return PL_error("wait_for_input", 3, NULL, ERR_DOMAIN,
		      PL_new_atom("file_stream"), head);
    }
    releaseStream(s);
					/* check for input in buffer */
    if ( s->bufp < s->limitp )
    { if ( !PL_unify_list(available, ahead, available) ||
	   !PL_unify(ahead, head) )
	fail;
      from_buffer++;
    }

    e         = alloca(sizeof(*e));
    e->fd     = fd;
    e->stream = PL_copy_term_ref(head);
    e->next   = map;
    map       = e;

#ifdef __WINDOWS__
    FD_SET((SOCKET)fd, &fds);
#else
    FD_SET(fd, &fds);
#endif

    if ( fd > max )
      max = fd;
    if( fd < min )
      min = fd;
  }
  if ( !PL_get_nil(streams) )
    return PL_error("wait_for_input", 3, NULL, ERR_TYPE, ATOM_list, Streams);

  if ( from_buffer > 0 )
    return PL_unify_nil(available);

  if ( PL_get_atom(timeout, &a) && a == ATOM_infinite )
  { to = NULL;
  } else if ( PL_is_integer(timeout) )
  { long v;

    PL_get_long(timeout, &v);
    if ( v > 0L )
    { t.tv_sec = v;
      t.tv_usec = 0;
      to = &t;
    } else if ( v == 0 )
    { to = NULL;
    } else
    { t.tv_sec  = 0;
      t.tv_usec = 0;
      to = &t;
    }
  } else
  { if ( !PL_get_float(timeout, &time) )
      return PL_error("wait_for_input", 3, NULL,
		      ERR_TYPE, ATOM_float, timeout);
  
    if ( time >= 0.0 )
    { t.tv_sec  = (int)time;
      t.tv_usec = ((int)(time * 1000000) % 1000000);
    } else
    { t.tv_sec  = 0;
      t.tv_usec = 0;
    }
    to = &t;
  }

  while( (ret=select(max+1, &fds, NULL, NULL, to)) == -1 &&
	 errno == EINTR )
  { fdentry *e;

    if ( PL_handle_signals() < 0 )
      fail;				/* exception */

    FD_ZERO(&fds);			/* EINTR may leave fds undefined */
    for(e=map; e; e=e->next)		/* so we rebuild it to be safe */
    {
#ifdef __WINDOWS__
      FD_SET((SOCKET)e->fd, &fds);
#else
      FD_SET(e->fd, &fds);
#endif
    }
  }

  switch(ret)
  { case -1:
      return PL_error("wait_for_input", 3, MSG_ERRNO, ERR_FILE_OPERATION,
		      ATOM_select, ATOM_stream, Streams);

    case 0: /* Timeout */
      break;

    default: /* Something happend -> check fds */
      for(n=min; n <= max; n++)
      { if ( FD_ISSET(n, &fds) )
	{ if ( !PL_unify_list(available, ahead, available) ||
	       !PL_unify(ahead, findmap(map, n)) )
	    fail;
	}
      }
      break;
  }

  return PL_unify_nil(available);
}

#endif /* HAVE_SELECT */


		/********************************
		*      PROLOG CONNECTION        *
		*********************************/

#define MAX_PENDING SIO_BUFSIZE		/* 4096 */

static void
re_buffer(IOSTREAM *s, const char *from, size_t len)
{ if ( s->bufp < s->limitp )
  { size_t size = s->limitp - s->bufp;

    memmove(s->buffer, s->bufp, size);
    s->bufp = s->buffer;
    s->limitp = &s->bufp[size];
  } else
  { s->bufp = s->limitp = s->buffer;
  }

  memcpy(s->bufp, from, len);
  s->bufp += len;
}


#ifndef HAVE_MBSNRTOWCS
static size_t
mbsnrtowcs(wchar_t *dest, const char **src,
	   size_t nms, size_t len, mbstate_t *ps)
{ wchar_t c;
  const char *us = *src;
  const char *es = us+nms;
  size_t count = 0;

  assert(dest == NULL);			/* incomplete implementation */

  while(us<es)
  { size_t skip = mbrtowc(&c, us, es-us, ps);

    if ( skip == (size_t)-1 )		/* error */
    { DEBUG(1, Sdprintf("mbsnrtowcs(): bad multibyte seq\n"));
      return skip;
    }
    if ( skip == (size_t)-2 )		/* incomplete */
    { *src = us;
      return count;
    }

    count++;
    us += skip;
  }

  *src = us;
  return count;
}
#else
#if defined(HAVE_DECL_MBSNRTOWCS) && !HAVE_DECL_MBSNRTOWCS
size_t mbsnrtowcs(wchar_t *dest, const char **src,
		  size_t nms, size_t len, mbstate_t *ps);
#endif
#endif /*HAVE_MBSNRTOWCS*/

static int
skip_cr(IOSTREAM *s)
{ if ( s->flags&SIO_TEXT )
  { switch(s->newline)
    { case SIO_NL_DETECT:
	s->newline = SIO_NL_DOS;
        /*FALLTHROUGH*/
      case SIO_NL_DOS:
	return TRUE;
    }
  }
  return FALSE;
}


static 
PRED_IMPL("read_pending_input", 3, read_pending_input, 0)
{ PRED_LD
  IOSTREAM *s;

  if ( getInputStream(A1, &s) )
  { char buf[MAX_PENDING];
    ssize_t n;
    word gstore, lp;
    int64_t off0 = Stell64(s);
    IOPOS pos0;

    if ( Sferror(s) )
      return streamStatus(s);

    n = Sread_pending(s, buf, sizeof(buf), 0);
    if ( n < 0 )			/* should not happen */
      return streamStatus(s);
    if ( n == 0 )			/* end-of-file */
    { S__fcheckpasteeof(s, -1);
      return PL_unify(A2, A3);
    }
    if ( s->position )
    { pos0 = *s->position;
    } else
    { memset(&pos0, 0, sizeof(pos0));	/* make compiler happy */
    }

    switch(s->encoding)
    { case ENC_OCTET:
      case ENC_ISO_LATIN_1:
      case ENC_ASCII:
      { ssize_t i;
	lp = gstore = INIT_SEQ_CODES(n);
	//	lp = gstore = allocGlobal(1+n*3); /* TBD: shift */
    
	for(i=0; i<n; i++)
	{ int c = buf[i]&0xff;
    
	  if ( c == '\r' && skip_cr(s) )
	    continue;

	  if ( s->position )
	    S__fupdatefilepos_getc(s, c);
    
	  gstore = EXTEND_SEQ_CODES(gstore, c);
	}
	if ( s->position )
	  s->position->byteno = pos0.byteno+n;

	break;
      }
      case ENC_ANSI:
      { size_t count, i;
	mbstate_t s0;
	const char *us = buf;
	const char *es = buf+n;

	if ( !s->mbstate )
	{ if ( !(s->mbstate = malloc(sizeof(*s->mbstate))) )
	  { PL_error(NULL, 0, NULL, ERR_NOMEM);
	    goto failure;
	  }
	  memset(s->mbstate, 0, sizeof(*s->mbstate));
	}
	s0 = *s->mbstate;
	count = mbsnrtowcs(NULL, &us, n, 0, &s0);
	if ( count == (size_t)-1 )
	{ Sseterr(s, SIO_WARN, "Illegal multibyte Sequence");
	  goto failure;
	}
	
	DEBUG(2, Sdprintf("Got %ld codes from %d bytes; incomplete: %ld\n",
			  count, n, es-us));

	lp = gstore = INIT_SEQ_CODES(count);
    
	for(us=buf,i=0; i<count; i++)
	{ wchar_t c;

	  us += mbrtowc(&c, us, es-us, s->mbstate);
	  if ( c == '\r' && skip_cr(s) )
	    continue;
    	  if ( s->position )
	    S__fupdatefilepos_getc(s, c);
    
	  gstore = EXTEND_SEQ_CODES(gstore, c);	  
	}
	if ( s->position )
	  s->position->byteno = pos0.byteno+us-buf;

	re_buffer(s, us, es-us);
        break;
      } 
      case ENC_UTF8:
      { const char *us = buf;
	const char *es = buf+n;
	size_t count = 0, i;

	while(us<es)
	{ const char *ec = us + UTF8_FBN(us[0]) + 1;
	  
	  if ( ec <= es )
	  { count++;
	    us=ec;
	  } else
	    break;
	}

	DEBUG(2, Sdprintf("Got %ld codes from %d bytes; incomplete: %ld\n",
			  count, n, es-us));
	
	lp = gstore = INIT_SEQ_CODES(count);
    
	for(us=buf,i=0; i<count; i++)
	{ int c;

	  us = utf8_get_char(us, &c);
	  if ( c == '\r' && skip_cr(s) )
	    continue;
    	  if ( s->position )
	    S__fupdatefilepos_getc(s, c);
    
	  gstore = EXTEND_SEQ_CODES(gstore, c);	  
	}
	if ( s->position )
	  s->position->byteno = pos0.byteno+us-buf;

	re_buffer(s, us, es-us);
        break;
      }
      case ENC_UNICODE_BE:
      case ENC_UNICODE_LE:
      { size_t count = (size_t)n/2;
	const char *us = buf;
	size_t done, i;

	lp = gstore = INIT_SEQ_CODES(count);
    
	for(i=0; i<count; us+=2, i++)
	{ int c;

	  if ( s->encoding == ENC_UNICODE_BE )
	    c = ((us[0]&0xff)<<8)+(us[1]&0xff);
	  else
	    c = ((us[1]&0xff)<<8)+(us[0]&0xff);
	  if ( c == '\r' && skip_cr(s) )
	    continue;
    
	  if ( s->position )
	    S__fupdatefilepos_getc(s, c);
    
	  gstore = EXTEND_SEQ_CODES(gstore, c);	  
	}	

	done = count*2;
	if ( s->position )
	  s->position->byteno = pos0.byteno+done;
	re_buffer(s, buf+done, n-done);
        break;
      }
      case ENC_WCHAR:
      { const pl_wchar_t *ws = (const pl_wchar_t*)buf;
	size_t count = (size_t)n/sizeof(pl_wchar_t);
	size_t done, i;

	lp = gstore = INIT_SEQ_CODES(count);
    
	for(i=0; i<count; i++)
	{ int c = ws[i];
    
	  if ( c == '\r' && skip_cr(s) )
	    continue;
	  if ( s->position )
	    S__fupdatefilepos_getc(s, c);
    
	  gstore = EXTEND_SEQ_CODES(gstore, c);	  
	}

	done = count*sizeof(pl_wchar_t);
	if ( s->position )
	  s->position->byteno = pos0.byteno+done;
	re_buffer(s, buf+done, n-done);
        break;
      }
      case ENC_UNKNOWN:
      default:
	assert(0);
        fail;
    }


    if (!CLOSE_SEQ_OF_CODES(gstore, lp, A2, A3))
      goto failure;
      
    releaseStream(s);
    succeed;

  failure:
    Sseek64(s, off0, SIO_SEEK_SET);	/* TBD: error? */
    if ( s->position )
      *s->position = pos0;
    releaseStream(s);
    fail;
  }

  fail;
}

int
PL_get_char(term_t c, int *p, int eof)
{ GET_LD
  int chr;
  atom_t name;
  PL_chars_t text;

  if ( PL_get_integer(c, &chr) )
  { if ( chr >= 0 )
    { *p = chr;
      return TRUE;
    }
    if ( eof && chr == -1 )
    { *p = chr;
      return TRUE;
    }
  } else if ( PL_get_text(c, &text, CVT_ATOM|CVT_STRING|CVT_LIST) &&
	      text.length == 1 )
  { *p = text.encoding == ENC_ISO_LATIN_1 ? text.text.t[0]&0xff
					  : text.text.w[0];
    return TRUE;
  } else if ( eof && PL_get_atom(c, &name) && name == ATOM_end_of_file )
  { *p = -1;
    return TRUE;
  }

  return PL_error(NULL, 0, NULL, ERR_TYPE, ATOM_character, c);
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PL_unify_char(term_t chr, int c, int how)
    Unify a character.  Try to be as flexible as possible, only binding a
    variable `chr' to a code or one-char-atom.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

int
PL_unify_char(term_t chr, int c, int how)
{ GET_LD
  int c2 = -1;

  if ( PL_is_variable(chr) )
  { switch(how)
    { case CHAR_MODE:
      { atom_t a = (c == -1 ? ATOM_end_of_file : codeToAtom(c));

	return PL_unify_atom(chr, a);
      }
      case CODE_MODE:
      case BYTE_MODE:
      default:
	return PL_unify_integer(chr, c);
    }
  } else if ( PL_get_char(chr, &c2, TRUE) )
    return c == c2;

  fail;
}

static foreign_t
put_byte(term_t stream, term_t byte ARG_LD)
{ IOSTREAM *s;
  int c;
  
  if ( !PL_get_integer(byte, &c) || c < 0 || c > 255 )
    return PL_error(NULL, 0, NULL, ERR_TYPE, ATOM_byte, byte);
  if ( !getOutputStream(stream, &s) )
    fail;

  Sputc(c, s);
  
  return streamStatus(s);
}


static 
PRED_IMPL("put_byte", 2, put_byte2, 0)
{ PRED_LD

  return put_byte(A1, A2 PASS_LD);
}


static 
PRED_IMPL("put_byte", 1, put_byte1, 0)
{ PRED_LD

  return put_byte(0, A1 PASS_LD);
}


static foreign_t
put_code(term_t stream, term_t chr ARG_LD)
{ IOSTREAM *s;
  int c = 0;

  if ( !PL_get_char(chr, &c, FALSE) )
    fail;
  if ( !getOutputStream(stream, &s) )
    fail;

  Sputcode(c, s);
  
  return streamStatus(s);
}


static 
PRED_IMPL("put_code", 2, put_code2, 0)
{ PRED_LD

  return put_code(A1, A2 PASS_LD);
}


static 
PRED_IMPL("put_code", 1, put_code1, 0)
{ PRED_LD

  return put_code(0, A1 PASS_LD);
}


static 
PRED_IMPL("put", 2, put2, 0)
{ PRED_LD

  return put_code(A1, A2 PASS_LD);
}


static 
PRED_IMPL("put", 1, put1, 0)
{ PRED_LD

  return put_code(0, A1 PASS_LD);
}


static foreign_t
get_nonblank(term_t in, term_t chr ARG_LD)
{ IOSTREAM *s;

  if ( getInputStream(in, &s) )
  { int c;

    for(;;)
    { c = Sgetcode(s);

      if ( c == EOF )
      { TRY(PL_unify_integer(chr, -1));
	return streamStatus(s);
      }

      if ( !isBlankW(c) )
      { releaseStream(s);
	return PL_unify_integer(chr, c);
      }
    }
  }

  fail;
}


static 
PRED_IMPL("get", 1, get1, 0)
{ PRED_LD

  return get_nonblank(0, A1 PASS_LD);
}


static 
PRED_IMPL("get", 2, get2, 0)
{ PRED_LD

  return get_nonblank(A1, A2 PASS_LD);
}


static foreign_t
skip(term_t in, term_t chr ARG_LD)
{ int c = -1;
  int r;
  IOSTREAM *s;

  if ( !PL_get_char(chr, &c, FALSE) )
    fail;
  if ( !getInputStream(in, &s) )
    fail;
  
  while((r=Sgetcode(s)) != c && r != EOF )
    ;

  return streamStatus(s);
}


static 
PRED_IMPL("skip", 1, skip1, 0)
{ PRED_LD

  return skip(0, A1 PASS_LD);
}


static 
PRED_IMPL("skip", 2, skip2, 0)
{ PRED_LD

  return skip(A1, A2 PASS_LD);
}


static word
pl_get_single_char(term_t chr)
{ GET_LD
  IOSTREAM *s = getStream(Suser_input);
  int c = getSingleChar(s, TRUE);

  if ( c == EOF )
  { PL_unify_integer(chr, -1);
    return streamStatus(s);
  }

  releaseStream(s);

  return PL_unify_integer(chr, c);
}


static foreign_t
pl_get_byte2(term_t in, term_t chr ARG_LD)
{ IOSTREAM *s;

  if ( getInputStream(in, &s) )
  { int c = Sgetc(s);

    if ( PL_unify_integer(chr, c) )
      return streamStatus(s);

    if ( Sferror(s) )
      return streamStatus(s);

    PL_get_char(chr, &c, TRUE);		/* set type-error */
  }

  fail;
}


static 
PRED_IMPL("get_byte", 2, get_byte2, 0)
{ PRED_LD

  return pl_get_byte2(A1, A2 PASS_LD);
}


static 
PRED_IMPL("get_byte", 1, get_byte1, 0)
{ PRED_LD

  return pl_get_byte2(0, A1 PASS_LD);
}


static foreign_t
pl_get_code2(term_t in, term_t chr)
{ GET_LD
  IOSTREAM *s;

  if ( getInputStream(in, &s) )
  { int c = Sgetcode(s);

    if ( PL_unify_integer(chr, c) )
      return streamStatus(s);

    if ( Sferror(s) )
      return streamStatus(s);

    PL_get_char(chr, &c, TRUE);		/* set type-error */
    releaseStream(s);
  }

  fail;
}


static 
PRED_IMPL("get_code", 2, get_code2, 0)
{ return pl_get_code2(A1, A2);
}


static 
PRED_IMPL("get_code", 1, get_code1, 0)
{ return pl_get_code2(0, A1);
}


static foreign_t
pl_get_char2(term_t in, term_t chr)
{ GET_LD
  IOSTREAM *s;

  if ( getInputStream(in, &s) )
  { int c = Sgetcode(s);

    if ( PL_unify_atom(chr, c == -1 ? ATOM_end_of_file : codeToAtom(c)) )
      return streamStatus(s);

    if ( Sferror(s) )
      return streamStatus(s);

    PL_get_char(chr, &c, TRUE);		/* set type-error */
    releaseStream(s);
  }

  fail;
}


static 
PRED_IMPL("get_char", 2, get_char2, 0)
{ return pl_get_char2(A1, A2);
}


static 
PRED_IMPL("get_char", 1, get_char1, 0)
{ return pl_get_char2(0, A1);
}


static word
pl_ttyflush(void)
{ GET_LD
  IOSTREAM *s = getStream(Suser_output);

  Sflush(s);

  return streamStatus(s);
}


static word
pl_protocol(term_t file)
{ return openProtocol(file, FALSE);
}


static word
pl_protocola(term_t file)
{ return openProtocol(file, TRUE);
}


static word
pl_protocolling(term_t file)
{ GET_LD
  IOSTREAM *s;

  if ( (s = Sprotocol) )
  { atom_t a;

    if ( (a = fileNameStream(s)) )
      return PL_unify_atom(file, a);
    else
      return PL_unify_stream_or_alias(file, s);
  }

  fail;
}


static word
pl_prompt(term_t old, term_t new)
{ GET_LD
  atom_t a;

  if ( PL_unify_atom(old, LD->prompt.current) &&
       PL_get_atom(new, &a) )
  { if ( LD->prompt.current )
      PL_unregister_atom(LD->prompt.current);
    LD->prompt.current = a;
    PL_register_atom(a);
    succeed;
  }

  fail;
}


static void
prompt1(atom_t prompt)
{ GET_LD

  if ( LD->prompt.first != prompt )
  { if ( LD->prompt.first )
      PL_unregister_atom(LD->prompt.first);
    LD->prompt.first = prompt;
    PL_register_atom(LD->prompt.first);
  }

  LD->prompt.first_used = FALSE;
}


static word
pl_prompt1(term_t prompt)
{ GET_LD
  atom_t a;
  PL_chars_t txt;

  if ( PL_get_atom(prompt, &a) )
  { prompt1(a);
  } else if ( PL_get_text(prompt, &txt,  CVT_ALL|CVT_EXCEPTION) )
  { prompt1(textToAtom(&txt));
  } else
    fail;

  succeed;
}


atom_t
PrologPrompt()
{ GET_LD

  if ( !LD->prompt.first_used && LD->prompt.first )
  { LD->prompt.first_used = TRUE;

    return LD->prompt.first;
  }

  if ( Sinput->position && Sinput->position->linepos == 0 )
    return LD->prompt.current;
  else
    return 0;				/* "" */
}


static word
pl_tab2(term_t out, term_t spaces)
{ GET_LD
  number n;
  int rval = FALSE;
  IOSTREAM *s;

  if ( !getOutputStream(out, &s) )
    fail;

  if ( valueExpression(spaces, &n PASS_LD) )
  { if ( toIntegerNumber(&n, 0) )
    { int64_t m;

      switch(n.type)
      { case V_INTEGER:
	  m = n.value.i;
	  break;
#ifdef O_GMP
	case V_MPZ:
	{ if ( !mpz_to_int64(n.value.mpz, &m) )
	  { PL_error(NULL, 0, NULL, ERR_EVALUATION, ATOM_int_overflow);
	    goto error;
	  }
	}
#endif
	default:
	  assert(0);
      }
  
      while(m-- > 0)
      { if ( Sputcode(' ', s) < 0 )
	  break;
      }
  
      rval = TRUE;
    }

    clearNumber(&n);
  } else
  { rval = PL_error("tab", 1, NULL, ERR_TYPE, ATOM_integer, spaces);
  }

  if ( rval )
    return streamStatus(s);

#ifdef O_GMP
error:
#endif
  (void)streamStatus(s);
  fail;
}

static word
pl_tab(term_t n)
{ return pl_tab2(0, n);
}


		 /*******************************
		 *	      ENCODING		*
		 *******************************/

typedef struct encname
{ IOENC  code;
  atom_t name;
} encoding_name;

INIT_DEF(struct encname, encoding_names, 10)
  ADD_ENCODING( ENC_UNKNOWN,     ATOM_unknown )
  ADD_ENCODING( ENC_OCTET,       ATOM_octet )
  ADD_ENCODING( ENC_ASCII,       ATOM_ascii )
  ADD_ENCODING( ENC_ISO_LATIN_1, ATOM_iso_latin_1 )
  ADD_ENCODING( ENC_ANSI,	     ATOM_text )
  ADD_ENCODING( ENC_UTF8,        ATOM_utf8 )
  ADD_ENCODING( ENC_UNICODE_BE,  ATOM_unicode_be )
  ADD_ENCODING( ENC_UNICODE_LE,  ATOM_unicode_le )
  ADD_ENCODING( ENC_WCHAR,	     ATOM_wchar_t )
END_ENCODINGS( ENC_UNKNOWN,     0 )


IOENC
atom_to_encoding(atom_t a)
{ struct encname *en;

  for(en=encoding_names; en->name; en++)
  { if ( en->name == a ) 
      return en->code;
  }

  return ENC_UNKNOWN;
}


static atom_t
encoding_to_atom(IOENC enc)
{ return encoding_names[enc].name;
}


static int
bad_encoding(atom_t name)
{ GET_LD
  term_t t = PL_new_term_ref();

  PL_put_atom(t, name);
  return PL_error(NULL, 0, NULL, ERR_DOMAIN, ATOM_encoding, t);
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
fn_to_atom() translates a 8-bit  filename  into   a  unicode  atom.  The
encoding is generic `multibyte' on Unix systems   and  fixed to UTF-8 on
Windows, where the uxnt layer  translates   the  UTF-8  sequences to the
Windows *W() functions.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

static atom_t
fn_to_atom(const char *fn)
{ PL_chars_t text;
  atom_t a;

  text.text.t    = (char *)fn;
  text.encoding  = ((REP_FN&REP_UTF8) ? ENC_UTF8 :
		    (REP_FN&REP_MB)   ? ENC_ANSI : ENC_ISO_LATIN_1);
  text.storage   = PL_CHARS_HEAP;
  text.length    = strlen(fn);
  text.canonical = FALSE;

  a = textToAtom(&text);
  PL_free_text(&text);
  
  return a;
}


		/********************************
		*       STREAM BASED I/O        *
		*********************************/


INIT_DEF(opt_spec, open4_options, 10)
  ADD_OPEN4_OPT( ATOM_type,		 OPT_ATOM )
  ADD_OPEN4_OPT( ATOM_reposition,     OPT_BOOL )
  ADD_OPEN4_OPT( ATOM_alias,	         OPT_ATOM )
  ADD_OPEN4_OPT( ATOM_eof_action,     OPT_ATOM )
  ADD_OPEN4_OPT( ATOM_close_on_abort, OPT_BOOL )
  ADD_OPEN4_OPT( ATOM_buffer,	 OPT_ATOM )
  ADD_OPEN4_OPT( ATOM_lock,		 OPT_ATOM )
  ADD_OPEN4_OPT( ATOM_encoding,	 OPT_ATOM )
  ADD_OPEN4_OPT( ATOM_bom,	 	 OPT_BOOL )
END_OPEN4_DEFS(NULL_ATOM, 0)


IOSTREAM *
openStream(term_t file, term_t mode, term_t options)
{ GET_LD
  atom_t mname;
  atom_t type           = ATOM_text;
  bool   reposition     = TRUE;
  atom_t alias	        = NULL_ATOM;
  atom_t eof_action     = ATOM_eof_code;
  atom_t buffer         = ATOM_full;
  atom_t lock		= ATOM_none;
  atom_t encoding	= NULL_ATOM;
  bool   close_on_abort = TRUE;
  bool	 bom		= -1;
  char   how[10];
  char  *h		= how;
  char *path;
  IOSTREAM *s;
  IOENC enc;

  if ( options )
  { if ( !
scan_options(options, 0, ATOM_stream_option, open4_options,
		       &type, &reposition, &alias, &eof_action,
		       &close_on_abort, &buffer, &lock, &encoding, &bom) )
      fail;
  }

					/* MODE */
  if ( PL_get_atom(mode, &mname) )
  { if ( mname == ATOM_write )
    { *h++ = 'w';
    } else if ( mname == ATOM_append )
    { bom = FALSE;
      *h++ = 'a';
    } else if ( mname == ATOM_update )
    { bom = FALSE;
      *h++ = 'u';
    } else if ( mname == ATOM_read )
    { *h++ = 'r';
    } else
    { PL_error(NULL, 0, NULL, ERR_DOMAIN, ATOM_io_mode, mode);
      return NULL;
    }
  } else
  { PL_error(NULL, 0, NULL, ERR_TYPE, ATOM_atom, mode);
    return NULL;
  }

					/* ENCODING */
  if ( encoding != NULL_ATOM )
  { enc = atom_to_encoding(encoding);
    if ( enc == ENC_UNKNOWN )
    { bad_encoding(encoding);

      return NULL;
    }
  } else if ( type == ATOM_binary )
  { enc = ENC_OCTET;
    bom = FALSE;
  } else
  { enc = LD->encoding;
  }

  if ( bom == -1 )
    bom = (mname == ATOM_read ? TRUE : FALSE);
  if ( type == ATOM_binary )
    *h++ = 'b';

					/* LOCK */
  if ( lock != ATOM_none )
  { *h++ = 'l';
    if ( lock == ATOM_read || lock == ATOM_shared )
      *h++ = 'r';
    else if ( lock == ATOM_write || lock == ATOM_exclusive )
      *h++ = 'w';
    else
    { term_t l = PL_new_term_ref();
      PL_put_atom(l, lock);
      PL_error(NULL, 0, NULL, ERR_DOMAIN, ATOM_lock, l);
      return NULL;
    }
  }

  *h = EOS;

					/* FILE */
  if ( PL_get_chars(file, &path,
		    CVT_ATOM|CVT_STRING|CVT_EXCEPTION|REP_FN) )
  { if ( !(s = Sopen_file(path, how)) )
    { PL_error(NULL, 0, OsError(), ERR_FILE_OPERATION,
	       ATOM_open, ATOM_source_sink, file);
      return NULL;
    }
    setFileNameStream(s, fn_to_atom(path));
  } 
#ifdef HAVE_POPEN
  else if ( PL_is_functor(file, FUNCTOR_pipe1) )
  { term_t a = PL_new_term_ref();
    char *cmd;

    PL_get_arg(1, file, a);
    if ( !PL_get_chars(a, &cmd, CVT_ATOM|CVT_STRING|REP_FN) )
    { PL_error(NULL, 0, NULL, ERR_TYPE, ATOM_atom, a);
      return NULL;
    }

    if ( !(s = Sopen_pipe(cmd, how)) )
    { PL_error(NULL, 0, OsError(), ERR_FILE_OPERATION,
	       ATOM_open, ATOM_source_sink, file);
      return NULL;
    }
  }
#endif /*HAVE_POPEN*/
  else
  { return NULL;
  }

  s->encoding = enc;
  if ( !close_on_abort )
    s->flags |= SIO_NOCLOSE;

  if ( how[0] == 'r' )
  { if ( eof_action != ATOM_eof_code )
    { if ( eof_action == ATOM_reset )
	s->flags |= SIO_NOFEOF;
      else if ( eof_action == ATOM_error )
	s->flags |= SIO_FEOF2ERR;
    }
  } else
  { if ( buffer != ATOM_full )
    { s->flags &= ~SIO_FBUF;
      if ( buffer == ATOM_line )
	s->flags |= SIO_LBUF;
      if ( buffer == ATOM_false )
	s->flags |= SIO_NBUF;
    }
  }

  if ( alias != NULL_ATOM )
    aliasStream(s, alias);
  if ( !reposition )
    s->position = NULL;

  if ( bom )
  { if ( mname == ATOM_read )
    { if ( ScheckBOM(s) < 0 )
      { bom_error:

	streamStatus(getStream(s));
	return NULL;
      }
    } else
    { if ( SwriteBOM(s) < 0 )
	goto bom_error;
    }
  }

  return s;
}


static word
pl_open4(term_t file, term_t mode, term_t stream, term_t options)
{ IOSTREAM *s = openStream(file, mode, options);

  if ( s )
    return PL_unify_stream_or_alias(stream, s);

  fail;
}


static word
pl_open(term_t file, term_t mode, term_t stream)
{ return pl_open4(file, mode, stream, 0);
}

		 /*******************************
		 *	  EDINBURGH I/O		*
		 *******************************/

static IOSTREAM *
findStreamFromFile(atom_t name, unsigned int flags)
{ TableEnum e;
  Symbol symb;
  IOSTREAM *s = NULL;

  e = newTableEnum(streamContext);
  while( (symb=advanceTableEnum(e)) )
  { stream_context *ctx = symb->value;

    if ( ctx->filename == name &&
	 true(ctx, flags) )
    { s = symb->name;
      break;
    }
  }
  freeTableEnum(e);

  return s;
}


static word
pl_see(term_t f)
{ GET_LD
  IOSTREAM *s;
  atom_t a;
  term_t mode;

  LOCK();
  if ( get_stream_handle(f, &s, SH_ALIAS|SH_UNLOCKED) )
  { Scurin = s;
    goto ok;
  }

  if ( PL_get_atom(f, &a) && a == ATOM_user )
  { Scurin = Suser_input;
    goto ok;
  }
  if ( (s = findStreamFromFile(a, IO_SEE)) )
  { Scurin = s;
    goto ok;
  }

  mode = PL_new_term_ref();
  PL_put_atom(mode, ATOM_read);
  if ( !(s = openStream(f, mode, 0)) )
  { UNLOCK();
    fail;
  }

  set(getStreamContext(s), IO_SEE);
  pl_push_input_context();
  Scurin = s;

ok:
  UNLOCK();

  succeed;
}

static word
pl_seeing(term_t f)
{ GET_LD
  if ( Scurin == Suser_input )
    return PL_unify_atom(f, ATOM_user);

  return pl_current_input(f);
}

static word
pl_seen(void)
{ GET_LD
  IOSTREAM *s = getStream(Scurin);

  pl_pop_input_context();

  if ( s->flags & SIO_NOFEOF )
    succeed;

  return closeStream(s);
}

/* MT: Does not create a lock on the stream
*/

static word
do_tell(term_t f, atom_t m)
{ GET_LD
  IOSTREAM *s;
  atom_t a;
  term_t mode;

  LOCK();
  if ( get_stream_handle(f, &s, SH_UNLOCKED) )
  { Scurout = s;
    goto ok;
  }

  if ( PL_get_atom(f, &a) && a == ATOM_user )
  { Scurout = Suser_output;
    goto ok;
  }

  if ( (s = findStreamFromFile(a, IO_TELL)) )
  { Scurout = s;
    goto ok;
  }

  mode = PL_new_term_ref();
  PL_put_atom(mode, m);
  if ( !(s = openStream(f, mode, 0)) )
  { UNLOCK();
    fail;
  }

  set(getStreamContext(s), IO_TELL);
  pushOutputContext();
  Scurout = s;

ok:
  UNLOCK();
  succeed;
}

static word
pl_tell(term_t f)
{ return do_tell(f, ATOM_write);
}

static word
pl_append(term_t f)
{ return do_tell(f, ATOM_append);
}

static word
pl_telling(term_t f)
{ GET_LD
  if ( Scurout == Suser_output )
    return PL_unify_atom(f, ATOM_user);

  return pl_current_output(f);
}

static word
pl_told(void)
{ GET_LD
  IOSTREAM *s = getStream(Scurout);

  popOutputContext();

  if ( s->flags & SIO_NOFEOF )
    succeed;

  return closeStream(s);
}

		 /*******************************
		 *	   NULL-STREAM		*
		 *******************************/

static ssize_t
Swrite_null(void *handle, char *buf, size_t size)
{ return size;
}


static ssize_t
Sread_null(void *handle, char *buf, size_t size)
{ return 0;
}


static long
Sseek_null(void *handle, long offset, int whence)
{ switch(whence)
  { case SIO_SEEK_SET:
	return offset;
    case SIO_SEEK_CUR:
    case SIO_SEEK_END:
    default:
        return -1;
  }
}


static int
Sclose_null(void *handle)
{ return 0;
}


static const IOFUNCTIONS nullFunctions =
{ Sread_null,
  Swrite_null,
  Sseek_null,
  Sclose_null
};


static word
pl_open_null_stream(term_t stream)
{ int sflags = SIO_NBUF|SIO_RECORDPOS|SIO_OUTPUT;
  IOSTREAM *s = Snew((void *)NULL, sflags, (IOFUNCTIONS *)&nullFunctions);

  if ( s )
  { s->encoding = ENC_UTF8;
    return PL_unify_stream_or_alias(stream, s);
  }

  fail;
}


static word
pl_close(term_t stream)
{ IOSTREAM *s;

  if ( PL_get_stream_handle(stream, &s) )
    return closeStream(s);

  fail;
}


INIT_DEF(opt_spec, close2_options, 2)
  ADD_CLOSE2_OPT( ATOM_force,		 OPT_BOOL )
END_CLOSE2_DEFS( NULL_ATOM,		 0 )


static word
pl_close2(term_t stream, term_t options)
{ IOSTREAM *s;
  bool force = FALSE;

  if ( !scan_options(options, 0, ATOM_close_option, close2_options, &force) )
    fail;

  if ( !force )
    return pl_close(stream);

  if ( !PL_get_stream_handle(stream, &s) )
    fail;

  if ( s == Sinput )
    Sclearerr(s);
  else if ( s == Soutput || s == Serror )
  { Sflush(s);
    Sclearerr(s);
  } else
  { Sflush(s);
    Sclose(s);
  }
  
  succeed;
}


		 /*******************************
		 *	 STREAM-PROPERTY	*
		 *******************************/

static int
stream_file_name_propery(IOSTREAM *s, term_t prop ARG_LD)
{ atom_t name;

  if ( (name = getStreamContext(s)->filename) )
  { return PL_unify_atom(prop, name);
  }

  fail;
}


static int
stream_mode_property(IOSTREAM *s, term_t prop ARG_LD)
{ atom_t mode;

  if ( s->flags & SIO_INPUT )
    mode = ATOM_read;
  else
  { assert(s->flags & SIO_OUTPUT);

    if ( s->flags & SIO_APPEND )
      mode = ATOM_append;
    else if ( s->flags & SIO_UPDATE )
      mode = ATOM_update;
    else
      mode = ATOM_write;
  }

  return PL_unify_atom(prop, mode);
}


static int
stream_input_prop(IOSTREAM *s ARG_LD)
{ return (s->flags & SIO_INPUT) ? TRUE : FALSE;
}


static int
stream_output_prop(IOSTREAM *s ARG_LD)
{ return (s->flags & SIO_OUTPUT) ? TRUE : FALSE;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Incomplete: should be non-deterministic if the stream has multiple aliases!
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

static int
stream_alias_prop(IOSTREAM *s, term_t prop ARG_LD)
{ atom_t name;
  stream_context *ctx = getStreamContext(s);
  int i;
  
  if ( PL_get_atom(prop, &name) )
  { alias *a;

    for( a = ctx->alias_head; a; a = a->next )
    { if ( a->name == name )
	return TRUE;
    }
    
    if ( (i=standardStreamIndexFromName(name)) >= 0 &&
	 i < 6 &&
	 s == LD->IO.streams[i] )
      return TRUE;

    return FALSE;
  }

  if ( (i=standardStreamIndexFromStream(s)) >= 0 && i < 3 )
    return PL_unify_atom(prop, standardStreams[i]);
  if ( ctx->alias_head )
    return PL_unify_atom(prop, ctx->alias_head->name);

  return FALSE;
}


static int
stream_position_prop(IOSTREAM *s, term_t prop ARG_LD)
{ if ( s->position )
  { return PL_unify_term(prop,
			 PL_FUNCTOR, FUNCTOR_stream_position4,
			   PL_INT64, s->position->charno,
			   PL_INT, s->position->lineno,
			   PL_INT, s->position->linepos,
			   PL_INT64, s->position->byteno);
  }

  fail;
}


static int
stream_end_of_stream_prop(IOSTREAM *s, term_t prop ARG_LD)
{ if ( s->flags & SIO_INPUT )
  { GET_LD
    atom_t val;

    if ( s->flags & SIO_FEOF2 )
      val = ATOM_past;
    else if ( s->flags & SIO_FEOF )
      val = ATOM_at;
    else
      val = ATOM_not;

    return PL_unify_atom(prop, val);
  }

  return FALSE;
}


static int
stream_eof_action_prop(IOSTREAM *s, term_t prop ARG_LD)
{ atom_t val;

  if ( s->flags & SIO_NOFEOF )
    val = ATOM_reset;
  else if ( s->flags & SIO_FEOF2ERR )
    val = ATOM_error;
  else
    val = ATOM_eof_code;

  return PL_unify_atom(prop, val);
}


#ifdef HAVE_FSTAT
#include <sys/stat.h>
#endif

#if !defined(S_ISREG) && defined(S_IFREG)
#define S_ISREG(m) ((m&S_IFMT) == S_IFREG)
#endif

static int
stream_reposition_prop(IOSTREAM *s, term_t prop ARG_LD)
{ atom_t val;

  if ( s->functions->seek )
  {
#ifdef HAVE_FSTAT
    int fd = Sfileno(s);
    struct stat buf;

    if ( fstat(fd, &buf) == 0 && S_ISREG(buf.st_mode) )
      val = ATOM_true;
    else
      val = ATOM_false;
#else
    val = ATOM_true;
#endif
  } else
    val = ATOM_false;
  
  return PL_unify_atom(prop, val);
}


static int
stream_close_on_abort_prop(IOSTREAM *s, term_t prop ARG_LD)
{ return PL_unify_bool_ex(prop, !(s->flags & SIO_NOCLOSE));
}


static int
stream_type_prop(IOSTREAM *s, term_t prop ARG_LD)
{ return PL_unify_atom(prop, s->flags & SIO_TEXT ? ATOM_text : ATOM_binary);
}


static int
stream_file_no_prop(IOSTREAM *s, term_t prop ARG_LD)
{ int fd;

  if ( (fd = Sfileno(s)) >= 0 )
    return PL_unify_integer(prop, fd);

  fail;
}


static int
stream_tty_prop(IOSTREAM *s, term_t prop ARG_LD)
{ if ( (s->flags & SIO_ISATTY) ) 
    return PL_unify_bool_ex(prop, TRUE);

  fail;
}


static int
stream_bom_prop(IOSTREAM *s, term_t prop ARG_LD)
{ if ( (s->flags & SIO_BOM) ) 
    return PL_unify_bool_ex(prop, TRUE);

  fail;
}


static int
stream_newline_prop(IOSTREAM *s, term_t prop ARG_LD)
{ switch ( s->newline ) 
  { case SIO_NL_POSIX:
    case SIO_NL_DETECT:
      return PL_unify_atom(prop, ATOM_posix);
    case SIO_NL_DOS:
      return PL_unify_atom(prop, ATOM_dos);
  }

  fail;
}


static int
stream_encoding_prop(IOSTREAM *s, term_t prop ARG_LD)
{ return PL_unify_atom(prop, encoding_to_atom(s->encoding));
}


static int
stream_reperror_prop(IOSTREAM *s, term_t prop ARG_LD)
{ atom_t a;

  if ( (s->flags & SIO_REPXML) )
    a = ATOM_xml;
  else if ( (s->flags & SIO_REPPL) )
    a = ATOM_prolog;
  else
    a = ATOM_error;

  return PL_unify_atom(prop, a);
}


static int
stream_buffer_prop(IOSTREAM *s, term_t prop ARG_LD)
{ atom_t b;

  if ( s->flags & SIO_FBUF )
    b = ATOM_full;
  else if ( s->flags & SIO_LBUF )
    b = ATOM_line;
  else /*if ( s->flags & SIO_NBUF )*/
    b = ATOM_false;

  return PL_unify_atom(prop, b);
}


static int
stream_buffer_size_prop(IOSTREAM *s, term_t prop ARG_LD)
{ if ( (s->flags & SIO_NBUF) )
    fail;
    
  return PL_unify_integer(prop, s->bufsize);
}


static int
stream_timeout_prop(IOSTREAM *s, term_t prop ARG_LD)
{ if ( s->timeout == -1 )
    return PL_unify_atom(prop, ATOM_infinite);
  
  return PL_unify_float(prop, (double)s->timeout/1000.0);
}


typedef struct
{ functor_t functor;			/* functor of property */
  int (*function)();		/* function to generate */
} sprop;


INIT_DEF(sprop, sprop_list, 24)
  ADD_SPROP( FUNCTOR_file_name1,	    stream_file_name_propery )
  ADD_SPROP( FUNCTOR_mode1,	    stream_mode_property )
  ADD_SPROP( FUNCTOR_input0,	    stream_input_prop )
  ADD_SPROP( FUNCTOR_output0,	    stream_output_prop )
  ADD_SPROP( FUNCTOR_alias1,	    stream_alias_prop )
  ADD_SPROP( FUNCTOR_position1,	    stream_position_prop )
  ADD_SPROP( FUNCTOR_end_of_stream1, stream_end_of_stream_prop )
  ADD_SPROP( FUNCTOR_eof_action1,    stream_eof_action_prop )
  ADD_SPROP( FUNCTOR_reposition1,    stream_reposition_prop )
  ADD_SPROP( FUNCTOR_type1,    	    stream_type_prop )
  ADD_SPROP( FUNCTOR_file_no1,	    stream_file_no_prop )
  ADD_SPROP( FUNCTOR_buffer1,	    stream_buffer_prop )
  ADD_SPROP( FUNCTOR_buffer_size1,   stream_buffer_size_prop )
  ADD_SPROP( FUNCTOR_close_on_abort1,stream_close_on_abort_prop )
  ADD_SPROP( FUNCTOR_tty1,	    stream_tty_prop )
  ADD_SPROP( FUNCTOR_encoding1,	    stream_encoding_prop )
  ADD_SPROP( FUNCTOR_bom1,	    stream_bom_prop )
  ADD_SPROP( FUNCTOR_newline1,	    stream_newline_prop )
  ADD_SPROP( FUNCTOR_representation_errors1, stream_reperror_prop )
  ADD_SPROP( FUNCTOR_timeout1,       stream_timeout_prop )
END_SPROP_DEFS( 0,			    NULL)


typedef struct
{ TableEnum e;				/* Enumerator on stream-table */
  IOSTREAM *s;				/* Stream we are enumerating */
  const sprop *p;			/* Pointer in properties */
  int fixed_p;				/* Propety is given */
} prop_enum;


static foreign_t
pl_stream_property(term_t stream, term_t property, control_t h)
{ GET_LD
  IOSTREAM *s;
  prop_enum *pe;
  fid_t fid;
  term_t a1;

  switch( ForeignControl(h) )
  { case FRG_FIRST_CALL:
      a1 = PL_new_term_ref();
      
      if ( PL_is_variable(stream) )	/* generate */
      {	const sprop *p = sprop_list;
	int fixed = FALSE;
	functor_t f;

	if ( PL_get_functor(property, &f) ) /* test for defined property */
	{ for( ; p->functor; p++ )
	  { if ( f == p->functor )
	    { fixed = TRUE;
	      break;
	    }
	  }
	  if ( !p->functor )
	    return PL_error(NULL, 0, NULL, ERR_DOMAIN,
			    ATOM_stream_property, property);
	}

	pe = allocHeap(sizeof(*pe));

	pe->e = newTableEnum(streamContext);
	pe->s = NULL;
	pe->p = p;
	pe->fixed_p = fixed;
	
	break;
      }

      LOCK();				/* given stream */
      if ( get_stream_handle(stream, &s, SH_ERRORS|SH_UNLOCKED) )
      { functor_t f;

	if ( PL_is_variable(property) )	/* generate properties */
	{ pe = allocHeap(sizeof(*pe));

	  pe->e = NULL;
	  pe->s = s;
	  pe->p = sprop_list;
	  pe->fixed_p = FALSE;
	  UNLOCK();

	  break;
	}

	if ( PL_get_functor(property, &f) )
	{ const sprop *p = sprop_list;

	  for( ; p->functor; p++ )
	  { if ( f == p->functor )
	    { int rval;

	      switch(arityFunctor(f))
	      { case 0:
		  rval = (*p->function)(s PASS_LD);
		  break;
		case 1:
		{ term_t a1 = PL_new_term_ref();

		  _PL_get_arg(1, property, a1);
		  rval = (*p->function)(s, a1 PASS_LD);
		  break;
		}
		default:
		  assert(0);
		  rval = FALSE;
	      }
	      UNLOCK();
	      return rval;
	    }
	  }
	} else
	{ UNLOCK();
	  return PL_error(NULL, 0, NULL, ERR_DOMAIN,
			  ATOM_stream_property, property);
	}
      }
      UNLOCK();
      fail;				/* bad stream handle */
    case FRG_REDO:
    { pe = ForeignContextPtr(h);
      a1 = PL_new_term_ref();
      
      break;
    }
    case FRG_CUTTED:
    { pe = ForeignContextPtr(h);

      if ( pe )				/* 0 if exception on FRG_FIRST_CALL */
      { if ( pe->e )
	  freeTableEnum(pe->e);

	freeHeap(pe, sizeof(*pe));
      }
      succeed;
    }
    default:
      assert(0);
      fail;
  }


  fid = PL_open_foreign_frame();

  for(;;)
  { if ( pe->s )				/* given stream */
    { fid_t fid2;
  
      if ( PL_is_variable(stream) )
      { if ( !PL_unify_stream(stream, pe->s) )
	  goto enum_e;
      }

      fid2 = PL_open_foreign_frame();
      for( ; pe->p->functor ; pe->p++ )
      { if ( PL_unify_functor(property, pe->p->functor) )
	{ int rval;

	  switch(arityFunctor(pe->p->functor))
	  { case 0:
	      rval = (*pe->p->function)(pe->s PASS_LD);
	      break;
	    case 1:
	    { _PL_get_arg(1, property, a1);

	      rval = (*pe->p->function)(pe->s, a1 PASS_LD);
	      break;
	    }
	    default:
	      assert(0);
	      rval = FALSE;
	  }
	  if ( rval )
	  { if ( pe->fixed_p )
	      pe->s = NULL;
	    else
	      pe->p++;
	    ForeignRedoPtr(pe);
	  }
	}

	if ( pe->fixed_p )
	  break;
	PL_rewind_foreign_frame(fid2);
      }
      PL_close_foreign_frame(fid2);
      pe->s = NULL;
    }
  
  enum_e:
    if ( pe->e )
    { Symbol symb;

      while ( (symb=advanceTableEnum(pe->e)) )
      { PL_rewind_foreign_frame(fid);
	if ( PL_unify_stream(stream, symb->name) )
	{ pe->s = symb->name;
	  if ( !pe->fixed_p )
	    pe->p = sprop_list;
	  break;
	}
      } 
    }

    if ( !pe->s )
    { if ( pe->e )
	freeTableEnum(pe->e);

      freeHeap(pe, sizeof(*pe));
      fail;
    }
  }
}


static 
PRED_IMPL("is_stream", 1, is_stream, 0)
{ GET_LD
  IOSTREAM *s;

  if ( get_stream_handle(A1, &s, SH_SAFE) )
  { releaseStream(s);
    succeed;
  }

  fail;
}



		 /*******************************
		 *	      FLUSH		*
		 *******************************/


static word
pl_flush_output1(term_t out)
{ IOSTREAM *s;

  if ( getOutputStream(out, &s) )
  { Sflush(s);
    return streamStatus(s);
  }

  fail;
}


static word
pl_flush_output(void)
{ return pl_flush_output1(0);
}


static int
getStreamWithPosition(term_t stream, IOSTREAM **sp)
{ IOSTREAM *s;

  if ( PL_get_stream_handle(stream, &s) )
  { if ( !s->position )
    { PL_error(NULL, 0, NULL, ERR_PERMISSION, /* non-ISO */
	       ATOM_property, ATOM_position, stream);
      releaseStream(s);
      return FALSE;
    }

    *sp = s;
    return TRUE;
  }

  return FALSE;
}


static int
getRepositionableStream(term_t stream, IOSTREAM **sp)
{ GET_LD
  IOSTREAM *s;

  if ( get_stream_handle(stream, &s, SH_ERRORS) )
  { if ( !s->position || !s->functions || !s->functions->seek )
    { PL_error(NULL, 0, NULL, ERR_PERMISSION,
	       ATOM_reposition, ATOM_stream, stream);
      releaseStream(s);
      return FALSE;
    }

    *sp = s;
    return TRUE;
  }

  return FALSE;
}


static word
pl_set_stream_position(term_t stream, term_t pos)
{ GET_LD
  IOSTREAM *s;
  int64_t charno, byteno;
  long linepos, lineno;
  term_t a = PL_new_term_ref();

  if ( !(getRepositionableStream(stream, &s)) )
    fail;

  if ( !PL_is_functor(pos, FUNCTOR_stream_position4) ||
       !PL_get_arg(1, pos, a) ||
       !PL_get_int64(a, &charno) ||
       !PL_get_arg(2, pos, a) ||
       !PL_get_long(a, &lineno) ||
       !PL_get_arg(3, pos, a) ||
       !PL_get_long(a, &linepos) || 
       !PL_get_arg(4, pos, a) ||
       !PL_get_int64(a, &byteno) )
  { releaseStream(s);
    return PL_error("stream_position", 3, NULL,
		    ERR_DOMAIN, ATOM_stream_position, pos);
  }

  if ( Sseek64(s, byteno, SIO_SEEK_SET) != 0 )
    return PL_error(NULL, 0, MSG_ERRNO, ERR_FILE_OPERATION,
		    ATOM_reposition, ATOM_stream, stream);

  s->position->byteno  = byteno;
  s->position->charno  = charno;
  s->position->lineno  = (int)lineno;
  s->position->linepos = (int)linepos;

  releaseStream(s);

  succeed;
}


static word
pl_seek(term_t stream, term_t offset, term_t method, term_t newloc)
{ GET_LD
  atom_t m;
  int whence = -1;
  int64_t off, new;
  IOSTREAM *s;

  if ( !(PL_get_atom_ex(method, &m)) )
    return FALSE;

  if ( m == ATOM_bof )
    whence = SIO_SEEK_SET;
  else if ( m == ATOM_current )
    whence = SIO_SEEK_CUR;
  else if ( m == ATOM_eof )
    whence = SIO_SEEK_END;
  else
    return PL_error("seek", 4, NULL, ERR_DOMAIN, ATOM_seek_method, method);
  
  if ( !PL_get_int64(offset, &off) )
    return PL_error("seek", 4, NULL, ERR_DOMAIN, ATOM_integer, offset);

  if ( PL_get_stream_handle(stream, &s) )
  { int unit = Sunit_size(s);

    off *= unit;
    if ( Sseek64(s, off, whence) < 0 )
    { if ( errno == EINVAL )
	PL_error("seek", 4, "offset out of range", ERR_DOMAIN,
		 ATOM_position, offset);
      else
	PL_error("seek", 4, OsError(), ERR_PERMISSION,
		 ATOM_reposition, ATOM_stream, stream);
      releaseStream(s);
      fail;
    }

    new = Stell64(s);
    releaseStream(s);
    new /= unit;

    return PL_unify_int64(newloc, new);
  }

  fail;
}


static word
pl_set_input(term_t stream)
{ GET_LD
  IOSTREAM *s;

  if ( getInputStream(stream, &s) )
  { Scurin = s;
    releaseStream(s);
    return TRUE;
  }

  return FALSE;
}


static word
pl_set_output(term_t stream)
{ GET_LD
  IOSTREAM *s;

  if ( getOutputStream(stream, &s) )
  { Scurout = s;
    releaseStream(s);
    return TRUE;
  }

  return FALSE;
}


word
pl_current_input(term_t stream)
{ GET_LD
  return PL_unify_stream(stream, Scurin);
}


word
pl_current_output(term_t stream)
{ GET_LD
  return PL_unify_stream(stream, Scurout);
}


static
PRED_IMPL("byte_count", 2, byte_count, 0)
{ IOSTREAM *s;

  if ( getStreamWithPosition(A1, &s) )
  { int64_t n = s->position->byteno;

    releaseStream(s);
    return PL_unify_int64(A2, n);
  }

  fail;
}


static
PRED_IMPL("character_count", 2, character_count, 0)
{ IOSTREAM *s;

  if ( getStreamWithPosition(A1, &s) )
  { int64_t n = s->position->charno;

    releaseStream(s);
    return PL_unify_int64(A2, n);
  }

  fail;
}


static
PRED_IMPL("line_count", 2, line_count, 0)
{ GET_LD
  IOSTREAM *s;

  if ( getStreamWithPosition(A1, &s) )
  { intptr_t n = s->position->lineno;

    releaseStream(s);
    return PL_unify_integer(A2, n);
  }

  fail;
}


static
PRED_IMPL("line_position", 2, line_position, 0)
{ GET_LD
  IOSTREAM *s;

  if ( getStreamWithPosition(A1, &s) )
  { intptr_t n = s->position->linepos;

    releaseStream(s);
    return PL_unify_integer(A2, n);
  }

  fail;
}


static word
pl_source_location(term_t file, term_t line)
{ GET_LD
  if ( ReadingSource &&
       PL_unify_atom(file, source_file_name) &&
       PL_unify_integer(line, source_line_no) )
    succeed;
  
  fail;
}


static word
pl_at_end_of_stream1(term_t stream)
{ GET_LD
  IOSTREAM *s;

  if ( getInputStream(stream, &s) )
  { int rval = Sfeof(s);

    if ( rval < 0 )
    { PL_error(NULL, 0, "not-buffered stream", ERR_PERMISSION,
	       ATOM_end_of_stream, ATOM_stream, stream);
      rval = FALSE;
    }
    
    if ( rval && Sferror(s) )		/* due to error */
      return streamStatus(s);
    else
      releaseStream(s);

    return rval;
  }

  return FALSE;				/* exception */
}


static word
pl_at_end_of_stream0(void)
{ return pl_at_end_of_stream1(0);
}

static foreign_t
peek(term_t stream, term_t chr, int how)
{ GET_LD
  IOSTREAM *s;
  IOPOS pos;
  int c;

  if ( !getInputStream(stream, &s) )
    fail;

  pos = s->posbuf;
  if ( how == BYTE_MODE )
  { c = Sgetc(s);
    if ( c != EOF )
      Sungetc(c, s);
  } else
  { c = Sgetcode(s);
    if ( c != EOF )
      Sungetcode(c, s);
  }
  s->posbuf = pos;
  if ( Sferror(s) )
    return streamStatus(s);
  releaseStream(s);

  return PL_unify_char(chr, c, how);
}


static 
PRED_IMPL("peek_byte", 2, peek_byte2, 0)
{ return peek(A1, A2, BYTE_MODE);
}


static 
PRED_IMPL("peek_byte", 1, peek_byte1, 0)
{ return peek(0, A1, BYTE_MODE);
}


static 
PRED_IMPL("peek_code", 2, peek_code2, 0)
{ return peek(A1, A2, CODE_MODE);
}


static 
PRED_IMPL("peek_code", 1, peek_code1, 0)
{ return peek(0, A1, CODE_MODE);
}


static 
PRED_IMPL("peek_char", 2, peek_char2, 0)
{ return peek(A1, A2, CHAR_MODE);
}


static 
PRED_IMPL("peek_char", 1, peek_char1, 0)
{ return peek(0, A1, CHAR_MODE);
}


		 /*******************************
		 *	    INTERACTION		*
		 *******************************/

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
set_prolog_OI(+In, +Out, +Error)

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

typedef struct wrappedIO
{ void		   *wrapped_handle;	/* original handle */
  IOFUNCTIONS      *wrapped_functions;	/* original functions */
  IOSTREAM	   *wrapped_stream;	/* stream we wrapped */
  IOFUNCTIONS       functions;		/* new function block */
} wrappedIO;


static ssize_t
Sread_user(void *handle, char *buf, size_t size)
{ GET_LD
  wrappedIO *wio = handle;

  if ( LD->prompt.next && ttymode != TTY_RAW )
    PL_write_prompt(TRUE);
  else
    Sflush(Suser_output);

  size = (*wio->wrapped_functions->read)(wio->wrapped_handle, buf, size);
  if ( size == 0 )			/* end-of-file */
  { Sclearerr(Suser_input);
    LD->prompt.next = TRUE;
  } else if ( size > 0 && buf[size-1] == '\n' )
    LD->prompt.next = TRUE;

  return size;
}


static int
closeWrappedIO(void *handle)
{ wrappedIO *wio = handle;
  int rval;

  if ( wio->wrapped_functions->close )
    rval = (*wio->wrapped_functions->close)(wio->wrapped_handle);
  else
    rval = 0;
  
  wio->wrapped_stream->functions = wio->wrapped_functions;
  wio->wrapped_stream->handle = wio->wrapped_handle;
  PL_free(wio);

  return rval;
}


static void
wrapIO(IOSTREAM *s,
       ssize_t (*read)(void *, char *, size_t),
       ssize_t (*write)(void *, char *, size_t))
{ wrappedIO *wio = PL_malloc(sizeof(*wio));

  wio->wrapped_functions = s->functions;
  wio->wrapped_handle =	s->handle;
  wio->wrapped_stream = s;

  wio->functions = *s->functions;
  if ( read  ) wio->functions.read  = read;
  if ( write ) wio->functions.write = write;
  wio->functions.close = closeWrappedIO;

  s->functions = &wio->functions;
  s->handle = wio;
}


static
PRED_IMPL("set_prolog_IO", 3, set_prolog_IO, 0)
{ PRED_LD
  IOSTREAM *in = NULL, *out = NULL, *error = NULL;
  int rval = FALSE;

  if ( !PL_get_stream_handle(A1, &in) ||
       !PL_get_stream_handle(A2, &out) )
    goto out;

  if ( PL_compare(A2, A3) == 0 )	/* == */
  { error = getStream(Snew(out->handle, out->flags, out->functions));
    error->flags &= ~SIO_ABUF;		/* disable buffering */
    error->flags |= SIO_NBUF;
  } else
  { if ( !PL_get_stream_handle(A3, &error) )
      goto out;
  }

  LOCK();
  out->flags &= ~SIO_ABUF;		/* output: line buffered */
  out->flags |= SIO_LBUF;

  LD->IO.streams[0] = in;		/* user_input */
  LD->IO.streams[1] = out;		/* user_output */
  LD->IO.streams[2] = error;		/* user_error */
  LD->IO.streams[3] = in;		/* current_input */
  LD->IO.streams[4] = out;		/* current_output */

  wrapIO(in, Sread_user, NULL);
  LD->prompt.next = TRUE;

  UNLOCK();
  rval = TRUE;

out:
  if ( in )
    releaseStream(in);
  if ( out )
    releaseStream(out);
  if ( error && error != out )
    releaseStream(error);

  return rval;
}


		/********************************
		*             FILES             *
		*********************************/

static bool
unifyTime(term_t t, intptr_t time)
{ return PL_unify_float(t, (double)time);
}


static void
add_option(term_t options, functor_t f, atom_t val)
{ GET_LD
  term_t head = PL_new_term_ref();

  PL_unify_list(options, head, options);
  PL_unify_term(head, PL_FUNCTOR, f, PL_ATOM, val);

  PL_reset_term_refs(head);
}

#define CVT_FILENAME (CVT_ATOM|CVT_STRING|CVT_LIST)

static int
PL_get_file_name(term_t n, char **namep, int flags)
{ GET_LD
  char *name;
  char tmp[MAXPATHLEN];
  char ospath[MAXPATHLEN];

  if ( flags & PL_FILE_SEARCH )
  { predicate_t pred = PL_predicate("absolute_file_name", 3, "system");
    term_t av = PL_new_term_refs(3);
    term_t options = PL_copy_term_ref(av+2);
    int cflags = ((flags&PL_FILE_NOERRORS) ? PL_Q_CATCH_EXCEPTION
					   : PL_Q_PASS_EXCEPTION);

    PL_put_term(av+0, n);
    
    if ( flags & PL_FILE_EXIST )
      add_option(options, FUNCTOR_access1, ATOM_exist);
    if ( flags & PL_FILE_READ )
      add_option(options, FUNCTOR_access1, ATOM_read);
    if ( flags & PL_FILE_WRITE )
      add_option(options, FUNCTOR_access1, ATOM_write);
    if ( flags & PL_FILE_EXECUTE )
      add_option(options, FUNCTOR_access1, ATOM_execute);

    PL_unify_nil(options);

    if ( !PL_call_predicate(NULL, cflags, pred, av) )
      fail;
    
    return PL_get_chars_ex(av+1, namep, CVT_ATOMIC|BUF_RING|REP_FN);
  }

  if ( flags & PL_FILE_NOERRORS )
  { if ( !PL_get_chars(n, &name, CVT_FILENAME|REP_FN) )
      fail;
  } else
  { if ( !PL_get_chars_ex(n, &name, CVT_FILENAME|REP_FN) )
      fail;
  }

  if ( trueFeature(FILEVARS_FEATURE) )
  { if ( !(name = ExpandOneFile(name, tmp)) )
      fail;
  }
  
  if ( !(flags & PL_FILE_NOERRORS) )
  { atom_t op = 0;

    if ( (flags&PL_FILE_READ) && !AccessFile(name, ACCESS_READ) )
      op = ATOM_read;
    if ( !op && (flags&PL_FILE_WRITE) && !AccessFile(name, ACCESS_WRITE) )
      op = ATOM_write;
    if ( !op && (flags&PL_FILE_EXECUTE) && !AccessFile(name, ACCESS_EXECUTE) )
      op = ATOM_execute;

    if ( op )
      return PL_error(NULL, 0, NULL, ERR_PERMISSION, ATOM_file, op, n);
		    
    if ( (flags & PL_FILE_EXIST) && !AccessFile(name, ACCESS_EXIST) )
      return PL_error(NULL, 0, NULL, ERR_EXISTENCE, ATOM_file, n);
  }

  if ( flags & PL_FILE_ABSOLUTE )
  { if ( !(name = AbsoluteFile(name, tmp)) )
      fail;
  }

  if ( flags & PL_FILE_OSPATH )
  { if ( !(name = OsPath(name, ospath)) )
      fail;
  }
    
  *namep = buffer_string(name, BUF_RING);
  succeed;
}


static word
pl_time_file(term_t name, term_t t)
{ char *fn;

  if ( PL_get_file_name(name, &fn, 0) )
  { intptr_t time;

    if ( (time = LastModifiedFile(fn)) == -1 )
      return PL_error(NULL, 0, NULL, ERR_FILE_OPERATION, ATOM_time, ATOM_file, name);

    return unifyTime(t, time);
  }

  fail;
}


static word
pl_size_file(term_t name, term_t len)
{ char *n;

  if ( PL_get_file_name(name, &n, 0) )
  { int64_t size;

    if ( (size = SizeFile(n)) < 0 )
      return PL_error("size_file", 2, OsError(), ERR_FILE_OPERATION,
		      ATOM_size, ATOM_file, name);

    return PL_unify_int64(len, size);
  }

  fail;
}


static word
pl_size_stream(term_t stream, term_t len)
{ GET_LD
  IOSTREAM *s;
  int rval;

  if ( !PL_get_stream_handle(stream, &s) )
    fail;

  rval = PL_unify_integer(len, Ssize(s));
  PL_release_stream(s);

  return rval;
}


static word
pl_access_file(term_t name, term_t mode)
{ GET_LD
  char *n;
  int md;
  atom_t m;

  if ( !PL_get_atom(mode, &m) )
    return PL_error("access_file", 2, NULL, ERR_TYPE, ATOM_atom, mode);
  if ( !PL_get_file_name(name, &n, 0) )
    fail;

  if ( m == ATOM_none )
    succeed;
  
  if      ( m == ATOM_write || m == ATOM_append )
    md = ACCESS_WRITE;
  else if ( m == ATOM_read )
    md = ACCESS_READ;
  else if ( m == ATOM_execute )
    md = ACCESS_EXECUTE;
  else if ( m == ATOM_exist )
    md = ACCESS_EXIST;
  else
    return PL_error("access_file", 2, NULL, ERR_DOMAIN, ATOM_io_mode, mode);

  if ( AccessFile(n, md) )
    succeed;

  if ( md == ACCESS_WRITE && !AccessFile(n, ACCESS_EXIST) )
  { char tmp[MAXPATHLEN];
    char *dir = DirName(n, tmp);

    if ( dir[0] )
    { if ( !ExistsDirectory(dir) )
	fail;
    }
    if ( AccessFile(dir[0] ? dir : ".", md) )
      succeed;
  }

  fail;
}


static word
pl_read_link(term_t file, term_t link, term_t to)
{ char *n, *l, *t;
  char buf[MAXPATHLEN];

  if ( !PL_get_file_name(file, &n, 0) )
    fail;

  if ( (l = ReadLink(n, buf)) &&
       PL_unify_atom_chars(link, l) &&
       (t = DeRefLink(n, buf)) &&
       PL_unify_atom_chars(to, t) )
    succeed;

  fail;
}


word
pl_exists_file(term_t name)
{ char *n;

  if ( !PL_get_file_name(name, &n, 0) )
    fail;
  
  return ExistsFile(n);
}


static word
pl_exists_directory(term_t name)
{ char *n;

  if ( !PL_get_file_name(name, &n, 0) )
    fail;
  
  return ExistsDirectory(n);
}


static word
pl_tmp_file(term_t base, term_t name)
{ GET_LD
  char *n;

  if ( !PL_get_chars(base, &n, CVT_ALL) )
    return PL_error("tmp_file", 2, NULL, ERR_TYPE, ATOM_atom, base);

  return PL_unify_atom(name, TemporaryFile(n));
}


static word
pl_delete_file(term_t name)
{ char *n;

  if ( !PL_get_file_name(name, &n, 0) )
    fail;
  
  if ( RemoveFile(n) )
    succeed;

  return PL_error(NULL, 0, MSG_ERRNO, ERR_FILE_OPERATION,
		    ATOM_delete, ATOM_file, name);
}


static word
pl_delete_directory(term_t name)
{ char *n;

  if ( !PL_get_file_name(name, &n, 0) )
    fail;
  
  if ( rmdir(n) == 0 )
    succeed;
  else
    return PL_error(NULL, 0, MSG_ERRNO, ERR_FILE_OPERATION,
		    ATOM_delete, ATOM_directory, name);
}


static word
pl_make_directory(term_t name)
{ char *n;

  if ( !PL_get_file_name(name, &n, 0) )
    fail;
  
  if ( mkdir(n, 0777) == 0 )
    succeed;
  else
    return PL_error(NULL, 0, MSG_ERRNO, ERR_FILE_OPERATION,
		    ATOM_create, ATOM_directory, name);
}


static word
pl_same_file(term_t file1, term_t file2)
{ char *n1, *n2;

  if ( PL_get_file_name(file1, &n1, 0) &&
       PL_get_file_name(file2, &n2, 0) )
    return SameFile(n1, n2);

  fail;
}


static word
pl_rename_file(term_t old, term_t new)
{ GET_LD
  char *o, *n;

  if ( PL_get_file_name(old, &o, 0) &&
       PL_get_file_name(new, &n, 0) )
  { if ( SameFile(o, n) )
    { if ( fileerrors )
	return PL_error("rename_file", 2, "same file", ERR_PERMISSION,
			ATOM_rename, ATOM_file, old);
      fail;
    }

    if ( RenameFile(o, n) )
      succeed;

    if ( fileerrors )
      return PL_error("rename_file", 2, OsError(), ERR_FILE_OPERATION,
		      ATOM_rename, ATOM_file, old);
    fail;
  }

  fail;
}


static word
pl_fileerrors(term_t old, term_t new)
{ GET_LD
  return setBoolean(&fileerrors, old, new);
}


static word
pl_absolute_file_name(term_t name, term_t expanded)
{ char *n;
  char tmp[MAXPATHLEN];

  if ( PL_get_file_name(name, &n, 0) &&
       (n = AbsoluteFile(n, tmp)) )
    return PL_unify_chars(expanded, PL_ATOM|REP_FN, -1, n);

  fail;
}


static word
pl_is_absolute_file_name(term_t name)
{ char *n;

  if ( PL_get_file_name(name, &n, 0) &&
       IsAbsolutePath(n) )
    succeed;

  fail;
}


static word
pl_working_directory(term_t old, term_t new)
{ GET_LD
  const char *wd;

  if ( !(wd = PL_cwd()) )
    fail;

  if ( PL_unify_chars(old, PL_ATOM|REP_FN, -1, wd) )
  { if ( PL_compare(old, new) != 0 )
    { char *n;

      if ( PL_get_file_name(new, &n, 0) )
      { if ( ChDir(n) )
	  succeed;

	if ( fileerrors )
	  return PL_error(NULL, 0, NULL, ERR_FILE_OPERATION,
			  ATOM_chdir, ATOM_directory, new);
	fail;
      }
    }

    succeed;
  }

  fail;
}


static word
pl_file_base_name(term_t f, term_t b)
{ char *n;

  if ( !PL_get_chars_ex(f, &n, CVT_ALL|REP_FN) )
    fail;

  return PL_unify_chars(b, PL_ATOM|REP_FN, -1, BaseName(n));
}


static word
pl_file_dir_name(term_t f, term_t b)
{ char *n;
  char tmp[MAXPATHLEN];

  if ( !PL_get_chars_ex(f, &n, CVT_ALL|REP_FN) )
    fail;

  return PL_unify_chars(b, PL_ATOM|REP_FN, -1, DirName(n, tmp));
}


static int
has_extension(const char *name, const char *ext)
{ GET_LD
  const char *s = name + strlen(name);

  if ( ext[0] == EOS )
    succeed;

  while(*s != '.' && *s != '/' && s > name)
    s--;
  if ( *s == '.' && s > name && s[-1] != '/' )
  { if ( ext[0] == '.' )
      ext++;
    if ( trueFeature(FILE_CASE_FEATURE) )
      return strcmp(&s[1], ext) == 0;
    else
      return strcasecmp(&s[1], ext) == 0;
  }

  fail;
}


static int
name_too_long(void)
{ return PL_error(NULL, 0, NULL, ERR_REPRESENTATION, ATOM_max_path_length);
}


static word
pl_file_name_extension(term_t base, term_t ext, term_t full)
{ GET_LD
  char *b = NULL, *e = NULL, *f;
  char buf[MAXPATHLEN];

  if ( !PL_is_variable(full) )
  { if ( PL_get_chars(full, &f, CVT_ALL|CVT_EXCEPTION|REP_FN) )
    { char *s = f + strlen(f);		/* ?base, ?ext, +full */
  
      while(*s != '.' && *s != '/' && s > f)
	s--;
      if ( *s == '.' )
      { if ( PL_get_chars(ext, &e, CVT_ALL|REP_FN) )
	{ if ( e[0] == '.' )
	    e++;
	  if ( trueFeature(FILE_CASE_FEATURE) )
	  { TRY(strcmp(&s[1], e) == 0);
	  } else
	  { TRY(strcasecmp(&s[1], e) == 0);
	  }
	} else
	{ TRY(PL_unify_chars(ext, PL_ATOM|REP_FN, -1, &s[1]));
	}
	if ( s-f > MAXPATHLEN )
	  return name_too_long();
	strncpy(buf, f, s-f);
	buf[s-f] = EOS;
  
	return PL_unify_chars(base, PL_ATOM|REP_FN, -1, buf);
      }
      if ( PL_unify_atom_chars(ext, "") &&
	   PL_unify(full, base) )
	PL_succeed;
    }
    PL_fail;
  }

  if ( PL_get_chars_ex(base, &b, CVT_ALL|BUF_RING|REP_FN) &&
       PL_get_chars_ex(ext, &e, CVT_ALL|REP_FN) )
  { char *s;

    if ( e[0] == '.' )		/* +Base, +Extension, -full */
      e++;
    if ( has_extension(b, e) )
      return PL_unify(base, full);
    if ( strlen(b) + 1 + strlen(e) + 1 > MAXPATHLEN )
      return name_too_long();
    strcpy(buf, b);
    s = buf + strlen(buf);
    *s++ = '.';
    strcpy(s, e);

    return PL_unify_chars(full, PL_ATOM|REP_FN, -1, buf);
  } else
    fail;
}


static word
pl_prolog_to_os_filename(term_t pl, term_t os)
{ GET_LD
#ifdef O_XOS
  wchar_t *wn;

  if ( !PL_is_variable(pl) )
  { char *n;
    wchar_t buf[MAXPATHLEN];

    if ( PL_get_chars_ex(pl, &n, CVT_ALL|REP_UTF8) )
    { if ( !_xos_os_filenameW(n, buf, MAXPATHLEN) )
	return name_too_long();

      return PL_unify_wchars(os, PL_ATOM, -1, buf);
    }
    fail;
  }

  if ( PL_get_wchars(os, NULL, &wn, CVT_ALL) )
  { wchar_t lbuf[MAXPATHLEN];
    char buf[MAXPATHLEN];

    _xos_long_file_nameW(wn, lbuf, MAXPATHLEN);
    _xos_canonical_filenameW(lbuf, buf, MAXPATHLEN, 0);

    return PL_unify_chars(pl, PL_ATOM|REP_UTF8, -1, buf);
  }

  return PL_error(NULL, 0, NULL, ERR_TYPE, ATOM_atom, pl);
#else /*O_XOS*/
  return PL_unify(pl, os);
#endif /*O_XOS*/
}


static foreign_t
pl_mark_executable(term_t path)
{ char *name;

  if ( !PL_get_file_name(path, &name, 0) )
    return PL_error(NULL, 0, NULL, ERR_DOMAIN, ATOM_source_sink, path);

  return MarkExecutable(name);
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
copy_stream_data(+StreamIn, +StreamOut, [Len])
	Copy all data from StreamIn to StreamOut.  Should be somewhere else,
	and maybe we need something else to copy resources.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


static foreign_t
pl_copy_stream_data3(term_t in, term_t out, term_t len)
{ GET_LD
  IOSTREAM *i, *o;
  int c;
  int count = 0;

  if ( !getInputStream(in, &i) )
    return FALSE;
  if ( !getOutputStream(out, &o) )
  { releaseStream(i);
    return FALSE;
  }

  if ( !len )
  { while ( (c = Sgetcode(i)) != EOF )
    { if ( (++count % 4096) == 0 && PL_handle_signals() < 0 )
      { releaseStream(i);
	releaseStream(o);
	fail;
      }
      if ( Sputcode(c, o) < 0 )
      { releaseStream(i);
	return streamStatus(o);
      }
    }
  } else
  { int64_t n;

    if ( !PL_get_int64_ex(len, &n) )
      fail;
    
    while ( n-- > 0 && (c = Sgetcode(i)) != EOF )
    { if ( (++count % 4096) == 0 && PL_handle_signals() < 0 )
      { releaseStream(i);
	releaseStream(o);
	fail;
      }
      if ( Sputcode(c, o) < 0 )
      { releaseStream(i);
	return streamStatus(o);
      }
    }
  }

  releaseStream(o);
  return streamStatus(i);
}

static foreign_t
pl_copy_stream_data2(term_t in, term_t out)
{ return pl_copy_stream_data3(in, out, 0);
}


		 /*******************************
		 *      PUBLISH PREDICATES	*
		 *******************************/

BeginPredDefs(file)
  PRED_DEF("swi_set_prolog_IO", 3, set_prolog_IO, 0)
  PRED_DEF("swi_read_pending_input", 3, read_pending_input, 0)
  PRED_DEF("swi_get_code", 2, get_code2, PL_FA_ISO)
  PRED_DEF("swi_get_code", 1, get_code1, PL_FA_ISO)
  PRED_DEF("swi_get_char", 2, get_char2, PL_FA_ISO)
  PRED_DEF("swi_get_char", 1, get_char1, PL_FA_ISO)
  PRED_DEF("swi_get_byte", 2, get_byte2, PL_FA_ISO)
  PRED_DEF("swi_get_byte", 1, get_byte1, PL_FA_ISO)
  PRED_DEF("swi_peek_code", 2, peek_code2, PL_FA_ISO)
  PRED_DEF("swi_peek_code", 1, peek_code1, PL_FA_ISO)
  PRED_DEF("swi_peek_char", 2, peek_char2, PL_FA_ISO)
  PRED_DEF("swi_peek_char", 1, peek_char1, PL_FA_ISO)
  PRED_DEF("swi_peek_byte", 2, peek_byte2, PL_FA_ISO)
  PRED_DEF("swi_peek_byte", 1, peek_byte1, PL_FA_ISO)
  PRED_DEF("swi_put_byte", 2, put_byte2, PL_FA_ISO)
  PRED_DEF("swi_put_byte", 1, put_byte1, PL_FA_ISO)
  PRED_DEF("swi_put_code", 2, put_code2, PL_FA_ISO)
  PRED_DEF("swi_put_code", 1, put_code1, PL_FA_ISO)
  PRED_DEF("swi_put_char", 2, put_code2, PL_FA_ISO)
  PRED_DEF("swi_put_char", 1, put_code1, PL_FA_ISO)
  PRED_DEF("swi_put", 2, put2, 0)
  PRED_DEF("swi_put", 1, put1, 0)
  PRED_DEF("swi_skip", 1, skip1, 0)
  PRED_DEF("swi_skip", 2, skip2, 0)
  PRED_DEF("swi_get", 1, get1, 0)
  PRED_DEF("swi_get", 2, get2, 0)
  PRED_DEF("swi_get0", 2, get_code2, 0)
  PRED_DEF("swi_get0", 1, get_code1, 0)
  PRED_DEF("swi_is_stream", 1, is_stream, 0)
  PRED_DEF("swi_byte_count", 2, byte_count, 0)
  PRED_DEF("swi_character_count", 2, character_count, 0)
  PRED_DEF("swi_line_count", 2, line_count, 0)
  PRED_DEF("swi_line_position", 2, line_position, 0)
  PRED_DEF("swi_with_output_to", 2, with_output_to, PL_FA_TRANSPARENT)
EndPredDefs

static const PL_extension file_foreigns[] = {
  FRG("swi_get_single_char",	1, pl_get_single_char,		0),
  FRG("swi_$push_input_context",	0, pl_push_input_context,	0),
  FRG("swi_$pop_input_context",	0, pl_pop_input_context,	0),
  FRG("swi_seeing",			1, pl_seeing,			0),
  FRG("swi_telling",		1, pl_telling,			0),
  FRG("swi_seen",			0, pl_seen,			0),
  FRG("swi_tmp_file",		2, pl_tmp_file,			0),
  FRG("swi_delete_file",		1, pl_delete_file,		0),
  FRG("swi_delete_directory",	1, pl_delete_directory,		0),
  FRG("swi_make_directory",		1, pl_make_directory,		0),
  FRG("swi_access_file",		2, pl_access_file,		0),
  FRG("swi_read_link",		3, pl_read_link,		0),
  FRG("swi_exists_file",		1, pl_exists_file,		0),
  FRG("swi_exists_directory",	1, pl_exists_directory,		0),
  FRG("swi_rename_file",		2, pl_rename_file,		0),
  FRG("swi_same_file",		2, pl_same_file,		0),
  FRG("swi_time_file",		2, pl_time_file,		0),
  FRG("swi_told",			0, pl_told,			0),
  FRG("swi_see",			1, pl_see,			0),
  FRG("swi_tell",			1, pl_tell,			0),
  FRG("swi_append",			1, pl_append,			0),
  FRG("swi_ttyflush",		0, pl_ttyflush,			0),
  FRG("swi_flush_output",		0, pl_flush_output,		0),
  FRG("swi_prompt",			2, pl_prompt,			0),
  FRG("swi_prompt1",		1, pl_prompt1,			0),
  FRG("swi_$absolute_file_name",	2, pl_absolute_file_name,	0),
  FRG("swi_is_absolute_file_name",	1, pl_is_absolute_file_name,	0),
  FRG("swi_file_base_name",		2, pl_file_base_name,		0),
  FRG("swi_file_directory_name",	2, pl_file_dir_name,		0),
  FRG("swi_file_name_extension",	3, pl_file_name_extension,	0),
  FRG("swi_prolog_to_os_filename",	2, pl_prolog_to_os_filename,	0),
  FRG("swi_set_stream_position",	2, pl_set_stream_position,    ISO),
  FRG("swi_wait_for_input",		3, pl_wait_for_input,		0),
  FRG("swi_protocol",		1, pl_protocol,			0),
  FRG("swi_protocola",		1, pl_protocola,		0),
  FRG("swi_noprotocol",		0, pl_noprotocol,		0),
  FRG("swi_protocolling",		1, pl_protocolling,		0),
  FRG("swi_tab",			1, pl_tab,			0),
  FRG("swi_open",			3, pl_open,		      ISO),
  FRG("swi_open",			4, pl_open4,		      ISO),
  FRG("swi_open_null_stream",	1, pl_open_null_stream,		0),
  FRG("swi_close",			1, pl_close,		      ISO),
  FRG("swi_close",			2, pl_close2,		      ISO),
  FRG("swi_stream_property",	2, pl_stream_property,	 NDET|ISO),
  FRG("swi_flush_output",		1, pl_flush_output1,	      ISO),
  FRG("swi_set_stream_position",	2, pl_set_stream_position,    ISO),
  FRG("swi_seek",			4, pl_seek,			0),
  FRG("swi_set_input",		1, pl_set_input,	      ISO),
  FRG("swi_set_output",		1, pl_set_output,	      ISO),
  FRG("swi_set_stream",		2, pl_set_stream,		0),
  FRG("swi_current_input",		1, pl_current_input,	      ISO),
  FRG("swi_current_output",		1, pl_current_output,	      ISO),
  FRG("swi_source_location",	2, pl_source_location,		0),
  FRG("swi_at_end_of_stream",	1, pl_at_end_of_stream1,      ISO),
  FRG("swi_at_end_of_stream",	0, pl_at_end_of_stream0,      ISO),
  FRG("swi_size_file",		2, pl_size_file,		0),
  FRG("swi_$size_stream",		2, pl_size_stream,		0),
  FRG("swi_fileerrors",		2, pl_fileerrors,		0),
  FRG("swi_working_directory",	2, pl_working_directory,	0),
  FRG("swi_$mark_executable",	1, pl_mark_executable,		0),
  FRG("swi_copy_stream_data",	2, pl_copy_stream_data2,	0),
  FRG("swi_copy_stream_data",	3, pl_copy_stream_data3,	0)

};

static void
registerBuiltins(const PL_extension *f)
{
  PL_register_extensions( f);
}

void
initIO(void)
{ GET_LD
  const atom_t *np;
  int i;
  initCharTypes();
  init_standardStreams();
  init_encoding_names();
  init_open4_options();
  init_close2_options();
  init_sprop_list();
  streamAliases = newHTable(16);
  streamContext = newHTable(16);

  registerBuiltins(file_foreigns);
  registerBuiltins(PL_predicates_from_file);
  fileerrors = TRUE;
#ifdef __unix__
{ int fd;

  if ( (fd=Sfileno(Sinput))  < 0 || !isatty(fd) ||
       (fd=Sfileno(Soutput)) < 0 || !isatty(fd) )
    defFeature("tty_control", FT_BOOL, FALSE);
}
#endif
  ResetTty();

  Sclosehook(freeStream);

  Sinput->position  = &Sinput->posbuf;	/* position logging */
  Soutput->position = &Sinput->posbuf;
  Serror->position  = &Sinput->posbuf;

  ttymode = TTY_COOKED;
  PushTty(Sinput, &ttytab, TTY_SAVE);
  LD->prompt.current = ATOM_prompt;
  PL_register_atom(ATOM_prompt);

  Suser_input  = Sinput;
  Suser_output = Soutput;
  Suser_error  = Serror;
  Scurin       = Sinput;		/* see/tell */
  Scurout      = Soutput;
  Sprotocol    = NULL;			/* protocolling */

  getStreamContext(Sinput);		/* add for enumeration */
  getStreamContext(Soutput);
  getStreamContext(Serror);
  for( i=0, np = standardStreams; *np; np++, i++ )
    addHTable(streamAliases, (void *)*np, (void *)(intptr_t)i);

  GD->io_initialised = TRUE;
}
