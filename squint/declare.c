#include "store.h"
#include "node.h"
#include "symbol.h"
#include "ydefs.h"
#include <u.h>
#include <lib9.h>

#include "fns.h"

extern int	nscope;
void		dcl(Node *, Node *, int, Node *, int);
void		progid(Node *);

void
declare(Node *n, int stclass, int dotypchk, int docomp)
{
	extern int iflag;
	if(n==0)
		return;
	if(n->t==NList){
		declare(n->l, stclass, dotypchk, docomp);
		declare(n->r, stclass, dotypchk, docomp);
		return;
	}
	if(n->t==NDeclsc){
		declare(n->l, n->o.i, dotypchk, docomp);
		return;
	}
	if(dotypchk)
		type(n->o.n, 0);
	if(n->r==0){
		if(n->o.n==0)
			panic("declare: no type");
		if(n->o.n->t==NMk && n->o.n->l==0)
			lerror(n, "can't derive type in declaration");
		n->r=dupnode(etypeof(n->o.n));
	}
	if(dotypchk){
		type(n->r, 0);
		if(n->o.n){
			/*
			 * Make it a mk
			 */
			if(n->o.n->t!=NMk)
				n->o.n=new(NMk, dupnode(etypeof(n->o.n)), n->o.n, (Node *)0);
			if(!compatmktype(n->r, n->o.n))
				lerror(n, "type clash in declaration (%t %t)\n",
					n->r, etypeof(n->o.n));
		}
	}
	if(docomp && n->o.n){
		if(dotypchk)	/* top level declaration */
			n->o.n=constants(n->o.n);
		gen(n->o.n, 1);
		dupgen(n->r, length(n->l)-1);
	}else
		docomp=0;
	dcl(n->l, n->r, stclass, n->o.n, docomp);
	if(n->o.n && docomp && nscope==0){
		if(iflag)
			idump();
		if(executeallowed)
			execute();
	}
}

void
dcl(Node *id, Node *typ, int stclass, Node *val, int docomp)
{
	if(id->t==NList){
		dcl(id->l, typ, stclass, val, docomp);
		dcl(id->r, typ, stclass, val, docomp);
		return;
	}
	if(typ->o.t==TID && typ->l->o.s->val->type->o.t!=TType)
		error("%m not a type", typ->l);
	if(id->t!=NID)
		panic("dcl not ID");
	pushval(id->o.s, dupnode(typ));
	if(stclass&SCbltin){
		if(!istopscope())
			lerror(typ, "builtins cannot be declared as locals");
		id->o.s->val->store.l=bltinval(id->o.s->name, typ);
	}
	if(docomp)
		lgen(id);
	id->o.s->val->stclass=stclass;
}

/*
 * To compile this
 * 	rec {
 * 		x : chan of T = f(x,y);
 * 		y : chan of T = g(x,y);
 * 	};
 * convert it to this
 * 	x : chan of T = mk();
 * 	y : chan of T = mk();
 * 	x1 : chan of T = f(x,y);
 * 	y1 : chan of T = g(x,y);
 * 	x <- x1;
 * 	y <- y1;
 *	toss x1, y1;
 * where the operator x <- x1 means copy the representation of x1 into x.
 *
 *	rec type T: struct of { t:T; };
 *
 * is handled similarly.
 */

Node *
op1(Node *n)
{
	Node *m;
	if(n->t==NDeclsc){
		m=op1(n->l);
		return newi(NDeclsc, m, (Node *)0, n->o.i);
	}
	if(n->r==0){
		if(n->o.n && (n->o.n->t==NProg || (n->o.n->t==NMk && n->o.n->l)))
			n->r=dupnode(n->o.n->l);
		else			
			lerror(n, "can't deduce type for rec decl");
	}else if(n->r->o.t==TType){
		m=newi(NType, (Node *)0, (Node *)0, n->r->l->o.t);
		m=new(NDecl, dupnode(n->l), m, (Node *)0);
		return m;
	}
	m=new(NMk, dupnode(n->r), (Node *)0, (Node *)0);
	m=new(NDecl, dupnode(n->l), dupnode(n->r), m);
	return m;
}

Node *
op2(Node *n)
{
	Node *m;
	char s[Namesize+2];
	if(n->t==NDeclsc){
		m=op2(n->l);
		return newi(NDeclsc, m, (Node *)0, n->o.i);
	}
	if(n->l->t==NList)
		error("no identifier lists in rec's, please");
	strcpy(s+1, n->l->o.s->name);
	s[0]='*';
	m=new(NDecl, idnode(lookup(s, ID)), dupnode(n->r), dupnode(n->o.n));
	return m;
}

Node *
op3(Node *n)
{
	Node *m;
	char s[Namesize+2];
	if(n->t==NDeclsc)
		return op3(n->l);
	if(n->l->t==NList)
		error("no lists in rec's, please");
	strcpy(s+1, n->l->o.s->name);
	s[0]='*';
	m=new(NSmash, idnode(lookup(s+1, ID)), idnode(lookup(s, ID)), (Node *)0);
	return m;
}

Node *
rewr(Node *n, Node *(*f)(Node *))
{
	if(n->t==NList)
		return new(NList, rewr(n->l, f), rewr(n->r, f), (Node *)0);
	return (*f)(n);
}

void
recrewrite(Node *n)
{
	Node *n1, *n2, *n3;
	n1=rewr(n->l, op1);
	n2=rewr(n->l, op2);
	n3=rewr(n->l, op3);
	freenode(n->l);
	n->t=NList;
	n->r=n3;
	n->l=new(NList, n1, n2, (Node *)0);
	ndump(n);
}

/*
 *
 * To compile this
 *
 *	prog(a:int){
 *		begin prog(b:int){ f(a, b); }(b);
 *	}
 *
 * convert it to this
 *
 *	prog(a:int){
 *		begin prog(b:int, a:int){ f(a, b); }(b, a);
 *	}
 *
 */

Node 	*begf;
Node	*bega;
int	fscope;
int	progerr;

void
proglocals(Node *n)
{
	progerr=1;
	pushscope();
	fscope=nscope;
	begf=n->l->l;
	bega=0;
	dclformals(begf);
	progid(n->r);
	popscope();
}

void
begrewrite(Node *n)
{
	progerr=0;
	pushscope();
	fscope=nscope;
	begf=n->l->l->l;
	bega=n->r;
	dclformals(begf);
	progid(n->l->r);
	popscope();
	n->l->l->l=begf;
	n->r=bega;
}

int
alreadyformal(Node *n, Node *f)
{
	if(f==0)
		return 0;
	if(f->t==NList)
		return alreadyformal(n, f->l) || alreadyformal(n, f->r);
	return strcmp(n->o.s->name, f->l->o.s->name)==0;
}

void
addformal(Node *n)
{
	Node *nf;
	if(!alreadyformal(n, begf)){
		nf=new(NFormal, dupnode(n), dupnode(n->o.s->val->type), (Node *)0);
		if(begf)
			begf=new(NList, begf, nf, (Node *)0);
		else
			begf=nf;
		nf=dupnode(n);
		if(bega)
			bega=new(NExprlist, bega, nf, (Node *)0);
		else
			bega=nf;
	}		
}

void
progid(Node *n)
{
	if(n==0)
		return;
	switch(n->t){
	case NArrayref:
	case NArraycom:
	case NBecome:
	case NBegin:
	case NBreak:
	case NCall:
	case NCase:
	case NContinue:
		break;
	case NDecl:
		progid(n->r);
		progid(n->o.n);
		declare(n, 0, 0, 0);
		return;
	case NDeclsc:
	case NDefault:
		break;
	case NElem:
		return;
	case NExpr:
	case NExprlist:
	case NFormal:
		break;
	case NID:
		if(n->o.s->val)
		if(0<n->o.s->val->scope && n->o.s->val->scope<fscope){
			if(progerr)
				lerror(n, "%m not in an accessible scope", n);
			addformal(n);
		}
		return;
	case NLabel:
	case NList:
	case NLoop:
		break;
	case NLoopexpr:
		progid(n->o.n);
		break;
	case NIf:
		progid(n->o.n);
		break;
	case NMk:
		break;
	case NNum:
		return;
	case NProg:
		pushscope();
		dclformals(n->l->l);
		progid(n->r);
		popscope();
		return;
	case NResult:
		break;
	case NScope:
		pushscope();
		progid(n->l);
		popscope();
		return;
	case NSelect:
		break;
	case NSmash:
		return;	/* ?? */
	case NString:
		return;
	case NSwitch:
		break;
	case NStructref:
		progid(n->l);
		return;
	case NType:
		break;
	case NUnit:
		return;
	case NVal:
		break;
	default:
		fprint(2, "can't progid node %n\n", n);
		return;
	}
	progid(n->l);
	progid(n->r);
}

