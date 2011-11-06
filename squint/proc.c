/*
 *	Process control and communications
 */
#include "store.h"
#include "comm.h"
#include <u.h>
#include <lib9.h>

#include "fns.h"

Proc	interpreter;
Proc	**proctab;
int	iproctab=0;
long	Nproc=256;
Pqelem	*pqfree;
extern	int	mflag;
extern	int	pflag;
extern	void	pqins(Pqueue *, Proc *);
extern	Proc	*pqdel(Pqueue *);
extern	void	pqclean(Pqueue *, Proc *);

int
nrand(int n)
{
	static unsigned long next=1;
	next=next*1103515245+12345;
	return (next>>16)%n;
}

void
pqgrow(void){
	int i;
	Pqelem *p;
	p=emalloc(100*sizeof(Pqelem));
	p[99].next=0;
	for(i=0; i<99; i++)
		p[i].next=&p[i+1];
	pqfree=p;
}

void
procinit(void){
	proctab=emalloc(Nproc*sizeof(Proc **));
	proctab[0]=&interpreter;
}

int
idone(Proc *proc)
{
	USED(proc);
	return 0;
}

int
ibegin(Proc *proc)
{
	Store *s;
	long stacksize;
	Proc *p;
	int i;
	for(i=0; ; i++, iproctab++){
if(i==Nproc){print("!"); return 0;}
		if(i==Nproc)
			rpanic("out of processes");
		if(iproctab==Nproc)
			iproctab=0;
		if(proctab[iproctab]==0)
			break;
	}
	if(pflag)
		fprint(2, "%ld begins %d %s\n", proc->procnum, iproctab, (char *)proc->sp[-3]);
	stacksize=*--proc->sp/WS;
	s=(Store *)proc->sp[-1];
	p=emalloc(sizeof(Proc));
	p->prbuf=0;
	p->nprbuf=0;
	p->maxprbuf=0;
	proctab[iproctab]=p;
	p->procnum=iproctab;
	p->sp=p->stack+stacksize;
	proc->sp-=stacksize;
	memcpy((char *)p->stack, (char *)proc->sp, stacksize*WS);
	p->fp=0;
	*p->sp++=0;	/* new pc ==0 => halt */
	p->pc=((int (**)(Proc*))s->data+1);
	run(p);
	return 1;
}

/*
 * Communications
 */

/*
 * commset:	(set up)
 *	sp[-1] is number of channels on stack
 *	sp[-2] is CFlag describing first channel
 *	sp[-3] is channel pointer for first channel
 * 	 ....
 *
 *	If communication is possible already, pop all
 *	channels from stack, immediately run
 *	target (waiting at commcln) and skip your commcln.
 *	Otherwise enqueue your communications and suspend.
 *
 * commcln:	(clean up)
 *	unqueue all pending channels for this action.
 *	pop all channels from stack.
 *	if send, push target on stack.
 *
 * commset1 and commcln1 are the same, but the number of
 * channels is known to be 1.
 */

int
icommset(Proc *proc)
{
	int nchan, flags, tflags;
	Chan *chan, *c;
	Store *s;
	Proc *target;
	Pqueue *pq;
	int i, j, ti, tj, n;
	n=0;
	nchan=proc->sp[-1];
	target=0;
	chan=0;		/* set */
	tflags=0;	/* set */
	ti=0;		/* set */
	tj=-1;
	/*
	 * Scan 1: see if we can proceed immediately
	 */
	for(i=0; i<nchan; i++){
		flags=proc->sp[-2*(1+i)];
		c=(Chan *)(proc->sp[-2*(1+i)-1]);
		if(flags&Cisary){
			s=(Store *)c;
			for(j=0; j<s->len; j++){
				c=(Chan *)s->data[j];
				if(c==0)
					rerror("undefined element of array of chan in select");
				if(flags&Cissnd)
					pq=&c->rcvq;
				else
					pq=&c->sndq;
				if(pq->head && nrand(++n)==0){
					chan=c;
					target=pq->head->p;
					tflags=flags;
					ti=i;
					tj=j;
				}
			}
			continue;
		}
		if(flags&Cissnd)
			pq=&c->rcvq;
		else
			pq=&c->sndq;
		if(pq->head && nrand(++n)==0){
			chan=c;
			target=pq->head->p;
			tflags=flags;
			ti=i;
			tj=-1;
		}
	}
	if(target){
		proc->pc+=2;				/* skip commset & commcln */
		proc->pc+=((long)proc->pc[ti])/WS-1;	/* through jump table */
		target->chan=chan;
		target->target=proc;
		target->issnd=!(tflags&Cissnd);
		proc->chan=chan;
		proc->target=target;
		proc->issnd=tflags&Cissnd;
		(**target->pc)(target);
		target->pc++;
		--proc->sp;	/* pop nchan */
		for(i=0; i<nchan; i++){
			c=(Chan *)proc->sp[-2];
			decref((Store **)&c);
			proc->sp-=2;
		}
		if(tj>=0)
			*proc->sp++=tj;	/* array index */
		if(tflags&Cissnd){	/* sender must evaluate value, while receiver waits */
			*proc->sp++=(SWord)proc->target;
			return 1;
		}else{
			run(target);
			return 0;
		}
	}
	/*
	 * Post
 	 */
	for(i=0; i<nchan; i++){
		flags=proc->sp[-2*(1+i)];
		c=(Chan *)proc->sp[-2*(1+i)-1];
		if(flags&Cisary){
			s=(Store *)c;
			for(j=0; j<s->len; j++){
				c=(Chan *)s->data[j];
				if(flags&Cissnd)
					pqins(&c->sndq, proc);
				else
					pqins(&c->rcvq, proc);
			}
			continue;
		}
		if(flags&Cissnd)
			pqins(&c->sndq, proc);
		else
			pqins(&c->rcvq, proc);
	}
	return 0;
}

int
icommcln(Proc *proc)
{
	int nchan, issnd, i, j, n, flags, ti, tj, tflags;
	Chan *chan, *c;
	Store *s;
	nchan=*--proc->sp;
	issnd=proc->issnd;
	chan=proc->chan;
	n=0;
	tflags=0;	/* set */
	ti=-1;
	tj=-1;
	for(i=0; i<nchan; i++){
		flags=proc->sp[-2*i-1];
		c=(Chan *)proc->sp[-2*i-2];
		if(flags&Cisary){
			s=(Store *)c;
			for(j=0; j<s->len; j++){
				c=(Chan *)s->data[j];
				if(flags&Cissnd)
					pqclean(&c->sndq, proc);
				else
					pqclean(&c->rcvq, proc);
				if(c==chan && issnd==(flags&Cissnd) && nrand(++n)==0){
					tflags=flags;
					ti=i;
					tj=j;
				}
			}
			decref(&s);
			continue;
		}
		if(flags&Cissnd)
			pqclean(&c->sndq, proc);
		else
			pqclean(&c->rcvq, proc);
		if(c==chan && issnd==(flags&Cissnd) && nrand(++n)==0){
			tflags=flags;
			ti=i;
			tj=-1;
		}
		decref((Store **)&c);
	}
	if(ti==-1 || (tj==-1 && (tflags&Cisary)))
		panic("icommcln");
	proc->sp-=2*nchan;
	if(tj>=0)
		*proc->sp++=tj;			/* array index */
	proc->pc++;				/* skip commcln */
	proc->pc+=((long)proc->pc[ti])/WS-1;	/* through jump table */
	if(issnd)
		*proc->sp++=(SWord)proc->target;
	/* no return value; never run from xec() */
	return 0;
}

int
icommset1(Proc *proc)
{
	int issnd;
	Chan *chan;
	Proc *target;
	issnd=proc->sp[-1]&Cissnd;
	chan=(Chan *)proc->sp[-2];
	if(issnd){
		if(chan->rcvq.head==0){
			pqins(&chan->sndq, proc);
			return 0;
		}
		target=chan->rcvq.head->p;
	}else{
		if(chan->sndq.head==0){
			pqins(&chan->rcvq, proc);
			return 0;
		}
		target=chan->sndq.head->p;
	}
	proc->pc++;	/* skip commcln */
	target->chan=chan;
	target->target=proc;
	target->issnd=!issnd;
	proc->chan=chan;
	proc->target=target;
	proc->issnd=issnd;
	(**target->pc)(target);
	target->pc++;
	decref((Store **)&chan);
	proc->sp-=2;
	if(issnd){	/* sender must evaluate value, while receiver waits */
		*proc->sp++=(SWord)target;
		return 1;
	}else{
		run(target);
		return 0;
	}
}

int
icommcln1(Proc *proc)
{
	Chan *chan;
	chan=proc->chan;
	if(proc->issnd)
		pqclean(&chan->sndq, proc);
	else
		pqclean(&chan->rcvq, proc);
	decref((Store **)&chan);
	proc->sp-=2;
	if(proc->issnd)
		*proc->sp++=(SWord)proc->target;
	/* no return value; never run from xec() */
	return 0;
}

int
isnd(Proc *proc)
{
	Proc *target;
	SWord s;
	s=*--proc->sp;
	target=(Proc *)proc->sp[-1];
	proc->sp[-1]=s;
	*target->sp++=s;
	run(target);
	if(mflag)
		fprint(2, "%ld sends %ld to %ld\n", proc->procnum, s, target->procnum);
	return 1;
}

int
isndptr(Proc *proc)
{
	Proc *target;
	Store *s;
	s=(Store *)*--proc->sp;
	target=(Proc *)proc->sp[-1];
	s->ref++;
	proc->sp[-1]=(SWord)s;
	*target->sp++=(SWord)s;
	run(target);
	if(mflag)
		fprint(2, "%ld sends 0x%p to %ld\n", proc->procnum, s, target->procnum);
	return 1;
}

/*
 * Pqueue management
 */

void
pqins(Pqueue *q, Proc *p)
{
	Pqelem *pq;
	if(pqfree==0)
		pqgrow();
	pq=pqfree;
	pqfree=pqfree->next;
	pq->p=p;
	pq->next=0;
	if(q->tail==0)
		q->head=pq;
	else
		q->tail->next=pq;
	q->tail=pq;
}

Proc *
pqdel(Pqueue *q)
{
	Proc *p;
	Pqelem *pq;
	pq=q->head;
	if(pq==0)
		panic("qdel");
	q->head=pq->next;
	if(q->head==0)
		q->tail=0;
	p=pq->p;
	pq->next=pqfree;
	pqfree=pq;
	return p;
}

void
pqclean(Pqueue *q, Proc *p)
{
	Pqelem *pq, *last, *next;
	for(last=0,pq=q->head; pq; pq=next){
		next=pq->next;
		if(pq->p!=p)
			last=pq;
		else{
			if(last)
				last->next=pq->next;
			if(pq==q->head){
				q->head=pq->next;
				if(q->head==0)
					last=0;
			}
			if(pq==q->tail)
				q->tail=last;
			pq->next=pqfree;
			pqfree=pq;			/* CAN WE RETURN HERE? */
		}
	}
}

/*
 * Debugging
 */

void
processes(int all)
{
	int i;
	Proc *proc=curproc();
	if(proc){
		print("\nprocesses: curproc (%ld):\n", proc->procnum);
		stacktrace(proc);
	}
	if(all)
		for(i=0; i<Nproc; i++)
			if(proctab[i] && proctab[i]!=proc){
				print("%d:\n", i);
				stacktrace(proctab[i]);
			}
}

void
stacktrace(Proc *p)
{
	long *fp;
	char *s;
	char al[100];
	static char *fmt[]={"%ld%c", "'%c'%c", "%U%c", "%C%c", "%A%c", "0x%lx%c"};
	int i, n;
	for(fp=p->fp; fp && fp[-3]; fp=(long *)*fp){
		s=(char *)fp[-3];
		print("\t%s(", s);
		n=nargs(s, al);
		for(i=0; i<n; i++)
			print(fmt[(uchar)al[i]], fp[-(3+1+i)], ",)"[i==n-1]);
		if(n==0)
			print(")");
		print("\n");
	}
}

int
Aconv(va_list *va, Fconv *f)
{
	int i, n;
	Store *s;
	char buf[32];
	s = va_arg(*va, Store*);
	n=s->len;
	if(n>4)
		n=4;
	sprint(buf, "[%ld]{", s->len);
	strconv(buf, f);
	for(i=0; i<n; i++){
		sprint(buf, "0x%lx%c", s->data[i], ","[i==n]);
		strconv(buf, f);
	}
	if(s->len!=n)
		strconv(",...", f);
	strconv("}", f);
	return sizeof(long);
}

int
Cconv(va_list *va, Fconv *f)
{
	Store *s;

	s = va_arg(*va, Store*);
	if(s->len>128){
		strconv("\"very long string\"", f);
		return sizeof(long);
	}
	strconv("\"", f);
	strconv((char *)(s->data), f);
	strconv("\"", f);
	return sizeof(long);
}

int
Uconv(va_list *va, Fconv *f)
{
	USED(va);
	strconv("unit", f);
	return sizeof(long);
}
