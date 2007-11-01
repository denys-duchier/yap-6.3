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
* File:		cdmgr.c							 *
* comments:	Code manager						 *
*									 *
* Last rev:     $Date: 2007-11-01 10:01:35 $,$Author: vsc $						 *
* $Log: not supported by cvs2svn $
* Revision 1.207  2007/10/29 22:48:54  vsc
* small fixes
*
* Revision 1.206  2007/04/10 22:13:20  vsc
* fix max modules limitation
*
* Revision 1.205  2007/03/26 15:18:43  vsc
* debugging and clause/3 over tabled predicates would kill YAP.
*
* Revision 1.204  2007/01/25 22:11:55  vsc
* all/3 should fail on no solutions.
* get rid of annoying gcc complaints.
*
* Revision 1.203  2007/01/24 10:01:38  vsc
* fix matrix mess
*
* Revision 1.202  2006/12/27 01:32:37  vsc
* diverse fixes
*
* Revision 1.201  2006/12/13 16:10:14  vsc
* several debugger and CLP(BN) improvements.
*
* Revision 1.200  2006/11/27 17:42:02  vsc
* support for UNICODE, and other bug fixes.
*
* Revision 1.199  2006/11/15 00:13:36  vsc
* fixes for indexing code.
*
* Revision 1.198  2006/11/14 11:42:25  vsc
* fix bug in growstack
*
* Revision 1.197  2006/11/06 18:35:03  vsc
* 1estranha
*
* Revision 1.196  2006/10/16 17:12:48  vsc
* fixes for threaded version.
*
* Revision 1.195  2006/10/11 17:24:36  vsc
* make sure we only follow pointers *before* we removed the respective code block,
* ie don't kill the child before checking pointers from parent!
*
* Revision 1.194  2006/10/11 15:08:03  vsc
* fix bb entries
* comment development code for timestamp overflow.
*
* Revision 1.193  2006/10/11 14:53:57  vsc
* fix memory leak
* fix overflow handling
* VS: ----------------------------------------------------------------------
*
* Revision 1.192  2006/10/10 14:08:16  vsc
* small fixes on threaded implementation.
*
* Revision 1.191  2006/09/20 20:03:51  vsc
* improve indexing on floats
* fix sending large lists to DB
*
* Revision 1.190  2006/08/07 18:51:44  vsc
* fix garbage collector not to try to garbage collect when we ask for large
* chunks of stack in a single go.
*
* Revision 1.189  2006/05/24 02:35:39  vsc
* make chr work and other minor fixes.
*
* Revision 1.188  2006/05/18 16:33:04  vsc
* fix info reported by memory manager under DL_MALLOC and SYSTEM_MALLOC
*
* Revision 1.187  2006/04/29 01:15:18  vsc
* fix expand_consult patch
*
* Revision 1.186  2006/04/28 17:53:44  vsc
* fix the expand_consult patch
*
* Revision 1.185  2006/04/28 13:23:22  vsc
* fix number of overflow bugs affecting threaded version
* make current_op faster.
*
* Revision 1.184  2006/04/27 14:11:57  rslopes
* *** empty log message ***
*
* Revision 1.183  2006/03/29 16:00:10  vsc
* make tabling compile
*
* Revision 1.182  2006/03/24 16:26:26  vsc
* code review
*
* Revision 1.181  2006/03/22 20:07:28  vsc
* take better care of zombies
*
* Revision 1.180  2006/03/22 16:14:20  vsc
* don't be too eager at throwing indexing code for static predicates away.
*
* Revision 1.179  2006/03/21 17:11:39  vsc
* prevent breakage
*
* Revision 1.178  2006/03/20 19:51:43  vsc
* fix indexing and tabling bugs
*
* Revision 1.177  2006/03/06 14:04:56  vsc
* fixes to garbage collector
* fixes to debugger
*
* Revision 1.176  2006/02/01 13:28:56  vsc
* bignum support fixes
*
* Revision 1.175  2006/01/08 03:12:00  vsc
* fix small bug in attvar handling.
*
* Revision 1.174  2005/12/23 00:20:13  vsc
* updates to gprof
* support for __POWER__
* Try to saveregs before longjmp.
*
* Revision 1.173  2005/12/17 03:25:39  vsc
* major changes to support online event-based profiling
* improve error discovery and restart on scanner.
*
* Revision 1.172  2005/11/23 03:01:33  vsc
* fix several bugs in save/restore.b
*
* Revision 1.171  2005/10/29 01:28:37  vsc
* make undefined more ISO compatible.
*
* Revision 1.170  2005/10/18 17:04:43  vsc
* 5.1:
* - improvements to GC
*    2 generations
*    generic speedups
* - new scheme for attvars
*    - hProlog like interface also supported
* - SWI compatibility layer
*    - extra predicates
*    - global variables
*    - moved to Prolog module
* - CLP(R) by Leslie De Koninck, Tom Schrijvers, Cristian Holzbaur, Bart
* Demoen and Jan Wielemacker
* - load_files/2
*
* from 5.0.1
*
* - WIN32 missing include files (untested)
* - -L trouble (my thanks to Takeyuchi Shiramoto-san)!
* - debugging of backtrable user-C preds would core dump.
* - redeclaring a C-predicate as Prolog core dumps.
* - badly protected  YapInterface.h.
* - break/0 was failing at exit.
* - YAP_cut_fail and YAP_cut_succeed were different from manual.
* - tracing through data-bases could core dump.
* - cut could break on very large computations.
* - first pass at BigNum issues (reported by Roberto).
* - debugger could get go awol after fail port.
* - weird message on wrong debugger option.
*
* Revision 1.169  2005/10/15 02:05:57  vsc
* fix for trying to add clauses to a C pred.
*
* Revision 1.168  2005/08/05 14:55:02  vsc
* first steps to allow mavars with tabling
* fix trailing for tabling with multiple get_cons
*
* Revision 1.167  2005/08/02 03:09:49  vsc
* fix debugger to do well nonsource predicates.
*
* Revision 1.166  2005/08/01 15:40:37  ricroc
* TABLING NEW: better support for incomplete tabling
*
* Revision 1.165  2005/07/06 19:33:52  ricroc
* TABLING: answers for completed calls can now be obtained by loading (new option) or executing (default) them from the trie data structure.
*
* Revision 1.164  2005/07/06 15:10:03  vsc
* improvements to compiler: merged instructions and fixes for ->
*
* Revision 1.163  2005/06/08 00:35:27  vsc
* fix silly calls such as 0.15 ( bug reported by Jude Shavlik)
*
* Revision 1.162  2005/06/04 07:27:33  ricroc
* long int support for tabling
*
* Revision 1.161  2005/06/03 08:26:32  ricroc
* float support for tabling
*
* Revision 1.160  2005/06/01 14:02:47  vsc
* get_rid of try_me?, retry_me? and trust_me? instructions: they are not
* significantly used nowadays.
*
* Revision 1.159  2005/05/31 19:42:27  vsc
* insert some more slack for indices in LU
* Use doubly linked list for LU indices so that updating is less cumbersome.
*
* Revision 1.158  2005/05/31 00:30:23  ricroc
* remove abort_yapor function
*
* Revision 1.157  2005/05/12 03:36:32  vsc
* debugger was making predicates meta instead of testing
* fix handling of dbrefs in facts and in subarguments.
*
* Revision 1.156  2005/04/20 04:02:15  vsc
* fix a few variable warnings
* fix erase clause to pass a pointer to clause, not code
* get rid of Yap4.4 code in Yap_EraseStaticClause
*
* Revision 1.155  2005/04/10 04:01:10  vsc
* bug fixes, I hope!
*
* Revision 1.154  2005/03/04 20:30:11  ricroc
* bug fixes for YapTab support
*
* Revision 1.153  2005/02/25 03:39:44  vsc
* fix fixes to undefp
* fix bug where clause mistook cp for ap
*
* Revision 1.152  2005/02/08 18:04:57  vsc
* library_directory may not be deterministic (usually it isn't).
*
* Revision 1.151  2005/02/08 04:05:23  vsc
* fix mess with add clause
* improves on sigsegv handling
*
* Revision 1.150  2005/01/28 23:14:34  vsc
* move to Yap-4.5.7
* Fix clause size
*
* Revision 1.149  2005/01/05 05:35:01  vsc
* get rid of debugging stub.
*
* Revision 1.148  2005/01/04 02:50:21  vsc
* - allow MegaClauses with blobs
* - change Diffs to be thread specific
* - include Christian's updates
*
* Revision 1.147  2004/12/28 22:20:35  vsc
* some extra bug fixes for trail overflows: some cannot be recovered that easily,
* some can.
*
* Revision 1.146  2004/12/20 21:44:57  vsc
* more fixes to CLPBN
* fix some Yap overflows.
*
* Revision 1.145  2004/12/16 05:57:23  vsc
* fix overflows
*
* Revision 1.144  2004/12/08 00:10:48  vsc
* more grow fixes
*
* Revision 1.143  2004/12/05 05:01:23  vsc
* try to reduce overheads when running with goal expansion enabled.
* CLPBN fixes
* Handle overflows when allocating big clauses properly.
*
* Revision 1.142  2004/11/18 22:32:31  vsc
* fix situation where we might assume nonextsing double initialisation of C predicates (use
* Hidden Pred Flag).
* $host_type was double initialised.
*
* Revision 1.141  2004/11/04 18:22:31  vsc
* don't ever use memory that has been freed (that was done by LU).
* generic fixes for WIN32 libraries
*
* Revision 1.140  2004/10/31 02:18:03  vsc
* fix bug in handling Yap heap overflow while adding new clause.
*
* Revision 1.139  2004/10/28 20:12:21  vsc
* Use Doug Lea's malloc as an alternative to YAP's standard malloc
* don't use TR directly in scanner/parser, this avoids trouble with ^C while
* consulting large files.
* pass gcc -mno-cygwin to library compilation in cygwin environment (cygwin should
* compile out of the box now).
*
* Revision 1.138  2004/10/26 20:15:51  vsc
* More bug fixes for overflow handling
*
* Revision 1.137  2004/10/22 16:53:19  vsc
* bug fixes
*
* Revision 1.136  2004/10/06 16:55:46  vsc
* change configure to support big mem configs
* get rid of extra globals
* fix trouble with multifile preds
*
* Revision 1.135  2004/09/30 21:37:40  vsc
* fixes for thread support
*
* Revision 1.134  2004/09/30 19:51:53  vsc
* fix overflow from within clause/2
*
* Revision 1.133  2004/09/27 20:45:02  vsc
* Mega clauses
* Fixes to sizeof(expand_clauses) which was being overestimated
* Fixes to profiling+indexing
* Fixes to reallocation of memory after restoring
* Make sure all clauses, even for C, end in _Ystop
* Don't reuse space for Streams
* Fix Stream_F on StreaNo+1
*
* Revision 1.132  2004/09/17 19:34:51  vsc
* simplify frozen/2
*
* Revision 1.131  2004/09/08 17:56:45  vsc
* source: a(X) :- true is a fact!
* fix use of value after possible overflow in IPred
*
* Revision 1.130  2004/09/07 16:48:04  vsc
* fix bug in unwinding trail at amiops.h
*
* Revision 1.129  2004/09/07 16:25:22  vsc
* memory management bug fixes
*
* Revision 1.128  2004/09/03 03:11:07  vsc
* memory management fixes
*
* Revision 1.127  2004/08/16 21:02:04  vsc
* more fixes for !
*
* Revision 1.126  2004/07/22 21:32:20  vsc
* debugger fixes
* initial support for JPL
* bad calls to garbage collector and gc
* debugger fixes
*
* Revision 1.125  2004/06/29 19:04:41  vsc
* fix multithreaded version
* include new version of Ricardo's profiler
* new predicat atomic_concat
* allow multithreaded-debugging
* small fixes
*
* Revision 1.124  2004/06/05 03:36:59  vsc
* coroutining is now a part of attvars.
* some more fixes.
*
* Revision 1.123  2004/05/17 21:42:09  vsc
* misc fixes
*
* Revision 1.122  2004/05/13 21:36:45  vsc
* get rid of pesky debugging prints
*
* Revision 1.121  2004/05/13 20:54:57  vsc
* debugger fixes
* make sure we always go back to current module, even during initizlization.
*
* Revision 1.120  2004/04/27 16:21:16  vsc
* stupid bug
*
* Revision 1.119  2004/04/27 15:03:43  vsc
* more fixes for expand_clauses
*
* Revision 1.118  2004/04/14 19:10:23  vsc
* expand_clauses: keep a list of clauses to expand
* fix new trail scheme for multi-assignment variables
*
* Revision 1.117  2004/04/07 22:04:03  vsc
* fix memory leaks
*
* Revision 1.116  2004/03/31 01:03:09  vsc
* support expand group of clauses
*
* Revision 1.115  2004/03/19 11:35:42  vsc
* trim_trail for default machine
* be more aggressive about try-retry-trust chains.
*    - handle cases where block starts with a wait
*    - don't use _killed instructions, just let the thing rot by itself.
*
*									 *
*************************************************************************/
#ifdef SCCS
static char     SccsId[] = "@(#)cdmgr.c	1.1 05/02/98";
#endif

#include "Yap.h"
#include "clause.h"
#include "yapio.h"
#include "eval.h"
#include "tracer.h"
#ifdef YAPOR
#include "or.macros.h"
#endif	/* YAPOR */
#ifdef TABLING
#include "tab.macros.h"
#endif /* TABLING */
#ifdef YAPOR
#include "or.macros.h"
#endif	/* YAPOR */
#if HAVE_STRING_H
#include <string.h>
#endif


STATIC_PROTO(void retract_all, (PredEntry *, int));
STATIC_PROTO(void add_first_static, (PredEntry *, yamop *, int));
STATIC_PROTO(void add_first_dynamic, (PredEntry *, yamop *, int));
STATIC_PROTO(void asserta_stat_clause, (PredEntry *, yamop *, int));
STATIC_PROTO(void asserta_dynam_clause, (PredEntry *, yamop *));
STATIC_PROTO(void assertz_stat_clause, (PredEntry *, yamop *, int));
STATIC_PROTO(void assertz_dynam_clause, (PredEntry *, yamop *));
STATIC_PROTO(void expand_consult, (void));
STATIC_PROTO(int  not_was_reconsulted, (PredEntry *, Term, int));
STATIC_PROTO(int  RemoveIndexation, (PredEntry *));
#if EMACS
STATIC_PROTO(int  last_clause_number, (PredEntry *));
#endif
STATIC_PROTO(int  static_in_use, (PredEntry *, int));
#if !defined(YAPOR) && !defined(THREADS)
STATIC_PROTO(Int  search_for_static_predicate_in_use, (PredEntry *, int));
STATIC_PROTO(void mark_pred, (int, PredEntry *));
STATIC_PROTO(void do_toggle_static_predicates_in_use, (int));
#endif
STATIC_PROTO(Int  p_number_of_clauses, (void));
STATIC_PROTO(Int  p_compile, (void));
STATIC_PROTO(Int  p_compile_dynamic, (void));
STATIC_PROTO(Int  p_purge_clauses, (void));
STATIC_PROTO(Int  p_setspy, (void));
STATIC_PROTO(Int  p_rmspy, (void));
STATIC_PROTO(Int  p_startconsult, (void));
STATIC_PROTO(Int  p_showconslultlev, (void));
STATIC_PROTO(Int  p_endconsult, (void));
STATIC_PROTO(Int  p_undefined, (void));
STATIC_PROTO(Int  p_in_use, (void));
STATIC_PROTO(Int  p_new_multifile, (void));
STATIC_PROTO(Int  p_is_multifile, (void));
STATIC_PROTO(Int  p_optimizer_on, (void));
STATIC_PROTO(Int  p_optimizer_off, (void));
STATIC_PROTO(Int  p_in_this_f_before, (void));
STATIC_PROTO(Int  p_first_cl_in_f, (void));
STATIC_PROTO(Int  p_mk_cl_not_first, (void));
STATIC_PROTO(Int  p_is_dynamic, (void));
STATIC_PROTO(Int  p_kill_dynamic, (void));
STATIC_PROTO(Int  p_compile_mode, (void));
STATIC_PROTO(Int  p_is_profiled, (void));
STATIC_PROTO(Int  p_profile_info, (void));
STATIC_PROTO(Int  p_profile_reset, (void));
STATIC_PROTO(Int  p_is_call_counted, (void));
STATIC_PROTO(Int  p_call_count_info, (void));
STATIC_PROTO(Int  p_call_count_set, (void));
STATIC_PROTO(Int  p_call_count_reset, (void));
STATIC_PROTO(Int  p_toggle_static_predicates_in_use, (void));
STATIC_PROTO(Atom  YapConsultingFile, (void));
STATIC_PROTO(Int  PredForCode,(yamop *, Atom *, UInt *, Term *));
STATIC_PROTO(void  kill_first_log_iblock,(LogUpdIndex *, LogUpdIndex *, PredEntry *));
STATIC_PROTO(LogUpdIndex *find_owner_log_index,(LogUpdIndex *, yamop *));
STATIC_PROTO(StaticIndex *find_owner_static_index,(StaticIndex *, yamop *));

#define PredArity(p) (p->ArityOfPE)
#define TRYCODE(G,F,N) ( (N)<5 ? (op_numbers)((int)F+(N)*3) : G)
#define NEXTOP(V,TYPE)    ((yamop *)(&((V)->u.TYPE.next)))

#define IN_BLOCK(P,B,SZ)     ((CODEADDR)(P) >= (CODEADDR)(B) && \
			      (CODEADDR)(P) < (CODEADDR)(B)+(SZ))

static PredEntry *
PredForChoicePt(yamop *p_code) {
  while (TRUE) {
    op_numbers opnum = Yap_op_from_opcode(p_code->opc);
    switch(opnum) {
    case _Nstop:
      return NULL;
    case _retry_me:
    case _trust_me:
      return p_code->u.ld.p;
    case _try_logical:
    case _retry_logical:
    case _trust_logical:
    case _count_retry_logical:
    case _count_trust_logical:
    case _profiled_retry_logical:
    case _profiled_trust_logical:
      return p_code->u.lld.d->ClPred;
#ifdef TABLING
    case _trie_retry_null:
    case _trie_trust_null:
    case _trie_retry_var:
    case _trie_trust_var:
    case _trie_retry_val:
    case _trie_trust_val:
    case _trie_retry_atom:
    case _trie_trust_atom:
    case _trie_retry_list:
    case _trie_trust_list:
    case _trie_retry_struct:
    case _trie_trust_struct:
    case _trie_retry_extension:
    case _trie_trust_extension:
    case _trie_retry_float:
    case _trie_trust_float:
    case _trie_retry_long:
    case _trie_trust_long:
      return NULL;
    case _table_load_answer:
    case _table_try_answer:
    case _table_answer_resolution:
    case _table_completion:
      return NULL; /* ricroc: is this OK? */
      /* compile error --> return ENV_ToP(gc_B->cp_cp); */
#endif /* TABLING */
    case _or_else:
      if (p_code == p_code->u.sla.sla_u.l) {
	/* repeat */
	Atom at = Yap_LookupAtom("repeat ");
	return RepPredProp(PredPropByAtom(at, PROLOG_MODULE));
      } else {
	return p_code->u.sla.p0;
      }
      break;
    case _or_last:
#ifdef YAPOR
      return p_code->u.sla.p0;
#else
      return p_code->u.p.p;
#endif /* YAPOR */
      break;
    case _count_retry_me:
    case _retry_profiled:
    case _retry2:
    case _retry3:
    case _retry4:
      p_code = NEXTOP(p_code,l);
      break;
    default:
      return p_code->u.ld.p;
    }
  }
  return NULL;
}

PredEntry *
Yap_PredForChoicePt(choiceptr cp) {
  if (cp == NULL)
    return NULL;
  return PredForChoicePt(cp->cp_ap);
}

/******************************************************************
  
			EXECUTING PROLOG CLAUSES
  
******************************************************************/


static int 
static_in_use(PredEntry *p, int check_everything)
{
#if defined(YAPOR) || defined(THREADS)
  return TRUE;
#else
  CELL pflags = p->PredFlags;
  if (pflags & (DynamicPredFlag|LogUpdatePredFlag)) {
    return (FALSE);
  }
  if (STATIC_PREDICATES_MARKED) {
    return (p->PredFlags & InUsePredFlag);
  } else {
    /* This code does not work for YAPOR or THREADS!!!!!!!! */
    return(search_for_static_predicate_in_use(p, check_everything));
  }
#endif
}

/******************************************************************
  
		ADDING AND REMOVE INFO TO A PROCEDURE
  
******************************************************************/


/*
 * we have three kinds of predicates: dynamic		DynamicPredFlag
 * static 		CompiledPredFlag fast		FastPredFlag all the
 * database predicates are supported for dynamic predicates only abolish and
 * assertz are supported for static predicates no database predicates are
 * supportted for fast predicates 
 */

#define is_dynamic(pe)  (pe->PredFlags & DynamicPredFlag)
#define is_static(pe) 	(pe->PredFlags & CompiledPredFlag)
#define is_logupd(pe)	(pe->PredFlags & LogUpdatePredFlag)
#ifdef TABLING
#define is_tabled(pe)   (pe->PredFlags & TabledPredFlag)
#endif /* TABLING */


/******************************************************************

		Mega Clauses
  
******************************************************************/


#define PtoPredAdjust(X) (X)
#define PtoOpAdjust(X) (X)
#define PtoLUClauseAdjust(P) (P)
#define PtoLUIndexAdjust(P) (P)
#define XAdjust(X) (X)
#define YAdjust(X) (X)
#define AtomTermAdjust(X) (X)
#define CellPtoHeapAdjust(X) (X)
#define FuncAdjust(X) (X)
#define CodeAddrAdjust(X) (X)
#define rehash(A,B,C)
static Term BlobTermAdjust(Term t)
{
#if TAGS_FAST_OPS
  return t-ClDiff;
#else
  return t+ClDiff;
#endif
}

#include "rclause.h"

#ifdef DEBUG
static UInt  total_megaclause, total_released, nof_megaclauses;
#endif

void
Yap_BuildMegaClause(PredEntry *ap)
{
  StaticClause *cl;
  UInt sz;
  MegaClause *mcl;
  yamop *ptr;
  UInt required;
  UInt has_blobs = 0;

  if (ap->PredFlags & (DynamicPredFlag|LogUpdatePredFlag|MegaClausePredFlag
#ifdef TABLING
		       |TabledPredFlag
#endif /* TABLING */
		       ) ||
      ap->cs.p_code.FirstClause == NULL ||
      ap->cs.p_code.NOfClauses < 16) {
    return;
  }
  cl =
    ClauseCodeToStaticClause(ap->cs.p_code.FirstClause);
  sz = cl->ClSize;
  while (TRUE) {
    if (!(cl->ClFlags & FactMask)) return; /* no mega clause, sorry */
    if (cl->ClSize != sz) return; /* no mega clause, sorry */
    if (cl->ClCode == ap->cs.p_code.LastClause)
      break;
    has_blobs |= (cl->ClFlags & HasBlobsMask);
    cl = cl->ClNext;
  }
  /* ok, we got the chance for a mega clause */
  if (has_blobs) {
    sz -= sizeof(StaticClause);
  } else {
    sz -= (UInt)NEXTOP((yamop *)NULL,p) + sizeof(StaticClause);
  }
  required = sz*ap->cs.p_code.NOfClauses+sizeof(MegaClause)+(UInt)NEXTOP((yamop *)NULL,l);
#ifdef DEBUG
  total_megaclause += required;
  total_released += ap->cs.p_code.NOfClauses*(sz+sizeof(StaticClause));
  nof_megaclauses++;
#endif
  while (!(mcl = (MegaClause *)Yap_AllocCodeSpace(required))) {
    if (!Yap_growheap(FALSE, required, NULL)) {
      /* just fail, the system will keep on going */
      return;
    }
  }
  Yap_ClauseSpace += required;
  /* cool, it's our turn to do the conversion */
  mcl->ClFlags = MegaMask | has_blobs;
  mcl->ClSize = sz*ap->cs.p_code.NOfClauses;
  mcl->ClPred = ap;
  mcl->ClItemSize = sz;
  mcl->ClNext = NULL;
  cl =
    ClauseCodeToStaticClause(ap->cs.p_code.FirstClause);
  ptr = mcl->ClCode;
  while (TRUE) {
    memcpy((void *)ptr, (void *)cl->ClCode, sz);
    if (has_blobs) {
      ClDiff = (char *)(ptr)-(char *)cl->ClCode;
      restore_opcodes(ptr);
    }
    ptr = (yamop *)((char *)ptr + sz);
    if (cl->ClCode == ap->cs.p_code.LastClause)
      break;
    cl = cl->ClNext;
  }
  ptr->opc = Yap_opcode(_Ystop);
  cl =
    ClauseCodeToStaticClause(ap->cs.p_code.FirstClause);
  /* recover the space spent on the original clauses */
  while (TRUE) {
    StaticClause *ncl, *curcl = cl;

    ncl = cl->ClNext;
    Yap_InformOfRemoval((CODEADDR)cl);
    Yap_ClauseSpace -= cl->ClSize;
    Yap_FreeCodeSpace((ADDR)cl);
    if (curcl->ClCode == ap->cs.p_code.LastClause)
      break;
    cl = ncl;
  }
  ap->cs.p_code.FirstClause =
    ap->cs.p_code.LastClause =
    mcl->ClCode;
  ap->PredFlags |= MegaClausePredFlag;
}


static void
split_megaclause(PredEntry *ap)
{
  StaticClause *start = NULL, *prev = NULL;
  MegaClause *mcl;
  yamop *ptr;
  UInt ncls = ap->cs.p_code.NOfClauses, i;

  WRITE_LOCK(ap->PRWLock);
  RemoveIndexation(ap);
  mcl =
    ClauseCodeToMegaClause(ap->cs.p_code.FirstClause);
  for (i = 0, ptr = mcl->ClCode; i < ncls; i++) {
    StaticClause *new = (StaticClause *)Yap_AllocCodeSpace(sizeof(StaticClause)+mcl->ClItemSize);
    if (new == NULL) {
      if (!Yap_growheap(FALSE, (sizeof(StaticClause)+mcl->ClItemSize)*(ncls-i), NULL)) {
	while (start) {
	  StaticClause *cl = start;
	  start = cl->ClNext;
	  Yap_InformOfRemoval((CODEADDR)cl);
	  Yap_ClauseSpace -= cl->ClSize;
	  Yap_FreeCodeSpace((char *)cl);
	}
	if (ap->ArityOfPE) {
	  Yap_Error(OUT_OF_HEAP_ERROR,TermNil,"while breaking up mega clause for %s/%d\n",RepAtom(NameOfFunctor(ap->FunctorOfPred))->StrOfAE,ap->ArityOfPE);
	} else {
	  Yap_Error(OUT_OF_HEAP_ERROR,TermNil,"while breaking up mega clause for %s\n", RepAtom((Atom)ap->FunctorOfPred)->StrOfAE);
	}
	WRITE_UNLOCK(ap->PRWLock);
	return;
      }
    }
    Yap_ClauseSpace += sizeof(StaticClause)+mcl->ClItemSize;
    new->ClFlags = FactMask;
    new->ClSize = mcl->ClItemSize;
    new->usc.ClPred = ap;
    new->ClNext = NULL;
    memcpy((void *)new->ClCode, (void *)ptr, mcl->ClItemSize);
    if (prev) {
      prev->ClNext = new;
    } else {
      start = new;
    }
    ptr = (yamop *)((char *)ptr + mcl->ClItemSize);
    prev = new;
  }
  ap->PredFlags &= ~MegaClausePredFlag;
  ap->cs.p_code.FirstClause = start->ClCode;
  ap->cs.p_code.LastClause = prev->ClCode;
  WRITE_UNLOCK(ap->PRWLock);
}



/******************************************************************
  
		Indexation Info
  
******************************************************************/
#define ByteAdr(X)   ((Int) &(X))

/* Index a prolog pred, given its predicate entry */
/* ap is already locked. */
static void 
IPred(PredEntry *ap, UInt NSlots)
{
  yamop          *BaseAddr;

#ifdef DEBUG
  if (Yap_Option['i' - 'a' + 1]) {
    Term tmod = ap->ModuleOfPred;
    if (!tmod)
      tmod = TermProlog;
    Yap_DebugPutc(Yap_c_error_stream,'\t');
    Yap_plwrite(tmod, Yap_DebugPutc, 0);
    Yap_DebugPutc(Yap_c_error_stream,':');
    if (ap->ModuleOfPred == IDB_MODULE) {
      Term t = Deref(ARG1);
      if (IsAtomTerm(t)) {
	Yap_plwrite(t, Yap_DebugPutc, 0);
      } else if (IsIntegerTerm(t)) {
	Yap_plwrite(t, Yap_DebugPutc, 0);
      } else {
	Functor f = FunctorOfTerm(t);
	Atom At = NameOfFunctor(f);
	Yap_plwrite(MkAtomTerm(At), Yap_DebugPutc, 0);
	Yap_DebugPutc(Yap_c_error_stream,'/');
	Yap_plwrite(MkIntTerm(ArityOfFunctor(f)), Yap_DebugPutc, 0);
      }
    } else {
      if (ap->ArityOfPE == 0) {
	Atom At = (Atom)ap->FunctorOfPred;
	Yap_plwrite(MkAtomTerm(At), Yap_DebugPutc, 0);
      } else {
	Functor f = ap->FunctorOfPred;
	Atom At = NameOfFunctor(f);
	Yap_plwrite(MkAtomTerm(At), Yap_DebugPutc, 0);
	Yap_DebugPutc(Yap_c_error_stream,'/');
	Yap_plwrite(MkIntTerm(ArityOfFunctor(f)), Yap_DebugPutc, 0);
      }
    }
    Yap_DebugPutc(Yap_c_error_stream,'\n');
  }
#endif
  /* Do not try to index a dynamic predicate  or one whithout args */
  if (is_dynamic(ap)) {
    Yap_Error(SYSTEM_ERROR,TermNil,"trying to index a dynamic predicate");
    return;
  }
  if ((BaseAddr = Yap_PredIsIndexable(ap, NSlots)) != NULL) {
    ap->cs.p_code.TrueCodeOfPred = BaseAddr;
    ap->PredFlags |= IndexedPredFlag;
  }
  if (ap->PredFlags & SpiedPredFlag) {
    ap->OpcodeOfPred = Yap_opcode(_spy_pred);
    ap->CodeOfPred = (yamop *)(&(ap->OpcodeOfPred)); 
  } else {
    ap->CodeOfPred = ap->cs.p_code.TrueCodeOfPred;
    ap->OpcodeOfPred = ((yamop *)(ap->CodeOfPred))->opc;
  }
#ifdef DEBUG
  if (Yap_Option['i' - 'a' + 1])
    Yap_DebugPutc(Yap_c_error_stream,'\n');
#endif
}

void 
Yap_IPred(PredEntry *p, UInt NSlots)
{
  IPred(p, NSlots);
}

#define GONEXT(TYPE)      code_p = ((yamop *)(&(code_p->u.TYPE.next)))

static void
RemoveMainIndex(PredEntry *ap)
{
  yamop *First = ap->cs.p_code.FirstClause;
  int spied = ap->PredFlags & SpiedPredFlag;

  ap->PredFlags &= ~IndexedPredFlag;
  if (First == NULL) {
    ap->cs.p_code.TrueCodeOfPred = FAILCODE;
  } else {
    ap->cs.p_code.TrueCodeOfPred = First;
  }
  if (First != NULL && spied) {
    ap->OpcodeOfPred = Yap_opcode(_spy_pred);
    ap->cs.p_code.TrueCodeOfPred = ap->CodeOfPred = (yamop *)(&(ap->OpcodeOfPred)); 
  } else if (ap->cs.p_code.NOfClauses > 1
#ifdef TABLING
	     ||ap->PredFlags & TabledPredFlag
#endif /* TABLING */
	     ) {
    ap->OpcodeOfPred = INDEX_OPCODE;
    ap->CodeOfPred = ap->cs.p_code.TrueCodeOfPred = (yamop *)(&(ap->OpcodeOfPred)); 
  } else {
    ap->OpcodeOfPred = ap->cs.p_code.TrueCodeOfPred->opc;
    ap->CodeOfPred = ap->cs.p_code.TrueCodeOfPred;
  }
}

static void
decrease_ref_counter(yamop *ptr, yamop *b, yamop *e, yamop *sc)
{
  if (ptr != FAILCODE && ptr != sc && (ptr < b || ptr > e)) {
    LogUpdClause *cl = ClauseCodeToLogUpdClause(ptr);
    LOCK(cl->ClLock);
    cl->ClRefCount--;
    if (cl->ClFlags & ErasedMask &&
	!(cl->ClRefCount) &&
	!(cl->ClFlags & InUseMask)) {
      /* last ref to the clause */
      UNLOCK(cl->ClLock);
      Yap_ErLogUpdCl(cl);
    } else {
      UNLOCK(cl->ClLock);
    }
  }
}

static void
release_wcls(yamop *cop, OPCODE ecs)
{
  if (cop->opc == ecs) {
    cop->u.sp.s3--;
    if (!cop->u.sp.s3) {
    LOCK(ExpandClausesListLock);
    if (ExpandClausesFirst == cop)
      ExpandClausesFirst = cop->u.sp.snext;
    if (ExpandClausesLast == cop) {
      ExpandClausesLast = cop->u.sp.sprev;
    }
    if (cop->u.sp.sprev) {
      cop->u.sp.sprev->u.sp.snext = cop->u.sp.snext;
    }
    if (cop->u.sp.snext) {
      cop->u.sp.snext->u.sp.sprev = cop->u.sp.sprev;
    }
    UNLOCK(ExpandClausesListLock);
#if DEBUG
      Yap_expand_clauses_sz -= (UInt)(NEXTOP((yamop *)NULL,sp)+cop->u.sp.s1*sizeof(yamop *));
#endif
      Yap_InformOfRemoval((CODEADDR)cop);
      if (cop->u.sp.p->PredFlags & LogUpdatePredFlag) {
	Yap_LUIndexSpace_EXT -= (UInt)NEXTOP((yamop *)NULL,sp)+cop->u.sp.s1*sizeof(yamop *);
      } else {
	Yap_IndexSpace_EXT -= (UInt)NEXTOP((yamop *)NULL,sp)+cop->u.sp.s1*sizeof(yamop *);
      }
      Yap_FreeCodeSpace((char *)cop);
    }
  }
}


static void
cleanup_dangling_indices(yamop *ipc, yamop *beg, yamop *end, yamop *suspend_code)
{
  OPCODE ecs = Yap_opcode(_expand_clauses);

  while (ipc) {
    op_numbers op = Yap_op_from_opcode(ipc->opc);
    /*    fprintf(stderr,"op: %d %p->%p\n", op, ipc, end);*/
    switch(op) {
    case _Ystop:
      /* end of clause, for now */
      return;
    case _index_dbref:
    case _index_blob:
      ipc = NEXTOP(ipc,e);
      break;
    case _lock_lu:
      /* just skip for now, but should worry about locking */
      ipc = NEXTOP(ipc,p);
      break;
    case _unlock_lu:
      /* just skip for now, but should worry about locking */
      ipc = NEXTOP(ipc,e);
      break;
    case _retry_profiled:
    case _count_retry:
      ipc = NEXTOP(ipc,p);
      break;
    case _try_clause2:
    case _try_clause3:
    case _try_clause4:
      ipc = NEXTOP(ipc,l);
      break;
    case _retry2:
    case _retry3:
    case _retry4:
      decrease_ref_counter(ipc->u.l.l, beg, end, suspend_code);
      ipc = NEXTOP(ipc,l);
      break;
    case _retry:
    case _trust:
      decrease_ref_counter(ipc->u.ld.d, beg, end, suspend_code);
      ipc = NEXTOP(ipc,ld);
      break;
    case _try_clause:
    case _try_me:
    case _retry_me:
    case _profiled_trust_me:
    case _trust_me:
    case _count_trust_me:
      ipc = NEXTOP(ipc,ld);
      break;
    case _try_logical:
    case _retry_logical:
    case _count_retry_logical:
    case _profiled_retry_logical:
      {
	yamop *oipc = ipc;
	decrease_ref_counter(ipc->u.lld.d->ClCode, beg, end, suspend_code);
	ipc = ipc->u.lld.n;
	Yap_LUIndexSpace_CP -= (UInt)NEXTOP((yamop *)NULL,lld);
	Yap_FreeCodeSpace((ADDR)oipc);
#ifdef DEBUG
	Yap_DirtyCps--;
	Yap_FreedCps++;
#endif
      }
      end = ipc;
      break;
    case _trust_logical:
    case _count_trust_logical:
    case _profiled_trust_logical:
#ifdef DEBUG
      Yap_DirtyCps--;
      Yap_FreedCps++;
#endif
      decrease_ref_counter(ipc->u.lld.d->ClCode, beg, end, suspend_code);
      Yap_LUIndexSpace_CP -= (UInt)NEXTOP((yamop *)NULL,lld);
      Yap_FreeCodeSpace((ADDR)ipc);
      end = ipc;
      return;
    case _enter_lu_pred:
      if (ipc->u.Ill.I->ClFlags & InUseMask || ipc->u.Ill.I->ClRefCount)
	return;
#ifdef DEBUG
      Yap_DirtyCps+=ipc->u.Ill.s;
      Yap_LiveCps-=ipc->u.Ill.s;
#endif
      ipc = ipc->u.Ill.l1;
      end = ipc;
      break;
    case _try_in:
    case _jump:
    case _jump_if_var:
      release_wcls(ipc->u.l.l, ecs);
      ipc = NEXTOP(ipc,l);
      break;
      /* instructions type xl */
    case _jump_if_nonvar:
      release_wcls(ipc->u.xll.l1, ecs);
      ipc = NEXTOP(ipc,xll);
      break;
      /* instructions type e */
    case _switch_on_type:
      release_wcls(ipc->u.llll.l1, ecs);
      release_wcls(ipc->u.llll.l2, ecs);
      release_wcls(ipc->u.llll.l3, ecs);
      release_wcls(ipc->u.llll.l4, ecs);
      ipc = NEXTOP(ipc,llll);
      break;
    case _switch_list_nl:
      release_wcls(ipc->u.ollll.l1, ecs);
      release_wcls(ipc->u.ollll.l2, ecs);
      release_wcls(ipc->u.ollll.l3, ecs);
      release_wcls(ipc->u.ollll.l4, ecs);
      ipc = NEXTOP(ipc,ollll);
      break;
    case _switch_on_arg_type:
      release_wcls(ipc->u.xllll.l1, ecs);
      release_wcls(ipc->u.xllll.l2, ecs);
      release_wcls(ipc->u.xllll.l3, ecs);
      release_wcls(ipc->u.xllll.l4, ecs);
      ipc = NEXTOP(ipc,xllll);
      break;
    case _switch_on_sub_arg_type:
      release_wcls(ipc->u.sllll.l1, ecs);
      release_wcls(ipc->u.sllll.l2, ecs);
      release_wcls(ipc->u.sllll.l3, ecs);
      release_wcls(ipc->u.sllll.l4, ecs);
      ipc = NEXTOP(ipc,sllll);
      break;
    case _if_not_then:
      ipc = NEXTOP(ipc,clll);
      break;
    case _switch_on_func:
    case _if_func:
    case _go_on_func:
    case _switch_on_cons:
    case _if_cons:
    case _go_on_cons:
      ipc = NEXTOP(ipc,sssl);
      break;
    default:
      Yap_Error(SYSTEM_ERROR,TermNil,"Bug in Indexing Code: opcode %d", op);
      return;
    }
#if defined(YAPOR) || defined(THREADS)
    ipc = (yamop *)((CELL)ipc & ~1);
#endif    
  }
}

void
Yap_cleanup_dangling_indices(yamop *ipc, yamop *beg, yamop *end, yamop *sc)
{
  cleanup_dangling_indices(ipc, beg, end, sc);
}

static void
decrease_log_indices(LogUpdIndex *c, yamop *suspend_code)
{
  /* decrease all reference counters */
  yamop *beg = c->ClCode, *end, *ipc;
  op_numbers op;
  if (c->ClFlags & SwitchTableMask) {
    CELL *end = (CELL *)((char *)c+c->ClSize);
    CELL *beg = (CELL *)(c->ClCode);
    OPCODE ecs = Yap_opcode(_expand_clauses);

    while (beg < end) {
      yamop *cop;
      cop = (yamop *)beg[1];
      beg += 2;
      release_wcls(cop, ecs);
    }
    return;
  }
  op = Yap_op_from_opcode(beg->opc);
  end = (yamop *)((CODEADDR)c+c->ClSize);
  ipc = beg;
  cleanup_dangling_indices(ipc, beg, end, suspend_code);
}

static void
kill_static_child_indxs(StaticIndex *indx, int in_use)
{
  StaticIndex *cl = indx->ChildIndex;
  while (cl != NULL) {
    StaticIndex *next = cl->SiblingIndex;
    kill_static_child_indxs(cl, in_use);
    cl = next;
  }
  if (in_use) {
    LOCK(DeadStaticIndicesLock);
    indx->SiblingIndex = DeadStaticIndices;
    indx->ChildIndex = NULL;
    DeadStaticIndices = indx;
    UNLOCK(DeadStaticIndicesLock);
  } else {
    Yap_InformOfRemoval((CODEADDR)indx);
    if (indx->ClFlags & SwitchTableMask)
      Yap_IndexSpace_SW -= indx->ClSize;
    else
      Yap_IndexSpace_Tree -= indx->ClSize;
    Yap_FreeCodeSpace((char *)indx);
  }
}

static void
kill_children(LogUpdIndex *c, PredEntry *ap)
{
  LogUpdIndex *ncl;

  LOCK(c->ClLock);
  c->ClRefCount++;
  ncl = c->ChildIndex;
  /* kill children */
  while (ncl) {
    UNLOCK(c->ClLock);
    kill_first_log_iblock(ncl, c, ap);
    LOCK(c->ClLock);
    ncl = c->ChildIndex;
  }
  c->ClRefCount--;
  UNLOCK(c->ClLock);
}

/* assumes c is already locked */
static void
kill_off_lu_block(LogUpdIndex *c, LogUpdIndex *parent, PredEntry *ap)
{
  /* first, make sure that I killed off all my children, some children may
     remain in case I have tables as children */
  if (parent != NULL) {
    /* sat bye bye */
    /* decrease refs */
    LOCK(parent->ClLock);
    parent->ClRefCount--;
    if (parent->ClFlags & ErasedMask &&
	!(parent->ClFlags & InUseMask) &&
	parent->ClRefCount == 0) {
      /* cool, I can erase the father too. */
      if (parent->ClFlags & SwitchRootMask) {
	kill_off_lu_block(parent, NULL, ap);
      } else {
	kill_off_lu_block(parent, parent->ParentIndex, ap);
      }
    } else {
      UNLOCK(parent->ClLock);
    }
  }
  UNLOCK(c->ClLock);
  {
    LogUpdIndex *parent = DBErasedIList, *c0 = NULL;
    while (parent != NULL) {
      if (c == parent) {
	if (c0) c0->SiblingIndex = c->SiblingIndex;
	else DBErasedIList = c->SiblingIndex;
	break;
      }
      c0 = parent;
      parent = parent->SiblingIndex;
    }
  }
  Yap_InformOfRemoval((CODEADDR)c);
  if (c->ClFlags & SwitchTableMask)
    Yap_LUIndexSpace_SW -= c->ClSize;
  else {
    Yap_LUIndexSpace_Tree -= c->ClSize;
  }
  Yap_FreeCodeSpace((char *)c);
}

static void
kill_first_log_iblock(LogUpdIndex *c, LogUpdIndex *parent, PredEntry *ap)
{
  decrease_log_indices(c, (yamop *)&(ap->cs.p_code.ExpandCode));
  /* parent is always locked, now I lock myself */
  if (parent != NULL) {
    /* remove myself from parent */
    LOCK(parent->ClLock);
    if (c == parent->ChildIndex) {
      parent->ChildIndex = c->SiblingIndex;
      if (parent->ChildIndex) {
	parent->ChildIndex->PrevSiblingIndex = NULL;
      }
    } else {
      c->PrevSiblingIndex->SiblingIndex =
	c->SiblingIndex;
      if (c->SiblingIndex) {
	c->SiblingIndex->PrevSiblingIndex =
	  c->PrevSiblingIndex;
      }
    }
    UNLOCK(parent->ClLock);
  } else {
    /* I am  top node */
    if (ap->cs.p_code.TrueCodeOfPred == c->ClCode) {
      RemoveMainIndex(ap);
    }
  }
  /* make sure that a child cannot remove us */
  kill_children(c, ap);
  /* check if we are still the main index */
  LOCK(c->ClLock);
  if (!((c->ClFlags & InUseMask) || c->ClRefCount)) {
    kill_off_lu_block(c, parent, ap);
  } else {
    if (c->ClFlags & ErasedMask)
      return;
    c->ClFlags |= ErasedMask;
    /* try to move up, so that we don't hold a switch table */
    if (parent != NULL &&
	parent->ClFlags & SwitchTableMask) {
    
      LOCK(parent->ClLock);
      c->ParentIndex = parent->ParentIndex;
      LOCK(parent->ParentIndex->ClLock);
      parent->ParentIndex->ClRefCount++;
      UNLOCK(parent->ParentIndex->ClLock);
      parent->ClRefCount--;
      UNLOCK(parent->ClLock);
    }
    c->SiblingIndex = DBErasedIList;
    DBErasedIList = c;
    UNLOCK(c->ClLock);
  }
}

static void
kill_top_static_iblock(StaticIndex *c, PredEntry *ap)
{
  kill_static_child_indxs(c, static_in_use(ap, TRUE));
  RemoveMainIndex(ap);
}

void
Yap_kill_iblock(ClauseUnion *blk, ClauseUnion *parent_blk, PredEntry *ap)
{
  if (ap->PredFlags & LogUpdatePredFlag) {
    LogUpdIndex *c = (LogUpdIndex *)blk;
    if (parent_blk != NULL) {
      LogUpdIndex *cl = (LogUpdIndex *)parent_blk;
#if defined(THREADS) || defined(YAPOR)
      LOCK(cl->ClLock);
      /* protect against attempts at erasing */
      cl->ClRefCount++;
      UNLOCK(cl->ClLock);
#endif
      kill_first_log_iblock(c, cl, ap);
#if defined(THREADS) || defined(YAPOR)
      LOCK(cl->ClLock);
      cl->ClRefCount--;
      UNLOCK(cl->ClLock);
#endif
    } else {
      kill_first_log_iblock(c, NULL, ap);
    }
  } else {
    StaticIndex *c = (StaticIndex *)blk;
    if (parent_blk != NULL) {
      StaticIndex *cl = parent_blk->si.ChildIndex;
      if (cl == c) {
	parent_blk->si.ChildIndex = c->SiblingIndex;
      } else {
	while (cl->SiblingIndex != c) {
	  cl = cl->SiblingIndex;
	}
	cl->SiblingIndex = c->SiblingIndex;
      }
    }
    kill_static_child_indxs(c, static_in_use(ap, TRUE));
  }
}

/*
  This predicate is supposed to be called with a
  lock on the current predicate
*/
void
Yap_ErLogUpdIndex(LogUpdIndex *clau)
{
  if (clau->ClFlags & ErasedMask) {
    if (!clau->ClRefCount) {
      decrease_log_indices(clau, (yamop *)&(clau->ClPred->cs.p_code.ExpandCode));
      if (clau->ClFlags & SwitchRootMask) {
	kill_off_lu_block(clau, NULL, clau->ClPred);
      } else {
	kill_off_lu_block(clau, clau->ParentIndex, clau->ClPred);
      }
    }
    /* otherwise, nothing I can do, I have been erased already */
    return;
  }
  if (clau->ClFlags & SwitchRootMask) {
    kill_first_log_iblock(clau, NULL, clau->ClPred);
  } else {
#if defined(THREADS) || defined(YAPOR)
    LOCK(clau->ParentIndex->ClLock);
    /* protect against attempts at erasing */
    clau->ClRefCount++;
    UNLOCK(clau->ParentIndex->ClLock);
#endif
    kill_first_log_iblock(clau, clau->ParentIndex, clau->ClPred);
#if defined(THREADS) || defined(YAPOR)
    LOCK(clau->ParentIndex->ClLock);
    /* protect against attempts at erasing */
    clau->ClRefCount--;
    UNLOCK(clau->ParentIndex->ClLock);
#endif
  }
}

/* Routine used when wanting to remove the indexation */
/* ap is known to already have been locked for WRITING */
static int 
RemoveIndexation(PredEntry *ap)
{ 
  if (ap->OpcodeOfPred == INDEX_OPCODE) {
    return TRUE;
  }
  if (ap->PredFlags & LogUpdatePredFlag) {
    kill_first_log_iblock(ClauseCodeToLogUpdIndex(ap->cs.p_code.TrueCodeOfPred), NULL, ap);
  } else {
    StaticIndex *cl;

    cl = ClauseCodeToStaticIndex(ap->cs.p_code.TrueCodeOfPred);

    kill_top_static_iblock(cl, ap);    
    
  }
  return (TRUE);
}

int 
Yap_RemoveIndexation(PredEntry *ap)
{
  return RemoveIndexation(ap);
}
/******************************************************************
  
			Adding clauses
  
******************************************************************/


#define	assertz	0
#define	consult	1
#define	asserta	2

/* p is already locked */
static void 
retract_all(PredEntry *p, int in_use)
{
  yamop          *q;

  q = p->cs.p_code.FirstClause;
  if (q != NULL) {
    if (p->PredFlags & LogUpdatePredFlag) { 
      LogUpdClause *cl = ClauseCodeToLogUpdClause(q);
      do {
	LogUpdClause *ncl = cl->ClNext;
	Yap_ErLogUpdCl(cl);
	cl = ncl;
      } while (cl != NULL);
    } else if (p->PredFlags & MegaClausePredFlag) { 
      MegaClause *cl = ClauseCodeToMegaClause(q);

      if (in_use || cl->ClFlags & HasBlobsMask) {
	LOCK(DeadMegaClausesLock);
	cl->ClNext = DeadMegaClauses;
	DeadMegaClauses = cl;
	UNLOCK(DeadMegaClausesLock);
      } else {
	Yap_InformOfRemoval((CODEADDR)cl);
	Yap_ClauseSpace -= cl->ClSize;
	Yap_FreeCodeSpace((char *)cl);
      }
      /* make sure this is not a MegaClause */
      p->PredFlags &= ~MegaClausePredFlag;
      p->cs.p_code.NOfClauses = 0;
    } else {
      StaticClause   *cl = ClauseCodeToStaticClause(q);

      while (cl) {
	StaticClause *ncl = cl->ClNext;

	if (in_use|| cl->ClFlags & HasBlobsMask) {
	  LOCK(DeadStaticClausesLock);
	  cl->ClNext = DeadStaticClauses;
	  DeadStaticClauses = cl;
	  UNLOCK(DeadStaticClausesLock);
	} else {
	  Yap_InformOfRemoval((CODEADDR)cl);
	  Yap_ClauseSpace -= cl->ClSize;
	  Yap_FreeCodeSpace((char *)cl);
	}
	p->cs.p_code.NOfClauses--;
	if (!ncl) break;
	cl = ncl;
      }
    }
  }
  p->cs.p_code.FirstClause = NULL;
  p->cs.p_code.LastClause = NULL;
  if (p->PredFlags & (DynamicPredFlag|LogUpdatePredFlag)) {
    p->OpcodeOfPred = FAIL_OPCODE;
  } else {
    p->OpcodeOfPred = UNDEF_OPCODE;
  }
  p->cs.p_code.TrueCodeOfPred = p->CodeOfPred = (yamop *)(&(p->OpcodeOfPred));
  p->StatisticsForPred.NOfEntries = 0;
  p->StatisticsForPred.NOfHeadSuccesses = 0;
  p->StatisticsForPred.NOfRetries = 0;
  if (PROFILING) {
    p->PredFlags |= ProfiledPredFlag;
  } else
    p->PredFlags &= ~ProfiledPredFlag;
#ifdef YAPOR
  if (SEQUENTIAL_IS_DEFAULT) {
    p->PredFlags |= SequentialPredFlag;
  }
#endif /* YAPOR */
  Yap_PutValue(AtomAbol, MkAtomTerm(AtomTrue));
}

/* p is already locked */
static void 
add_first_static(PredEntry *p, yamop *cp, int spy_flag)
{
  yamop *pt = cp;

  if (is_logupd(p)) {
    if (p == PredGoalExpansion) {
      PRED_GOAL_EXPANSION_ON = TRUE;
      Yap_InitComma();
    }
  } else {
#ifdef YAPOR
    if (SEQUENTIAL_IS_DEFAULT) {
      p->PredFlags |= SequentialPredFlag;
      PUT_YAMOP_SEQ(pt);
    }
    if (YAMOP_LTT(pt) != 1)
      Yap_Error(INTERNAL_ERROR, TermNil, "YAMOP_LTT error (add_first_static)");
#endif /* YAPOR */
#ifdef TABLING
    if (is_tabled(p)) {
      p->OpcodeOfPred = INDEX_OPCODE;
      p->cs.p_code.TrueCodeOfPred = p->CodeOfPred = (yamop *)(&(p->OpcodeOfPred)); 
    }
#endif /* TABLING */
  }
  p->cs.p_code.TrueCodeOfPred = pt;
  p->cs.p_code.FirstClause = p->cs.p_code.LastClause = cp;
  p->cs.p_code.NOfClauses = 1;
  p->StatisticsForPred.NOfEntries = 0;
  p->StatisticsForPred.NOfHeadSuccesses = 0;
  p->StatisticsForPred.NOfRetries = 0;
  if (PROFILING) {
    p->PredFlags |= ProfiledPredFlag;
  } else
    p->PredFlags &= ~ProfiledPredFlag;
#ifdef YAPOR
  p->PredFlags |= SequentialPredFlag;
  PUT_YAMOP_SEQ((yamop *)cp);
#endif /* YAPOR */
  if (spy_flag) {
    p->OpcodeOfPred = Yap_opcode(_spy_pred);
    p->CodeOfPred = (yamop *)(&(p->OpcodeOfPred)); 
  }
  if ((yap_flags[SOURCE_MODE_FLAG] ||
      (p->PredFlags & MultiFileFlag)) &&
      !(p->PredFlags & (DynamicPredFlag|LogUpdatePredFlag))) {
    p->PredFlags |= SourcePredFlag;
  } else {
    p->PredFlags &= ~SourcePredFlag;
  }
}

/* p is already locked */
static void 
add_first_dynamic(PredEntry *p, yamop *cp, int spy_flag)
{
  yamop    *ncp = ((DynamicClause *)NULL)->ClCode;
  DynamicClause   *cl;
  if (p == PredGoalExpansion) {
    PRED_GOAL_EXPANSION_ON = TRUE;
    Yap_InitComma();
  }
  p->StatisticsForPred.NOfEntries = 0;
  p->StatisticsForPred.NOfHeadSuccesses = 0;
  p->StatisticsForPred.NOfRetries = 0;
  if (PROFILING) {
    p->PredFlags |= ProfiledPredFlag;
  } else
    p->PredFlags &= ~ProfiledPredFlag;
#ifdef YAPOR
  p->PredFlags |= SequentialPredFlag;
#endif /* YAPOR */
  /* allocate starter block, containing info needed to start execution,
   * that is a try_mark to start the code and a fail to finish things up */
  cl =
    (DynamicClause *) Yap_AllocCodeSpace((Int)NEXTOP(NEXTOP(NEXTOP(ncp,ld),e),l));
  if (cl == NIL) {
    Yap_Error(OUT_OF_HEAP_ERROR,TermNil,"Heap crashed against Stacks");
    return;
  }
  Yap_ClauseSpace += (Int)NEXTOP(NEXTOP(NEXTOP(ncp,ld),e),l);
  /* skip the first entry, this contains the back link and will always be
     empty for this entry */
  ncp = (yamop *)(((CELL *)ncp)+1);
  /* next we have the flags. For this block mainly say whether we are
   *  being spied */
  cl->ClFlags = DynamicMask;
  ncp = cl->ClCode;
  INIT_LOCK(cl->ClLock);
  INIT_CLREF_COUNT(cl);
  /* next, set the first instruction to execute in the dyamic
   *  predicate */
  if (spy_flag)
    p->OpcodeOfPred = ncp->opc = Yap_opcode(_spy_or_trymark);
  else
    p->OpcodeOfPred = ncp->opc = Yap_opcode(_try_and_mark);
  ncp->u.ld.s = p->ArityOfPE;
  ncp->u.ld.p = p;
  ncp->u.ld.d = cp;
#ifdef YAPOR
  INIT_YAMOP_LTT(ncp, 1);
  PUT_YAMOP_SEQ(ncp);
#endif /* YAPOR */
  /* This is the point we enter the code */
  p->cs.p_code.TrueCodeOfPred = p->CodeOfPred = ncp;
  p->cs.p_code.NOfClauses = 1;
  /* set the first clause to have a retry and mark which will
   *  backtrack to the previous block */
  if (p->PredFlags & ProfiledPredFlag)
    cp->opc = Yap_opcode(_profiled_retry_and_mark);
  else if (p->PredFlags & CountPredFlag)
    cp->opc = Yap_opcode(_count_retry_and_mark);
  else
    cp->opc = Yap_opcode(_retry_and_mark);
  cp->u.ld.s = p->ArityOfPE;
  cp->u.ld.p = p;
  cp->u.ld.d = ncp;
  /* also, keep a backpointer for the days you delete the clause */
  ClauseCodeToDynamicClause(cp)->ClPrevious = ncp;
  /* Don't forget to say who is the only clause for the predicate so
     far */
  p->cs.p_code.LastClause = p->cs.p_code.FirstClause = cp;
  /* we're only missing what to do when we actually exit the procedure
   */
  ncp = NEXTOP(ncp,ld);
  /* and the last instruction to execute to exit the predicate, note
     the retry is pointing to this pseudo clause */
  ncp->opc = Yap_opcode(_trust_fail);
  /* we're only missing what to do when we actually exit the procedure
   */
  /* and close the code */
  ncp = NEXTOP(ncp,e);
  ncp->opc = Yap_opcode(_Ystop);
  ncp->u.l.l = cl->ClCode;
}

/* p is already locked */
static void 
asserta_stat_clause(PredEntry *p, yamop *q, int spy_flag)
{
  StaticClause *cl = ClauseCodeToStaticClause(q);

  p->cs.p_code.NOfClauses++;
  if (is_logupd(p)) {
    LogUpdClause
      *clp = ClauseCodeToLogUpdClause(p->cs.p_code.FirstClause),
      *clq = ClauseCodeToLogUpdClause(q);
    clq->ClPrev = NULL;
    clq->ClNext = clp;
    clp->ClPrev = clq;
    p->cs.p_code.FirstClause = q;
    if (p->PredFlags & SpiedPredFlag) {
      p->OpcodeOfPred = Yap_opcode(_spy_pred);
      p->cs.p_code.TrueCodeOfPred = p->CodeOfPred = (yamop *)(&(p->OpcodeOfPred)); 
    } else if (!(p->PredFlags & IndexedPredFlag)) {
      p->OpcodeOfPred = INDEX_OPCODE;
      p->cs.p_code.TrueCodeOfPred = p->CodeOfPred = (yamop *)(&(p->OpcodeOfPred)); 
    }
    return;
  }
  cl->ClNext = ClauseCodeToStaticClause(p->cs.p_code.FirstClause);
#ifdef YAPOR
  PUT_YAMOP_LTT(q, YAMOP_LTT((yamop *)(p->cs.p_code.FirstClause)) + 1);
#endif /* YAPOR */
  p->cs.p_code.FirstClause = q;
  p->cs.p_code.TrueCodeOfPred = q;
  if (p->PredFlags & SpiedPredFlag) {
    p->OpcodeOfPred = Yap_opcode(_spy_pred);
    p->CodeOfPred = (yamop *)(&(p->OpcodeOfPred)); 
  } else if (!(p->PredFlags & IndexedPredFlag)) {
    p->OpcodeOfPred = INDEX_OPCODE;
    p->CodeOfPred = (yamop *)(&(p->OpcodeOfPred)); 
  }
  p->cs.p_code.LastClause->u.ld.d = q;
}

/* p is already locked */
static void 
asserta_dynam_clause(PredEntry *p, yamop *cp)
{
  yamop        *q;
  DynamicClause *cl = ClauseCodeToDynamicClause(cp);
  q = cp;
  LOCK(ClauseCodeToDynamicClause(p->cs.p_code.FirstClause)->ClLock);
  /* also, keep backpointers for the days we'll delete all the clause */
  ClauseCodeToDynamicClause(p->cs.p_code.FirstClause)->ClPrevious = q;
  cl->ClPrevious = (yamop *)(p->CodeOfPred);
  cl->ClFlags |= DynamicMask;
  UNLOCK(ClauseCodeToDynamicClause(p->cs.p_code.FirstClause)->ClLock);
  q->u.ld.d = p->cs.p_code.FirstClause;
  q->u.ld.s = p->ArityOfPE;
  q->u.ld.p = p;
  if (p->PredFlags & ProfiledPredFlag)
    cp->opc = Yap_opcode(_profiled_retry_and_mark);
  else if (p->PredFlags & CountPredFlag)
    cp->opc = Yap_opcode(_count_retry_and_mark);
  else
    cp->opc = Yap_opcode(_retry_and_mark);
  cp->u.ld.s = p->ArityOfPE;
  cp->u.ld.p = p;
  p->cs.p_code.FirstClause = cp;
  q = p->CodeOfPred;
  q->u.ld.d = cp;
  q->u.ld.s = p->ArityOfPE;
  q->u.ld.p = p;

}

/* p is already locked */
static void 
assertz_stat_clause(PredEntry *p, yamop *cp, int spy_flag)
{
  yamop        *pt;

  p->cs.p_code.NOfClauses++;
  pt = p->cs.p_code.LastClause;
  if (is_logupd(p)) {
    LogUpdClause
      *clp = ClauseCodeToLogUpdClause(cp),
      *clq = ClauseCodeToLogUpdClause(pt);

    clq->ClNext = clp;
    clp->ClPrev = clq;
    clp->ClNext = NULL;
    p->cs.p_code.LastClause = cp;
    if (p->PredFlags & SpiedPredFlag) {
      p->OpcodeOfPred = Yap_opcode(_spy_pred);
      p->cs.p_code.TrueCodeOfPred = p->CodeOfPred = (yamop *)(&(p->OpcodeOfPred)); 
    } else if (!(p->PredFlags & IndexedPredFlag)) {
      p->OpcodeOfPred = INDEX_OPCODE;
      p->cs.p_code.TrueCodeOfPred = p->CodeOfPred = (yamop *)(&(p->OpcodeOfPred)); 
    }
    return;
  } else {
    StaticClause *cl =   ClauseCodeToStaticClause(pt);

    cl->ClNext = ClauseCodeToStaticClause(cp);
  }
  if (p->cs.p_code.FirstClause == p->cs.p_code.LastClause) {
    if (!(p->PredFlags & SpiedPredFlag)) {
      p->OpcodeOfPred = INDEX_OPCODE;
      p->CodeOfPred = (yamop *)(&(p->OpcodeOfPred)); 
    }
  }
  p->cs.p_code.LastClause = cp;
}

/* p is already locked */
static void 
assertz_dynam_clause(PredEntry *p, yamop *cp)
{
  yamop       *q;
  DynamicClause *cl = ClauseCodeToDynamicClause(cp);

  q = p->cs.p_code.LastClause;
  LOCK(ClauseCodeToDynamicClause(q)->ClLock);
  q->u.ld.d = cp;
  p->cs.p_code.LastClause = cp;
  /* also, keep backpointers for the days we'll delete all the clause */
  cl->ClPrevious = q;
  cl->ClFlags |= DynamicMask;
  UNLOCK(ClauseCodeToDynamicClause(q)->ClLock);
  q = (yamop *)cp;
  if (p->PredFlags & ProfiledPredFlag)
    q->opc = Yap_opcode(_profiled_retry_and_mark);
  else if (p->PredFlags & CountPredFlag)
    q->opc = Yap_opcode(_count_retry_and_mark);
  else
    q->opc = Yap_opcode(_retry_and_mark);
  q->u.ld.d = p->CodeOfPred;
  q->u.ld.s = p->ArityOfPE;
  q->u.ld.p = p;
  p->cs.p_code.NOfClauses++;
}

static void  expand_consult(void)
{
  consult_obj *new_cl, *new_cs;
  UInt OldConsultCapacity = ConsultCapacity;

  /* now double consult capacity */
  ConsultCapacity += InitialConsultCapacity;
  /* I assume it always works ;-) */
  while ((new_cl = (consult_obj *)Yap_AllocCodeSpace(sizeof(consult_obj)*ConsultCapacity)) == NULL) {
    if (!Yap_growheap(FALSE, sizeof(consult_obj)*ConsultCapacity, NULL)) {
      Yap_Error(OUT_OF_HEAP_ERROR,TermNil,Yap_ErrorMessage);
      return;
    }
  }
  new_cs = new_cl + InitialConsultCapacity;
  /* start copying */
  memcpy((void *)new_cs, (void *)ConsultLow, OldConsultCapacity*sizeof(consult_obj));
  /* copying done, release old space */
  Yap_FreeCodeSpace((char *)ConsultLow);
  /* next, set up pointers correctly */
  new_cs += (ConsultSp-ConsultLow);
  /* put ConsultBase at same offset as before move */
  ConsultBase = ConsultBase+(new_cs-ConsultSp);
  /* new consult pointer */
  ConsultSp = new_cs;
  /* new end of memory */
  ConsultLow = new_cl;
}

/* p was already locked */
static int 
not_was_reconsulted(PredEntry *p, Term t, int mode)
{
  register consult_obj  *fp;
  Prop                   p0 = AbsProp((PropEntry *)p);

  for (fp = ConsultSp; fp < ConsultBase; ++fp)
    if (fp->p == p0)
      break;
  if (fp != ConsultBase)
    return (FALSE);
  if (mode) {
    if (ConsultSp == ConsultLow+1) {
      expand_consult();
    }
    --ConsultSp;
    ConsultSp->p = p0;
    if (ConsultBase[1].mode && 
	!(p->PredFlags & MultiFileFlag)) /* we are in reconsult mode */ {
      retract_all(p, static_in_use(p,TRUE));
    }
    p->src.OwnerFile = YapConsultingFile();
  }
  return TRUE;		/* careful */
}

static void
addcl_permission_error(AtomEntry *ap, Int Arity, int in_use) 
{
  Term t, ti[2];

  ti[0] = MkAtomTerm(AbsAtom(ap));
  ti[1] = MkIntegerTerm(Arity);
  t = Yap_MkApplTerm(Yap_MkFunctor(Yap_LookupAtom("/"),2), 2, ti);
  Yap_ErrorMessage = Yap_ErrorSay;
  Yap_Error_Term = t;
  Yap_Error_TYPE = PERMISSION_ERROR_MODIFY_STATIC_PROCEDURE;
  if (in_use) {
    if (Arity == 0)
      sprintf(Yap_ErrorMessage, "static predicate %s is in use", ap->StrOfAE);
    else
      sprintf(Yap_ErrorMessage,
#if SHORT_INTS
	      "static predicate %s/%ld is in use",
#else
	      "static predicate %s/%d is in use",
#endif
	      ap->StrOfAE, Arity);
  } else {
    if (Arity == 0)
      sprintf(Yap_ErrorMessage, "system predicate %s", ap->StrOfAE);
    else
      sprintf(Yap_ErrorMessage,
#if SHORT_INTS
	      "system predicate %s/%ld",
#else
	      "system predicate %s/%d",
#endif
	      ap->StrOfAE, Arity);
  }
}


static int
is_fact(Term t)
{
  Term a1;

  if (IsAtomTerm(t))
    return TRUE;
  if (FunctorOfTerm(t) != FunctorAssert)
    return TRUE;
  a1 = ArgOfTerm(2, t);
  if (a1 == MkAtomTerm(AtomTrue))
    return TRUE;
  return FALSE;
}

static int
addclause(Term t, yamop *cp, int mode, Term mod, Term *t4ref)
/*
 *
 mode
   0  assertz
   1  consult
   2  asserta
*/
{
  PredEntry      *p;
  int             spy_flag = FALSE;
  Atom           at;
  UInt           Arity;
  CELL		 pflags;
  Term		 tf;


  if (IsApplTerm(t) && FunctorOfTerm(t) == FunctorAssert)
    tf = ArgOfTerm(1, t);
  else
    tf = t;
  if (IsAtomTerm(tf)) {
    at = AtomOfTerm(tf);
    p = RepPredProp(PredPropByAtom(at, mod));
    Arity = 0;
  } else {
    Functor f = FunctorOfTerm(tf);
    Arity = ArityOfFunctor(f);
    at = NameOfFunctor(f);
    p = RepPredProp(PredPropByFunc(f, mod));
  }
  Yap_PutValue(AtomAbol, TermNil);
  WRITE_LOCK(p->PRWLock);
#if defined(YAPOR) || defined(THREADS)
  WPP = p;
#endif
  pflags = p->PredFlags;
  /* we are redefining a prolog module predicate */
  if ((pflags & (UserCPredFlag|CArgsPredFlag|NumberDBPredFlag|AtomDBPredFlag|TestPredFlag|AsmPredFlag|CPredFlag|BinaryTestPredFlag)) ||
      (p->ModuleOfPred == PROLOG_MODULE && 
       mod != TermProlog && mod) ) {
#if defined(YAPOR) || defined(THREADS)
    WPP = NULL;
#endif
    WRITE_UNLOCK(p->PRWLock);
    addcl_permission_error(RepAtom(at), Arity, FALSE);
    return TermNil;
  }
  /* we are redefining a prolog module predicate */
  if (pflags & MegaClausePredFlag) {
    split_megaclause(p);
  }
  /* The only problem we have now is when we need to throw away
     Indexing blocks
  */
  if (pflags & IndexedPredFlag) {
    Yap_AddClauseToIndex(p, cp, mode == asserta);
  }
  if (pflags & SpiedPredFlag)
    spy_flag = TRUE;
  if (p == PredGoalExpansion) {
    Term tg = ArgOfTerm(1, tf);
    Term tm = ArgOfTerm(2, tf);

    if (IsVarTerm(tg) || IsVarTerm(tm)) {
      if (!IsVarTerm(tg)) {
	/* this is the complicated case, first I need to inform
	   predicates for this functor */ 
	PRED_GOAL_EXPANSION_FUNC = TRUE;
	if (IsAtomTerm(tg)) {
	  AtomEntry *ae = RepAtom(AtomOfTerm(tg));
	  Prop p0 = ae->PropsOfAE;
	  int found = FALSE;

	  while (p0) {
	    PredEntry *pe = RepPredProp(p0);
	    if (pe->KindOfPE == PEProp) {
	      pe->PredFlags |= GoalExPredFlag;
	      found = TRUE;
	    }
	    p0 = pe->NextOfPE;
	  }
	  if (!found) {
	    PredEntry *npe = RepPredProp(PredPropByAtom(AtomOfTerm(tg),IDB_MODULE));
	    npe->PredFlags |= GoalExPredFlag;	    
	  }
	} else if (IsApplTerm(tg)) {
	  FunctorEntry *fe = (FunctorEntry *)FunctorOfTerm(tg);
	  Prop p0;
	  int found = FALSE;

	  p0 = fe->PropsOfFE;
	  while (p0) {
	    PredEntry *pe = RepPredProp(p0);

	    pe->PredFlags |= GoalExPredFlag;
	    p0 = pe->NextOfPE;
	    found = TRUE;
	  }
	  if (!found) {
	    PredEntry *npe = RepPredProp(PredPropByFunc(fe,IDB_MODULE));
	    npe->PredFlags |= GoalExPredFlag;	    
	  }
	}
      } else {
	PRED_GOAL_EXPANSION_ALL = TRUE;
      }
    } else {
      if (IsAtomTerm(tm)) {
	if (IsAtomTerm(tg)) {
	  PredEntry *p = RepPredProp(PredPropByAtom(AtomOfTerm(tg), tm));
	  p->PredFlags |= GoalExPredFlag;
	} else if (IsApplTerm(tg)) {
	  PredEntry *p = RepPredProp(PredPropByFunc(FunctorOfTerm(tg), tm));
	  p->PredFlags |= GoalExPredFlag;
	}
      }
    }
  }
  if (mode == consult)
    not_was_reconsulted(p, t, TRUE);
  /* always check if we have a valid error first */
  if (Yap_ErrorMessage && Yap_Error_TYPE == PERMISSION_ERROR_MODIFY_STATIC_PROCEDURE) {
#if defined(YAPOR) || defined(THREADS)
    WPP = NULL;
#endif
    WRITE_UNLOCK(p->PRWLock);
    return TermNil;
  }
  if (!is_dynamic(p)) {
    if (pflags & LogUpdatePredFlag) {
      LogUpdClause     *clp = ClauseCodeToLogUpdClause(cp);
      clp->ClFlags |= LogUpdMask;
      if (is_fact(t)) {
	clp->ClFlags |= FactMask;
	clp->ClSource = NULL;
      }
    } else {
      StaticClause     *clp = ClauseCodeToStaticClause(cp);
      clp->ClFlags |= StaticMask;
      if (is_fact(t) && !(p->PredFlags & TabledPredFlag)) {
	clp->ClFlags |= FactMask;
	clp->usc.ClPred = p;
      }
    }
    if (compile_mode)
      p->PredFlags = p->PredFlags | CompiledPredFlag;
    else
      p->PredFlags = p->PredFlags | CompiledPredFlag;
  }
  if (p->cs.p_code.FirstClause == NULL) {
    if (!(pflags & DynamicPredFlag)) {
      add_first_static(p, cp, spy_flag);
      /* make sure we have a place to jump to */
      if (p->OpcodeOfPred == UNDEF_OPCODE ||
	  p->OpcodeOfPred == FAIL_OPCODE) {  /* log updates */
	p->CodeOfPred = p->cs.p_code.TrueCodeOfPred;
	p->OpcodeOfPred = ((yamop *)(p->CodeOfPred))->opc;
      }
    } else {
      add_first_dynamic(p, cp, spy_flag);
    }
  } else if (mode == asserta) {
    if (pflags & DynamicPredFlag)
      asserta_dynam_clause(p, cp);
    else
      asserta_stat_clause(p, cp, spy_flag);
  } else if (pflags & DynamicPredFlag)
    assertz_dynam_clause(p, cp);
  else {
    assertz_stat_clause(p, cp, spy_flag);
    if (p->OpcodeOfPred != INDEX_OPCODE &&
	p->OpcodeOfPred != Yap_opcode(_spy_pred)) {
      p->CodeOfPred = p->cs.p_code.TrueCodeOfPred;
      p->OpcodeOfPred = ((yamop *)(p->CodeOfPred))->opc;
    }
  }
#if defined(YAPOR) || defined(THREADS)
  WPP = NULL;
#endif
  WRITE_UNLOCK(p->PRWLock);
  if (pflags & LogUpdatePredFlag) {
    tf = MkDBRefTerm((DBRef)ClauseCodeToLogUpdClause(cp));
  } else {
    tf = Yap_MkStaticRefTerm(ClauseCodeToStaticClause(cp));
  }
  if (*t4ref != TermNil) {
    if (!Yap_unify(*t4ref,tf)) {
      return FALSE;
    }
  }
  if (pflags & MultiFileFlag) {
    /* add Info on new clause for multifile predicates to the DB */
    Term t[5], tn;
    t[0] = MkAtomTerm(YapConsultingFile());
    t[1] = MkAtomTerm(at);
    t[2] = MkIntegerTerm(Arity);
    t[3] = mod;
    t[4] = tf;
    tn = Yap_MkApplTerm(FunctorMultiFileClause,5,t);
    Yap_Recordz(AtomMultiFile,tn);
  }
  return TRUE;
}

int
Yap_addclause(Term t, yamop *cp, int mode, Term mod, Term *t4ref) {
  return addclause(t, cp, mode, mod, t4ref);
}

void
Yap_EraseMegaClause(yamop *cl,PredEntry *ap) {
  /* just make it fail */
  cl->opc = Yap_opcode(_op_fail);
}

void
Yap_EraseStaticClause(StaticClause *cl, Term mod) {
  PredEntry *ap;

  /* ok, first I need to find out the parent predicate */
  if (cl->ClFlags & FactMask) {
    ap = cl->usc.ClPred;
  } else {
    Term t = ArgOfTerm(1,cl->usc.ClSource->Entry);
    if (IsAtomTerm(t)) {
      Atom at = AtomOfTerm(t);
      ap = RepPredProp(Yap_GetPredPropByAtom(at, mod));
    } else {
      Functor fun = FunctorOfTerm(t);
      ap = RepPredProp(Yap_GetPredPropByFunc(fun, mod));
    }
  }
  WRITE_LOCK(ap->PRWLock);
#if defined(YAPOR) || defined(THREADS)
  WPP = NULL;
#endif
  if (ap->PredFlags & MegaClausePredFlag) {
    split_megaclause(ap);
  }
  if (ap->PredFlags & IndexedPredFlag)
    RemoveIndexation(ap);
  ap->cs.p_code.NOfClauses--;
  if (ap->cs.p_code.FirstClause == cl->ClCode) {
    /* got rid of first clause */
    if (ap->cs.p_code.LastClause == cl->ClCode) {
      /* got rid of all clauses */
      ap->cs.p_code.LastClause = ap->cs.p_code.FirstClause = NULL;
      ap->OpcodeOfPred = UNDEF_OPCODE;
      ap->cs.p_code.TrueCodeOfPred =
	(yamop *)(&(ap->OpcodeOfPred)); 
    } else {
      yamop *ncl = cl->ClNext->ClCode;
      ap->cs.p_code.FirstClause = ncl;
      ap->cs.p_code.TrueCodeOfPred =
	ncl;
      ap->OpcodeOfPred = ncl->opc;
    }
  } else {
    StaticClause *pcl = ClauseCodeToStaticClause(ap->cs.p_code.FirstClause),
      *ocl = NULL;

    while (pcl != cl) {
      ocl = pcl;
      pcl = pcl->ClNext;
    }
    ocl->ClNext = cl->ClNext;
    if (cl->ClCode ==  ap->cs.p_code.LastClause) {
      ap->cs.p_code.LastClause = ocl->ClCode;
    }
  }
  if (ap->cs.p_code.NOfClauses == 1) {
    ap->cs.p_code.TrueCodeOfPred =
      ap->cs.p_code.FirstClause;
    ap->OpcodeOfPred =
      ap->cs.p_code.TrueCodeOfPred->opc;
  }
#if defined(YAPOR) || defined(THREADS)
  WPP = NULL;
#endif
  WRITE_UNLOCK(ap->PRWLock);
  if (cl->ClFlags & HasBlobsMask || static_in_use(ap,TRUE)) {
    LOCK(DeadStaticClausesLock);
    cl->ClNext = DeadStaticClauses;
    DeadStaticClauses = cl;
    UNLOCK(DeadStaticClausesLock);
  } else {
    Yap_InformOfRemoval((CODEADDR)cl);
    Yap_ClauseSpace -= cl->ClSize;
    Yap_FreeCodeSpace((char *)cl);
  }
  if (ap->cs.p_code.NOfClauses == 0) {
    ap->CodeOfPred = 
      ap->cs.p_code.TrueCodeOfPred;
  } else if (ap->cs.p_code.NOfClauses > 1) {
    ap->OpcodeOfPred = INDEX_OPCODE;
    ap->CodeOfPred = ap->cs.p_code.TrueCodeOfPred = (yamop *)(&(ap->OpcodeOfPred)); 
  } else if (ap->PredFlags & SpiedPredFlag) {
      ap->OpcodeOfPred = Yap_opcode(_spy_pred);
      ap->CodeOfPred = ap->cs.p_code.TrueCodeOfPred = (yamop *)(&(ap->OpcodeOfPred)); 
  } else {
    ap->CodeOfPred = ap->cs.p_code.TrueCodeOfPred;
  }
}

void
Yap_add_logupd_clause(PredEntry *pe, LogUpdClause *cl, int mode) {
  yamop *cp = cl->ClCode;

  if (pe->PredFlags & IndexedPredFlag) {
    Yap_AddClauseToIndex(pe, cp, mode == asserta);
  }
  if (pe->cs.p_code.FirstClause == NULL) {
    add_first_static(pe, cp, FALSE);
    /* make sure we have a place to jump to */
    if (pe->OpcodeOfPred == UNDEF_OPCODE ||
	pe->OpcodeOfPred == FAIL_OPCODE) {  /* log updates */
      pe->CodeOfPred = pe->cs.p_code.TrueCodeOfPred;
      pe->OpcodeOfPred = ((yamop *)(pe->CodeOfPred))->opc;
    }
  } else if (mode == asserta) {
    asserta_stat_clause(pe, cp, FALSE);
  } else {
    assertz_stat_clause(pe, cp, FALSE);
    if (pe->OpcodeOfPred != INDEX_OPCODE &&
	pe->OpcodeOfPred != Yap_opcode(_spy_pred)) {
      pe->CodeOfPred = pe->cs.p_code.TrueCodeOfPred;
      pe->OpcodeOfPred = ((yamop *)(pe->CodeOfPred))->opc;
    }
  }
}

static Int 
p_in_this_f_before(void)
{				/* '$in_this_file_before'(N,A,M) */
  unsigned int    arity;
  Atom            at;
  Term            t;
  register consult_obj  *fp;
  Prop            p0;
  Term            mod;

  if (IsVarTerm(t = Deref(ARG1)) || !IsAtomTerm(t))
    return (FALSE);
  else
    at = AtomOfTerm(t);
  if (IsVarTerm(t = Deref(ARG2)) || !IsIntTerm(t))
    return (FALSE);
  else
    arity = IntOfTerm(t);
  if (IsVarTerm(mod = Deref(ARG3)) || !IsAtomTerm(mod))
    return FALSE;
  if (arity)
    p0 = PredPropByFunc(Yap_MkFunctor(at, arity), mod);
  else
    p0 = PredPropByAtom(at, mod);
  if (ConsultSp == ConsultBase || (fp = ConsultSp)->p == p0)
    return (FALSE);
  else
    fp++;
  for (; fp < ConsultBase; ++fp)
    if (fp->p == p0)
      break;
  if (fp != ConsultBase)
    return TRUE;
  else
    return FALSE;
}

static Int 
p_first_cl_in_f(void)
{				/* '$first_cl_in_file'(+N,+Ar,+Mod) */
  unsigned int    arity;
  Atom            at;
  Term            t;
  register consult_obj  *fp;
  Prop            p0;
  Term	          mod;
  

  if (IsVarTerm(t = Deref(ARG1)) || !IsAtomTerm(t))
    return (FALSE);
  else
    at = AtomOfTerm(t);
  if (IsVarTerm(t = Deref(ARG2)) || !IsIntTerm(t))
    return (FALSE);
  else
    arity = IntOfTerm(t);
  if (IsVarTerm(mod = Deref(ARG3)) || !IsAtomTerm(mod))
    return (FALSE);
  if (arity)
    p0 = PredPropByFunc(Yap_MkFunctor(at, arity),mod);
  else
    p0 = PredPropByAtom(at, mod);
  for (fp = ConsultSp; fp < ConsultBase; ++fp)
    if (fp->p == p0)
      break;
  if (fp != ConsultBase)
    return (FALSE);
  return (TRUE);
}

static Int 
p_mk_cl_not_first(void)
{				/* '$mk_cl_not_first'(+N,+Ar) */
  unsigned int    arity;
  Atom            at;
  Term            t;
  Prop            p0;

  if (IsVarTerm(t = Deref(ARG1)) && !IsAtomTerm(t))
    return (FALSE);
  else
    at = AtomOfTerm(t);
  if (IsVarTerm(t = Deref(ARG2)) && !IsIntTerm(t))
    return (FALSE);
  else
    arity = IntOfTerm(t);
  if (arity)
    p0 = PredPropByFunc(Yap_MkFunctor(at, arity),CurrentModule);
  else
    p0 = PredPropByAtom(at, CurrentModule);
  --ConsultSp;
  ConsultSp->p = p0;
  return (TRUE);
}

#if EMACS

/*
 * the place where one would add a new clause for the propriety pred_prop 
 */
int 
where_new_clause(pred_prop, mode)
     Prop            pred_prop;
     int             mode;
{
  PredEntry      *p = RepPredProp(pred_prop);

  if (mode == consult && not_was_reconsulted(p, TermNil, FALSE))
    return (1);
  else
    return (p->cs.p_code.NOfClauses + 1);
}
#endif

static Int 
p_compile(void)
{				/* '$compile'(+C,+Flags, Mod) */
  Term            t = Deref(ARG1);
  Term            t1 = Deref(ARG2);
  Term            mod = Deref(ARG4);
  Term            tn = TermNil;
  yamop           *codeadr;

  if (IsVarTerm(t1) || !IsIntTerm(t1))
    return (FALSE);
  if (IsVarTerm(mod) || !IsAtomTerm(mod))
    return (FALSE);

  YAPEnterCriticalSection();
  codeadr = Yap_cclause(t, 4, mod, Deref(ARG3)); /* vsc: give the number of arguments
			      to cclause in case there is overflow */
  t = Deref(ARG1);        /* just in case there was an heap overflow */
  if (!Yap_ErrorMessage)
    addclause(t, codeadr, (int) (IntOfTerm(t1) & 3), mod, &tn);
  YAPLeaveCriticalSection();
  if (Yap_ErrorMessage) {
    if (IntOfTerm(t1) & 4) {
      Yap_Error(Yap_Error_TYPE, Yap_Error_Term,
	    "in line %d, %s", Yap_FirstLineInParse(), Yap_ErrorMessage);
    } else {
      Yap_Error(Yap_Error_TYPE, Yap_Error_Term, Yap_ErrorMessage);
    }
    return FALSE;
  }
  return TRUE;
}

static Int 
p_compile_dynamic(void)
{				/* '$compile_dynamic'(+C,+Flags,Mod,-Ref) */
  Term            t = Deref(ARG1);
  Term            t1 = Deref(ARG2);
  Term            mod = Deref(ARG4);
  yamop        *code_adr;
  int             old_optimize, mode;

  if (IsVarTerm(t1) || !IsAtomicTerm(t1))
    return FALSE;
  if (IsVarTerm(mod) || !IsAtomTerm(mod))
    return FALSE;
  if (IsAtomTerm(t1)) {
    if (RepAtom(AtomOfTerm(t1))->StrOfAE[0] == 'f') mode = asserta;
    else mode = assertz;						    
  } else mode = IntegerOfTerm(t1);
  old_optimize = optimizer_on;
  optimizer_on = FALSE;
  YAPEnterCriticalSection();
  code_adr = Yap_cclause(t, 5, mod, Deref(ARG3)); /* vsc: give the number of arguments to
			       cclause() in case there is a overflow */
  t = Deref(ARG1);        /* just in case there was an heap overflow */
  if (!Yap_ErrorMessage) {
    
    
    optimizer_on = old_optimize;
    addclause(t, code_adr, mode , mod, &ARG5);
  } 
  if (Yap_ErrorMessage) {
    if (!Yap_Error_Term)
      Yap_Error_Term = TermNil;
    Yap_Error(Yap_Error_TYPE, Yap_Error_Term, Yap_ErrorMessage);
    YAPLeaveCriticalSection();
    return FALSE;
  }
  YAPLeaveCriticalSection();
  return TRUE;
}

static int      consult_level = 0;

static Atom
YapConsultingFile (void)
{
  if (consult_level == 0) {
    return(Yap_LookupAtom("user"));
  } else {
    return(Yap_LookupAtom(ConsultBase[2].filename));
  }
}

Atom
Yap_ConsultingFile (void)
{
  return YapConsultingFile();
}

/* consult file *file*, *mode* may be one of either consult or reconsult */
static void
init_consult(int mode, char *file)
{
  ConsultSp--;
  ConsultSp->filename = file;
  ConsultSp--;
  ConsultSp->mode = mode;
  ConsultSp--;
  ConsultSp->c = (ConsultBase-ConsultSp);
  ConsultBase = ConsultSp;
#if !defined(YAPOR) && !defined(SBA)
  /*  if (consult_level == 0)
      do_toggle_static_predicates_in_use(TRUE); */
#endif
  consult_level++;
}

void
Yap_init_consult(int mode, char *file)
{
  init_consult(mode,file);
}

static Int 
p_startconsult(void)
{				/* '$start_consult'(+Mode)	 */
  Term            t;
  char           *smode = RepAtom(AtomOfTerm(Deref(ARG1)))->StrOfAE;
  int             mode;
  
  mode = strcmp("consult",smode);
  init_consult(mode, RepAtom(AtomOfTerm(Deref(ARG2)))->StrOfAE);
  t = MkIntTerm(consult_level);
  return (Yap_unify_constant(ARG3, t));
}

static Int 
p_showconslultlev(void)
{
  Term            t;

  t = MkIntTerm(consult_level);
  return (Yap_unify_constant(ARG1, t));
}

static void
end_consult(void)
{
  ConsultSp = ConsultBase;
  ConsultBase = ConsultSp+ConsultSp->c;
  ConsultSp += 3;
  consult_level--;
#if !defined(YAPOR) && !defined(SBA)
  /*  if (consult_level == 0)
      do_toggle_static_predicates_in_use(FALSE);*/
#endif
}

void
Yap_end_consult(void) {
  end_consult();
}


static Int 
p_endconsult(void)
{				/* '$end_consult'		 */
  end_consult();
  return (TRUE);
}

static void
purge_clauses(PredEntry *pred)
{
  if (pred->PredFlags & IndexedPredFlag)
    RemoveIndexation(pred);
  Yap_PutValue(AtomAbol, MkAtomTerm(AtomTrue));
  retract_all(pred, static_in_use(pred,TRUE));
  pred->src.OwnerFile = AtomNil;
  if (pred->PredFlags & MultiFileFlag)
    pred->PredFlags ^= MultiFileFlag;
}

void
Yap_Abolish(PredEntry *pred)
{
  purge_clauses(pred);
}

static Int 
p_purge_clauses(void)
{				/* '$purge_clauses'(+Func) */
  PredEntry      *pred;
  Term            t = Deref(ARG1);
  Term            mod = Deref(ARG2);

  Yap_PutValue(AtomAbol, MkAtomTerm(AtomNil));
  if (IsVarTerm(t))
    return FALSE;
  if (IsVarTerm(mod) || !IsAtomTerm(mod)) {
    return FALSE;
  }
  if (IsAtomTerm(t)) {
    Atom at = AtomOfTerm(t);
    pred = RepPredProp(PredPropByAtom(at, mod));
  } else if (IsApplTerm(t)) {
    Functor         fun = FunctorOfTerm(t);
    pred = RepPredProp(PredPropByFunc(fun, mod));
  } else
    return (FALSE);
  WRITE_LOCK(pred->PRWLock);
#if defined(YAPOR) || defined(THREADS)
  WPP = pred;
#endif
  if (pred->PredFlags & StandardPredFlag) {
#if defined(YAPOR) || defined(THREADS)
    WPP = NULL;
#endif
    WRITE_UNLOCK(pred->PRWLock);
    Yap_Error(PERMISSION_ERROR_MODIFY_STATIC_PROCEDURE, t, "assert/1");
    return (FALSE);
  }
  purge_clauses(pred);
#if defined(YAPOR) || defined(THREADS)
  WPP = NULL;
#endif
  WRITE_UNLOCK(pred->PRWLock);
  return (TRUE);
}

/******************************************************************
  
		MANAGING SPY-POINTS
  
******************************************************************/

static Int 
p_setspy(void)
{				/* '$set_spy'(+Fun,+M)	 */
  Atom            at;
  PredEntry      *pred;
  CELL            fg;
  Term            t, mod;

  at = Yap_FullLookupAtom("$spy");
  pred = RepPredProp(PredPropByFunc(Yap_MkFunctor(at, 1),0));
  SpyCode = pred;
  t = Deref(ARG1);
  mod = Deref(ARG2);
  if (IsVarTerm(mod) || !IsAtomTerm(mod))
    return (FALSE);
  if (IsVarTerm(t))
    return (FALSE);
  if (IsAtomTerm(t)) {
    Atom at = AtomOfTerm(t);
    pred = RepPredProp(Yap_PredPropByAtomNonThreadLocal(at, mod));
  } else if (IsApplTerm(t)) {
    Functor fun = FunctorOfTerm(t);
    pred = RepPredProp(Yap_PredPropByFunctorNonThreadLocal(fun, mod));
  } else {
    return (FALSE);
  }
  WRITE_LOCK(pred->PRWLock);
 restart_spy:
  if (pred->PredFlags & (CPredFlag | SafePredFlag)) {
    WRITE_UNLOCK(pred->PRWLock);
    return (FALSE);
  }
  if (pred->OpcodeOfPred == UNDEF_OPCODE ||
      pred->OpcodeOfPred == FAIL_OPCODE) {
    WRITE_UNLOCK(pred->PRWLock);
    return (FALSE);
  }
  if (pred->OpcodeOfPred == INDEX_OPCODE) {
    int i = 0;
    for (i = 0; i < pred->ArityOfPE; i++) {
      XREGS[i+1] = MkVarTerm();
    }
    IPred(pred, 0);
    goto restart_spy;
  }
  fg = pred->PredFlags;
  if (fg & DynamicPredFlag) {
    pred->OpcodeOfPred =
      ((yamop *)(pred->CodeOfPred))->opc =
      Yap_opcode(_spy_or_trymark);
  } else {
    pred->OpcodeOfPred = Yap_opcode(_spy_pred);
    pred->CodeOfPred = (yamop *)(&(pred->OpcodeOfPred)); 
  }
  pred->PredFlags |= SpiedPredFlag;
  WRITE_UNLOCK(pred->PRWLock);
  return (TRUE);
}

static Int 
p_rmspy(void)
{				/* '$rm_spy'(+T,+Mod)	 */
  Atom            at;
  PredEntry      *pred;
  Term            t;
  Term            mod;

  t = Deref(ARG1);
  mod = Deref(ARG2);
  if (IsVarTerm(mod) || !IsAtomTerm(mod))
    return (FALSE);
  if (IsVarTerm(t))
    return (FALSE);
  if (IsAtomTerm(t)) {
    at = AtomOfTerm(t);
    pred = RepPredProp(Yap_PredPropByAtomNonThreadLocal(at, mod));
  } else if (IsApplTerm(t)) {
    Functor fun = FunctorOfTerm(t);
    pred = RepPredProp(Yap_PredPropByFunctorNonThreadLocal(fun, mod));
  } else
    return (FALSE);
  WRITE_LOCK(pred->PRWLock);
  if (!(pred->PredFlags & SpiedPredFlag)) {
    WRITE_UNLOCK(pred->PRWLock);
    return (FALSE);
  }
#if THREADS
  if (!(pred->PredFlags & ThreadLocalPredFlag)) {
    pred->OpcodeOfPred = Yap_opcode(_thread_local);
  } else
#endif
    if (!(pred->PredFlags & DynamicPredFlag)) {
    pred->CodeOfPred = pred->cs.p_code.TrueCodeOfPred;
    pred->OpcodeOfPred = ((yamop *)(pred->CodeOfPred))->opc;
  } else if (pred->OpcodeOfPred == Yap_opcode(_spy_or_trymark)) {
    pred->OpcodeOfPred = Yap_opcode(_try_and_mark);
  } else
    return (FALSE);
  pred->PredFlags ^= SpiedPredFlag;
  WRITE_UNLOCK(pred->PRWLock);
  return (TRUE);
}


/******************************************************************
  
		INFO ABOUT PREDICATES
  
******************************************************************/

static Int 
p_number_of_clauses(void)
{				/* '$number_of_clauses'(Predicate,M,N) */
  Term            t = Deref(ARG1);
  Term            mod = Deref(ARG2);
  int ncl = 0;
  Prop            pe;

  if (IsVarTerm(mod)  || !IsAtomTerm(mod)) {
    return(FALSE);
  }
  if (IsAtomTerm(t)) {
    Atom a = AtomOfTerm(t);
    pe = Yap_GetPredPropByAtom(a, mod);
  } else if (IsApplTerm(t)) {
    register Functor f = FunctorOfTerm(t);
    pe = Yap_GetPredPropByFunc(f, mod);
  } else {
    return (FALSE);
  }
  if (EndOfPAEntr(pe))
    return FALSE;
  READ_LOCK(RepPredProp(pe)->PRWLock);
  ncl = RepPredProp(pe)->cs.p_code.NOfClauses;
  READ_UNLOCK(RepPredProp(pe)->PRWLock);
  return (Yap_unify_constant(ARG3, MkIntegerTerm(ncl)));
}

static Int 
p_in_use(void)
{				/* '$in_use'(+P,+Mod)	 */
  Term            t = Deref(ARG1);
  Term            mod = Deref(ARG2);
  PredEntry      *pe;
  Int            out;

  if (IsVarTerm(t))
    return (FALSE);
  if (IsVarTerm(mod) || !IsAtomTerm(mod))
    return (FALSE);
  if (IsAtomTerm(t)) {
    Atom at = AtomOfTerm(t);
    pe = RepPredProp(Yap_GetPredPropByAtom(at, mod));
  } else if (IsApplTerm(t)) {
    Functor         fun = FunctorOfTerm(t);
    pe = RepPredProp(Yap_GetPredPropByFunc(fun, mod));
  } else
    return FALSE;
  if (EndOfPAEntr(pe))
    return FALSE;
  READ_LOCK(pe->PRWLock);
  out = static_in_use(pe,TRUE);
  READ_UNLOCK(pe->PRWLock);
  return(out);
}

static Int 
p_new_multifile(void)
{				/* '$new_multifile'(+N,+Ar,+Mod)  */
  Atom            at;
  int             arity;
  PredEntry      *pe;
  Term            t = Deref(ARG1);
  Term            mod = Deref(ARG3);

  if (IsVarTerm(t))
    return (FALSE);
  if (IsAtomTerm(t))
    at = AtomOfTerm(t);
  else
    return (FALSE);
  t = Deref(ARG2);
  if (IsVarTerm(t))
    return (FALSE);
  if (IsIntTerm(t))
    arity = IntOfTerm(t);
  else
    return FALSE;
  if (arity == 0) 
    pe = RepPredProp(PredPropByAtom(at, mod));
  else 
    pe = RepPredProp(PredPropByFunc(Yap_MkFunctor(at, arity),mod));
  WRITE_LOCK(pe->PRWLock);
  pe->PredFlags |= MultiFileFlag;
  if (!(pe->PredFlags & (DynamicPredFlag|LogUpdatePredFlag))) {
    /* static */
    pe->PredFlags |= (SourcePredFlag|CompiledPredFlag);
  }
  WRITE_UNLOCK(pe->PRWLock);
  return (TRUE);
}


static Int 
p_is_multifile(void)
{				/* '$is_multifile'(+S,+Mod)	 */
  PredEntry      *pe;
  Term            t = Deref(ARG1);
  Term            mod = Deref(ARG2);
  Int		  out;

  if (IsVarTerm(t))
    return (FALSE);
  if (IsVarTerm(mod))
    return (FALSE);
  if (!IsAtomTerm(mod))
    return (FALSE);
  if (IsAtomTerm(t)) {
    pe = RepPredProp(Yap_GetPredPropByAtom(AtomOfTerm(t), mod));
  } else if (IsApplTerm(t)) {
    pe = RepPredProp(Yap_GetPredPropByFunc(FunctorOfTerm(t), mod));
  } else
    return(FALSE);
  if (EndOfPAEntr(pe))
    return FALSE;
  READ_LOCK(pe->PRWLock);
  out = (pe->PredFlags & MultiFileFlag);
  READ_UNLOCK(pe->PRWLock);
  return(out);
}

static Int 
p_is_log_updatable(void)
{				/* '$is_dynamic'(+P)	 */
  PredEntry      *pe;
  Term            t = Deref(ARG1);
  Int             out;
  Term            mod = Deref(ARG2);

  if (IsVarTerm(t)) {
    return (FALSE);
  } else if (IsAtomTerm(t)) {
    Atom at = AtomOfTerm(t);
    pe = RepPredProp(Yap_GetPredPropByAtom(at, mod));
  } else if (IsApplTerm(t)) {
    Functor         fun = FunctorOfTerm(t);
    pe = RepPredProp(Yap_GetPredPropByFunc(fun, mod));
  } else
    return (FALSE);
  if (EndOfPAEntr(pe))
    return FALSE;
  READ_LOCK(pe->PRWLock);
  out = (pe->PredFlags & LogUpdatePredFlag);
  READ_UNLOCK(pe->PRWLock);
  return(out);
}

static Int 
p_is_source(void)
{				/* '$is_dynamic'(+P)	 */
  PredEntry      *pe;
  Term            t = Deref(ARG1);
  Term            mod = Deref(ARG2);
  Int             out;

  if (IsVarTerm(t)) {
    return (FALSE);
  } else if (IsAtomTerm(t)) {
    Atom at = AtomOfTerm(t);
    pe = RepPredProp(Yap_GetPredPropByAtom(at, mod));
  } else if (IsApplTerm(t)) {
    Functor         fun = FunctorOfTerm(t);
    pe = RepPredProp(Yap_GetPredPropByFunc(fun, mod));
  } else
    return (FALSE);
  if (EndOfPAEntr(pe))
    return FALSE;
  READ_LOCK(pe->PRWLock);
  out = (pe->PredFlags & SourcePredFlag);
  READ_UNLOCK(pe->PRWLock);
  return(out);
}

static Int 
p_is_dynamic(void)
{				/* '$is_dynamic'(+P)	 */
  PredEntry      *pe;
  Term            t = Deref(ARG1);
  Term            mod = Deref(ARG2);
  Int             out;

  if (IsVarTerm(t)) {
    return (FALSE);
  } else if (IsAtomTerm(t)) {
    Atom at = AtomOfTerm(t);
    pe = RepPredProp(Yap_GetPredPropByAtom(at, mod));
  } else if (IsApplTerm(t)) {
    Functor         fun = FunctorOfTerm(t);
    pe = RepPredProp(Yap_GetPredPropByFunc(fun, mod));
  } else
    return (FALSE);
  if (EndOfPAEntr(pe))
    return FALSE;
  READ_LOCK(pe->PRWLock);
  out = (pe->PredFlags & (DynamicPredFlag|LogUpdatePredFlag));
  READ_UNLOCK(pe->PRWLock);
  return(out);
}

static Int 
p_is_metapredicate(void)
{				/* '$is_metapredicate'(+P)	 */
  PredEntry      *pe;
  Term            t = Deref(ARG1);
  Term            mod = Deref(ARG2);
  Int             out;

  if (IsVarTerm(t)) {
    return (FALSE);
  } else if (IsAtomTerm(t)) {
    Atom at = AtomOfTerm(t);
    pe = RepPredProp(Yap_GetPredPropByAtom(at, mod));
  } else if (IsApplTerm(t)) {
    Functor         fun = FunctorOfTerm(t);
    pe = RepPredProp(Yap_GetPredPropByFunc(fun, mod));
  } else
    return (FALSE);
  if (EndOfPAEntr(pe))
    return FALSE;
  READ_LOCK(pe->PRWLock);
  out = (pe->PredFlags & MetaPredFlag);
  READ_UNLOCK(pe->PRWLock);
  return out;
}

static Int 
p_is_expandgoalormetapredicate(void)
{				/* '$is_expand_goal_predicate'(+P)	 */
  PredEntry      *pe;
  Term            t = Deref(ARG1);
  Term            mod = Deref(ARG2);
  Int             out;

  if (PRED_GOAL_EXPANSION_ALL)
    return TRUE;
  if (IsVarTerm(t)) {
    return (FALSE);
  } else if (IsAtomTerm(t)) {
    Atom at = AtomOfTerm(t);
    pe = RepPredProp(Yap_GetPredPropByAtom(at, mod));
    if (EndOfPAEntr(pe)) {
      if (PRED_GOAL_EXPANSION_FUNC) {
	Prop p1 = RepAtom(at)->PropsOfAE;

	while (p1) {
	  PredEntry *pe = RepPredProp(p1);

	  if (pe->KindOfPE == PEProp) {
	    if (pe->PredFlags & GoalExPredFlag) {
	      PredPropByAtom(at, mod);
	      return TRUE;
	    } else {
	      return FALSE;
	    }
	  }
	  p1 = pe->NextOfPE;
	}
      }
      return FALSE;
    }
  } else if (IsApplTerm(t)) {
    Functor         fun = FunctorOfTerm(t);

    if (IsExtensionFunctor(fun)) {
      return FALSE;
    }
    pe = RepPredProp(Yap_GetPredPropByFunc(fun, mod));
    if (EndOfPAEntr(pe)) {
      if (PRED_GOAL_EXPANSION_FUNC) {
	FunctorEntry *fe = (FunctorEntry *)fun;
	if (fe->PropsOfFE &&
	    (RepPredProp(fe->PropsOfFE)->PredFlags & GoalExPredFlag)) {
	  PredPropByFunc(fun, mod);
	  return TRUE;
	}
      }
      return FALSE;
    }
  } else {
    return FALSE;
  }

  READ_LOCK(pe->PRWLock);
  out = (pe->PredFlags & (GoalExPredFlag|MetaPredFlag));
  READ_UNLOCK(pe->PRWLock);
  return(out);
}

static Int 
p_pred_exists(void)
{				/* '$pred_exists'(+P,+M)	 */
  PredEntry      *pe;
  Term            t = Deref(ARG1);
  Term            mod = Deref(ARG2);
  Int             out;

  if (IsVarTerm(t)) {
    return (FALSE);
  } else if (IsAtomTerm(t)) {
    Atom at = AtomOfTerm(t);
    pe = RepPredProp(Yap_GetPredPropByAtom(at, mod));
  } else if (IsApplTerm(t)) {
    Functor         fun = FunctorOfTerm(t);
    pe = RepPredProp(Yap_GetPredPropByFunc(fun, mod));
  } else
    return (FALSE);
  if (EndOfPAEntr(pe))
    return FALSE;
  READ_LOCK(pe->PRWLock);
  if (pe->PredFlags & HiddenPredFlag){
    READ_UNLOCK(pe->PRWLock);
    return FALSE;
  }
  out = (pe->OpcodeOfPred != UNDEF_OPCODE);
  READ_UNLOCK(pe->PRWLock);
  return out;
}

static Int 
p_set_pred_module(void)
{				/* '$set_pred_module'(+P,+Mod)	 */
  PredEntry      *pe;
  Term            t = Deref(ARG1);
  Term            mod = CurrentModule;

 restart_set_pred:
  if (IsVarTerm(t)) {
    return (FALSE);
  } else if (IsAtomTerm(t)) {
    pe = RepPredProp(PredPropByAtom(AtomOfTerm(t), mod));
  } else if (IsApplTerm(t)) {
    Functor         fun = FunctorOfTerm(t);
    if (fun == FunctorModule) {
      Term tmod = ArgOfTerm(1, t);
      if (IsVarTerm(tmod) ) {
	Yap_Error(INSTANTIATION_ERROR,ARG1,"set_pred_module/1");
	return(FALSE);
      }
      if (!IsAtomTerm(tmod) ) {
	Yap_Error(TYPE_ERROR_ATOM,ARG1,"set_pred_module/1");
	return(FALSE);
      }
      mod = tmod;
      t = ArgOfTerm(2, t);
      goto restart_set_pred;
    }
    pe = RepPredProp(PredPropByFunc(fun, mod));
  } else
    return (FALSE);
  if (EndOfPAEntr(pe))
    return FALSE;
  WRITE_LOCK(pe->PRWLock);
  pe->ModuleOfPred = Deref(ARG2);
  WRITE_UNLOCK(pe->PRWLock);
  return(TRUE);
}

static Int 
p_undefined(void)
{				/* '$undefined'(P,Mod)	 */
  PredEntry      *pe;
  Term            t;
  Term            mod;

  t = Deref(ARG1);
  mod = Deref(ARG2);
  if (IsVarTerm(mod)) {
    Yap_Error(INSTANTIATION_ERROR,ARG2,"undefined/1");
    return(FALSE);
  }
  if (!IsAtomTerm(mod)) {
    Yap_Error(TYPE_ERROR_ATOM,ARG2,"undefined/1");
    return(FALSE);
  }
 restart_undefined:
  if (IsVarTerm(t)) {
    Yap_Error(INSTANTIATION_ERROR,ARG1,"undefined/1");
    return(FALSE);
  }
  if (IsAtomTerm(t)) {
    Atom at = AtomOfTerm(t);
    pe = RepPredProp(Yap_GetPredPropByAtom(at,mod));
  } else if (IsApplTerm(t)) {
    Functor         funt = FunctorOfTerm(t);
    if (funt == FunctorModule) {
      Term tmod = ArgOfTerm(1, t);
      if (IsVarTerm(tmod) ) {
	Yap_Error(INSTANTIATION_ERROR,ARG1,"undefined/1");
	return(FALSE);
      }
      if (!IsAtomTerm(tmod) ) {
	Yap_Error(TYPE_ERROR_ATOM,ARG1,"undefined/1");
	return(FALSE);
      }
      mod = tmod;
      t = ArgOfTerm(2, t);
      goto restart_undefined;
    }
    pe = RepPredProp(Yap_GetPredPropByFunc(funt, mod));
  } else {
    return TRUE;
  }
  if (EndOfPAEntr(pe))
    return TRUE;
  READ_LOCK(pe->PRWLock);
  if (pe->PredFlags & (CPredFlag|UserCPredFlag|TestPredFlag|AsmPredFlag|DynamicPredFlag|LogUpdatePredFlag)) {
    READ_UNLOCK(pe->PRWLock);
    return FALSE;
  }
  if (pe->OpcodeOfPred == UNDEF_OPCODE) {
    READ_UNLOCK(pe->PRWLock);
    return TRUE;
  }
  READ_UNLOCK(pe->PRWLock);
  return FALSE;
}

/*
 * this predicate should only be called when all clauses for the dynamic
 * predicate were remove, otherwise chaos will follow!! 
 */

static Int 
p_kill_dynamic(void)
{				/* '$kill_dynamic'(P,M)       */
  PredEntry      *pe;
  Term            t;
  Term            mod;

  mod = Deref(ARG2);
  if (IsVarTerm(mod)) {
    Yap_Error(INSTANTIATION_ERROR,ARG2,"undefined/1");
    return(FALSE);
  }
  if (!IsAtomTerm(mod)) {
    Yap_Error(TYPE_ERROR_ATOM,ARG2,"undefined/1");
    return(FALSE);
  }
  t = Deref(ARG1);
  if (IsAtomTerm(t)) {
    Atom at = AtomOfTerm(t);
    pe = RepPredProp(Yap_GetPredPropByAtom(at, mod));
  } else if (IsApplTerm(t)) {
    Functor         funt = FunctorOfTerm(t);
    pe = RepPredProp(Yap_GetPredPropByFunc(funt, mod));
  } else
    return (FALSE);
  if (EndOfPAEntr(pe))
    return TRUE;
  WRITE_LOCK(pe->PRWLock);
  if (!(pe->PredFlags & (DynamicPredFlag|LogUpdatePredFlag))) {
    WRITE_UNLOCK(pe->PRWLock);
    return (FALSE);
  }
  if (pe->cs.p_code.LastClause != pe->cs.p_code.FirstClause) {
    WRITE_UNLOCK(pe->PRWLock);
    return (FALSE);
  }
  pe->cs.p_code.LastClause = pe->cs.p_code.FirstClause = NULL;
  pe->OpcodeOfPred = UNDEF_OPCODE;
  pe->cs.p_code.TrueCodeOfPred = pe->CodeOfPred = (yamop *)(&(pe->OpcodeOfPred)); 
  pe->PredFlags = pe->PredFlags & GoalExPredFlag;
  WRITE_UNLOCK(pe->PRWLock);
  return (TRUE);
}

static Int 
p_optimizer_on(void)
{				/* '$optimizer_on'		 */
  optimizer_on = TRUE;
  return (TRUE);
}

static Int 
p_optimizer_off(void)
{				/* '$optimizer_off'		 */
  optimizer_on = FALSE;
  return (TRUE);
}

static Int 
p_compile_mode(void)
{				/* $compile_mode(Old,New)	 */
  Term            t2, t3 = MkIntTerm(compile_mode);
  if (!Yap_unify_constant(ARG1, t3))
    return (FALSE);
  t2 = Deref(ARG2);
  if (IsVarTerm(t2) || !IsIntTerm(t2))
    return (FALSE);
  compile_mode = IntOfTerm(t2) & 1;
  return (TRUE);
}

#if !defined(YAPOR) && !defined(THREADS)
static yamop *cur_clause(PredEntry *pe, yamop *codeptr)
{
  StaticClause *cl;

  cl = ClauseCodeToStaticClause(pe->cs.p_code.FirstClause);
  do {
    if (IN_BLOCK(codeptr,cl,cl->ClSize)) {
      return cl->ClCode;
    }
    if (cl->ClCode == pe->cs.p_code.LastClause)
      break;
    cl = cl->ClNext;
  } while (TRUE);
  Yap_Error(SYSTEM_ERROR,TermNil,"could not find clause for indexing code");
  return(NULL);
}

static yamop *cur_log_upd_clause(PredEntry *pe, yamop *codeptr)
{
  LogUpdClause *cl;
  cl = ClauseCodeToLogUpdClause(pe->cs.p_code.FirstClause);
  do {
    if (IN_BLOCK(codeptr,cl->ClCode,cl->ClSize)) {
      return((yamop *)cl->ClCode);
    }
    cl = cl->ClNext;
  } while (cl != NULL);
  Yap_Error(SYSTEM_ERROR,TermNil,"could not find clause for indexing code");
  return(NULL);
}

static Int
search_for_static_predicate_in_use(PredEntry *p, int check_everything)
{
  choiceptr b_ptr = B;
  CELL *env_ptr = ENV;

  if (check_everything && P) {
    PredEntry *pe = EnvPreg(P);
    if (p == pe) return TRUE;
    pe = EnvPreg(CP);
    if (p == pe) return TRUE;
  }
  do {
    PredEntry *pe;

    /* check first environments that are younger than our latest choicepoint */
    if (check_everything && env_ptr) {
      /* 
	 I do not need to check environments for asserts,
	 only for retracts
      */
      while (env_ptr && b_ptr > (choiceptr)env_ptr) {
	PredEntry *pe = EnvPreg(env_ptr[E_CP]);
	if (p == pe) return(TRUE);
	if (env_ptr != NULL)
	  env_ptr = (CELL *)(env_ptr[E_E]);
      }
    }
    /* now mark the choicepoint */
    
    if (b_ptr)
      pe = PredForChoicePt(b_ptr->cp_ap);
    else
      return FALSE;
    if (pe == p) {
      if (check_everything)
	return TRUE;
      READ_LOCK(pe->PRWLock);
      if (p->PredFlags & IndexedPredFlag) {
	yamop *code_p = b_ptr->cp_ap;
	yamop *code_beg = p->cs.p_code.TrueCodeOfPred;

	/* FIX ME */

	if (p->PredFlags & LogUpdatePredFlag) {
	  LogUpdIndex *cl = ClauseCodeToLogUpdIndex(code_beg);
	  if (find_owner_log_index(cl, code_p)) 
	    b_ptr->cp_ap = cur_log_upd_clause(pe, b_ptr->cp_ap->u.ld.d);
	} else if (p->PredFlags & MegaClausePredFlag) {
	  StaticIndex *cl = ClauseCodeToStaticIndex(code_beg);
	  if (find_owner_static_index(cl, code_p)) 
	    b_ptr->cp_ap = cur_clause(pe, b_ptr->cp_ap->u.ld.d);
	} else {
	  /* static clause */
	  StaticIndex *cl = ClauseCodeToStaticIndex(code_beg);
	  if (find_owner_static_index(cl, code_p)) {
	    b_ptr->cp_ap = cur_clause(pe, b_ptr->cp_ap->u.ld.d);
	  }
	}
      }
      READ_UNLOCK(pe->PRWLock);
    }
    env_ptr = b_ptr->cp_env;
    b_ptr = b_ptr->cp_b;
  } while (b_ptr != NULL);
  return(FALSE);
}

static void
mark_pred(int mark, PredEntry *pe)
{
  /* if the predicate is static mark it */
  if (pe->ModuleOfPred) {
    WRITE_LOCK(pe->PRWLock);
    if (mark) {
      pe->PredFlags |= InUsePredFlag;
    } else {
      pe->PredFlags &= ~InUsePredFlag;
    }
    WRITE_UNLOCK(pe->PRWLock);
  }
}

/* go up the chain of choice_points and environments,
   marking all static predicates that current execution is depending 
   upon */
static void
do_toggle_static_predicates_in_use(int mask)
{
  choiceptr b_ptr = B;
  CELL *env_ptr = ENV;

  if (b_ptr == NULL)
    return;

  do {
    PredEntry *pe;
    /* check first environments that are younger than our latest choicepoint */
    while (b_ptr > (choiceptr)env_ptr) {
      PredEntry *pe = EnvPreg(env_ptr[E_CP]);
      
      mark_pred(mask, pe);
      env_ptr = (CELL *)(env_ptr[E_E]);
    }
    /* now mark the choicepoint */
    if ((b_ptr)) {
      if ((pe = PredForChoicePt(b_ptr->cp_ap))) {
	mark_pred(mask, pe);
      }
    }
    env_ptr = b_ptr->cp_env;
    b_ptr = b_ptr->cp_b;
  } while (b_ptr != NULL);
  /* mark or unmark all predicates */
  STATIC_PREDICATES_MARKED = mask;
}

#endif /* !defined(YAPOR) && !defined(THREADS) */

static LogUpdIndex *
find_owner_log_index(LogUpdIndex *cl, yamop *code_p)
{
  yamop *code_beg = cl->ClCode;
  yamop *code_end = (yamop *)((char *)cl + cl->ClSize);
  
  if (code_p >= code_beg && code_p <= code_end) {
    return cl;
  }
  cl = cl->ChildIndex;
  while (cl != NULL) {
    LogUpdIndex *out;
    if ((out = find_owner_log_index(cl, code_p)) != NULL) {
      return out;
    }
    cl = cl->SiblingIndex;
  }
  return NULL;
}

static StaticIndex *
find_owner_static_index(StaticIndex *cl, yamop *code_p)
{
  yamop *code_beg = cl->ClCode;
  yamop *code_end = (yamop *)((char *)cl + cl->ClSize);
  
  if (code_p >= code_beg && code_p <= code_end) {
    return cl;
  }
  cl = cl->ChildIndex;
  while (cl != NULL) {
    StaticIndex *out;
    if ((out = find_owner_static_index(cl, code_p)) != NULL) {
      return out;
    }
    cl = cl->SiblingIndex;
  }
  return NULL;
}

ClauseUnion *
Yap_find_owner_index(yamop *ipc, PredEntry *ap)
{
  /* we assume we have an owner index */
  if (ap->PredFlags & LogUpdatePredFlag) {
    LogUpdIndex *cl = ClauseCodeToLogUpdIndex(ap->cs.p_code.TrueCodeOfPred);
    return (ClauseUnion *)find_owner_log_index(cl,ipc);
  } else {
    StaticIndex *cl = ClauseCodeToStaticIndex(ap->cs.p_code.TrueCodeOfPred);
    return (ClauseUnion *)find_owner_static_index(cl,ipc);
  }
}

static Term
all_envs(CELL *env_ptr)
{
  Term tf = AbsPair(H);
  CELL *bp = NULL;
  
  /* walk the environment chain */
  while (env_ptr != NULL) {
    bp = H;
    H += 2;
    /* notice that MkIntegerTerm may increase the Heap */
    bp[0] = MkIntegerTerm(LCL0-env_ptr);
    if (H >= ASP) {
      bp[1] = TermNil;
      return tf;
    } else {
      bp[1] = AbsPair(H);
    }
    env_ptr = (CELL *)(env_ptr[E_E]);      
  }
  bp[1] = TermNil;
  return tf;
}

static Term
all_cps(choiceptr b_ptr)
{
  CELL *bp = NULL;
  Term tf = AbsPair(H);

  while (b_ptr != NULL) {
    bp = H;
    H += 2;
    /* notice that MkIntegerTerm may increase the Heap */
    bp[0] = MkIntegerTerm((Int)(LCL0-(CELL *)b_ptr));
    if (H >= ASP) {
      bp[1] = TermNil;
      return tf;
    } else {
      bp[1] = AbsPair(H);
    }
    b_ptr = b_ptr->cp_b;
  }
  bp[1] = TermNil;
  return tf;
}


static Term
all_calls(void)
{
  Term ts[3];
  Functor f = Yap_MkFunctor(AtomLocal,3);

  ts[0] = MkIntegerTerm((Int)P);
  if (yap_flags[STACK_DUMP_ON_ERROR_FLAG]) {
    ts[1] = all_envs(ENV);
    ts[2] = all_cps(B);
  } else {
    ts[1] = ts[2] = TermNil;
  }
  return(Yap_MkApplTerm(f,3,ts));
}

Term
Yap_all_calls(void)
{
  return all_calls();
}

static Int
p_all_choicepoints(void)
{
  return Yap_unify(ARG1,all_cps(B));
}

static Int
p_all_envs(void)
{
  return Yap_unify(ARG1,all_envs(ENV));
}

static Int
p_current_stack(void)
{
#ifdef YAPOR
  return(FALSE);
#else
  return(Yap_unify(ARG1,all_calls()));
#endif
}

/* This predicate is to be used by reconsult to mark all predicates
   currently in use as being executed.

   The idea is to go up the chain of choice_points and environments.

 */
static Int
p_toggle_static_predicates_in_use(void)
{
#if !defined(YAPOR) && !defined(THREADS)
  Term t = Deref(ARG1);
  Int mask;
  
  /* find out whether we need to mark or unmark */
  if (IsVarTerm(t)) {
    Yap_Error(INSTANTIATION_ERROR,t,"toggle_static_predicates_in_use/1");
    return(FALSE);
  }
  if (!IsIntTerm(t)) {
    Yap_Error(TYPE_ERROR_INTEGER,t,"toggle_static_predicates_in_use/1");
    return(FALSE);
  }  else {
    mask = IntOfTerm(t);
  }
  do_toggle_static_predicates_in_use(mask);
#endif
  return TRUE;
}

static void
clause_was_found(PredEntry *pp, Atom *pat, UInt *parity) { 
  if (pp->ModuleOfPred == IDB_MODULE) {
    if (pp->PredFlags & NumberDBPredFlag) {
      *parity = 0;
      *pat = Yap_LookupAtom("integer");
    } else  if (pp->PredFlags & AtomDBPredFlag) {
      *parity = 0;
      *pat = (Atom)pp->FunctorOfPred;
    } else {
      *pat = NameOfFunctor(pp->FunctorOfPred);
      *parity = ArityOfFunctor(pp->FunctorOfPred);
    }
  } else {
    *parity = pp->ArityOfPE;
    if (pp->ArityOfPE) {
      *pat = NameOfFunctor(pp->FunctorOfPred);
    } else {
      *pat = (Atom)(pp->FunctorOfPred);
    }  
  }
}

static void
code_in_pred_info(PredEntry *pp, Atom *pat, UInt *parity) {
  clause_was_found(pp, pat, parity);
}

static int
code_in_pred_lu_index(LogUpdIndex *icl, yamop *codeptr, CODEADDR *startp, CODEADDR *endp) {
  LogUpdIndex *cicl;
  if (IN_BLOCK(codeptr,icl,icl->ClSize)) {
    if (startp) *startp = (CODEADDR)icl;
    if (endp) *endp = (CODEADDR)icl+icl->ClSize;
    return TRUE;
  }
  cicl = icl->ChildIndex;
  while (cicl != NULL) {
    if (code_in_pred_lu_index(cicl, codeptr, startp, endp))
      return TRUE;
    cicl = cicl->SiblingIndex;
  }
  return FALSE;
}

static int
code_in_pred_s_index(StaticIndex *icl, yamop *codeptr, CODEADDR *startp, CODEADDR *endp) {
  StaticIndex *cicl;
  if (IN_BLOCK(codeptr,icl,icl->ClSize)) {
    if (startp) *startp = (CODEADDR)icl;
    if (endp) *endp = (CODEADDR)icl+icl->ClSize;
    return TRUE;
  }
  cicl = icl->ChildIndex;
  while (cicl != NULL) {
    if (code_in_pred_s_index(cicl, codeptr, startp, endp))
      return TRUE;
    cicl = cicl->SiblingIndex;
  }
  return FALSE;
}

static Int
find_code_in_clause(PredEntry *pp, yamop *codeptr, CODEADDR *startp, CODEADDR *endp) {
  Int i = 1;
  yamop *clcode;

  clcode = pp->cs.p_code.FirstClause;
  if (clcode != NULL) {
    if (pp->PredFlags & LogUpdatePredFlag) {
      LogUpdClause *cl = ClauseCodeToLogUpdClause(clcode);
      do {
	if (IN_BLOCK(codeptr,(CODEADDR)cl,cl->ClSize)) {
	  if (startp)
	    *startp = (CODEADDR)cl;
	  if (endp)
	    *endp = (CODEADDR)cl+cl->ClSize;
	  return i;
	}
	i++;
	cl = cl->ClNext;
      } while (cl != NULL);
    } else if (pp->PredFlags & DynamicPredFlag) {
      do {
	DynamicClause *cl;
	
	cl = ClauseCodeToDynamicClause(clcode);
	if (IN_BLOCK(codeptr,cl,cl->ClSize)) {
	  if (startp)
	    *startp = (CODEADDR)cl;
	  if (endp)
	    *endp = (CODEADDR)cl+cl->ClSize;
	  return i;
	}
	if (clcode == pp->cs.p_code.LastClause)
	  break;
	i++;
	clcode = NextDynamicClause(clcode);
      } while (TRUE);
    } else if (pp->PredFlags & MegaClausePredFlag) {
      MegaClause *cl;
	
      cl = ClauseCodeToMegaClause(clcode);
      if (IN_BLOCK(codeptr,cl,cl->ClSize)) {
	  if (startp)
	    *startp = (CODEADDR)cl;
	  if (endp)
	    *endp = (CODEADDR)cl+cl->ClSize;
	return 1+((char *)codeptr-(char *)cl->ClCode)/cl->ClItemSize;
      }
    } else {
      StaticClause *cl;
	
      cl = ClauseCodeToStaticClause(clcode);
      do {
	if (IN_BLOCK(codeptr,cl,cl->ClSize)) {
	  if (startp)
	    *startp = (CODEADDR)cl;
	  if (endp)
	    *endp = (CODEADDR)cl+cl->ClSize;
	  return i;
	}
	if (cl->ClCode == pp->cs.p_code.LastClause)
	  break;
	i++;
	cl = cl->ClNext;
      } while (TRUE);
    }
  }
  return(0);
}

static int
cl_code_in_pred(PredEntry *pp, yamop *codeptr, CODEADDR *startp, CODEADDR *endp) {
  Int out;

  READ_LOCK(pp->PRWLock);
  /* check if the codeptr comes from the indexing code */
  if (pp->PredFlags & IndexedPredFlag) {
    if (pp->PredFlags & LogUpdatePredFlag) {
      if (code_in_pred_lu_index(ClauseCodeToLogUpdIndex(pp->cs.p_code.TrueCodeOfPred), codeptr, startp, endp)) {
	READ_UNLOCK(pp->PRWLock);
	return TRUE;
      }
    } else {
      if (code_in_pred_s_index(ClauseCodeToStaticIndex(pp->cs.p_code.TrueCodeOfPred), codeptr, startp, endp)) {
	READ_UNLOCK(pp->PRWLock);
	return TRUE;
      }
    }
  }
  if (pp->PredFlags & (CPredFlag|AsmPredFlag|UserCPredFlag)) {
    StaticClause *cl = ClauseCodeToStaticClause(pp->CodeOfPred);
    if (IN_BLOCK(codeptr,(CODEADDR)cl,cl->ClSize)) {
      if (startp)
	*startp = (CODEADDR)cl;
      if (endp)
	*endp = (CODEADDR)cl+cl->ClSize;
      return TRUE;
    } else {
      return FALSE;
    }
  } else {
    out = find_code_in_clause(pp, codeptr, startp, endp);
  }
  READ_UNLOCK(pp->PRWLock); 
  if (out) return TRUE;
  return FALSE;
}

static Int
code_in_pred(PredEntry *pp, Atom *pat, UInt *parity, yamop *codeptr) {
  Int out;

  READ_LOCK(pp->PRWLock);
  /* check if the codeptr comes from the indexing code */
  if (pp->PredFlags & IndexedPredFlag) {
    if (pp->PredFlags & LogUpdatePredFlag) {
      if (code_in_pred_lu_index(ClauseCodeToLogUpdIndex(pp->cs.p_code.TrueCodeOfPred), codeptr, NULL, NULL)) {
	code_in_pred_info(pp, pat, parity);
	READ_UNLOCK(pp->PRWLock);
	return -1;
      }
    } else {
      if (code_in_pred_s_index(ClauseCodeToStaticIndex(pp->cs.p_code.TrueCodeOfPred), codeptr, NULL, NULL)) {
	code_in_pred_info(pp, pat, parity);
	READ_UNLOCK(pp->PRWLock);
	return -1;
      }
    }
  }
  if ((out = find_code_in_clause(pp, codeptr, NULL, NULL))) {
    clause_was_found(pp, pat, parity);
  }
  READ_UNLOCK(pp->PRWLock); 
  return out;
}

static Int
PredForCode(yamop *codeptr, Atom *pat, UInt *parity, Term *pmodule) {
  Int found = 0;
  ModEntry *me = CurrentModules;

  /* should we allow the user to see hidden predicates? */
  while (me) {

    PredEntry *pp;
    pp = me->PredForME;
    while (pp != NULL) {
      if ((found = code_in_pred(pp,  pat, parity, codeptr)) != 0) {
	*pmodule = MkAtomTerm(me->AtomOfME);
	return found;
      }
      pp = pp->NextPredOfModule;
    }
    me = me->NextME;
  }
  return(0);
}

Int
Yap_PredForCode(yamop *codeptr, find_pred_type where_from, Atom *pat, UInt *parity, Term *pmodule) {
  PredEntry *p;

  if (where_from == FIND_PRED_FROM_CP) {
    p = PredForChoicePt(codeptr);
  } else if (where_from == FIND_PRED_FROM_ENV) {
    p = EnvPreg(codeptr);
    if (p) {
      Int out;
      if (p->ModuleOfPred == PROLOG_MODULE)
	*pmodule = TermProlog;
      else
	*pmodule = p->ModuleOfPred;
      out = find_code_in_clause(p, codeptr, NULL, NULL); 
      clause_was_found(p, pat, parity);
      return out;
    }
  } else {
    return PredForCode(codeptr, pat, parity, pmodule);
  }
  if (p == NULL) {
    return 0;
  }
  clause_was_found(p, pat, parity);
  if (p->ModuleOfPred == PROLOG_MODULE)
    *pmodule = TermProlog;
  else
    *pmodule = p->ModuleOfPred;
  return -1;
}

static PredEntry *
ClauseInfoForCode(yamop *codeptr, CODEADDR *startp, CODEADDR *endp) {
  yamop *pc;
  PredEntry *pp = NULL;
  int clause_code = FALSE;

  if (codeptr >= COMMA_CODE &&
	codeptr < FAILCODE) {
    pp = RepPredProp(Yap_GetPredPropByFunc(FunctorComma,CurrentModule));
    *startp = (CODEADDR)COMMA_CODE;
    *endp = (CODEADDR)(FAILCODE-1);
    return pp;
  }
  pc = codeptr;
  while (TRUE) {
    op_numbers op;

    op = Yap_op_from_opcode(pc->opc);
    /* C-code, maybe indexing */
    switch (op) {
    case _Nstop:
      return NULL;
    case _Ystop:
      if (pc == YESCODE) {
	pp = RepPredProp(Yap_GetPredPropByAtom(AtomTrue,CurrentModule));
	*startp = (CODEADDR)YESCODE;
	*endp = (CODEADDR)YESCODE; /*+(CELL)(NEXTOP((yamop *)NULL,e));*/
	return pp;
      }
      if (!pp) {
	/* must be an index */
	PredEntry **pep = (PredEntry **)pc->u.l.l;
	pp = pep[-1];
      }
      if (pp->PredFlags & LogUpdatePredFlag) {
	if (clause_code) {
	  LogUpdClause *cl = ClauseCodeToLogUpdClause(pc->u.l.l);
	  *startp = (CODEADDR)cl;
	  *endp = (CODEADDR)cl+cl->ClSize;
	} else {
	  LogUpdIndex *cl = ClauseCodeToLogUpdIndex(pc->u.l.l);
	  *startp = (CODEADDR)cl;
	  *endp = (CODEADDR)cl+cl->ClSize;
	}
      } else if (pp->PredFlags & DynamicPredFlag) {
	  DynamicClause *cl = ClauseCodeToDynamicClause(pc->u.l.l);
	  *startp = (CODEADDR)cl;
	  *endp = (CODEADDR)cl+cl->ClSize;
      } else {
	if (clause_code) {
	  StaticClause *cl = ClauseCodeToStaticClause(pc->u.l.l);
	  *startp = (CODEADDR)cl;
	  *endp = (CODEADDR)cl+cl->ClSize;
	} else {
	  StaticIndex *cl = ClauseCodeToStaticIndex(pc->u.l.l);
	  *startp = (CODEADDR)cl;
	  *endp = (CODEADDR)cl+cl->ClSize;
	}
      }
      return pp;
      /* instructions type ld */
    case _try_me:
    case _retry_me:
    case _trust_me:
    case _profiled_retry_me:
    case _profiled_trust_me:
    case _count_retry_me:
    case _count_trust_me:
    case _spy_or_trymark:
    case _try_and_mark:
    case _profiled_retry_and_mark:
    case _count_retry_and_mark:
    case _retry_and_mark:
    case _try_clause:
    case _retry:
    case _trust:
#ifdef YAPOR
    case _getwork:
    case _getwork_seq:
    case _sync:
#endif
#ifdef TABLING
    case _table_load_answer:
    case _table_try_answer:
    case _table_try_single:
    case _table_try_me:
    case _table_retry_me:
    case _table_trust_me:
    case _table_try:
    case _table_retry:
    case _table_trust:
    case _table_answer_resolution:
    case _table_completion:
#endif /* TABLING */
      pp = pc->u.ld.p;
      pc = NEXTOP(pc,ld);
      break;
    case _try_logical:
    case _retry_logical:
    case _trust_logical:
    case _count_retry_logical:
    case _count_trust_logical:
    case _profiled_retry_logical:
    case _profiled_trust_logical:
      pp = pc->u.lld.d->ClPred;
      pc = pc->u.lld.n;
      break;
    case _enter_lu_pred:
      pc = pc->u.Ill.l1;
      break;
      /* instructions type p */
    case _count_call:
    case _count_retry:
    case _enter_profiling:
    case _retry_profiled:
      pc = NEXTOP(pc,p);
      break;
#if !defined(YAPOR)
    case _or_last:
#endif
    case _procceed:
    case _lock_lu:
      pp = pc->u.p.p;
      if (pp->PredFlags & MegaClausePredFlag) {
	MegaClause *mcl = ClauseCodeToMegaClause(pp->cs.p_code.FirstClause);
	*startp = (CODEADDR)mcl;
	*endp = (CODEADDR)mcl+mcl->ClSize;
	return pp;
      }
      clause_code = TRUE;
      pc = NEXTOP(pc,p);
      break;
    case _execute:
    case _dexecute:
      clause_code = TRUE;
      pp = pc->u.pp.p0;
      pc = NEXTOP(pc,pp);
      break;
    case _jump:
    case _move_back:
    case _skip:
    case _jump_if_var:
    case _try_in:
    case _try_clause2:
    case _try_clause3:
    case _try_clause4:
    case _retry2:
    case _retry3:
    case _retry4:
    case _p_eq:
    case _p_dif:
      pc = NEXTOP(pc,l);
      break;
      /* instructions type EC */
    case _jump_if_nonvar:
      pc = NEXTOP(pc,xll);
      break;
      /* instructions type EC */
    case _alloc_for_logical_pred:
      {
	LogUpdClause *cl = pc->u.EC.ClBase;

	*startp = (CODEADDR)cl;
	*endp = (CODEADDR)cl+cl->ClSize;
	return cl->ClPred;
      }
      /* instructions type e */
    case _unify_idb_term:
    case _copy_idb_term:
      {
	LogUpdClause *cl = ClauseCodeToLogUpdClause(pc);

	*startp = (CODEADDR)cl;
	*endp = (CODEADDR)cl+cl->ClSize;
	return cl->ClPred;
      }
    case _cut:
    case _cut_t:
    case _cut_e:
    case _allocate:
    case _deallocate:
    case _write_void:
    case _write_list:
    case _write_l_list:
    case _pop:
#ifdef BEAM
    case _retry_eam:
#endif
#if THREADS
    case _thread_local:
#endif
    case _p_equal:
    case _p_functor:
    case _enter_a_profiling:
    case _count_a_call:
    case _index_dbref:
    case _index_blob:
    case _unlock_lu:
#ifdef YAPOR
    case _getwork_first_time:
#endif
#ifdef TABLING
    case _trie_do_null:
    case _trie_trust_null:
    case _trie_try_null:
    case _trie_retry_null:
    case _trie_do_var:
    case _trie_trust_var:
    case _trie_try_var:
    case _trie_retry_var:
    case _trie_do_val:
    case _trie_trust_val:
    case _trie_try_val:
    case _trie_retry_val:
    case _trie_do_atom:
    case _trie_trust_atom:
    case _trie_try_atom:
    case _trie_retry_atom:
    case _trie_do_list:
    case _trie_trust_list:
    case _trie_try_list:
    case _trie_retry_list:
    case _trie_do_struct:
    case _trie_trust_struct:
    case _trie_try_struct:
    case _trie_retry_struct:
    case _trie_do_extension:
    case _trie_trust_extension:
    case _trie_try_extension:
    case _trie_retry_extension:
    case _trie_do_float:
    case _trie_trust_float:
    case _trie_try_float:
    case _trie_retry_float:
    case _trie_do_long:
    case _trie_trust_long:
    case _trie_try_long:
    case _trie_retry_long:
#endif /* TABLING */
#ifdef TABLING_INNER_CUTS
    case _clause_with_cut:
#endif /* TABLING_INNER_CUTS */
      pc = NEXTOP(pc,e);
      break;
      /* instructions type x */
    case _save_b_x:
    case _commit_b_x:
    case _get_list:
    case _put_list:
    case _write_x_var:
    case _write_x_val:
    case _write_x_loc:
      pc = NEXTOP(pc,x);
      break;
      /* instructions type xF */
    case _p_atom_x:
    case _p_atomic_x:
    case _p_integer_x:
    case _p_nonvar_x:
    case _p_number_x:
    case _p_var_x:
    case _p_db_ref_x:
    case _p_primitive_x:
    case _p_compound_x:
    case _p_float_x:
    case _p_cut_by_x:
      pc = NEXTOP(pc,xF);
      break;
      /* instructions type y */
    case _save_b_y:
    case _commit_b_y:
    case _write_y_var:
    case _write_y_val: 
    case _write_y_loc:
      pc = NEXTOP(pc,y);
      break;
      /* instructions type yF */
    case _p_atom_y:
    case _p_atomic_y:
    case _p_integer_y:
    case _p_nonvar_y:
    case _p_number_y:
    case _p_var_y:
    case _p_db_ref_y:
    case _p_primitive_y:
    case _p_compound_y:
    case _p_float_y:
    case _p_cut_by_y:
      pc = NEXTOP(pc,yF);
      break;
      /* instructions type sla */      
    case _p_execute_tail:
    case _p_execute:
      clause_code = TRUE;
      pp = RepPredProp(Yap_GetPredPropByFunc(FunctorCall, CurrentModule));
      *startp = (CODEADDR)&(pp->OpcodeOfPred);
      *endp = (CODEADDR)NEXTOP((yamop *)&(pp->OpcodeOfPred),e);
      return pp;
    case _fcall:
    case _call:
#ifdef YAPOR
    case _or_last:
#endif
      clause_code = TRUE;
      pp = pc->u.sla.sla_u.p;
      pc = NEXTOP(pc,sla);
      break;
      /* instructions type sla, but for disjunctions */
    case _either:
    case _or_else:
    case _call_cpred:
    case _call_usercpred:
      clause_code = TRUE;
      pp = pc->u.sla.p0;
      pc = NEXTOP(pc,sla);
      break;
      /* instructions type xx */
    case _get_x_var:
    case _get_x_val:
    case _glist_valx:
    case _gl_void_varx:
    case _gl_void_valx:
    case _put_x_var:
    case _put_x_val:
      pc = NEXTOP(pc,xx);
      break;
    case _put_xx_val:
      pc = NEXTOP(pc,xxxx);
      break;
      /* instructions type yx */
    case _get_y_var:
    case _get_y_val:
    case _put_y_var:
    case _put_y_val:
    case _put_unsafe:
      pc = NEXTOP(pc,yx);
      break;
      /* instructions type xd */
    case _get_float:
    case _put_float:
      pc = NEXTOP(pc,xd);
      break;
      /* instructions type xi */
    case _get_longint:
    case _put_longint:
      pc = NEXTOP(pc,xi);
      break;
      /* instructions type xc */
    case _get_atom:
    case _put_atom:
    case _get_bigint:
      pc = NEXTOP(pc,xc);
      break;
      /* instructions type cc */
    case _get_2atoms:
      pc = NEXTOP(pc,cc);
      break;
      /* instructions type ccc */
    case _get_3atoms:
      pc = NEXTOP(pc,ccc);
      break;
      /* instructions type cccc */
    case _get_4atoms:
      pc = NEXTOP(pc,cccc);
      break;
      /* instructions type ccccc */
    case _get_5atoms:
      pc = NEXTOP(pc,ccccc);
      break;
      /* instructions type cccccc */
    case _get_6atoms:
      pc = NEXTOP(pc,cccccc);
      break;
      /* instructions type xf */
    case _get_struct:
    case _put_struct:
      pc = NEXTOP(pc,xf);
      break;
      /* instructions type xy */
    case _glist_valy:
    case _gl_void_vary:
    case _gl_void_valy:
      pc = NEXTOP(pc,xy);
      break;
      /* instructions type ox */
    case _unify_x_var:
    case _unify_x_var_write:
    case _unify_l_x_var:
    case _unify_l_x_var_write:
    case _unify_x_val_write:
    case _unify_x_val:
    case _unify_l_x_val_write:
    case _unify_l_x_val:
    case _unify_x_loc_write:
    case _unify_x_loc:
    case _unify_l_x_loc_write:
    case _unify_l_x_loc:
    case _save_pair_x_write:
    case _save_pair_x:
    case _save_appl_x_write:
    case _save_appl_x:
      pc = NEXTOP(pc,ox);
      break;
      /* instructions type oxx */
    case _unify_x_var2:
    case _unify_x_var2_write:
    case _unify_l_x_var2:
    case _unify_l_x_var2_write:
      pc = NEXTOP(pc,oxx);
      break;
      /* instructions type oy */
    case _unify_y_var:
    case _unify_y_var_write:
    case _unify_l_y_var:
    case _unify_l_y_var_write:
    case _unify_y_val_write:
    case _unify_y_val:
    case _unify_l_y_val_write:
    case _unify_l_y_val:
    case _unify_y_loc_write:
    case _unify_y_loc:
    case _unify_l_y_loc_write:
    case _unify_l_y_loc:
    case _save_pair_y_write:
    case _save_pair_y:
    case _save_appl_y_write:
    case _save_appl_y:
      pc = NEXTOP(pc,oy);
      break;
      /* instructions type o */
    case _unify_void_write:
    case _unify_void:
    case _unify_l_void_write:
    case _unify_l_void:
    case _unify_list_write:
    case _unify_list:
    case _unify_l_list_write:
    case _unify_l_list:
      pc = NEXTOP(pc,o);
      break;
      /* instructions type os */
    case _unify_n_voids_write:
    case _unify_n_voids:
    case _unify_l_n_voids_write:
    case _unify_l_n_voids:
      pc = NEXTOP(pc,os);
      break;
      /* instructions type od */
    case _unify_float:
    case _unify_l_float:
    case _unify_float_write:
    case _unify_l_float_write:
      pc = NEXTOP(pc,od);
      break;
    case _unify_longint:
    case _unify_l_longint:
    case _unify_longint_write:
    case _unify_l_longint_write:
      pc = NEXTOP(pc,oi);
      break;
      /* instructions type oc */
    case _unify_atom_write:
    case _unify_atom:
    case _unify_l_atom_write:
    case _unify_l_atom:
    case _unify_bigint:
    case _unify_l_bigint:
      pc = NEXTOP(pc,oc);
      break;
      /* instructions type osc */
    case _unify_n_atoms_write:
    case _unify_n_atoms:
      pc = NEXTOP(pc,osc);
      break;
      /* instructions type of */
    case _unify_struct_write:
    case _unify_struct:
    case _unify_l_struc_write:
    case _unify_l_struc:
      pc = NEXTOP(pc,of);
      break;
      /* instructions type s */
    case _write_n_voids:
    case _pop_n:
#ifdef BEAM
    case _run_eam:
#endif
#ifdef TABLING
    case _table_new_answer:
#endif /* TABLING */
      pc = NEXTOP(pc,s);
      break;
      /* instructions type c */
   case _write_atom:
      pc = NEXTOP(pc,c);
      break;
      /* instructions type d */
   case _write_float:
      pc = NEXTOP(pc,d);
      break;
      /* instructions type i */
   case _write_longint:
      pc = NEXTOP(pc,i);
      break;
      /* instructions type sc */
   case _write_n_atoms:
      pc = NEXTOP(pc,sc);
      break;
      /* instructions type f */
   case _write_struct:
   case _write_l_struc:
      pc = NEXTOP(pc,f);
      break;
      /* instructions type sdl */
    case _call_c_wfail:
      clause_code = TRUE;
      pp = pc->u.sdl.p;
      pc = NEXTOP(pc,sdl);
      break;
      /* instructions type lds */
    case _try_c:
    case _try_userc:
    case _retry_c:
    case _retry_userc:
      clause_code = TRUE;
      pp = pc->u.lds.p;
      pc = NEXTOP(pc,lds);
      break;
#ifdef CUT_C
    case _cut_c:
    case _cut_userc:
      /* don't need to do nothing here, because this two instructions 
       are "phantom" instructions. (see: cut_c implementation paper 
       on PADL 2006) */
      break;
#endif 
      /* instructions type llll */
    case _switch_on_type:
      pc = NEXTOP(pc,llll);
      break;
      /* instructions type ollll */
    case _switch_list_nl:
      pc = NEXTOP(pc,ollll);
      break;
      /* instructions type xllll */
    case _switch_on_arg_type:
      pc = NEXTOP(pc,xllll);
      break;
      /* instructions type sllll */
    case _switch_on_sub_arg_type:
      pc = NEXTOP(pc,sllll);
      break;
      /* instructions type clll */
    case _if_not_then:
      pc = NEXTOP(pc,clll);
      break;
      /* switch_on_func */
    case _switch_on_func:
    case _switch_on_cons:
    case _go_on_func:
    case _go_on_cons:
    case _if_func:
    case _if_cons:
      pc = NEXTOP(pc,sssl);
      break;
      /* instructions type xxx */
    case _p_plus_vv:
    case _p_minus_vv:
    case _p_times_vv:
    case _p_div_vv:
    case _p_and_vv:
    case _p_or_vv:
    case _p_sll_vv:
    case _p_slr_vv:
    case _p_arg_vv:
    case _p_func2s_vv:
    case _p_func2f_xx:
      clause_code = TRUE;
      pc = NEXTOP(pc,xxx);
      break;
      /* instructions type xxc */
    case _p_plus_vc:
    case _p_minus_cv:
    case _p_times_vc:
    case _p_div_cv:
    case _p_and_vc:
    case _p_or_vc:
    case _p_sll_vc:
    case _p_slr_vc:
    case _p_func2s_vc:
      clause_code = TRUE;
      pc = NEXTOP(pc,xxc);
      break;
    case _p_div_vc:
    case _p_sll_cv:
    case _p_slr_cv:
    case _p_arg_cv:
      clause_code = TRUE;
      pc = NEXTOP(pc,xcx);
      break;
    case _p_func2s_cv:
      clause_code = TRUE;
      pc = NEXTOP(pc,xcx);
      break;
      /* instructions type xyx */
    case _p_func2f_xy:
      clause_code = TRUE;
      pc = NEXTOP(pc,xyx);
      break;
      /* instructions type yxx */
    case _p_plus_y_vv:
    case _p_minus_y_vv:
    case _p_times_y_vv:
    case _p_div_y_vv:
    case _p_and_y_vv:
    case _p_or_y_vv:
    case _p_sll_y_vv:
    case _p_slr_y_vv:
    case _p_arg_y_vv:
    case _p_func2s_y_vv:
    case _p_func2f_yx:
      clause_code = TRUE;
      pc = NEXTOP(pc,yxx);
      break;
      /* instructions type yyx */
    case _p_func2f_yy:
      clause_code = TRUE;
      pc = NEXTOP(pc,yyx);
      break;
      /* instructions type yxc */
    case _p_plus_y_vc:
    case _p_minus_y_cv:
    case _p_times_y_vc:
    case _p_div_y_vc:
    case _p_div_y_cv:
    case _p_and_y_vc:
    case _p_or_y_vc:
    case _p_sll_y_vc:
    case _p_slr_y_vc:
    case _p_func2s_y_vc:
      clause_code = TRUE;
      pc = NEXTOP(pc,yxc);
      break;
      /* instructions type ycx */
    case _p_sll_y_cv:
    case _p_slr_y_cv:
    case _p_arg_y_cv:
      clause_code = TRUE;
      pc = NEXTOP(pc,ycx);
      break;
      /* instructions type ycx */
    case _p_func2s_y_cv:
      clause_code = TRUE;
      pc = NEXTOP(pc,ycx);
      break;
      /* instructions type llxx */
    case _call_bfunc_xx:
      clause_code = TRUE;
      pc = NEXTOP(pc,llxx);
      break;
      /* instructions type llxy */
    case _call_bfunc_yx:
    case _call_bfunc_xy:
      clause_code = TRUE;
      pc = NEXTOP(pc,llxy);
      break;
    case _call_bfunc_yy:
      clause_code = TRUE;
      pc = NEXTOP(pc,llyy);
      break;
    case _expand_index:
      pp = ((PredEntry *)(Unsigned(pc)-(CELL)(&(((PredEntry *)NULL)->cs.p_code.ExpandCode))));
      if (pc == codeptr) {
	*startp = (CODEADDR)&(pp->cs.p_code.ExpandCode);
	*endp = (CODEADDR)&(pp->cs.p_code.ExpandCode);
      }
      return pp;
    case _undef_p:
    case _spy_pred:
    case _index_pred:
      pp = ((PredEntry *)(Unsigned(pc)-(CELL)(&(((PredEntry *)NULL)->OpcodeOfPred))));
      *startp = (CODEADDR)&(pp->OpcodeOfPred);
      *endp = (CODEADDR)NEXTOP((yamop *)&(pp->OpcodeOfPred),e);
      return pp;
    case _expand_clauses:
      /* expansion points may not be found when following the indices tree */
      pp = codeptr->u.sp.p;
      if (pc == codeptr) {
	*startp = (CODEADDR)codeptr;
	*endp = (CODEADDR)NEXTOP(codeptr,sp);
      }
      return pp;
    case _op_fail:
      if (codeptr == FAILCODE) {
	pp = RepPredProp(Yap_GetPredPropByAtom(AtomFail,CurrentModule));
	*startp = *endp = (CODEADDR)FAILCODE;
	return pp;
      }
      pc = NEXTOP(pc,e);
      break;
    case _trust_fail:
      if (codeptr == TRUSTFAILCODE) {
	pp = RepPredProp(Yap_GetPredPropByAtom(AtomFail,CurrentModule));
	*startp = *endp = (CODEADDR)TRUSTFAILCODE;
	return pp;
      }
      pc = NEXTOP(pc,e);
      break;
    }
  }
  return NULL;
}

PredEntry *
Yap_PredEntryForCode(yamop *codeptr, find_pred_type where_from, CODEADDR *startp, CODEADDR *endp) {

  if (where_from == FIND_PRED_FROM_CP) {
    PredEntry *pp = PredForChoicePt(codeptr);
    if (cl_code_in_pred(pp, codeptr, startp, endp)) {
      return pp;
    }
  } else if (where_from == FIND_PRED_FROM_ENV) {
    PredEntry *pp = EnvPreg(codeptr);
    if (cl_code_in_pred(pp, codeptr, startp, endp)) {
      return pp;
    }
  } else {
    return ClauseInfoForCode(codeptr, startp, endp);
  }
  return NULL;
}


static Int
p_pred_for_code(void) {
  yamop *codeptr;
  Atom at;
  UInt arity;
  Term tmodule = TermProlog;
  Int cl;
  Term t = Deref(ARG1);

  if (IsVarTerm(t)) {
    return FALSE;
  } else if (IsApplTerm(t) && FunctorOfTerm(t) == FunctorStaticClause) {
    codeptr  = Yap_ClauseFromTerm(t)->ClCode;
  } else if (IsIntegerTerm(t)) {
    codeptr  = (yamop *)IntegerOfTerm(t);
  } else if (IsDBRefTerm(t)) {
    codeptr  = (yamop *)DBRefOfTerm(t);
  } else {
    return FALSE;
  }
  cl = PredForCode(codeptr, &at, &arity, &tmodule);
  if (!tmodule) tmodule = TermProlog;
  if (cl == 0) {
    return Yap_unify(ARG5,MkIntTerm(0));
  } else {
    return(Yap_unify(ARG2,MkAtomTerm(at)) &&
	   Yap_unify(ARG3,MkIntegerTerm(arity)) &&
	   Yap_unify(ARG4,tmodule) &&
	   Yap_unify(ARG5,MkIntegerTerm(cl)));
  }
}

static Int
p_is_profiled(void)
{
  Term t = Deref(ARG1);
  char *s;

  if (IsVarTerm(t)) {
    Term ta;

    if (PROFILING) ta = MkAtomTerm(Yap_LookupAtom("on"));
    else ta = MkAtomTerm(Yap_LookupAtom("off"));
    BIND((CELL *)t,ta,bind_is_profiled);
#ifdef COROUTINING
    DO_TRAIL(CellPtr(t), ta);
    if (CellPtr(t) < H0) Yap_WakeUp((CELL *)t);
  bind_is_profiled:
#endif
    return(TRUE);
  } else if (!IsAtomTerm(t)) return(FALSE);
  s = RepAtom(AtomOfTerm(t))->StrOfAE;
  if (strcmp(s,"on") == 0) {
    PROFILING = TRUE;
    Yap_InitComma();
    return(TRUE);
  } else if (strcmp(s,"off") == 0) {
    PROFILING = FALSE;
    Yap_InitComma();
    return(TRUE);
  }
  return(FALSE);
}

static Int
p_profile_info(void)
{
  Term mod = Deref(ARG1);
  Term tfun = Deref(ARG2);
  Term out;
  PredEntry *pe;
  Term p[3];

  if (IsVarTerm(mod) || !IsAtomTerm(mod))
    return(FALSE);
  if (IsVarTerm(tfun)) {
    return(FALSE);
  } else if (IsApplTerm(tfun)) {
    Functor f = FunctorOfTerm(tfun);
    if (IsExtensionFunctor(f)) {
      return(FALSE);
    }
    pe = RepPredProp(Yap_GetPredPropByFunc(f, mod));
  } else if (IsAtomTerm(tfun)) {
    pe = RepPredProp(Yap_GetPredPropByAtom(AtomOfTerm(tfun), mod));
  } else {
    return(FALSE);
  }
  if (EndOfPAEntr(pe))
    return(FALSE);
  LOCK(pe->StatisticsForPred.lock);
  if (!(pe->StatisticsForPred.NOfEntries)) {
    UNLOCK(pe->StatisticsForPred.lock);
    return(FALSE);
  }
  p[0] = Yap_MkULLIntTerm(pe->StatisticsForPred.NOfEntries);
  p[1] = Yap_MkULLIntTerm(pe->StatisticsForPred.NOfHeadSuccesses);
  p[2] = Yap_MkULLIntTerm(pe->StatisticsForPred.NOfRetries);
  UNLOCK(pe->StatisticsForPred.lock);
  out = Yap_MkApplTerm(Yap_MkFunctor(AtomProfile,3),3,p);
  return(Yap_unify(ARG3,out));
}

static Int
p_profile_reset(void)
{
  Term mod = Deref(ARG1);
  Term tfun = Deref(ARG2);
  PredEntry *pe;

  if (IsVarTerm(mod) || !IsAtomTerm(mod))
    return(FALSE);
  if (IsVarTerm(tfun)) {
    return(FALSE);
  } else if (IsApplTerm(tfun)) {
    Functor f = FunctorOfTerm(tfun);
    if (IsExtensionFunctor(f)) {
      return(FALSE);
    }
    pe = RepPredProp(Yap_GetPredPropByFunc(f, mod));
  } else if (IsAtomTerm(tfun)) {
    pe = RepPredProp(Yap_GetPredPropByAtom(AtomOfTerm(tfun), mod));
  } else {
    return(FALSE);
  }
  if (EndOfPAEntr(pe))
    return(FALSE);
  LOCK(pe->StatisticsForPred.lock);
  pe->StatisticsForPred.NOfEntries = 0;
  pe->StatisticsForPred.NOfHeadSuccesses = 0;
  pe->StatisticsForPred.NOfRetries = 0;
  UNLOCK(pe->StatisticsForPred.lock);
  return(TRUE);
}

static Int
p_is_call_counted(void)
{
  Term t = Deref(ARG1);
  char *s;

  if (IsVarTerm(t)) {
    Term ta;

    if (CALL_COUNTING) ta = MkAtomTerm(Yap_LookupAtom("on"));
    else ta = MkAtomTerm(Yap_LookupAtom("off"));
    BIND((CELL *)t,ta,bind_is_call_counted);
#ifdef COROUTINING
    DO_TRAIL(CellPtr(t), ta);
    if (CellPtr(t) < H0) Yap_WakeUp((CELL *)t);
  bind_is_call_counted:
#endif
    return(TRUE);
  } else if (!IsAtomTerm(t)) return(FALSE);
  s = RepAtom(AtomOfTerm(t))->StrOfAE;
  if (strcmp(s,"on") == 0) {
    CALL_COUNTING = TRUE;
    Yap_InitComma();
    return(TRUE);
  } else if (strcmp(s,"off") == 0) {
    CALL_COUNTING = FALSE;
    Yap_InitComma();
    return(TRUE);
  }
  return(FALSE);
}

static Int
p_call_count_info(void)
{
  return(Yap_unify(MkIntegerTerm(ReductionsCounter),ARG1) &&
	 Yap_unify(MkIntegerTerm(PredEntriesCounter),ARG2) &&
	 Yap_unify(MkIntegerTerm(PredEntriesCounter),ARG3));
}

static Int
p_call_count_reset(void)
{
  ReductionsCounter = 0;
  ReductionsCounterOn = FALSE;
  PredEntriesCounter = 0;
  PredEntriesCounterOn = FALSE;
  RetriesCounter = 0;
  RetriesCounterOn = FALSE;
  return(TRUE);
}

static Int
p_call_count_set(void)
{
  int do_calls = IntOfTerm(ARG2);
  int do_retries = IntOfTerm(ARG4);
  int do_entries = IntOfTerm(ARG6);

  if (do_calls)
    ReductionsCounter = IntegerOfTerm(Deref(ARG1));
  ReductionsCounterOn = do_calls;
  if (do_retries)
    RetriesCounter = IntegerOfTerm(Deref(ARG3));
  RetriesCounterOn = do_retries;
  if (do_entries)
    PredEntriesCounter = IntegerOfTerm(Deref(ARG5));
  PredEntriesCounterOn = do_entries;
  return(TRUE);
}

static Int
p_clean_up_dead_clauses(void)
{
  while (DeadStaticClauses != NULL) {
    char *pt = (char *)DeadStaticClauses;
    Yap_ClauseSpace -= DeadStaticClauses->ClSize;
    DeadStaticClauses = DeadStaticClauses->ClNext;
    Yap_InformOfRemoval((CODEADDR)pt);
    Yap_FreeCodeSpace(pt);
  }
  while (DeadStaticIndices != NULL) {
    char *pt = (char *)DeadStaticIndices;
    if (DeadStaticIndices->ClFlags & SwitchTableMask)
      Yap_IndexSpace_SW -= DeadStaticIndices->ClSize;
    else
      Yap_IndexSpace_Tree -= DeadStaticIndices->ClSize;
    DeadStaticIndices = DeadStaticIndices->SiblingIndex;
    Yap_InformOfRemoval((CODEADDR)pt);
    Yap_FreeCodeSpace(pt);
  }
  while (DeadMegaClauses != NULL) {
    char *pt = (char *)DeadMegaClauses;
    Yap_ClauseSpace -= DeadMegaClauses->ClSize;
    DeadMegaClauses = DeadMegaClauses->ClNext;
    Yap_InformOfRemoval((CODEADDR)pt);
    Yap_FreeCodeSpace(pt);
  }
  return TRUE;
}

static Int			/* $parent_pred(Module, Name, Arity) */
p_parent_pred(void)
{
  /* This predicate is called from the debugger.
     We assume a sequence of the form a -> b */
  Atom at;
  UInt arity;
  Term module;
  if (!PredForCode(P_before_spy, &at, &arity, &module)) {
    return(Yap_unify(ARG1, MkIntTerm(0)) &&
	   Yap_unify(ARG2, MkAtomTerm(AtomMetaCall)) &&
	   Yap_unify(ARG3, MkIntTerm(0)));
  }
  return(Yap_unify(ARG1, MkIntTerm(module)) &&
	 Yap_unify(ARG2, MkAtomTerm(at)) &&
	 Yap_unify(ARG3, MkIntTerm(arity)));
}

static Int			/* $system_predicate(P) */
p_system_pred(void)
{
  PredEntry      *pe;

  Term t1 = Deref(ARG1);
  Term mod = Deref(ARG2);

 restart_system_pred:
  if (IsVarTerm(t1))
    return FALSE;
  if (IsAtomTerm(t1)) {
    pe = RepPredProp(Yap_GetPredPropByAtom(AtomOfTerm(t1), mod));
  } else if (IsApplTerm(t1)) {
    Functor         funt = FunctorOfTerm(t1);
    if (IsExtensionFunctor(funt)) {
      return FALSE;
    } 
    if (funt == FunctorModule) {
      Term nmod = ArgOfTerm(1, t1);
      if (IsVarTerm(nmod)) {
	Yap_Error(INSTANTIATION_ERROR,ARG1,"system_predicate/1");
	return FALSE;
      } 
      if (!IsAtomTerm(nmod)) {
	Yap_Error(TYPE_ERROR_ATOM,ARG1,"system_predicate/1");
	return FALSE;
      }
      t1 = ArgOfTerm(2, t1);
      goto restart_system_pred;
    }
    pe = RepPredProp(Yap_GetPredPropByFunc(funt, mod));
  } else if (IsPairTerm(t1)) {
    return TRUE;
  } else
    return FALSE;
  if (EndOfPAEntr(pe))
    return FALSE;
  return(!pe->ModuleOfPred || /* any predicate in prolog module */
	 /* any C-pred */
	 pe->PredFlags & (UserCPredFlag|CPredFlag|BinaryTestPredFlag|AsmPredFlag|TestPredFlag) ||
	 /* any weird user built-in */
	 pe->OpcodeOfPred == Yap_opcode(_try_userc));
}

static Int			/* $system_predicate(P) */
p_hide_predicate(void)
{
  PredEntry      *pe;

  Term t1 = Deref(ARG1);
  Term mod = Deref(ARG2);

 restart_system_pred:
  if (IsVarTerm(t1))
    return (FALSE);
  if (IsAtomTerm(t1)) {
    pe = RepPredProp(Yap_GetPredPropByAtom(AtomOfTerm(t1), mod));
  } else if (IsApplTerm(t1)) {
    Functor         funt = FunctorOfTerm(t1);
    if (IsExtensionFunctor(funt)) {
      return(FALSE);
    } 
    if (funt == FunctorModule) {
      Term nmod = ArgOfTerm(1, t1);
      if (IsVarTerm(nmod)) {
	Yap_Error(INSTANTIATION_ERROR,ARG1,"hide_predicate/1");
	return(FALSE);
      } 
      if (!IsAtomTerm(nmod)) {
	Yap_Error(TYPE_ERROR_ATOM,ARG1,"hide_predicate/1");
	return(FALSE);
      }
      t1 = ArgOfTerm(2, t1);
      goto restart_system_pred;
    }
    pe = RepPredProp(Yap_GetPredPropByFunc(funt, mod));
  } else if (IsPairTerm(t1)) {
    return (TRUE);
  } else
    return (FALSE);
  if (EndOfPAEntr(pe))
    return(FALSE);
  pe->PredFlags |= HiddenPredFlag;
  return(TRUE);
}

static Int			/* $hidden_predicate(P) */
p_hidden_predicate(void)
{
  PredEntry      *pe;

  Term t1 = Deref(ARG1);
  Term mod = Deref(ARG2);

 restart_system_pred:
  if (IsVarTerm(t1))
    return (FALSE);
  if (IsAtomTerm(t1)) {
    pe = RepPredProp(Yap_GetPredPropByAtom(AtomOfTerm(t1), mod));
  } else if (IsApplTerm(t1)) {
    Functor         funt = FunctorOfTerm(t1);
    if (IsExtensionFunctor(funt)) {
      return(FALSE);
    } 
    if (funt == FunctorModule) {
      Term nmod = ArgOfTerm(1, t1);
      if (IsVarTerm(nmod)) {
	Yap_Error(INSTANTIATION_ERROR,ARG1,"hide_predicate/1");
	return(FALSE);
      } 
      if (!IsAtomTerm(nmod)) {
	Yap_Error(TYPE_ERROR_ATOM,ARG1,"hide_predicate/1");
	return(FALSE);
      }
      t1 = ArgOfTerm(2, t1);
      goto restart_system_pred;
    }
    pe = RepPredProp(Yap_GetPredPropByFunc(funt, mod));
  } else if (IsPairTerm(t1)) {
    return (TRUE);
  } else
    return (FALSE);
  if (EndOfPAEntr(pe))
    return(FALSE);
  return(pe->PredFlags & HiddenPredFlag);
}

static PredEntry *
get_pred(Term t1, Term mod, char *command)
{

 restart_system_pred:
  if (IsVarTerm(t1))
    return NULL;
  if (IsAtomTerm(t1)) {
    return RepPredProp(Yap_GetPredPropByAtom(AtomOfTerm(t1), mod));
  } else if (IsApplTerm(t1)) {
    Functor         funt = FunctorOfTerm(t1);
    if (IsExtensionFunctor(funt)) {
      return NULL;
    } 
    if (funt == FunctorModule) {
      Term nmod = ArgOfTerm(1, t1);
      if (IsVarTerm(nmod)) {
	Yap_Error(INSTANTIATION_ERROR,t1,command);
	return NULL;
      } 
      if (!IsAtomTerm(nmod)) {
	Yap_Error(TYPE_ERROR_ATOM,t1,command);
	return NULL;
      }
      t1 = ArgOfTerm(2, t1);
      goto restart_system_pred;
    }
    return RepPredProp(Yap_GetPredPropByFunc(funt, mod));
  } else if (IsPairTerm(t1)) {
    return NULL;
  } else
    return NULL;
}

static Int
fetch_next_lu_clause(PredEntry *pe, yamop *i_code, Term th, Term tb, Term tr, yamop *cp_ptr, int first_time)
{
  LogUpdClause *cl;
  Term rtn;
  Term Terms[3];

  Terms[0] = th;
  Terms[1] = tb;
  Terms[2] = tr;
  cl = Yap_FollowIndexingCode(pe, i_code, Terms, NEXTOP(PredLogUpdClause->CodeOfPred,ld), cp_ptr);
  th = Terms[0];
  tb = Terms[1];
  tr = Terms[2];
  /* don't do this!! I might have stored a choice-point and changed ASP
     Yap_RecoverSlots(3);
  */
  if (cl == NULL) {
    return FALSE;
  }
  rtn = MkDBRefTerm((DBRef)cl);
#if defined(YAPOR) || defined(THREADS)
  LOCK(cl->ClLock);
  TRAIL_CLREF(cl);		/* So that fail will erase it */
  INC_CLREF_COUNT(cl);
  UNLOCK(cl->ClLock);
#else
  if (!(cl->ClFlags & InUseMask)) {
    cl->ClFlags |= InUseMask;
    TRAIL_CLREF(cl);	/* So that fail will erase it */
  }
#endif
#if defined(YAPOR) || defined(THREADS)
  if (PP == pe) {
    READ_UNLOCK(pe->PRWLock);
    PP = NULL;
  }
#endif
#if defined(YAPOR) || defined(THREADS)
  WPP = NULL;
#endif
  if (cl->ClFlags & FactMask) {
    if (!Yap_unify_constant(tb, MkAtomTerm(AtomTrue)) ||
	!Yap_unify(tr, rtn))
      return FALSE;
    if (pe->ArityOfPE) {
      Functor f = FunctorOfTerm(th);
      UInt arity = ArityOfFunctor(f), i;
      CELL *pt = RepAppl(th)+1;

      for (i=0; i<arity; i++) {
	XREGS[i+1] = pt[i];
      }
      /* don't need no ENV */
      if (first_time) {
	CP = P;
	ENV = YENV;
	YENV = ASP;
	YENV[E_CB] = (CELL) B;
      }
      P = cl->ClCode;
    }
    return TRUE;
  } else {
    Term t;

    while ((t = Yap_FetchTermFromDB(cl->ClSource)) == 0L) {
      if (first_time) {
	ARG5 = th;
	ARG6 = tb;
	ARG7 = tr;
	if (Yap_Error_TYPE == OUT_OF_ATTVARS_ERROR) {
	  Yap_Error_TYPE = YAP_NO_ERROR;
	  if (!Yap_growglobal(NULL)) {
	    Yap_Error(OUT_OF_ATTVARS_ERROR, TermNil, Yap_ErrorMessage);
	    return FALSE;
	  }
	} else {
	  Yap_Error_TYPE = YAP_NO_ERROR;
	  if (!Yap_gcl(Yap_Error_Size, 7, YENV, P)) {
	    Yap_Error(OUT_OF_STACK_ERROR, TermNil, Yap_ErrorMessage);
	    return FALSE;
	  }
	}
	th = ARG5;
	tb = ARG6;
	tr = ARG7;
      } else {
	ARG6 = th;
	ARG7 = tb;
	ARG8 = tr;
	if (!Yap_gcl(Yap_Error_Size, 8, ENV, CP)) {
	  Yap_Error(OUT_OF_STACK_ERROR, TermNil, Yap_ErrorMessage);
	  return FALSE;
	}
	th = ARG6;
	tb = ARG7;
	tr = ARG8;
      }
    }
    return(Yap_unify(th, ArgOfTerm(1,t)) &&
	   Yap_unify(tb, ArgOfTerm(2,t)) &&
	   Yap_unify(tr, rtn));
  }
}

static Int			/* $hidden_predicate(P) */
p_log_update_clause(void)
{
  PredEntry      *pe;
  Term t1 = Deref(ARG1);
  Int ret;

  pe = get_pred(t1, Deref(ARG2), "clause/3");
  if (pe == NULL || EndOfPAEntr(pe))
    return FALSE;
#if defined(YAPOR) || defined(THREADS)
  READ_LOCK(pe->PRWLock);
  PP = pe;
#endif
  ret = fetch_next_lu_clause(pe, pe->CodeOfPred, t1, ARG3, ARG4, P, TRUE);
  return ret;
}

static Int			/* $hidden_predicate(P) */
p_continue_log_update_clause(void)
{
  PredEntry *pe = (PredEntry *)IntegerOfTerm(Deref(ARG1));
  yamop *ipc = (yamop *)IntegerOfTerm(ARG2);

#if defined(YAPOR) || defined(THREADS)
  READ_LOCK(pe->PRWLock);
  PP = pe;
#endif
  return fetch_next_lu_clause(pe, ipc, Deref(ARG3), ARG4, ARG5, B->cp_cp, FALSE);
}

static Int
fetch_next_lu_clause0(PredEntry *pe, yamop *i_code, Term th, Term tb, yamop *cp_ptr, int first_time)
{
  LogUpdClause *cl;
  Term Terms[3];

  Terms[0] = th;
  Terms[1] = tb;
  Terms[2] = TermNil;
  cl = Yap_FollowIndexingCode(pe, i_code, Terms, NEXTOP(PredLogUpdClause0->CodeOfPred,ld), cp_ptr);
  th = Terms[0];
  tb = Terms[1];
  /* don't do this!! I might have stored a choice-point and changed ASP
     Yap_RecoverSlots(2);
  */
#if defined(YAPOR) || defined(THREADS)
  if (PP == pe) {
    READ_UNLOCK(pe->PRWLock);
    PP = NULL;
  }
#endif
  if (cl == NULL) {
    return FALSE;
  }
  if (cl->ClFlags & FactMask) {
    if (!Yap_unify(tb, MkAtomTerm(AtomTrue)))
      return FALSE;
    if (pe->ArityOfPE) {
      Functor f = FunctorOfTerm(th);
      UInt arity = ArityOfFunctor(f), i;
      CELL *pt = RepAppl(th)+1;

      for (i=0; i<arity; i++) {
	XREGS[i+1] = pt[i];
      }
      /* don't need no ENV */
      if (first_time) {
	CP = P;
	ENV = YENV;
	YENV = ASP;
	YENV[E_CB] = (CELL) B;
      }
      P = cl->ClCode;
    }
    return TRUE;
  } else {
    Term t;

    while ((t = Yap_FetchTermFromDB(cl->ClSource)) == 0L) {
      if (first_time) {
	if (Yap_Error_TYPE == OUT_OF_ATTVARS_ERROR) {
	  Yap_Error_TYPE = YAP_NO_ERROR;
	  if (!Yap_growglobal(NULL)) {
	    Yap_Error(OUT_OF_ATTVARS_ERROR, TermNil, Yap_ErrorMessage);
	    return FALSE;
	  }
	} else {
	  Yap_Error_TYPE = YAP_NO_ERROR;
	  if (!Yap_gcl(Yap_Error_Size, 4, YENV, P)) {
	    Yap_Error(OUT_OF_STACK_ERROR, TermNil, Yap_ErrorMessage);
	    return FALSE;
	  }
	}
      } else {
	if (!Yap_gcl(Yap_Error_Size, 5, ENV, CP)) {
	  Yap_Error(OUT_OF_STACK_ERROR, TermNil, Yap_ErrorMessage);
	  return FALSE;
	}
      }
    }
    return(Yap_unify(th, ArgOfTerm(1,t)) &&
	   Yap_unify(tb, ArgOfTerm(2,t)));
  }
}

static Int			/* $hidden_predicate(P) */
p_log_update_clause0(void)
{
  PredEntry      *pe;
  Term           t1 = Deref(ARG1);
  Int ret;

  pe = get_pred(t1, Deref(ARG2), "clause/3");
  if (pe == NULL || EndOfPAEntr(pe))
    return FALSE;
#if defined(YAPOR) || defined(THREADS)
  READ_LOCK(pe->PRWLock);
  PP = pe;
#endif
  ret = fetch_next_lu_clause0(pe, pe->CodeOfPred, t1, ARG3, P, TRUE);
  return ret;
}

static Int			/* $hidden_predicate(P) */
p_continue_log_update_clause0(void)
{
  PredEntry *pe = (PredEntry *)IntegerOfTerm(Deref(ARG1));
  yamop *ipc = (yamop *)IntegerOfTerm(ARG2);

#if defined(YAPOR) || defined(THREADS)
  READ_LOCK(pe->PRWLock);
  PP = pe;
#endif
  return fetch_next_lu_clause0(pe, ipc, Deref(ARG3), ARG4, B->cp_cp, FALSE);
}

static void
adjust_cl_timestamp(LogUpdClause *cl, UInt *arp, UInt *base)
{
  UInt clstamp = cl->ClTimeEnd;
  if (cl->ClTimeEnd != TIMESTAMP_EOT) {
    while (arp[0] > clstamp)
      arp--;
    if (arp[0] == clstamp) {
      cl->ClTimeEnd = (arp-base);
    } else {
      cl->ClTimeEnd = (arp-base)+1;
    }
  }
  clstamp = cl->ClTimeStart;
  while (arp[0] > clstamp)
    arp--;
  if (arp[0] == clstamp) {
    cl->ClTimeStart = (arp-base);
  } else {
    cl->ClTimeStart = (arp-base)+1;
  }
  clstamp = cl->ClTimeEnd;
}


static Term
replace_integer(Term orig, UInt new)
{
  CELL *pt;

  if (IntInBnd((Int)new)) 
    return MkIntTerm(new);
  /* should create an old integer */
  if (!IsApplTerm(orig)) {
    Yap_Error(SYSTEM_ERROR,orig,"%uld-->%uld  where it should increase",(unsigned long int)IntegerOfTerm(orig),(unsigned long int)new);
    return MkIntegerTerm(new);
  }
  /* appl->appl */
  /* replace integer in situ */
  pt = RepAppl(orig)+1;
  *pt = new;
  return orig;
}

void			/* $hidden_predicate(P) */
Yap_UpdateTimestamps(PredEntry *ap)
{
  choiceptr bptr = B;
  yamop *cl0 = NEXTOP(PredLogUpdClause0->CodeOfPred,ld);
  yamop *cl = NEXTOP(PredLogUpdClause->CodeOfPred,ld);
  UInt ar = ap->ArityOfPE;
  UInt *arp, *top, *base;
  LogUpdClause *lcl;

#if THREADS
  Yap_Error(SYSTEM_ERROR,TermNil,"Timestamp overflow %p", ap);
  return;
#endif
  if (!ap->cs.p_code.NOfClauses)
    return;
 restart:
  *--ASP = TIMESTAMP_EOT;
  top = arp = (UInt *)ASP;
  while (bptr) {
    op_numbers opnum = Yap_op_from_opcode(bptr->cp_ap->opc);

    switch (opnum) {
    case _retry_logical:
    case _count_retry_logical:
    case _profiled_retry_logical:
    case _trust_logical:
    case _count_trust_logical:
    case _profiled_trust_logical:
      if (bptr->cp_ap->u.lld.d->ClPred == ap) {
	UInt ts = IntegerOfTerm(bptr->cp_args[ar]);
	if (ts != arp[0]) {
	  if (arp-H < 1024) {
	    goto overflow;
	  }
	  /* be thrifty, have this in case there is a hole */
	  if (ts != arp[0]-1) {
	    UInt x = arp[0];
	    *--arp = x;
	  }
	  *--arp = ts;
	}
      }
      bptr = bptr->cp_b;
      break;
    case _retry:
      if ((bptr->cp_ap == cl0 || bptr->cp_ap == cl) &&
	  ((PredEntry *)IntegerOfTerm(bptr->cp_args[0]) == ap)) {
	UInt ts = IntegerOfTerm(bptr->cp_args[5]);
	if (ts != arp[0]) {
	  if (arp-H < 1024) {
	    goto overflow;
	  }
	  if (ts != arp[0]-1) {
	    UInt x = arp[0];
	    *--arp = x;
	  }
	  *--arp = ts;
	}
      }
      bptr = bptr->cp_b;
      break;
    default:
      bptr = bptr->cp_b;
      continue;
    }
  }
  if (*arp)
    *--arp = 0L;
  base = arp;
  lcl = ClauseCodeToLogUpdClause(ap->cs.p_code.FirstClause);
  while (lcl) {
    adjust_cl_timestamp(lcl, top-1, base);
    lcl = lcl->ClNext;
  }
  lcl = DBErasedList;
  while (lcl) {
    if (lcl->ClPred == ap)
      adjust_cl_timestamp(lcl, top-1, base);
    lcl = lcl->ClNext;
  }
  arp = top-1;
  bptr = B;
  while (bptr) {
    op_numbers opnum = Yap_op_from_opcode(bptr->cp_ap->opc);

    switch (opnum) {
    case _retry_logical:
    case _count_retry_logical:
    case _profiled_retry_logical:
    case _trust_logical:
    case _count_trust_logical:
    case _profiled_trust_logical:
      if (bptr->cp_ap->u.lld.d->ClPred == ap) {
	UInt ts = IntegerOfTerm(bptr->cp_args[ar]);
	while (ts != arp[0])
	  arp--;
	bptr->cp_args[ar] = replace_integer(bptr->cp_args[ar], arp-base);
      }
      bptr = bptr->cp_b;
      break;
    case _retry:
      if ((bptr->cp_ap == cl0 || bptr->cp_ap == cl) &&
	  ((PredEntry *)IntegerOfTerm(bptr->cp_args[0]) == ap)) {
	UInt ts = IntegerOfTerm(bptr->cp_args[5]);
	while (ts != arp[0])
	  arp--;
	bptr->cp_args[5] = replace_integer(bptr->cp_args[5], arp-base);
      }
      bptr = bptr->cp_b;
      break;
    default:
      bptr = bptr->cp_b;
      continue;
    }
  }
  return;
 overflow:
  if (!Yap_growstack(64*1024)) {
    Yap_Error(OUT_OF_STACK_ERROR, TermNil, Yap_ErrorMessage);
    return;
  }
  goto restart;
}

static Int
fetch_next_static_clause(PredEntry *pe, yamop *i_code, Term th, Term tb, Term tr, yamop *cp_ptr, int first_time)
{
  StaticClause *cl;
  Term rtn;
  Term Terms[3];

  Terms[0] = th;
  Terms[1] = tb;
  Terms[2] = tr;
  cl = (StaticClause *)Yap_FollowIndexingCode(pe, i_code, Terms, NEXTOP(PredStaticClause->CodeOfPred,ld), cp_ptr);
  th = Terms[0];
  tb = Terms[1];
  tr = Terms[2];
  /* don't do this!! I might have stored a choice-point and changed ASP
     Yap_RecoverSlots(3);
  */
  if (cl == NULL)
    return FALSE;
  if (pe->PredFlags & MegaClausePredFlag) {
    yamop *code = (yamop *)cl;
    rtn = Yap_MkMegaRefTerm(pe,code);
    if (!Yap_unify(tb, MkAtomTerm(AtomTrue)) ||
	!Yap_unify(tr, rtn))
      return FALSE;
    if (pe->ArityOfPE) {
      Functor f = FunctorOfTerm(th);
      UInt arity = ArityOfFunctor(f), i;
      CELL *pt = RepAppl(th)+1;

      for (i=0; i<arity; i++) {
	XREGS[i+1] = pt[i];
      }
      /* don't need no ENV */
      if (first_time) {
	CP = P;
	ENV = YENV;
	YENV = ASP;
	YENV[E_CB] = (CELL) B;
      }
      P = code;
    }
    return TRUE;
  }
  rtn = Yap_MkStaticRefTerm(cl);
  if (cl->ClFlags & FactMask) {
    if (!Yap_unify(tb, MkAtomTerm(AtomTrue)) ||
	!Yap_unify(tr, rtn))
      return FALSE;

    if (pe->ArityOfPE) {
      Functor f = FunctorOfTerm(th);
      UInt arity = ArityOfFunctor(f), i;
      CELL *pt = RepAppl(th)+1;

      for (i=0; i<arity; i++) {
	XREGS[i+1] = pt[i];
      }
      /* don't need no ENV */
      if (first_time) {
	CP = P;
	ENV = YENV;
	YENV = ASP;
	YENV[E_CB] = (CELL) B;
      }
      P = cl->ClCode;
    }
    return TRUE;
  } else {
    Term t;
    
    if (!(pe->PredFlags & SourcePredFlag)) {
      /* no source */
      rtn = Yap_MkStaticRefTerm(cl);
      return Yap_unify(tr, rtn);
    }

    if (!(pe->PredFlags & SourcePredFlag)) {
      rtn = Yap_MkStaticRefTerm(cl);
      return Yap_unify(tr, rtn);
    }
    while ((t = Yap_FetchTermFromDB(cl->usc.ClSource)) == 0L) {
      if (first_time) {
	if (Yap_Error_TYPE == OUT_OF_ATTVARS_ERROR) {
	  Yap_Error_TYPE = YAP_NO_ERROR;
	  if (!Yap_growglobal(NULL)) {
	    Yap_Error(OUT_OF_ATTVARS_ERROR, TermNil, Yap_ErrorMessage);
	    return FALSE;
	  }
	} else {
	  Yap_Error_TYPE = YAP_NO_ERROR;
	  ARG5 = th;
	  ARG6 = tb;
	  ARG7 = tr;
	  if (!Yap_gc(7, YENV, P)) {
	    Yap_Error(OUT_OF_STACK_ERROR, TermNil, Yap_ErrorMessage);
	    return FALSE;
	  }
	  th = ARG5;
	  tb = ARG6;
	  tr = ARG7;
	}
      } else {
	Yap_Error_TYPE = YAP_NO_ERROR;
	ARG6 = th;
	ARG7 = tb;
	ARG8 = tr;
	if (!Yap_gcl(Yap_Error_Size, 8, ENV, CP)) {
	  Yap_Error(OUT_OF_STACK_ERROR, TermNil, Yap_ErrorMessage);
	  return FALSE;
	}
	th = ARG6;
	tb = ARG7;
	tr = ARG8;
      }
    }
    rtn = Yap_MkStaticRefTerm(cl);
    if (!IsApplTerm(t) || FunctorOfTerm(t) != FunctorAssert) {
      return(Yap_unify(th, t) &&
	     Yap_unify(tb, MkAtomTerm(AtomTrue)) &&
	     Yap_unify(tr, rtn));
    } else {
      return(Yap_unify(th, ArgOfTerm(1,t)) &&
	     Yap_unify(tb, ArgOfTerm(2,t)) &&
	     Yap_unify(tr, rtn));
    }
  }
}

static Int			/* $hidden_predicate(P) */
p_static_clause(void)
{
  PredEntry      *pe;
  Term t1 = Deref(ARG1);

  pe = get_pred(t1, Deref(ARG2), "clause/3");
  if (pe == NULL || EndOfPAEntr(pe))
    return FALSE;
  return fetch_next_static_clause(pe, pe->CodeOfPred, ARG1, ARG3, ARG4, P, TRUE);
}

static Int			/* $hidden_predicate(P) */
p_nth_clause(void)
{
  PredEntry      *pe;
  Term t1 = Deref(ARG1);
  Term tn = Deref(ARG3);
  LogUpdClause *cl;
  Int ncls;

  if (!IsIntegerTerm(tn))
    return FALSE;
  ncls = IntegerOfTerm(tn);
  pe = get_pred(t1, Deref(ARG2), "clause/3");
  if (pe == NULL || EndOfPAEntr(pe))
    return FALSE;
  if (!(pe->PredFlags & (SourcePredFlag|LogUpdatePredFlag))) {
    return FALSE;
  }
  /* in case we have to index or to expand code */
  if (pe->ModuleOfPred != IDB_MODULE) {
    UInt i;

    for (i = 1; i <= pe->ArityOfPE; i++) {
      XREGS[i] = MkVarTerm();
    }
  } else {
      XREGS[2] = MkVarTerm();
  }
  if(pe->OpcodeOfPred == INDEX_OPCODE) {
    IPred(pe, 0);
  }
  cl = Yap_NthClause(pe, ncls);
  if (cl == NULL) 
    return FALSE;
  if (pe->PredFlags & LogUpdatePredFlag) {
#if defined(YAPOR) || defined(THREADS)
    LOCK(cl->ClLock);
    TRAIL_CLREF(cl);		/* So that fail will erase it */
    INC_CLREF_COUNT(cl);
    UNLOCK(cl->ClLock);
#else
    if (!(cl->ClFlags & InUseMask)) {
      cl->ClFlags |= InUseMask;
      TRAIL_CLREF(cl);	/* So that fail will erase it */
    }
#endif
    return Yap_unify(MkDBRefTerm((DBRef)cl), ARG4);
  } else if (pe->PredFlags & MegaClausePredFlag) {
    return Yap_unify(Yap_MkMegaRefTerm(pe,(yamop *)cl), ARG4);
  } else {
    return Yap_unify(Yap_MkStaticRefTerm((StaticClause *)cl), ARG4);
  }
}

static Int			/* $hidden_predicate(P) */
p_continue_static_clause(void)
{
  PredEntry *pe = (PredEntry *)IntegerOfTerm(Deref(ARG1));
  yamop *ipc = (yamop *)IntegerOfTerm(ARG2);

  return fetch_next_static_clause(pe, ipc, Deref(ARG3), ARG4, ARG5, B->cp_ap, FALSE);
}

#if LOW_PROF

static void
add_code_in_pred(PredEntry *pp) {
  yamop *clcode;

  READ_LOCK(pp->PRWLock);
  /* check if the codeptr comes from the indexing code */

  /* highly likely this is used for indexing */
  Yap_inform_profiler_of_clause((yamop *)&(pp->OpcodeOfPred), (yamop *)(&(pp->OpcodeOfPred)+1), pp, 1);
  if (pp->PredFlags & (CPredFlag|AsmPredFlag)) {
    char *code_end;
    StaticClause *cl;

    clcode = pp->CodeOfPred;
    cl = ClauseCodeToStaticClause(clcode);
    code_end = (char *)cl + cl->ClSize;
    Yap_inform_profiler_of_clause(clcode, (yamop *)code_end, pp,0);
    READ_UNLOCK(pp->PRWLock);
    return;
  }
  Yap_inform_profiler_of_clause((yamop *)&(pp->cs.p_code.ExpandCode), (yamop *)(&(pp->cs.p_code.ExpandCode)+1), pp, 1);
  clcode = pp->cs.p_code.TrueCodeOfPred;
  if (pp->PredFlags & IndexedPredFlag) {
    char *code_end;
    if (pp->PredFlags & LogUpdatePredFlag) {
      LogUpdIndex *cl = ClauseCodeToLogUpdIndex(clcode);
      code_end = (char *)cl + cl->ClSize;
    } else {
      StaticIndex *cl = ClauseCodeToStaticIndex(clcode);
      code_end = (char *)cl + cl->ClSize;
    }
    Yap_inform_profiler_of_clause(clcode, (yamop *)code_end, pp,0);
  }	      
  clcode = pp->cs.p_code.FirstClause;
  if (clcode != NULL) {
    if (pp->PredFlags & LogUpdatePredFlag) {
      LogUpdClause *cl = ClauseCodeToLogUpdClause(clcode);
      do {
	char *code_end;

	code_end = (char *)cl + cl->ClSize;
	Yap_inform_profiler_of_clause(cl->ClCode, (yamop *)code_end, pp,0);
	cl = cl->ClNext;
      } while (cl != NULL);
    } else if (pp->PredFlags & DynamicPredFlag) {
      do {
	DynamicClause *cl;
	CODEADDR code_end;

	cl = ClauseCodeToDynamicClause(clcode);
 	code_end = (CODEADDR)cl + cl->ClSize;
	Yap_inform_profiler_of_clause(clcode, (yamop *)code_end, pp,0);
	if (clcode == pp->cs.p_code.LastClause)
	  break;
	clcode = NextDynamicClause(clcode);
      } while (TRUE);
    } else {
      StaticClause *cl = ClauseCodeToStaticClause(clcode);
      do {
	char *code_end;

	code_end = (char *)cl + cl->ClSize;
	Yap_inform_profiler_of_clause(cl->ClCode, (yamop *)code_end, pp,0);
	if (cl->ClCode == pp->cs.p_code.FirstClause)
	  break;
	cl = cl->ClNext;
      } while (TRUE);
    }
  }
  READ_UNLOCK(pp->PRWLock); 
}


void
Yap_dump_code_area_for_profiler(void) {
  ModEntry *me = CurrentModules;

  while (me) {
    PredEntry *pp = me->PredForME;

    while (pp != NULL) {
      /*      if (pp->ArityOfPE) {
	fprintf(stderr,"%s/%d %p\n",
		RepAtom(NameOfFunctor(pp->FunctorOfPred))->StrOfAE,
		pp->ArityOfPE,
		pp);
      } else {
	fprintf(stderr,"%s %p\n",
		RepAtom((Atom)(pp->FunctorOfPred))->StrOfAE,
		pp);
		}*/
      add_code_in_pred(pp);
      pp = pp->NextPredOfModule;
    }
    me = me->NextME;
  }
  Yap_inform_profiler_of_clause(COMMA_CODE, FAILCODE, RepPredProp(Yap_GetPredPropByFunc(FunctorComma,0)),0);
  Yap_inform_profiler_of_clause(FAILCODE, FAILCODE+1, RepPredProp(Yap_GetPredPropByAtom(AtomFail,0)),0);
}

#endif /* LOW_PROF */

static UInt
index_ssz(StaticIndex *x)
{
  UInt sz = x->ClSize;
  x = x->ChildIndex;
  while (x != NULL) {
    sz += index_ssz(x);
    x = x->SiblingIndex;
  }
  return sz;
}

static Int
static_statistics(PredEntry *pe)
{
  UInt sz = 0, cls = 0, isz = 0;
  StaticClause *cl = ClauseCodeToStaticClause(pe->cs.p_code.FirstClause);

  if (pe->cs.p_code.NOfClauses > 1 &&
      pe->cs.p_code.TrueCodeOfPred != pe->cs.p_code.FirstClause) {
    isz = index_ssz(ClauseCodeToStaticIndex(pe->cs.p_code.TrueCodeOfPred));
  }
  if (pe->PredFlags & MegaClausePredFlag) {
    MegaClause *mcl = ClauseCodeToMegaClause(pe->cs.p_code.FirstClause);
    return Yap_unify(ARG3, MkIntegerTerm(mcl->ClSize/mcl->ClItemSize)) &&
      Yap_unify(ARG4, MkIntegerTerm(mcl->ClSize)) &&
      Yap_unify(ARG5, MkIntegerTerm(isz));
  }
  if (pe->cs.p_code.NOfClauses) {
    do {
      cls++;
      sz += cl->ClSize;
      if (cl->ClCode == pe->cs.p_code.LastClause)
	break;
      cl = cl->ClNext;
    } while (TRUE);
  }
  return Yap_unify(ARG3, MkIntegerTerm(cls)) &&
    Yap_unify(ARG4, MkIntegerTerm(sz)) &&
    Yap_unify(ARG5, MkIntegerTerm(isz));
}

static Int
p_static_pred_statistics(void)
{
  Term t = Deref(ARG1);
  Term mod = Deref(ARG2);
  PredEntry      *pe;

  if (IsVarTerm(t)) {
    return (FALSE);
  } else if (IsAtomTerm(t)) {
    Atom at = AtomOfTerm(t);
    pe = RepPredProp(Yap_GetPredPropByAtom(at, mod));
  } else if (IsApplTerm(t)) {
    Functor         fun = FunctorOfTerm(t);
    pe = RepPredProp(Yap_GetPredPropByFunc(fun, mod));
  } else
    return (FALSE);
  if (pe == NIL)
    return (FALSE);
  if (pe->PredFlags & (DynamicPredFlag|LogUpdatePredFlag|UserCPredFlag|AsmPredFlag|CPredFlag|BinaryTestPredFlag)) {
    /* should use '$recordedp' in this case */
    return FALSE;
  }
  return static_statistics(pe);
}

static Int
p_predicate_erased_statistics(void)
{
  UInt sz = 0, cls = 0;
  UInt isz = 0, icls = 0;
  PredEntry *pe;
  LogUpdClause *cl = DBErasedList;
  LogUpdIndex *icl = DBErasedIList;
  Term t = Deref(ARG1);
  Term mod = CurrentModule;

  if (IsVarTerm(t)) {
    return FALSE;
  } else if (IsAtomTerm(t)) {
    Atom at = AtomOfTerm(t);
    pe = RepPredProp(Yap_GetPredPropByAtom(at, mod));
  } else if (IsApplTerm(t)) {
    Functor         fun = FunctorOfTerm(t);
    pe = RepPredProp(Yap_GetPredPropByFunc(fun, mod));
  } else
    return FALSE;
  if (EndOfPAEntr(pe))
    return FALSE;
  while (cl) {
    if (cl->ClPred == pe) {
      cls++;
      sz += cl->ClSize;
    }
    cl = cl->ClNext;
  }
  while (icl) {
    if (pe == icl->ClPred) {
      icls++;
      isz += icl->ClSize;
    }
    icl = icl->SiblingIndex;
  }
  return
    Yap_unify(ARG2,MkIntegerTerm(cls)) &&
    Yap_unify(ARG3,MkIntegerTerm(sz)) &&
    Yap_unify(ARG4,MkIntegerTerm(icls)) &&
    Yap_unify(ARG5,MkIntegerTerm(isz));
}

#ifdef DEBUG
static Int
p_predicate_lu_cps(void)
{
  return Yap_unify(ARG1, MkIntegerTerm(Yap_LiveCps)) &&
    Yap_unify(ARG2, MkIntegerTerm(Yap_FreedCps)) &&
    Yap_unify(ARG3, MkIntegerTerm(Yap_DirtyCps)) &&
    Yap_unify(ARG4, MkIntegerTerm(Yap_NewCps));
}
#endif

static Int
p_program_continuation(void)
{
  PredEntry *pe = EnvPreg(((CELL *)ENV[E_E])[E_CP]);
  if (pe->ModuleOfPred) {
    if (!Yap_unify(ARG1,pe->ModuleOfPred))
      return FALSE;
  } else {
    if (!Yap_unify(ARG1,TermProlog))
      return FALSE;
  }
  if (pe->ArityOfPE) {
    if (!Yap_unify(ARG2,MkAtomTerm(NameOfFunctor(pe->FunctorOfPred))))
      return FALSE;
    if (!Yap_unify(ARG3,MkIntegerTerm(ArityOfFunctor(pe->FunctorOfPred))))
      return FALSE;
  } else {
    if (!Yap_unify(ARG2,MkAtomTerm((Atom)pe->FunctorOfPred)))
      return FALSE;
    if (!Yap_unify(ARG3,MkIntTerm(0)))
      return FALSE;
  }
  return TRUE;
}

static Term
BuildActivePred(PredEntry *ap, CELL *vect)
{
  UInt i;

  if (!ap->ArityOfPE) {
    return MkVarTerm();
  }
  for (i = 0; i < ap->ArityOfPE; i++) {
    Term t = Deref(vect[i]);
    if (IsVarTerm(t)) {
      CELL *pt = VarOfTerm(t);
      /* one stack */
      if (pt > H) {
	Term nt = MkVarTerm();
	Yap_unify(t, nt);
      }
    }
  }
  return Yap_MkApplTerm(ap->FunctorOfPred, ap->ArityOfPE, vect);
}

static int
UnifyPredInfo(PredEntry *pe, int start_arg) {
  UInt arity = pe->ArityOfPE;
  Term tmod, tname;

  if (pe->ModuleOfPred != IDB_MODULE) {
    if (pe->ModuleOfPred == PROLOG_MODULE) {
      tmod = TermProlog;
    } else {
      tmod = pe->ModuleOfPred;
    }
    if (pe->ArityOfPE == 0) {
      tname = MkAtomTerm((Atom)pe->FunctorOfPred);
    } else {
      Functor f = pe->FunctorOfPred;
      tname = MkAtomTerm(NameOfFunctor(f));
    }
  } else {
    tmod = pe->ModuleOfPred;
    if (pe->PredFlags & NumberDBPredFlag) {
      tname = MkIntegerTerm(pe->src.IndxId);
    } else if (pe->PredFlags & AtomDBPredFlag) {
      tname = MkAtomTerm((Atom)pe->FunctorOfPred);
    } else {
      Functor f = pe->FunctorOfPred;
      tname = MkAtomTerm(NameOfFunctor(f));
    }
  }
  
  return Yap_unify(XREGS[start_arg], tmod) &&
    Yap_unify(XREGS[start_arg+1],tname) &&
    Yap_unify(XREGS[start_arg+2],MkIntegerTerm(arity));
}


static Int
ClauseId(yamop *ipc, PredEntry *pe)
{
  if (!ipc)
    return 0;
  return find_code_in_clause(pe, ipc, NULL, NULL);
}

static Int
p_env_info(void)
{
  PredEntry *pe;
  CELL *env = LCL0-IntegerOfTerm(Deref(ARG1));
  yamop *env_cp;
  Term env_b, taddr;
  
  if (!env)
    return FALSE;
  env_b = MkIntegerTerm((Int)(LCL0-(CELL *)env[E_CB]));
  env_cp = (yamop *)env[E_CP];
  
  pe = PREVOP(env_cp,sla)->u.sla.p0;
  taddr = MkIntegerTerm((Int)env);
  return Yap_unify(ARG3,MkIntegerTerm((Int)env_cp)) &&
    Yap_unify(ARG2, taddr) &&
    Yap_unify(ARG4, env_b);
}

static Int
p_cpc_info(void)
{
  PredEntry *pe;
  yamop *ipc = (yamop *)IntegerOfTerm(Deref(ARG1));

  pe = PREVOP(ipc,sla)->u.sla.p0;
  return UnifyPredInfo(pe, 2) &&
    Yap_unify(ARG5,MkIntegerTerm(ClauseId(ipc,pe)));
}

static Int
p_choicepoint_info(void)
{
  choiceptr cptr = (choiceptr)(LCL0-IntegerOfTerm(Deref(ARG1)));
  PredEntry *pe = NULL;
  int go_on = TRUE;
  yamop *ipc = cptr->cp_ap;
  yamop *ncl = NULL;
  Term t = TermNil, taddr;
  
  taddr = MkIntegerTerm((Int)cptr);
  while (go_on) {
    op_numbers opnum = Yap_op_from_opcode(ipc->opc);

    go_on = FALSE;
    switch (opnum) {
#ifdef TABLING
    case _table_load_answer:
#ifdef LOW_LEVEL_TRACER
      pe = LOAD_CP(cptr)->cp_pred_entry;
#else
      pe = UndefCode;
#endif
      t = MkVarTerm();
      break;
    case _table_try_answer:
    case _table_retry_me:
    case _table_trust_me:
    case _table_retry:
    case _table_trust:
    case _table_completion:
#ifdef LOW_LEVEL_TRACER
      pe = GEN_CP(cptr)->cp_pred_entry;
      t = BuildActivePred(pe, (CELL *)(GEN_CP(B) + 1));
#else
      pe = UndefCode;
      t = MkVarTerm();
#endif
      break;
    case _table_answer_resolution:
#ifdef LOW_LEVEL_TRACER
      pe = CONS_CP(cptr)->cp_pred_entry;
#else
      pe = UndefCode;
#endif
      t = MkVarTerm();
      break;
    case _trie_retry_null:
    case _trie_trust_null:
    case _trie_retry_var:
    case _trie_trust_var:
    case _trie_retry_val:
    case _trie_trust_val:
    case _trie_retry_atom:
    case _trie_trust_atom:
    case _trie_retry_list:
    case _trie_trust_list:
    case _trie_retry_struct:
    case _trie_trust_struct:
    case _trie_retry_extension:
    case _trie_trust_extension:
    case _trie_retry_float:
    case _trie_trust_float:
    case _trie_retry_long:
    case _trie_trust_long:
      pe = UndefCode;
      t = MkVarTerm();
      break;
#endif /* TABLING */
    case _try_logical:
    case _retry_logical:
    case _trust_logical:
    case _count_retry_logical:
    case _count_trust_logical:
    case _profiled_retry_logical:
    case _profiled_trust_logical:
      ncl = ipc->u.lld.d->ClCode;
      pe = ipc->u.lld.d->ClPred;
      t = BuildActivePred(pe, cptr->cp_args);
      break;
    case _or_else:
      pe = ipc->u.sla.p0;
      ncl = ipc;
      t = Yap_MkNewApplTerm(FunctorOr, 2);
      break;
    case _or_last:
      pe = ipc->u.p.p;
      ncl = ipc;
      t = Yap_MkNewApplTerm(FunctorOr, 2);
      break;
    case _retry2:
    case _retry3:
    case _retry4:
      pe = NULL;
      t = TermNil;
      ipc = NEXTOP(ipc,l);
      if (!ncl)
	ncl = ipc->u.ld.d;
      go_on = TRUE;
      break;
    case _jump:
      pe = NULL;
      t = TermNil;
      ipc = ipc->u.l.l;
      go_on = TRUE;
      break;
    case _retry_c:
    case _retry_userc:
      ncl = ipc->u.ld.d;
      pe = ipc->u.lds.p;
      t = BuildActivePred(pe, cptr->cp_args);
      break;
    case _retry_profiled:
    case _count_retry:
      pe = NULL;
      t = TermNil;
      ncl = ipc->u.ld.d;
      ipc = NEXTOP(ipc,p);
      go_on = TRUE;
      break;
    case _retry_me:
    case _trust_me:
    case _count_retry_me:
    case _count_trust_me:
    case _profiled_retry_me:
    case _profiled_trust_me:
    case _retry_and_mark:
    case _profiled_retry_and_mark:
    case _retry:
    case _trust:
      if (!ncl)
	ncl = ipc->u.ld.d;
      pe = ipc->u.ld.p;
      t = BuildActivePred(pe, cptr->cp_args);
      break;
    case _Nstop:
      { 
	Atom at = Yap_FullLookupAtom("$live");
	t = MkAtomTerm(at);
	pe = RepPredProp(PredPropByAtom(at, CurrentModule));
      }
      break;
    case _Ystop:
    default:
      pe = NULL;
      t = TermNil;
      return FALSE;
    }
  }
  return UnifyPredInfo(pe, 3) &&
    Yap_unify(ARG2, taddr) &&
    Yap_unify(ARG6,t) &&
    Yap_unify(ARG7,MkIntegerTerm(ClauseId(ncl,pe)));
}

void 
Yap_InitCdMgr(void)
{
  Term cm = CurrentModule;

  Yap_InitCPred("$compile_mode", 2, p_compile_mode, SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$start_consult", 3, p_startconsult, SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$show_consult_level", 1, p_showconslultlev, SafePredFlag|HiddenPredFlag);
  Yap_InitCPred("$end_consult", 0, p_endconsult, SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$set_spy", 2, p_setspy, SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$rm_spy", 2, p_rmspy, SafePredFlag|SyncPredFlag|HiddenPredFlag);
  /* gc() may happen during compilation, hence these predicates are
	now unsafe */
  Yap_InitCPred("$compile", 4, p_compile, SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$compile_dynamic", 5, p_compile_dynamic, SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$purge_clauses", 2, p_purge_clauses, SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$in_use", 2, p_in_use, TestPredFlag | SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$is_dynamic", 2, p_is_dynamic, TestPredFlag | SafePredFlag|HiddenPredFlag);
  Yap_InitCPred("$is_metapredicate", 2, p_is_metapredicate, TestPredFlag | SafePredFlag|HiddenPredFlag);
  Yap_InitCPred("$is_expand_goal_or_meta_predicate", 2, p_is_expandgoalormetapredicate, TestPredFlag | SafePredFlag|HiddenPredFlag);
  Yap_InitCPred("$is_log_updatable", 2, p_is_log_updatable, TestPredFlag | SafePredFlag|HiddenPredFlag);
  Yap_InitCPred("$is_source", 2, p_is_source, TestPredFlag | SafePredFlag|HiddenPredFlag);
  Yap_InitCPred("$pred_exists", 2, p_pred_exists, TestPredFlag | SafePredFlag|HiddenPredFlag);
  Yap_InitCPred("$number_of_clauses", 3, p_number_of_clauses, SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$undefined", 2, p_undefined, SafePredFlag|TestPredFlag|HiddenPredFlag);
  Yap_InitCPred("$optimizer_on", 0, p_optimizer_on, SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$clean_up_dead_clauses", 0, p_clean_up_dead_clauses, SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$optimizer_off", 0, p_optimizer_off, SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$kill_dynamic", 2, p_kill_dynamic, SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$in_this_file_before", 3, p_in_this_f_before, SafePredFlag|HiddenPredFlag);
  Yap_InitCPred("$first_clause_in_file", 3, p_first_cl_in_f, SafePredFlag|HiddenPredFlag);
  Yap_InitCPred("$mk_cl_not_first", 2, p_mk_cl_not_first, SafePredFlag|HiddenPredFlag);
  Yap_InitCPred("$new_multifile", 3, p_new_multifile, SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$is_multifile", 2, p_is_multifile, TestPredFlag | SafePredFlag|HiddenPredFlag);
  Yap_InitCPred("$is_profiled", 1, p_is_profiled, SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$profile_info", 3, p_profile_info, SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$profile_reset", 2, p_profile_reset, SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$is_call_counted", 1, p_is_call_counted, SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$call_count_info", 3, p_call_count_info, SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$call_count_set", 6, p_call_count_set, SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$call_count_reset", 0, p_call_count_reset, SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$toggle_static_predicates_in_use", 0, p_toggle_static_predicates_in_use, SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$set_pred_module", 2, p_set_pred_module, SafePredFlag|HiddenPredFlag);
  Yap_InitCPred("$parent_pred", 3, p_parent_pred, SafePredFlag|HiddenPredFlag);
  Yap_InitCPred("$system_predicate", 2, p_system_pred, SafePredFlag|HiddenPredFlag);
  Yap_InitCPred("$hide_predicate", 2, p_hide_predicate, SafePredFlag|HiddenPredFlag);
  Yap_InitCPred("$hidden_predicate", 2, p_hidden_predicate, SafePredFlag|HiddenPredFlag);
  Yap_InitCPred("$pred_for_code", 5, p_pred_for_code, SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$current_stack", 1, p_current_stack, SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$log_update_clause", 4, p_log_update_clause, SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$continue_log_update_clause", 5, p_continue_log_update_clause, SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$log_update_clause", 3, p_log_update_clause0, SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$continue_log_update_clause", 4, p_continue_log_update_clause0, SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$static_clause", 4, p_static_clause, SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$continue_static_clause", 5, p_continue_static_clause, SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$static_pred_statistics", 5, p_static_pred_statistics, SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$p_nth_clause", 4, p_nth_clause, SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$program_continuation", 3, p_program_continuation, SafePredFlag|SyncPredFlag|HiddenPredFlag);
  CurrentModule = HACKS_MODULE;
  Yap_InitCPred("current_choicepoints", 1, p_all_choicepoints, HiddenPredFlag);
  Yap_InitCPred("current_continuations", 1, p_all_envs, HiddenPredFlag);
  Yap_InitCPred("choicepoint", 7, p_choicepoint_info, HiddenPredFlag);
  Yap_InitCPred("continuation", 4, p_env_info, HiddenPredFlag);
  Yap_InitCPred("cp_to_predicate", 5, p_cpc_info, HiddenPredFlag);
  CurrentModule = cm;
  Yap_InitCPred("$predicate_erased_statistics", 5, p_predicate_erased_statistics, SyncPredFlag);
#ifdef DEBUG
  Yap_InitCPred("$predicate_live_cps", 4, p_predicate_lu_cps, 0L);
#endif
}

