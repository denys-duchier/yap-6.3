/*************************************************************************
*									 *
*	 YAP Prolog 							 *
*									 *
*	Yap Prolog was developed at NCCUP - Universidade do Porto	 *
*									 *
* Copyright L.Damas, V.S.Costa and Universidade do Porto 1985-1997	 *
*									 *
**************************************************************************
*									 *
* File:		computils.c						 *
* Last rev:								 *
* mods:									 *
* comments:	some useful routines for YAP's compiler			 *
*									 *
*************************************************************************/
#ifdef SCCS
static char SccsId[] = "%W% %G%";
#endif

/*
 * This file includes a set of utilities, useful to the several compilation
 * modules 
 */

#include "Yap.h"
#include "Yatom.h"
#include "Heap.h"
#include "compile.h"
#include "yapio.h"
#if HAVE_STRING_H
#include <string.h>
#endif

#ifdef DEBUG
STATIC_PROTO (void ShowOp, (char *));
#endif /* DEBUG */

/*
 * The compiler creates an instruction chain which will be assembled after
 * afterwards 
 */

char *freep, *freep0;

Int arg, rn;

compiler_vm_op ic;

CELL *cptr;

char *
AllocCMem (int size)
{
  char *p;
  p = freep;
#if SIZEOF_INT_P==8
  size = (size + 7) & 0xfffffffffffffff8L;
#else
  size = (size + 3) & 0xfffffffcL;
#endif
  freep += size;
  if (ASP <= CellPtr (freep) + 256) {
    save_machine_regs();
    longjmp(CompilerBotch,3);
  }
  return (p);
}

int
is_a_test_pred (Term arg, SMALLUNSGN mod)
{
  if (IsVarTerm (arg))
    return (FALSE);
  else if (IsAtomTerm (arg))
    {
      Atom At = AtomOfTerm (arg);
      PredEntry *pe = RepPredProp(PredPropByAtom(At, mod));
      if (EndOfPAEntr(pe))
	return (FALSE);
      return (pe->PredFlags & TestPredFlag);
    }
  else if (IsApplTerm (arg))
    {
      Functor f = FunctorOfTerm (arg);
      PredEntry *pe = RepPredProp(PredPropByFunc(f, mod));
      if (EndOfPAEntr(pe))
	return (FALSE);
      return (pe->PredFlags & TestPredFlag);
    }
  else
    return (FALSE);
}

void
emit (compiler_vm_op o, Int r1, CELL r2)
{
  PInstr *p;
  p = (PInstr *) AllocCMem (sizeof (*p));
  p->op = o;
  p->rnd1 = r1;
  p->rnd2 = r2;
  p->nextInst = NIL;
  if (cpc == NIL)
    cpc = CodeStart = p;
  else
    {
      cpc->nextInst = p;
      cpc = p;
    }
}

void
emit_3ops (compiler_vm_op o, CELL r1, CELL r2, CELL r3)
{
  PInstr *p;
  p = (PInstr *) AllocCMem (sizeof (*p)+sizeof(CELL));
  p->op = o;
  p->rnd1 = r1;
  p->rnd2 = r2;
  p->rnd3 = r3;
  p->nextInst = NIL;
  if (cpc == NIL)
    cpc = CodeStart = p;
  else
    {
      cpc->nextInst = p;
      cpc = p;
    }
}

CELL *
emit_extra_size (compiler_vm_op o, CELL r1, int size)
{
  PInstr *p;
  p = (PInstr *) AllocCMem (sizeof (*p) + size - CellSize);
  p->op = o;
  p->rnd1 = r1;
  p->nextInst = NIL;
  if (cpc == NIL)
    cpc = CodeStart = p;
  else
    {
      cpc->nextInst = p;
      cpc = p;
    }
  return (p->arnds);
}

void
bip_name(Int op, char *s)
{
  switch (op) {
  case _atom:
    strcpy(s,"atom");
    break;
  case _atomic:
    strcpy(s,"atomic");
    break;
  case _integer:
    strcpy(s,"integer");
    break;
  case _nonvar:
    strcpy(s,"nonvar");
    break;
  case _number:
    strcpy(s,"number");
    break;
  case _var:
    strcpy(s,"var");
    break;
  case _cut_by:
    strcpy(s,"cut_by");
    break;
  case _db_ref:
    strcpy(s,"db_ref");
    break;
  case _compound:
    strcpy(s,"compound");
    break;
  case _float:
    strcpy(s,"float");
    break;
  case _primitive:
    strcpy(s,"primitive");
    break;
  case _equal:
    strcpy(s,"equal");
    break;
  case _dif:
    strcpy(s,"dif");
    break;
  case _eq:
    strcpy(s,"eq");
    break;
  case _functor:
    strcpy(s,"functor");
    break;
  case _plus:
    strcpy(s,"plus");
    break;
  case _minus:
    strcpy(s,"minus");
    break;
  case _times:
    strcpy(s,"times");
    break;
  case _div:
    strcpy(s,"div");
    break;
  case _and:
    strcpy(s,"and");
    break;
  case _or:
    strcpy(s,"or");
    break;
  case _sll:
    strcpy(s,"sll");
    break;
  case _slr:
    strcpy(s,"slr");
    break;
  case _arg:
    strcpy(s,"arg");
    break;
  default:
    strcpy(s,"");
    break;
  }
}

#ifdef DEBUG

static void
ShowOp (f)
     char *f;
{
  char ch;
  while ((ch = *f++) != 0)
    {
      if (ch == '%')
	switch (ch = *f++)
	  {
	  case 'a':
	  case 'n':
	    plwrite ((Term) arg, DebugPutc, 0);
	    break;
	  case 'b':
	    /* write a variable bitmap for a call */
	    {
	      int max = arg/(8*sizeof(CELL)), i;
	      CELL *ptr = cptr;
	      for (i = 0; i <= max; i++) {
		plwrite(MkIntegerTerm((Int)(*ptr++)), DebugPutc, 0);
	      }
	    }
	    break;		
	  case 'l':
	    plwrite (MkIntTerm (arg), DebugPutc, 0);
	    break;
	  case 'B':
	    {
	      char s[32];

	      bip_name(rn,s);
	      plwrite (MkAtomTerm(LookupAtom(s)), DebugPutc, 0);
	    }
	    break;
	  case 'd':
	    plwrite (MkIntTerm (rn), DebugPutc, 0);
	    break;
	  case 'z':
	    plwrite (MkIntTerm (cpc->rnd3), DebugPutc, 0);
	    break;
	  case 'v':
	    {
	      Ventry *v = (Ventry *) arg;
	      DebugPutc (c_output_stream,v->KindOfVE == PermVar ? 'Y' : 'X');
	      plwrite (MkIntTerm ((v->NoOfVE) & MaskVarAdrs), DebugPutc, 0);
	    }
	    break;
	  case 'N':
	    {
	      Ventry *v;

	      cpc = cpc->nextInst;
	      arg = cpc->rnd1;
	      v = (Ventry *) arg;
	      DebugPutc (c_output_stream,v->KindOfVE == PermVar ? 'Y' : 'X');
	      plwrite (MkIntTerm ((v->NoOfVE) & MaskVarAdrs), DebugPutc, 0);
	    }
	    break;
	  case 'm':
	    plwrite (MkAtomTerm ((Atom) arg), DebugPutc, 0);
	    DebugPutc (c_output_stream,'/');
	    plwrite (MkIntTerm (rn), DebugPutc, 0);
	    break;
	  case 'p':
	    {
	      PredEntry *p = RepPredProp ((Prop) arg);
	      Functor f = p->FunctorOfPred;
	      UInt arity = p->ArityOfPE;
	      SMALLUNSGN mod = 0;

	      if (p->ModuleOfPred) mod = IntOfTerm(p->ModuleOfPred);
	      plwrite (ModuleName[mod], DebugPutc, 0);
	      DebugPutc (c_output_stream,':');
	      if (arity == 0)
		plwrite (MkAtomTerm ((Atom)f), DebugPutc, 0);
	      else
		plwrite (MkAtomTerm (NameOfFunctor (f)), DebugPutc, 0);
	      DebugPutc (c_output_stream,'/');
	      plwrite (MkIntTerm (arity), DebugPutc, 0);
	    }
	    break;
	  case 'P':
	    {
	      PredEntry *p = RepPredProp((Prop) rn);
	      Functor f = p->FunctorOfPred;
	      UInt arity = p->ArityOfPE;
	      SMALLUNSGN mod = 0;

	      if (p->ModuleOfPred) mod = IntOfTerm(p->ModuleOfPred);
	      plwrite (ModuleName[mod], DebugPutc, 0);
	      DebugPutc (c_output_stream,':');
	      if (arity == 0)
		plwrite (MkAtomTerm ((Atom)f), DebugPutc, 0);
	      else
		plwrite (MkAtomTerm (NameOfFunctor (f)), DebugPutc, 0);
	      DebugPutc (c_output_stream,'/');
	      plwrite (MkIntTerm (arity), DebugPutc, 0);
	    }
	    break;
	  case 'f':
	    if (IsExtensionFunctor((Functor)arg)) {
	      if ((Functor)arg == FunctorDBRef) {
		plwrite(MkAtomTerm(LookupAtom("DBRef")), DebugPutc, 0);
	      } else if ((Functor)arg == FunctorLongInt) {
		plwrite(MkAtomTerm(LookupAtom("LongInt")), DebugPutc, 0);
	      } else if ((Functor)arg == FunctorDouble) {
		plwrite(MkAtomTerm(LookupAtom("Double")), DebugPutc, 0);
	      }
	    } else {
	      plwrite(MkAtomTerm(NameOfFunctor ((Functor) arg)), DebugPutc, 0);
	      DebugPutc (c_output_stream,'/');
	      plwrite(MkIntTerm(ArityOfFunctor ((Functor) arg)), DebugPutc, 0);
	    }
	    break;
	  case 'r':
	    DebugPutc (c_output_stream,'A');
	    plwrite (MkIntTerm (rn), DebugPutc, 0);
	    break;
	  case 'h':
	    {
	      CELL my_arg = *cptr++;
	      if (my_arg & 1)
		plwrite (MkIntTerm (my_arg),
			 DebugPutc, 0);
	      else if (my_arg == (CELL) FAILCODE)
		plwrite (MkAtomTerm (AtomFail), DebugPutc, 0);
	      else
		plwrite (MkIntegerTerm ((Int) my_arg),
			 DebugPutc, 0);
	    }
	    break;
	  case 'g':
	    if (arg & 1)
	      plwrite (MkIntTerm (arg),
		       DebugPutc, 0);
	    else if (arg == (CELL) FAILCODE)
	      plwrite (MkAtomTerm (AtomFail), DebugPutc, 0);
	    else
	      plwrite (MkIntegerTerm ((Int) arg), DebugPutc, 0);
	    break;
	  case 'i':
	    plwrite (MkIntTerm (arg), DebugPutc, 0);
	    break;
	  case 'j':
	    {
	      Functor fun = (Functor)*cptr++;
	      if (IsExtensionFunctor(fun)) {
		if (fun == FunctorDBRef) {
		  plwrite(MkAtomTerm(LookupAtom("DBRef")), DebugPutc, 0);
		} else if (fun == FunctorLongInt) {
		  plwrite(MkAtomTerm(LookupAtom("LongInt")), DebugPutc, 0);
		} else if (fun == FunctorDouble) {
		  plwrite(MkAtomTerm(LookupAtom("Double")), DebugPutc, 0);
		}
	      } else {
		plwrite (MkAtomTerm(NameOfFunctor(fun)), DebugPutc, 0);
		DebugPutc (c_output_stream,'/');
		plwrite (MkIntTerm(ArityOfFunctor(fun)), DebugPutc, 0);
	      }
	    }
	    break;
	  case 'O':
	    plwrite(AbsAppl(cptr), DebugPutc, 0);
	    break;
	  case 'x':
	    plwrite (MkIntTerm (rn >> 1), DebugPutc, 0);
	    DebugPutc (c_output_stream,'\t');
	    plwrite (MkIntTerm (rn & 1), DebugPutc, 0);
	    break;
	  case 'o':
	    plwrite ((Term) * cptr++, DebugPutc, 0);
	  case 'c':
	    {
	      int i;
	      for (i = 0; i < arg; ++i)
		{
		  CELL my_arg;
		  if (*cptr)
		    {
		      plwrite ((Term) * cptr++, DebugPutc, 0);
		    }
		  else
		    {
		      plwrite (MkIntTerm (0), DebugPutc, 0);
		      cptr++;
		    }
		  DebugPutc (c_output_stream,'\t');
		  my_arg = *cptr++;
		  if (my_arg & 1)
		    plwrite (MkIntTerm (my_arg),
			     DebugPutc, 0);
		  else if (my_arg == (CELL) FAILCODE)
		    plwrite (MkAtomTerm (AtomFail), DebugPutc, 0);
		  else
		    plwrite (MkIntegerTerm ((Int) my_arg), DebugPutc, 0);
		  DebugPutc (c_output_stream,'\n');
		}
	    }
	    break;
	  case 'e':
	    {
	      int i;
	      for (i = 0; i < arg; ++i)
		{
		  CELL my_arg;
		  if (*cptr)
		    {
		      plwrite (MkAtomTerm (NameOfFunctor ((Functor) * cptr)), DebugPutc, 0);
		      DebugPutc (c_output_stream,'/');
		      plwrite (MkIntTerm (ArityOfFunctor ((Functor) * cptr++)), DebugPutc, 0);
		    }
		  else
		    {
		      plwrite (MkIntTerm (0), DebugPutc, 0);
		      cptr++;
		    }
		  DebugPutc (c_output_stream,'\t');
		  my_arg = *cptr++;
		  if (my_arg & 1)
		    plwrite (MkIntTerm (my_arg),
			     DebugPutc, 0);
		  else if (my_arg == (CELL) FAILCODE)
		    plwrite (MkAtomTerm (AtomFail), DebugPutc, 0);
		  else
		    plwrite (MkIntegerTerm ((Int) my_arg), DebugPutc, 0);
		  DebugPutc (c_output_stream,'\n');
		}
	    }
	    break;
	  default:
	    DebugPutc (c_output_stream,'%');
	    DebugPutc (c_output_stream,ch);
	  }
      else
	DebugPutc (c_output_stream,ch);
    }
  DebugPutc (c_output_stream,'\n');
}

static char *opformat[] =
{
  "nop",
  "get_var\t\t%v,%r",
  "put_var\t\t%v,%r",
  "get_val\t\t%v,%r",
  "put_val\t\t%v,%r",
  "get_atom\t%a,%r",
  "put_atom\t%a,%r",
  "get_num\t\t%n,%r",
  "put_num\t\t%n,%r",
  "get_float\t\t%l,%r",
  "put_float\t\t%l,%r",
  "get_longint\t\t%l,%r",
  "put_longint\t\t%l,%r",
  "get_bigint\t\t%l,%r",
  "put_bigint\t\t%l,%r",
  "get_list\t%r",
  "put_list\t%r",
  "get_struct\t%f,%r",
  "put_struct\t%f,%r",
  "put_unsafe\t%v,%r",
  "unify_var\t%v",
  "write_var\t%v",
  "unify_val\t%v",
  "write_val\t%v",
  "unify_atom\t%a",
  "write_atom\t%a",
  "unify_num\t%n",
  "write_num\t%n",
  "unify_float\t%l",
  "write_float\t%l",
  "unify_longint\t%l",
  "write_longint\t%l",
  "unify_bigint\t%l",
  "write_bigint\t%l",
  "unify_list",
  "write_list",
  "unify_struct\t%f",
  "write_struct\t%f",
  "write_unsafe\t%v",
  "fail",
  "cut",
  "cutexit",
  "allocate",
  "deallocate",
  "try_me_else\t\t%l\t%x",
  "jump\t\t%l",
  "procceed",
  "call\t\t%p,%d,%z",
  "execute\t\t%p",
  "sys\t\t%p",
  "%l:",
  "name\t\t%m,%d",
  "pop\t\t%l",
  "retry_me_else\t\t%l\t%x",
  "trust_me_else_fail\t%x",
  "either_me\t\t%l,%d,%z",
  "or_else\t\t%l,%z",
  "or_last",
  "push_or",
  "pushpop_or",
  "pop_or",
  "save_by\t\t%v",
  "comit_by\t\t%v",
  "patch_by\t\t%v",
  "try\t\t%g\t%x",
  "retry\t\t%g\t%x",
  "trust\t\t%g\t%x",
  "try_in\t\t%g\t%x",
  "retry_in\t\t%g\t%x",
  "trust_in\t\t%g\t%x",
  "try_first\t\t%g\t%x",
  "retry_first\t\t%g\t%x",
  "trust_first\t\t%g\t%x",
  "try_first in\t\t%g\t%x",
  "retry_first in\t\t%g\t%x",
  "trust_first in\t\t%g\t%x",
  "try_tail\t\t%g\t%x",
  "retry_tail\t\t%g\t%x",
  "trust_tail\t\t%g\t%x",
  "try_tail_in\t\t%g\t%x",
  "retry_tail_in\t\t%g\t%x",
  "trust_tail_in\t\t%g\t%x",
  "try_head\t\t%g\t%x",
  "retry_head\t\t%g\t%x",
  "trust_head\t\t%g\t%x",
  "try_head_in\t\t%g\t%x",
  "retry_head_in\t\t%g\t%x",
  "trust_head_in\t\t%g\t%x",
  "try_last_first\t\t%g\t%x",
  "try_last_head\t\t%g\t%x",
  "jump_if_var\t\t%g",
  "switch_on_type\t%h\t%h\t%h\t%h",
  "switch_on_type_if_nonvar\t%h\t%h\t%h",
  "switch_on_type_of_last\t%h\t%h\t%h",
  "switch_on_type_of_head\t%h\t%h\t%h\t%h",
  "switch_on_list_or_nil\t%h\t%h\t%h\t%h",
  "switch_if_list_or_nil\t%h\t%h\t%h",
  "switch_on_last_list_or_nil\t%h\t%h\t%h",
  "switch_on_constant\t%i\n%c",
  "if_a_constant\t%i\t%h\n%c",
  "go_if_ equals_constant\t%o\t%h\t%h",
  "switch_on_functor\t%i\n%e",
  "if_a_functor\t%i\t%h\n%e",
  "go_if_equals_functor\t%j\t%h\t%h",
  "if_not_then\t%i\t%h\t%h\t%h",
  "save_pair\t%v",
  "save_appl\t%v",
  "fail_label\t%l",
  "unify_local\t%v",
  "write local\t%v",
  "unify_last_list",
  "write_last_list",
  "unify_last_struct\t%f",
  "write_last_struct\t%f",
  "unify_last_var\t%v",
  "unify_last_val\t%v",
  "unify_last_local\t%v",
  "unify_last_atom\t%a",
  "unify_last_num\t%n",
  "unify_last_float\t%l",
  "unify_last_longint\t%l",
  "unify_last_bigint\t%l",
  "pvar_bitmap\t%l,%b",
  "pvar_live_regs\t%l,%b",
  "fetch_reg1_reg2\t%N,%N",
  "fetch_constant_reg\t%l,%N",
  "fetch_reg_constant\t%l,%N",
  "function_to_var\t%v,%B",
  "function_to_al\t%v,%B",
  "enter_profiling\t\t%g",
  "retry_profiled\t\t%g",
  "restore_temps\t\t%l",
  "restore_temps_and_skip\t\t%l",
  "empty_call\t\t%l,%d",
#ifdef TABLING
  "table_new_answer",
#endif /* TABLING */
#ifdef YAPOR
  "sync",
#endif /* YAPOR */
  "fetch_args_for_bccall\t%v",
  "binary_cfunc\t\t%v,%P",
  "blob\t%O"
#ifdef SFUNC
  ,
  "get_s_f_op\t%f,%r",
  "put_s_f_op\t%f,%r",
  "unify_s_f_op\t%f",
  "write_s_f_op\t%f",
  "unify_s_var\t%v,%r",
  "write_s_var\t%v,%r",
  "unify_s_val\t%v,%r",
  "write_s_val\t%v,%r",
  "unify_s_a\t%a,%r",
  "write_s_a\t%a,%r",
  "get_s_end",
  "put_s_end",
  "unify_s_end",
  "write_s_end"
#endif
};


void
ShowCode ()
{
  CELL *OldH = H;

  cpc = CodeStart;
  /* MkIntTerm and friends may build terms in the global stack */
  H = (CELL *)freep;
  while (cpc)
    {
      ic = cpc->op;
      arg = cpc->rnd1;
      rn = cpc->rnd2;
      cptr = cpc->arnds;
      if (ic != nop_op)
	ShowOp (opformat[ic]);
      cpc = cpc->nextInst;
    }
  DebugPutc (c_output_stream,'\n');
  H = OldH;
}

#endif /* DEBUG */

