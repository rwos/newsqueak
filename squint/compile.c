#include "store.h"
#include "node.h"
#include "symbol.h"
#include "ydefs.h"
#include "comm.h"
#include "inst.h"
#include <u.h>
#include <lib9.h>
#include "errjmp.h"

#include "fns.h"

long		resultloc;
long		returnloc;
long		breakloc=-1;
long		continueloc=-1;
Node		*formals;
long		autooffset;
int		didbecome=0;
extern int	bflag;
extern int	cflag;
extern int	nscope;
extern Node	arychartype;

void		arygen(Node *, Node *, int, long);
void		callgen(Node *, int);
void		condgen(Node *, Node *, Inst, Inst, long, long, int);
void		constgen(long);
void		elembitsgen(Node *, int *, long *);
int		fdecrefgen(Node *, long);
void		ifgen(Node *);
void		lgen1(Node *, int, int, int);
void		loopgen(Node *);
void		mallocgen(Node *);
void		mkgen(Node *, Node *);
void		popgen(Node *);
void		printgen(Node *);
void		proggen(Node *, Node *);
void		selchangen(Node *);
void		seltblgen(Node *, int, int *, int *);
void		selgen(Node *);
void		switchgen(Node *, Node *);
void		switchgen1(Node *, int, int);
void		trlrgen(void);
void		valgen(Node *);

void
compile(Node *n)	/* called from parser only */
{
	extern long autooffset;
	Errjmp x;
	n=constants(n);
	if(cflag){
		fprint(2, "%z:constants:\n");
		dump(n, 0);
	}
	errsave(x);
	if(errmark()){
		autooffset=0;
		resultloc=0;
		breakloc=-1;
		continueloc=-1;
		didbecome=0;
		freenode(n);
		errrest(x);
		errjmp();
	}
	istart();
	gen(n, 0);
	freenode(n);
	errrest(x);
}

void
gen(Node *n, int retain)
{
	int i;

	if(n==0)
		return;
	switch(n->t){
	case NArrayref:
		arygen(n->l, n->r, 0, 0L);
		if(!retain)
			popgen(n->l->o.s->val->type->r);
		return;
	case NBecome:
		didbecome=1;
		if(n->l->t==NCall && !bflag){
			callgen(n->l, Ibecome);
			return;
		}
		gen(n->l, 1);
		n=n->r;
		if(n->o.t==TID)
			n=typeoftid(n);
		switch(n->o.t){
		case TInt:
		case TChar:
			emit(Istoreauto);
			emitconst(-WS*(4+length(formals)));
			break;
		case TArray:
		case TChan:
		case TProg:
		case TStruct:
			emit(Istoreptrauto);
			emitconst(-WS*(4+length(formals)));
			break;
		case TUnit:
			break;
		default:
			panic("can't compile %t become", n);
		}
		scopedecrefgen();
		trlrgen();
		return;
	case NBegin:
		callgen(n->l, Ibegin);
		return;
	case NBreak:
		if(breakloc==-1)
			lerror(n, "break not in loop");
		if(breakloc)	/* chain previous break to here */
			patch(breakloc, (long)(here()-breakloc-1)*WS);
		emit(Ijmp);
		breakloc=here();
		emitconst(0L);
		return;
	case NCall:
		callgen(n, Icall);
		if(!retain)
			popgen(etypeoft(n->l)->r);
		return;
	case NComplete:
		gen(n->l, retain);
		return;
	case NContinue:
		if(continueloc==-1)
			lerror(n, "continue not in loop");
		if(continueloc)	/* chain previous continue to here */
			patch(continueloc, (long)(here()-continueloc-1)*WS);
		emit(Ijmp);
		continueloc=here();
		emitconst(0L);
		return;
	case NDecl:
	case NDeclsc:
		declare(n, 0, 0, 1);
		return;
	case NExpr:
		switch(n->o.i){
		case GE:
			i=Ige;
		Binop:
			gen(n->l, 1);
			gen(n->r, 1);
			if(eqtype(etypeof(n->l), &arychartype)){
				emit(Istrcmp);
				constgen(0L);
			}
			emit(i);
		Popit:
			if(!retain)
				emit(Ipop);
			return;
		case LE:
			i=Ile;
			goto Binop;
		case NE:
			i=Ine;
			goto Binop;
		case EQ:
			i=Ieq;
			goto Binop;
		case '>':
			i=Igt;
			goto Binop;
		case '<':
			i=Ilt;
			goto Binop;
		case '+':
			i=Iadd;
			goto Binop;
		case '-':
			i=Isub;
			goto Binop;
		case '*':
			i=Imul;
			goto Binop;
		case '/':
			i=Idiv;
			goto Binop;
		case '%':
			i=Imod;
			goto Binop;
		case '&':
			i=Iand;
			goto Binop;
		case '|':
			i=Ior;
			goto Binop;
		case '^':
			i=Ixor;
			goto Binop;
		case LSH:
			i=Ilsh;
			goto Binop;
		case RSH:
			i=Irsh;
			goto Binop;
		case ANDAND:
			condgen(n->l, n->r, Ijmptrue, Ijmpfalse, 0L, 1L, retain);
			return;
		case OROR:
			condgen(n->l, n->r, Ijmpfalse, Ijmptrue, 1L, 0L, retain);
			return;
		case CAT:
			gen(n->l, 1);
			gen(n->r, 1);
			emit(Icat);
			return;
		case DEL:
			gen(n->l, 1);
			gen(n->r, 1);
			emit(Idel);
			return;
		case PRINT:
			gen(n->l, 1);
			printgen(n->l);
			emit(Isprint);
			if(!retain)
				emit(Iprint);
			return;
		case SND:
			gen(n->l, 1);
			constgen((long)Cissnd);
			emit(Icommset1);
			emit(Icommcln1);
			gen(n->r, 1);
			if(isptrtype(etypeoft(n->l)->r))
				emit(Isndptr);
			else
				emit(Isnd);
			if(!retain)
				popgen(etypeof(n));
			return;
		case RCV:
			gen(n->l, 1);
			constgen(0L);	/* not Cissnd */
			emit(Icommset1);
			emit(Icommcln1);
			if(!retain)
				popgen(etypeof(n));
			return;
		case '=':
			gen(n->r, 1);
			if(retain)
				dupgen(etypeof(n->r), 1);
			lgen(n->l);
			return;
		case LEN:
			gen(n->l, 1);
			emit(Ilen);
			goto Popit;
		case REF:
			if(isptrtype(etypeof(n->l))){
				gen(n->l, 1);
				emit(Iref);
			}else
				constgen(1L);
			goto Popit;
		case DEF:
			if(retain && n->l->t==NID && isinttype(etypeof(n->l))){
				constgen(1L);
				return;
			}
			/*
			 * don't really need to call lgen1, which will uniquify our
			 * array for us, but it does no harm, and it's easy.
			 */
			lgen1(n->l, Idefauto, Idef, Idefary);
			goto Popit;
		case UMINUS:
			gen(n->l, 1);
			emit(Ineg);
			goto Popit;
		case '~':
			gen(n->l, 1);
			emit(Inot);
			goto Popit;
		case '!':
			gen(n->l, 1);
			emit(Ilnot);
			goto Popit;
		case INC:
			lgen1(n->l, Iincauto, Iinc, Iincary);
			goto Popit;
		case DEC:
			lgen1(n->l, Idecauto, Idec, Idecary);
			goto Popit;
		default:
			panic("can't compile %e expression", n);
		}

	case NExprlist:
		/*
		 * This is an arg or element list; first is pushed last
		 */
		gen(n->r, 1);
		gen(n->l, 1);
		return;
	case NID:
		if(!retain)
			return;
		switch(type_of(n)->o.t){
		case TInt:
		case TChar:
			if(n->o.s->val->isauto){
				emit(Ipushauto);
				emitconst(n->o.s->val->store.off);
			}else{
				emit(Ipush);
				emitconst((long)&n->o.s->val->store.l);
			}
			return;
		case TProg:
		case TArray:
		case TChan:
		case TStruct:
			if(n->o.s->val->isauto){
				emit(Ipushptrauto);
				emitconst(n->o.s->val->store.off);
			}else{
				emit(Ipushptr);
				emitconst((long)&n->o.s->val->store.l);
			}
			return;
		case TUnit:
			if(retain)
				constgen(0L);
			return;
		case TType:
			lerror(n, "attempt to evaluate type variable %m", n);
		default:
			panic("can't compile type %t", n->o.s->val->type);
		}
	case NIf:
		ifgen(n);
		return;
	case NList:
		gen(n->l, 0);
		gen(n->r, 0);
		return;
	case NLoop:
		loopgen(n);
		return;
	case NMk:
		mkgen(n->l, n->r);
		return;
	case NNum:
		if(retain)
			constgen(n->o.l);
		return;
	case NProg:
		if(retain)
			proggen(n->l, n->r);
		return;
	case NResult:
		if(resultloc==0)
			lerror(n, "result not in val");
		gen(n->l, 1);
		emit(Ijmp);
		emitconst((long)(resultloc-here()-1)*WS);
		return;
	case NScope:
		pushscope();
		if(nscope==1){
			int nauto;
			autooffset=0;
			emit(Ipushfp);
			nauto=here();
			emitconst(0L);
			gen(n->l, 0);
			patch((int)nauto, autooffset);
			emit(Ipop);	/* Ipopfp() */
		}else
			gen(n->l, 0);
		scopedecrefgen();
		popscope();
		return;
	case NSelect:
		selgen(n->l);
		return;
	case NSmash:{
		Value *vl, *vr;
		vl=n->l->o.s->val;
		vr=n->r->o.s->val;
		if(vr->type->o.t==TType){
			freenode(vl->type);
			vl->type=dupnode(vr->type);
			return;
		}
		gen(n->r, 1);
		/*
		 * Free old values; tricky: push as int, pop as ptr
		 */
		if(isptrtype(vl->type)){
			if(vl->isauto){
				emit(Ipushauto);
				emitconst(vl->store.off);
			}else{
				emit(Ipush);
				emitconst((long)&vl->store.l);
			}
			emit(Ipopptr);
		}
		if(vl->isauto){
			emit(Istoreauto);
			emitconst(vl->store.l);
			return;
		}
		emit(Istore);
		emitconst((long)&vl->store.l);
		return;
	}
	case NString:
		if(retain){
			emit(Ipushdata);
			emitconst((long)n->o.st);
			emitstore(n->o.st);
			n->o.st->ref++;
		}
		return;
	case NStructref:
		arygen(n->l, n->r, 1, n->o.l);
		return;
	case NSwitch:
		switchgen(n->l, n->r);
		return;
	case NUnit:
		if(retain)
			constgen(0L);
		return;
	case NVal:
		valgen(n->l);
		if(!retain)
			popgen(n->o.n);
		return;
	}
	panic("can't compile node %n", n);
	return;
}

void
arygen(Node *a, Node *i, int isstr, long off)
{
	int ptr, ischar;
	if(isstr){
		ptr=isptrtype(i);
		constgen(off);
		ischar=0;
	}else{
		Node *t=etypeoft(a)->r;
		ptr=isptrtype(t);
		gen(i, 1);
		ischar=t->o.t==TChar;
	}
	if(a->t!=NID){
		gen(a, 1);
		emit(ptr? Ipusharyptrexpr :
			(ischar? Ipusharycharexpr :Ipusharyexpr));
	}else if(a->o.s->val->isauto){
		emit(ptr? Ipusharyptrauto :
			(ischar? Ipusharycharauto :Ipusharyauto));
		emitconst(a->o.s->val->store.off);
	}else{
		emit(ptr? Ipusharyptr :
			(ischar? Ipusharychar :Ipushary));
		emitconst((long)&a->o.s->val->store.l);
	}
}

void
lgen(Node *n)
{
	switch(n->t){
	case NID:
		switch(type_of(n)->o.t){
		case TChar:
			if(n->o.s->val->isauto){
				emit(Istorecharauto);
				emitconst(n->o.s->val->store.off);
				return;
			}
			emit(Istorechar);
			emitconst((long)&n->o.s->val->store.l);
			return;
		case TInt:
		case TUnit:
			if(n->o.s->val->isauto){
				emit(Istoreauto);
				emitconst(n->o.s->val->store.off);
				return;
			}
			emit(Istore);
			emitconst((long)&n->o.s->val->store.l);
			return;
		case TArray:
		case TChan:
		case TProg:
		case TStruct:
			if(n->o.s->val->isauto){
				emit(Istoreptrauto);
				emitconst(n->o.s->val->store.off);
				return;
			}
			emit(Istoreptr);
			emitconst((long)&n->o.s->val->store.l);
			return;

		default:
			panic("lgen: ID type %t", n->o.s->val->type);
			return;
		}
	case NArrayref:
		gen(n->r, 1);
		goto Genref;
	case NStructref:
		constgen(n->o.l);
	Genref:
		lgen1(n->l, Ipushuniqauto, Ipushuniq, Ipushuniqary);
		emit(Istoreary);
		return;
	default:
		panic("lgen: lvalue node %n", n);
	}
}

/*
 * n is a compound object about to be assigned into
 */
void
lgen1(Node *n, int Iauto, int Ivar, int Iary)
{
	switch(n->t){
	case NID:
		if(n->o.s->val->isauto){
			emit(Iauto);
			emitconst(n->o.s->val->store.off);
			return;
		}
		emit(Ivar);
		emitconst((long)&n->o.s->val->store.l);
		return;
	case NArrayref:
		gen(n->r, 1);
		goto Genref;
	case NStructref:
		constgen(n->o.l);
	Genref:
		lgen1(n->l, Ipushuniqauto, Ipushuniq, Ipushuniqary);
		emit(Iary);
		return;
	default:
		panic("lgen1: lvalue node %n", n);
	}
}

void
ifgen(Node *n)
{
	int loc1, loc2;
	gen(n->o.n, 1);
	emit(Ijmpfalse);
	loc1=here();
	emit(0);
	gen(n->l, 0);
	if(n->r==0){
		patch(loc1, (long)(here()-loc1-1)*WS);
		return;
	}
	emit(Ijmp);
	loc2=here();
	emit(0);
	patch(loc1, (long)(here()-loc1-1)*WS);
	gen(n->r, 0);
	patch(loc2, (long)(here()-loc2-1)*WS);
	return;
}

void
valgen(Node *n)
{
	int loc1, loc2;
	int orl;
	emit(Ijmp);
	loc1=here();
	emitconst(0L);
	orl=resultloc;
	resultloc=here();
	emit(Ijmp);
	loc2=here();
	emitconst(0L);
	patch(loc1, (long)(here()-loc1-1)*WS);
	gen(n, 1);
	emit(Ivalnoresult);
	patch(loc2, (long)(here()-loc2-1)*WS);
	resultloc=orl;
}

void
loopgen(Node *n)
{
	int loc0, loc1, loc2;
	int obreakloc, ocontinueloc;

	obreakloc=breakloc;
	ocontinueloc=continueloc;
	breakloc=0;
	continueloc=0;
	SET(loc0);
	SET(loc1);
	if(n->o.i){	/* enter loop at top, so jump to body */
		emit(Ijmp);
		loc0=here();
		emit(0);
	}
	gen(n->r->l, 0);	/* left expr */
	if(n->r->r){		/* jump to condition */
		emit(Ijmp);
		loc1=here();
		emit(0);
	}
	if(n->o.i)
		patch(loc0, (here()-loc0-1)*WS);
	loc2=here();
	gen(n->l, 0);		/* body */
	if(continueloc)
		patch(continueloc, (here()-continueloc-1)*WS);
	gen(n->r->o.n, 0);	/* right expr */
	if(n->r->r){
		patch(loc1, (here()-loc1-1)*WS);
		gen(n->r->r, 1);
		emit(Ijmptrue);
	}else
		emit(Ijmp);
	emitconst((loc2-here()-1)*WS);
	if(breakloc)
		patch(breakloc, (here()-breakloc-1)*WS);
	continueloc=ocontinueloc;
	breakloc=obreakloc;
}

void
condgen(Node *l, Node *r, Inst i1, Inst i2, long t1, long t2, int retain)
{
	int loc1, loc2, loc3;
	gen(l, 1);
	emit(i1);
	loc1=here();
	emit(0);
	loc2=here();
	if(retain)
		constgen(t1);
	emit(Ijmp);
	loc3=here();
	emit(0);
	patch(loc1, (long)(here()-loc1-1)*WS);
	gen(r, 1);
	emit(i2);
	emitconst((long)(loc2-here()-1)*WS);
	if(retain)
		constgen(t2);
	patch(loc3, (long)(here()-loc3-1)*WS);
}

void
callgen(Node *n, int callinst)
{
	Node *pt;
	pt=etypeof(n->l);
	/*
	 * Space for result
	 */
	constgen(0L);
	/*
	 * Args
	 */
	gen(n->r, 1);
	/*
	 * Call
	 */
	emit(Ipushconst);
	if(n->l->t==NID)
		emitconst((long)n->l->o.s->name);
	else{
		char buf[128];
		char *p;
		sprint(buf, "prog(){call on %Z}", n->line);
		p=emalloc(strlen(buf)+1);
		strcpy(p, buf);
		emitconst((long)p);
	}
	gen(n->l, 1);
	switch(callinst){
	case Icall:
		emit(Icall);
		return;
	case Ibegin:
		constgen(WS*(1+1+1+length(pt->l)));	/* result+procname+prog+args */
		emit(Ibegin);
		return;
	case Ibecome:
		constgen(WS*(1+1+1+length(pt->l)));	/* result+procname+prog+args */
		scopedecrefgen();
		fdecrefgen(formals, -4L*WS);
		emit(Ibecome);
		if(formals)
			emitconst(length(formals)*WS);
		else
			emitconst(0L);
		return;
	}
	panic("callgen");
}

void
selgen(Node *n)
{
	int tbl, i;
	long l;
	int ends[200];
	selchangen(n);
	l=length(n);
	constgen(l);
	emit(Icommset);
	emit(Icommcln);
	if(l>(sizeof ends/sizeof ends[0]))
		panic("selgen table too small");
	tbl=here();
	emitspace(l);
	i=0;
	seltblgen(n, tbl, ends, &i);
	for(i=0; i<l; i++)
		patch(ends[i], (long)(here()-ends[i]-1)*WS);
}

void
selchangen(Node *n)
{
	long flags;
	if(n->t==NList){
		selchangen(n->l);
		selchangen(n->r);
		return;
	}
	if(n->t!=NCase)
		panic("selchangen");
	n=n->l->l;
	if(n->o.t=='=')
		n=n->r;		/* n is now RCV or SND */
	flags=0;
	if(n->o.t==SND)
		flags|=Cissnd;
	n=n->l;			/* n is now channel */
	if(n->t==NArraycom){
		flags|=Cisary;
		n=n->l;
	}else if(etypeoft(n)->o.t==TArray)
		flags|=Cisary;
	gen(n, 1);
	constgen(flags);
}

void
seltblgen(Node *n, int tbl, int *ends, int *ip)
{
	Node *c, *s, *l, *t;
	if(n->t==NList){
		/* chans are eval'ed from the top, so table is backwards */
		seltblgen(n->r, tbl, ends, ip);
		seltblgen(n->l, tbl, ends, ip);
		return;
	}
	if(n->t!=NCase)
		panic("seltblgen");
	if(n->l->t==NList)
		error("sorry, empty cases not implemented");
	patch(tbl+*ip, (long)(here()-tbl)*WS);
	c=n->l->l;	/* communication */
	s=n->r;		/* statement */
	l=0;
	if(c->o.t=='='){
		l=c->l;	/* lvalue */
		c=c->r;
	}
	if(c->o.t==SND){
		gen(c->r, 1);
		if(isptrtype(etypeoft(c->l)->r))
			emit(Isndptr);
		else
			emit(Isnd);
	}
	c=c->l;	/* channel expression */
	/*
	 * The value is still on the stack; save it or toss it
	 */
	if(l)
		lgen(l);
	else if(c->t==NArraycom){
		t=etypeoft(c->l)->r;
		if(t->o.t==TID)
			t=typeoftid(t);
		popgen(t->r);
	}else
		popgen(etypeoft(c)->r);
	if(c->t==NArraycom){	/* save array index */
		if(c->r)
			lgen(c->r);
		else
			emit(Ipop);
	}
	gen(s, 0);
	emit(Ijmp);
	ends[*ip]=here();
	(*ip)++;
	emitconst(0L);
}

void
switchgen(Node *s, Node *e)
{
	int isptr, out;
	isptr=isptrtype(etypeof(e));
	gen(e, 1);
	emit(Ijmp);
	emitconst(2*WS);
	emit(Ijmp);	/* each case jumps to here to get out */
	out=here();
	emitconst(0L);
	switchgen1(s, isptr, out-1);
	/* pop leftover value if no case matched */
	if(isptr)
		emit(Ipopptr);
	else
		emit(Ipop);
	patch(out, (here()-out-1)*WS);
}

void
switchgen1(Node *s, int isptr, int out)
{
	Node *e;
	int loc;
	if(s->t==NList){
		switchgen1(s->l, isptr, out);
		switchgen1(s->r, isptr, out);
		return;
	}
	if(s->t!=NCase)
		panic("switchgen1");
	if(s->r==0)
		error("sorry; can't fold cases together yet");
	if(s->l->t==NDefault)
		loc=-1;
	else{
		e=s->l->l;
		if(isptr){	/* string */
			emit(Idupptr);
			gen(e, 1);
			emit(Istrcmp);
			constgen(0L);
		}else{
			emit(Idup);
			gen(e, 1);
		}
		emit(Ieq);
		emit(Ijmpfalse);
		loc=here();
		emitconst(0L);
	}
	if(isptr)
		emit(Ipopptr);
	else
		emit(Ipop);
	gen(s->r, 0);
	emit(Ijmp);
	emitconst((out-here()-1)*WS);
	if(loc!=-1)
		patch(loc, (here()-loc-1)*WS);
}

void
popgen(Node *t)
{
	if(isptrtype(t))
		emit(Ipopptr);
	else if(isinttype(t) || t->o.t==TUnit)
		emit(Ipop);
	else
		panic("popgen %t\n", t);
}

void
genfreeauto(Symbol *s)
{
	if(!s->val->isauto)
		panic("genfreeauto");
	if(isptrtype(s->val->type)){
		emit(Idecrefauto);
		emitconst(s->val->store.off);
	}
}

void
printgen(Node *n)
{
	Node *t;
	if(n==0)
		return;
	if(n->t==NExprlist){
		printgen(n->l);
		printgen(n->r);
		return;
	}
	t=etypeoft(n);
	switch(t->o.t){
	case TArray:
	case TChan:
	case TProg:
	case TStruct:
		emit(Iprintary);
		break;
	case TChar:
		emit(Iprintchar);
		break;
	case TInt:
		emit(Iprintint);
		break;
	case TUnit:
		emit(Iprintunit);
		break;
	default:
		panic("printgen: bad type %t", t);
	}
}

void
proggen(Node *t, Node *n)
{
	int or;
	Node *of;
	Errjmp s;
	Store *p;
	long len, loc;
	long nauto, oao;
	int osp, odb;
	extern int (**prog)(Proc*);
	extern int storeprog;
	odb=didbecome;
	oao=autooffset;
	or=returnloc;
	of=formals;
	osp=storeprog;
	autooffset=0;
	returnloc=0;
	formals=t->l;
	errsave(s);
	if(errmark()){
		didbecome=odb;
		returnloc=or;
		formals=of;
		autooffset=oao;
		storeprog=osp;
		errrest(s);
		errjmp();
	}
	loc=here();
	emitconst(0);	/* space for locals pointer */
	pushscope();
	dclformals(t->l);
	autooffset=0;
	emit(Ipushfp);
	nauto=here();
	storeprog=here();
	emitconst(0L);
	gen(n, 0);
	trlrgen();
	patch((int)nauto, autooffset);
	popscope();
	errrest(s);
	if(!didbecome && n && t->r->o.t!=TUnit)
		lerror(n, "prog has no become");
	didbecome=odb;
	autooffset=oao;
	returnloc=or;
	formals=of;
	storeprog=osp;
	len=here()-loc+1;
	p=emalloc(SHSZ+len*WS);
	memcpy(p->data, prog+loc, len*WS);
	p->ref=1;
	p->len=len;
	p->type=Sprog;
	setprog(loc);
	emit(Ipushdata);
	emitconst((long)p);
	emitstore(p);
}

void
trlrgen(void)
{
	if(returnloc){
		emit(Ijmp);
		emitconst((long)(returnloc-here()-1)*WS);
		return;
	}
	returnloc=here();
	fdecrefgen(formals, -4*WS);
	emit(Iret);
	if(formals)
		emitconst(length(formals)*WS);
	else
		emitconst(0L);
}

int
fdecrefgen(Node *types, long offset)
{
	if(types==0)
		return 0;
	if(types->t==NList){
		offset=fdecrefgen(types->l, offset);
		return fdecrefgen(types->r, offset);
	}
	if(types->t!=NFormal)
		panic("fdecrefgen");
	types=types->r;
	if(isptrtype(types)){
		emit(Idecrefauto);
		emitconst(offset);
	}
	return offset-WS;
}

void
dupgen(Node *t, int n)
{
	while(n--)
		emit(isptrtype(t)? Idupptr : Idup);
}

void
mkgen(Node *t, Node *v)
{
	switch(t->o.t){
	case TAggr:
		lerror(v, "can't derive type in mk %m", v);
	case TChar:
	case TInt:
	case TUnit:
		if(v)
			gen(v, 1);
		else
			constgen(0L);
		return;
	case TID:
		mkgen(typeoftid(t), v);
		return;
	case TChan:
		if(v)
			gen(v, 1);
		else{
			constgen((long)(sizeof(Chan)-sizeof(Store)));
			mallocgen(t);
		}
		return;
	case TArray:
		if(v==0){
			if(t->l==0)
				constgen(0L);
			else
				gen(t->l, 1);
			mallocgen(t);
			return;
		}
		gen(v, 1);
		if(v->t!=NExprlist && eqtype(t, etypeof(v)))
			return;
		if(v->t==NString && eqtype(t, etypeof(v)))
			constgen(v->o.st->len);
		else
			constgen(length(v));
		emit(Idup);
		if(t->l)
			gen(t->l, 1);
		else
			constgen(0L);
		emit(Imax);
		mallocgen(t);
		if(t->r->o.t==TChar){
			if(v->t==NString)
				emit(Imemcpychar);
			else
				emit(Imemcpycharint);
		}else
			emit(Imemcpy);
		return;
	case TProg:
		if(v==0){
			v=new(NProg, dupnode(t), (Node *)0, (Node *)0);
			gen(v, 1);
			freenode(v);
			return;
		}
		gen(v, 1);
		return;
	case TStruct:
		if(v==0){
			mallocgen(t);
			return;
		}
		gen(v, 1);
		if(v->t!=NExprlist && eqtype(t, etypeof(v)))
			return;
		constgen((long)length(v));
		mallocgen(t);
		emit(Imemcpystruct);
		return;		
	default:
		panic("mkgen: bad type %t", t);
	}
}

void
mallocgen(Node *t)
{
	switch(t->o.t){
	case TArray:
		t=t->r;
		if(t->o.t==TID)
			t=typeoftid(t);
		if(isptrtype(t)){
			constgen((long)Saryptr);
			emit(Imalloc);
		}else if(t->o.t==TInt || t->o.t==TUnit){
			constgen((long)Saryint);
			emit(Imalloc);
		}else if(t->o.t==TChar)
			emit(Imallocarychar);
		else
			panic("mallocgen array of %t", t);
		return;
	case TStruct:{
		int pos=0;
		long bits=0;
		t=t->l;
		elembitsgen(t, &pos, &bits);
		if(pos)
			constgen(bits);
		constgen((long)length(t));
		emit(Imallocstruct);
		return;
	}
	case TChan:
		constgen((long)Schan);
		emit(Imalloc);
		return;
	}
	panic("mallocgen of %t", t);
}

void
elembitsgen(Node *t, int *pos, long *bits)
{
	int i;
	if(t->t==NList){
		elembitsgen(t->l, pos, bits);
		elembitsgen(t->r, pos, bits);
		return;
	}
	if(t->t!=NElem)
		panic("elembitsgen %n", t);
	for(i=length(t); --i>=0; ){
		if(*pos==BPW){
			constgen(*bits);
			*pos=0;
			*bits=0;
		}
		if(isptrtype(t->r))
			*bits|=1L<<*pos;
		(*pos)++;
	}
}

void
constgen(long l)
{
	if(l<-2 || l>10){
		emit(Ipushconst);
		emitconst(l);
		return;
	};
	switch((int)l){
	case -2:
		emit(Ipush_2);
		break;
	case -1:
		emit(Ipush_1);
		break;
	case 0:
		emit(Ipush0);
		break;
	case 1:
		emit(Ipush1);
		break;
	case 2:
		emit(Ipush2);
		break;
	case 3:
		emit(Ipush3);
		break;
	case 4:
		emit(Ipush4);
		break;
	case 5:
		emit(Ipush5);
		break;
	case 6:
		emit(Ipush6);
		break;
	case 7:
		emit(Ipush7);
		break;
	case 8:
		emit(Ipush8);
		break;
	case 9:
		emit(Ipush9);
		break;
	case 10:
		emit(Ipush10);
		break;
	default:
		panic("constgen");
	}
}

int
printable(Node *n)
{
	if(n==0)
		return 0;
	switch(n->t){
	case NExpr:
		return n->o.t!='=';
	case NArrayref:
	case NCall:
	case NComplete:
	case NID:
	case NMk:
	case NNum:
	case NProg:
	case NString:
	case NStructref:
	case NUnit:
	case NVal:
		return 1;
	}
	return 0;
}
