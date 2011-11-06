#include "store.h"
#include "node.h"
#include "symbol.h"
#include "ydefs.h"
#include <u.h>
#include <lib9.h>

#include "fns.h"

Node	*freenodes;
enum{
	Nchunk=100
};

Node *
new(Ntype t, Node *l, Node *r, Node *o)
{
	Node *n;
	extern int line;
	if(freenodes==0){
		int i;
		n=emalloc(Nchunk*sizeof(Node));
		for(i=0; i<Nchunk; i++,n++){
			n->l=freenodes;
			freenodes=n;
		}
	}
	n=freenodes;
	freenodes=n->l;
	n->t=t;
	n->line=line;
	n->l=l;
	n->r=r;
	n->o.n=o;
	return n;
}

Node *
newl(Ntype t, Node *l, Node *r, long ll)
{
	union{
		long l;
		Node *n;
	}u;
	u.l=ll;
	return new(t, l, r, u.n);
}

Node *
newi(Ntype t, Node *l, Node *r, int i)
{
	union{
		int i;
		Node *n;
	}u;
	u.i=i;
	return new(t, l, r, u.n);
}

Node *
newst(Ntype t, Node *l, Node *r, Store *st)
{
	union{
		Store *st;
		Node *n;
	}u;
	u.st=st;
	return new(t, l, r, u.n);
}

Node *
idnode(Symbol *s)
{
	return new(NID, (Node *)0, (Node *)0, (Node *)s);
}

void
freenode(Node *n)
{
	Node *m;
    loop:
	if(n==0)
		return;
	switch(n->t){
	case NDecl:
	case NIf:
	case NLoopexpr:
	case NSelect:
	case NSwitch:
	case NVal:
		freenode(n->o.n);
		break;
	case NString:
		decref(&n->o.st);
		break;
	}
	freenode(n->r);
	/* tail recursion */
	m=n;
	n=n->l;
	m->l=freenodes;
	freenodes=m;
	goto loop;
}

int
length(Node *n)
{
	if(n==0)
		return 0;
	if(n->t==NList || n->t==NExprlist)
		return length(n->l)+length(n->r);
	if(n->t==NElem)
		return length(n->l);	/* list of id's */
	return 1;
}

Node *
concat(Node *m, Node *n)
{
	if(n->t!=NList)
		return new(NList, m, n, (Node *)0);
	return new(NList, concat(m, n->l), n->r, (Node *)0);
}

void
ndump(Node *n)
{
	extern int tflag;
	if(!tflag)
		return;
	fprint(2, "%z\n");
	dump(n, 1);
}

void
dump(Node *n, int o)
{
	if(n==0){
		print("%b()\n", o);
		return;
	}
	print("%b%n [%d]\n", o, n, n->line);
	o+=2;
	switch(n->t){
	case NNosuchnode:
		fprint(1, "%p\n", n);
		return;
	case NArraycom:
		break;
	case NArrayref:
		break;
	case NBecome:
		break;
	case NBegin:
		break;
	case NBreak:
		break;
	case NCall:
		break;
	case NCase:
		break;
	case NComplete:
		break;
	case NContinue:
		break;
	case NDecl:
		if(n->o.n)
			dump(n->o.n, o);
		break;
	case NDeclsc:
		if(n->o.i&SCconst)
			fprint(1, "%bconst \n", o);
		if(n->o.i&SCbltin)
			fprint(1, "%bbuiltin\n", o);
		break;
	case NDefault:
		break;
	case NElem:
		break;
	case NExpr:
		fprint(1, "%b%e\n", o, n);
		break;
	case NExprlist:
		break;
	case NFormal:
		break;
	case NID:
		fprint(1, "%b%s\n", o, n->o.s->name);
		if(n->o.s->val)
			fprint(1, "%b%t\n", o+2, n->o.s->val->type);
		return;
	case NIf:
		dump(n->o.n, o);
		break;
	case NLabel:
		break;
	case NList:
		break;
	case NLoopexpr:
		dump(n->o.n, o);
		break;
	case NLoop:
		fprint(1, "%bat %s\n", o, n->o.i? "bottom": "top");
		break;
	case NMk:
		break;
	case NNum:
		fprint(1, "%b%ld\n", o, n->o.l);
		return;
	case NProg:
		break;
	case NRec:
		break;
	case NResult:
		break;
	case NSave:
		break;
	case NScope:
		break;
	case NSelect:
		break;
	case NSmash:
		break;
	case NString:
		fprint(1, "%b\"%s\"\n", o, (char*)n->o.st->data);
		return;
	case NStructref:
		fprint(1, "%b%ld\n", o, n->o.l);
		break;
	case NSwitch:
		break;
	case NType:
		fprint(1, "%b%t\n", o, n);
		break;	
	case NUnit:
		return;
	case NVal:
		dump(n->o.n, o);
		break;
	default:
		print("dump: bad node %n\n", n);
		abort();
	}
	dump(n->l, o);
	dump(n->r, o);
}

Node *
dupnode(Node *n)
{
	Node *m;
	if(n==0)
		return 0;
	m=new(n->t, (Node *)0, (Node *)0, (Node *)0);
	m->line=n->line;
	switch(m->t){
	case NArraycom:
		break;
	case NArrayref:
		break;
	case NBecome:
		break;
	case NBegin:
		break;
	case NBreak:
		break;
	case NCall:
		break;
	case NCase:
		break;
	case NComplete:
		break;
	case NContinue:
		break;
	case NDecl:
		m->o.n=dupnode(n->o.n);
		break;
	case NDeclsc:
		m->o.i=n->o.i;
		break;
	case NDefault:
		break;
	case NElem:
		break;
	case NExpr:
		m->o.i=n->o.i;
		break;
	case NExprlist:
		break;
	case NFormal:
		break;
	case NID:
		m->o.s=n->o.s;
		return m;
	case NIf:
		m->o.n=dupnode(n->o.n);
		break;
	case NLabel:
		break;
	case NList:
		break;
	case NLoop:
		m->o.i=n->o.i;
		break;
	case NLoopexpr:
		m->o.n=dupnode(n->o.n);
	case NMk:
		break;
	case NNum:
		m->o.l=n->o.l;
		return m;
	case NProg:
		break;
	case NResult:
		break;
	case NSave:
		break;
	case NScope:
		break;
	case NSelect:
		m->o.n=dupnode(n->o.n);
		break;
	case NString:
		m->o.st=n->o.st;
		m->o.st->ref++;
		break;
	case NStructref:
		m->o.l=n->o.l;
		break;
	case NSwitch:
		m->o.n=dupnode(n->o.n);
		break;
	case NType:
		m->o.t=n->o.t;
		break;	
	case NUnit:
		break;
	case NVal:
		m->o.n=dupnode(n->o.n);
		break;
	default:
		print("dupnode: bad node %n\n", n);
		abort();
	}
	m->l=dupnode(n->l);
	m->r=dupnode(n->r);
	return m;
}
