#include "store.h"
#include "comm.h"
#include <u.h>
#include <lib9.h>

#include "fns.h"

extern	int	pflag;

/*
 * Jumps
 */

int
ijmp(Proc *proc)
{
	SWord l;
	l=(SWord)*++proc->pc;
	proc->pc+=l/WS;
	return 1;
}

int
ijmpfalse(Proc *proc)
{
	SWord l;
	l=(SWord)*++proc->pc;
	if(*--proc->sp==0)
		proc->pc+=l/WS;
	return 1;
}

int
ijmptrue(Proc *proc)
{
	SWord l;
	l=(SWord)*++proc->pc;
	if(*--proc->sp!=0)
		proc->pc+=l/WS;
	return 1;
}

int
ivalnoresult(Proc *proc)
{
	USED(proc);
	rerror("val produces no result");
	return 0;
}

/*
 * Progs
 *
 *   Layout of a stack frame
 *
 *	sp:
 *		automatics
 *	fp:	old fp
 *		old pc
 *		called prog
 *		symbol
 *		arg1
 *		arg2
 *		...
 *		result
 */

int
iret(Proc *proc)
{
	SWord nargs;
	Store *s;
	nargs=(SWord)(proc->pc[1]);
	proc->sp=(SWord *)proc->fp+1;
	proc->fp=(SWord *)*--proc->sp;
	proc->pc=(int (**)(Proc*))*--proc->sp;
	s=(Store *)*--proc->sp;
	decref(&s);
	proc->sp-=(sizeof(char *)+nargs)/WS;
	if(proc->pc==0){
		if(pflag)
			fprint(2, "%ld halts\n", proc->procnum);
		halt(proc);
		return 0;
	}
	return 1;
}

int
ibecome(Proc *proc)
{
	int nargs;
	Store *oldprog;
	int (**newpc)(Proc*);
	SWord oldfp, oldpc, *oldresultaddr, *newresultaddr;
	Store *s;
	oldprog=(Store *)proc->fp[-2];
	nargs=*--proc->sp/WS;
	nargs+=2;	/* includes result, sym, and prog; add pc, fp */
	s=(Store *)proc->sp[-1];
	newpc=((int (**)(Proc*))s->data);
	oldfp=proc->fp[0];
	oldpc=proc->fp[-1];
	*proc->sp++=oldpc;
	*proc->sp++=oldfp;
	oldresultaddr=proc->fp-4-(long)(*++proc->pc)/WS;
	newresultaddr=proc->sp-nargs;
	memcpy(oldresultaddr, newresultaddr, WS*nargs);
	/* args in place.  do the call by hand, jmp to pushfp */
	proc->sp=oldresultaddr+(nargs-2);
	*proc->sp++=oldpc;
	proc->fp=(SWord *)oldfp;
	proc->pc=newpc;	/* auto inc will skip locals pointer */
	decref(&oldprog);
	return 1;
}

int
ipushfp(Proc *proc)
{
	int nauto;
	*proc->sp=(SWord)proc->fp;
	proc->fp=proc->sp++;
	nauto=((SWord)*++proc->pc)/WS;
	while(nauto--)
		*proc->sp++=0;
	if(proc->sp>=&proc->stack[NSTACK])
		rerror("stack overflow");
	return 1;
}

int
icall(Proc *proc)
{
	int (**newpc)(Proc*);
	Store *s;
	s=(Store *)proc->sp[-1];
	newpc=((int (**)(Proc*))s->data);
	*proc->sp++=(SWord)proc->pc;
	proc->pc=newpc;	/* auto advance of pc will skip over data pointer */
	return 1;
}
