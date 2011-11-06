#include "store.h"
#include "node.h"
#include "comm.h"
#include "inst.h"
#include "insttab.h"
#include <u.h>
#include <lib9.h>
#include "errjmp.h"

#include "fns.h"

int	(**prog)(Proc*);
int	nprog=0;
int	storeprog=0;
int	Nprog=0;

Proc	*phead;
Proc	*ptail;

void	xec(void);
void	xxec(void);

int
iconv(va_list *va, Fconv *f)
{
	int i;
	char buf[16];
	void *o;	/* really int (**o)(Proc*) */

	o = va_arg(*va, void*);
	for(i=0; i<NInst; i++)
		if((void*)insttab[i].fp==o){
			strconv(insttab[i].name, f);
			return sizeof o;
		}
	sprint(buf, "  0x%p", o);
	strconv(buf, f);
	return sizeof o;
}

void
idump(void)
{
	int i;
	fprint(2, "%z\n");
	for(i=0; i<nprog; i++)
		print("  %p:\t%i\n", &prog[i], prog[i]);
}

long
topofstack(void){
	return interpreter.sp[-1];
}

void
run(Proc *p)	/* only called at run-time, so queue is not empty */
{
	if(phead==ptail)
		phead->next=p;
	ptail->next=p;
	ptail=p;
}

void
erun(Proc *p)	/* queue may be empty */
{
	if(phead==0){
		ptail=p;
		phead=p;
		p->next=0;
	}else
		run(p);
}

void
halt(Proc *proc)
{
	/* kinda yucky, but keeps the loop clean */
	static Procnostack kludge;
	proctab[proc->procnum]=0;
	kludge.next=proc->next;
	if(phead==ptail)
		ptail=(Proc *)&kludge;
	phead=(Proc *)&kludge;
	proctab[proc->procnum]=0;
	free((char *)proc);
}

void
executeinhibit(void)
{
	if(!interactive)
		executeallowed=0;
}

void
execute(void)
{
	extern int xflag;
	Errjmp s;
	interpreter.fp=0;
	interpreter.sp=interpreter.stack;
	interpreter.pc=prog+1;
	phead=&interpreter;
	ptail=&interpreter;
	interpreter.next=0;
	errsave(s);
	if(errmark()){
		istart();
		errrest(s);
		libgflush();
		errjmp();
	}
    loop:
	if(xflag)
		xxec();
	else
		xec();
	phead=0;
	while(moreio()){
		schedio(1);
		if(phead)
			goto loop;
	}

	if(interpreter.pc[-1]!=idone)
		error("interpreter process blocked");
	errrest(s);
	istart();
	libgflush();
}
/*
	without randomness:
	xec()
	{
		for(;;){
			schedio();
			if((**phead->pc)(phead)){
				phead->pc++;
				if(phead!=ptail){
					ptail->next=phead;
					ptail=phead;
					phead=phead->next;
				}
				continue;
			}
			phead->pc++;
			if(phead==ptail)
				return;
			phead=phead->next;
		}
	}
*/
long	randx=-1;
void
xec(void)
{
	int n, x=randx;

	for(;;){
    top:
		schedio(0);
		x+=x;
		if(x<0)
			x^=0x88888EEFL;
		n=x&0x1F;
		while((**phead->pc)(phead)){
			phead->pc++;
			if(--n<0){
				if(phead!=ptail){
					ptail->next=phead;
					ptail=phead;
					phead=phead->next;
				}
				goto top;
			}
		}
		phead->pc++;
		if(phead==ptail)
			break;
		phead=phead->next;
	}
	randx=x;
}
void
xxec(void){
	int n, x=randx;

	for(;;){
    top:
		schedio(0);
		x+=x;
		if(x<0)
			x^=0x88888EEFL;
		n=x&0x1F;
		goto Xprint;
		while((**phead->pc)(phead)){
			phead->pc++;
			if(--n<0){
				if(phead!=ptail){
					ptail->next=phead;
					ptail=phead;
					phead=phead->next;
				}
				goto top;
			}
    Xprint:
			print("%ld: %p: %i(#%p)\n", phead->procnum, phead->pc,
				phead->pc[0], phead->pc[1]);
		}
		phead->pc++;
		if(phead==ptail)
			break;
		phead=phead->next;
	}
	randx=x;
}

void
ovfl(void){
	enum{
		Delta=1000
	};
	if(nprog>=Nprog-1){
		prog=erealloc(prog, (Nprog+Delta)*sizeof(*prog));
		if(Nprog==0)
			prog[0]=0;
		Nprog+=Delta;
	}
}

void
istart(void)
{
	ovfl();
	storeprog=0;
	if(prog[0]){		/* Saryptr of local storage */
		decref((Store **)&prog[0]);
		if(prog[0])
			panic("istart");
	}
	prog[1]=idone;
	nprog=1;
}

Proc *
curproc(void){
	return phead;
}

void
emit(int i)
{
	extern int idone(Proc*);
	ovfl();
	prog[nprog++]=insttab[i].fp;
	prog[nprog]=idone;
}

void
emitspace(long i)
{
	nprog+=i;
	/* ovfl will be called on the next emit, so this is safe */
}

void
emitconst(long l)
{
	ovfl();
	prog[nprog++]=(int (*)(Proc*))l;
	prog[nprog]=idone;
}

void
setprog(long h)
{
	nprog=h;
	prog[nprog]=idone;
}

void
patch(int n, long l)
{
	prog[n]=(int (*)(Proc*))l;
}

int
here(void){
	return nprog;
}

void
emitstore(Store *s)
{
	Store *sa;
	enum{
		Delta=4
	};
	sa=(Store *)prog[storeprog];
	if(sa==0){
		sa=emalloc(SHSZ);
		sa->ref=1;
		sa->type=Saryptr;
		sa->sbits=0;
		sa->len=0;
	}
	/* realloc in units of Delta, but len goes up by one */
	if(sa->len%Delta==0){
		sa=erealloc(sa, SHSZ+(sa->len+Delta)*WS);
		prog[storeprog]=(int (*)(Proc*))sa;
	}
	sa->data[sa->len++]=(SWord)s;
}
