#include "store.h"
#include "node.h"
#include "symbol.h"
#include "ydefs.h"
#include <u.h>
#include <lib9.h>
#include "errjmp.h"

#include "fns.h"

Node	inttype;
Node	unittype;
Node	anytype;
Node	chartype;
Node	arychartype;
Node	aggrtype;
Node	*progtype;
Node	*resulttype;
Node	*switchtype;
Node	*etypeof(Node *);
Node	*type_of(Node *);
Node	*promote(Node *, Node *);
void	typecheck(Node *);
int	eqtypeid(Node *, Node *);
void	array(Node *);
void	chan(Node *, char *);
int	identifier(Node *);
void	lvalue(Node *, Node *);
int	argcheck(Node *, Node *, Node *, int, Node *);
void	fixstructref(Node *);
void	mkcheck(Node *, Node *);
void	structtypes(Node *, Node *);
void	arytypes(Node *, Node *);
int	oktype(Node *, Node *);
int	switchctr=0;

void
typeinit(void){
	inttype.t=NType;
	inttype.r=0;
	inttype.l=0;
	inttype.o.t=TInt;
	chartype.t=NType;
	chartype.r=0;
	chartype.l=0;
	chartype.o.t=TChar;
	arychartype.t=NType;
	arychartype.r=&chartype;
	arychartype.l=0;
	arychartype.o.t=TArray;
	unittype=inttype;
	unittype.o.t=TUnit;
	anytype=inttype;
	anytype.o.t=TAny;
	aggrtype=inttype;
	aggrtype.o.t=TAggr;
}

void
type(Node *n, int dofree)
{
	Errjmp x;
	errsave(x);
	if(errmark()){
		progtype=0;
		resulttype=0;
		switchtype=0;
		topscope();
		if(dofree)
			freenode(n);
		errrest(x);
		errjmp();
	}
	typecheck(n);
	errrest(x);
}

void
typecheck(Node *n)
{
	Node *tr, *tl;
	if(n==0)
		return;
	if(n->t==NScope){
		pushscope();
		typecheck(n->l);
		popscope();
		return;
	}
	/*
	 * Must make sure our leaves are typechecked before asking questions about
	 * their composition.  Therefore we typecheck(n->l) and n->r before the
	 * rest.  But this causes problems with declarations, which must happen
	 * before their consequences.  Thus declarations are handled first.
	 * Structure references and function definitions are similar because
	 * they contain variable-like elements that need definition (the structure
	 * tags and formal parameters).  Vals, Switches and Progs also need to
	 * have their top types recorded before checking their bodies, so
	 * they get handled here too.
	 */
	switch(n->t){
	case NBegin:
		if(!istopscope() && n->l->l->t==NProg)
			begrewrite(n->l);	/* to bind any necessary formals */
		break;	/* and check it normally */
	case NDecl:
	case NDeclsc:
		declare(n, 0, 1, 0);
		return;
	case NElem:		/* ignore the identifier part */
	case NFormal:
		typecheck(n->r);
		return;
	case NProg:{
		Node *oprogtype;
		typecheck(n->l);
		pushscope();
		dclformals(n->l->l);
		oprogtype=progtype;
		progtype=n->l->r;
		typecheck(n->r);
		progtype=oprogtype;
		popscope();
		if(!istopscope())
			proglocals(n);
		return;
	    }
	case NRec:
		recrewrite(n);
		typecheck(n);
		return;
	case NStructref:
		typecheck(n->l);	/* should also check for dups */
		tl=etypeoft(n->l);
		if(tl->o.t!=TStruct)
			lerror(n, "%m (in %m) not a structure", n->l, n);
		/*
		 * Rewrite the tree to put type in n->r and offset in n->o.l
		 */
		fixstructref(n);
		return;
	case NSelect:{
		Node *oswitchtype;
		oswitchtype=switchtype;
		switchtype=&anytype;
		typecheck(n->l);
		switchtype=oswitchtype;
		return;
	    }
	case NSmash:	/* very special internally generated node (see declare.c) */
		return;
	case NSwitch:{
		Node *oswitchtype;
		typecheck(n->r);
		oswitchtype=switchtype;
		switchtype=etypeof(n->r);
		typecheck(n->l);
		switchtype=oswitchtype;
		return;
	    }
	case NVal:{
		Node *oresulttype;
		oresulttype=resulttype;
		resulttype=0;
		typecheck(n->l);
		if(resulttype==0)
			lerror(n, "val has no result");
		n->o.n=resulttype;
		resulttype=oresulttype;
		return;
	    }
	}
	typecheck(n->l);
	typecheck(n->r);
	switch(n->t){
	case NArraycom:
		array(n);
		tl=etypeoft(n->l);
		tl=tl->r;
		if(tl->o.t==TID)
			tl=typeoftid(tl);
		if(tl->o.t!=TChan)
			lerror(n, "array %m in communication not of type chan (is array of %t)",
				n->l, tl);
		break;
	case NArrayref:
		array(n);
		break;
	case NBecome:
		if(progtype==0)
			lerror(n, "become outside prog");
		if(!compatmktype(progtype, n->l))
			lerror(n, "type mismatch (%t, %t) on become %m",
				progtype, etypeof(n->l), n->l);
		if(n->r==0)
			n->r=dupnode(progtype);
		break;
	case NBegin:
		break;
	case NBreak:
		break;
	case NCall:
		tl=etypeoft(n->l);
		if(tl->o.t!=TProg)
			lerror(n, "call of non-prog %m", n->l);
		argcheck(n->l, n->r, tl->l, 1, n->l);
		break;
	case NCase:
		break;
	case NComplete:
		n=n->l;
		if(n->t==NMk && !compatmktype(n->l, n))
			lerror(n, "incompatible types (%t,%t) in mk",
				n->l, etypeof(n->r));
		break;
	case NContinue:
		break;
	case NDefault:
		break;
	case NExpr:
		switch(n->o.i){
		case GE:
		case LE:
		case NE:
		case EQ:
		case ANDAND:
		case OROR:
		case '<':
		case '>':
			tl=etypeoft(n->r);
			tr=etypeoft(n->l);
			if(eqtype(tl, &arychartype)){
		Strcmp:
				if(tr->o.t!=TArray || tr->r->o.t!=TChar)
					lerror(n, "comparison of string (%m) with %t (%m)",
						n->r, tr, n->l);
				break;
			}
			if(eqtype(tr, &arychartype)){
				tr=tl;
				goto Strcmp;
			}
			if(!isinttype(tl) || !isinttype(tr))
				lerror(n, "logical operands (%m%e%m) must be int or array of char (are %t,%t)",
					n->l, n, n->r, tl, tr);
			break;
		case '=':
			lvalue(n->l, n);
			tl=type_of(n->l);
			if(n->r->t==NMk && n->r->l==0)	/* x=mk() */
				n->r->l=dupnode(tl);
			if(!compatmktype(tl, n->r))
				lerror(n, "type mismatch (%t, %t) on assignment to %m",
					tl, etypeof(n->r), n->l);
			break;
		case PRINT:
			break;
		case LEN:
			tl=etypeoft(n->l);
			if(tl->o.t!=TArray)
				lerror(n, "len: %m is type %t", n->l, tl);
			break;
		case REF:
			break;
		case DEF:
			if(!identifier(n->l))
				lerror(n, "def applied to %m", n->l);
			break;
		case CAT:
			tl=etypeoft(n->l);
			if(tl->o.t!=TArray)
				lerror(n, "cat applied to %m (%t)", n->l, tl);
			tr=etypeoft(n->r);
			if(tr->o.t!=TArray)
				lerror(n, "cat applied to %m (type %t)", n->r, tr);
			if(!eqtype(tl, tr))
				lerror(n, "cat: incompatible array types (%t,%t)",
					tl->r, tr->r);
			break;
		case DEL:
			tl=etypeoft(n->l);
			if(tl->o.t!=TArray)
				lerror(n, "del applied to %m (%t)", n->l, tl);
			tr=etypeoft(n->r);
			if(!isinttype(tr))
				lerror(n, "del: %m not integral", n->r);
			break;
		case INC:
		case DEC:
			tl=etypeof(n->l);
			if(!eqtype(tl, &inttype))
				lerror(n, "%e applied to %m type %t\n", n, n->l, tl);
			if(!identifier(n->l))
				lerror(n, "%e applied to %m", n, n->l);
			lvalue(n->l, n);
			/* fall through */
		case UMINUS:
		case '!':
		case '~':
			tl=etypeof(n->l);
			if(!isinttype(tl))
				lerror(n, "%e: %m is type %t", n, n->l, tl);
			break;
		case RCV:
			chan(n->l, "rcv");
			break;
		case SND:
			chan(n->l, "send");
			tl=etypeoft(n->l);
			tr=etypeof(n->r);
			if(!compattype(tl->r, tr))
				lerror(n, "type mismatch (%t, %t) on send to %m",
					tl->r, tr, n->l);
			break;
		default:
			tl=etypeof(n->l);
			tr=etypeof(n->r);
			if(!isinttype(tl) || !isinttype(tr))
				lerror(n, "operands of %e (%m, %m) must be of type int (are %t,%t)",
					n, n->l, n->r, tl, tr);
			break;
		}
		break;
	case NExprlist:
		break;
	case NFormal:
		break;
	case NID:
		if(n->o.s->val==0)
			lerror(n, "%m undeclared", n);
		return;
	case NIf:
		typecheck(n->o.n);
		tl=etypeoft(n->o.n);
		if(tl->o.t!=TInt)
			lerror(n, "if() conditional %m must be of type int (is %t)",
				n->o.n, tl);
		break;
	case NLabel:
		if(switchtype==0)
			lerror(n, "label outside switch or select");
		tl=etypeof(n->l);
		if(switchtype->o.t!=TAny && !compattype(tl, switchtype))
			lerror(n, "type mismatch in switch (%t,%t)",
				tl, switchtype);
		break;
	case NList:
		break;
	case NLoop:
		break;
	case NLoopexpr:
		if(n->r && !isinttype(tr=etypeof(n->r)))
			lerror(n, "loop conditional %m must be of type int (is %t)",
				n->r, tr);
		typecheck(n->o.n);
		break;
	case NMk:
		if(n->l && n->l->o.t!=TAggr)
			if(!compatmktype(n->l, n))
				lerror(n, "type mismatch (%t,%t) on mk\n",
					n->l, etypeof(n->r));
		break;
	case NNum:
		break;
	case NResult:
		tl=etypeof(n->l);
		if(resulttype==0)
			resulttype=dupnode(tl);
		else if(!compattype(tl, resulttype))
			lerror(n, "type mismatch (%t, %t) on result %m",
				resulttype, tl, n->l);
		break;
	case NSelect:
		break;
	case NString:
		return;
	case NType:
		if(n->o.t==TArray && n->l && etypeoft(n->l)->o.t!=TInt)
			lerror(n, "array subscript %m must be of type int (is %t)",
				n->l, etypeof(n->l));
		break;
	case NUnit:
		break;
	default:
		panic("typecheck: bad node %n", n);
	}
}

/*
 * n is fully type-checked, and should be an lvalue.
 */
Node *
type_of(Node *n)
{
	if(n==0)
		panic("type_of");
	switch(n->t){
	case NArrayref:
		return etypeof(n->l)->r;
	case NID:
		if(n->o.s->val==0)
			lerror(n, "%m undeclared", n);
		n=n->o.s->val->type;
		if(n->o.t==TID)
			return typeoftid(n);
		return n;
	case NStructref:
		if(n->r->t==NID)
			panic("type_of");
		return n->r;
	}
	panic("type_of: bad node %n", n);
	return 0;
}

/*
 * n is fully typechecked, and is an evaluatable object
 */
Node *
etypeof(Node *n)
{
	if(n==0)
		panic("etypeof empty");
	switch(n->t){
	case NArraycom:
	case NArrayref:
		return etypeoft(n->l)->r;
	case NCall:
		return etypeoft(n->l)->r;
	case NComplete:
		return etypeof(n->l);
	case NExpr:
		switch(n->o.i){
		default:
			return &inttype;
		case PRINT:
			return &arychartype;
		case RCV:
		case SND:
			return etypeoft(n->l)->r;
		case CAT:
		case DEL:
		case '=':
			return etypeof(n->l);
		}
	case NExprlist:
		return &aggrtype;
	case NID:
		if(n->o.s->val==0)
			lerror(n, "%m undeclared", n);
		n=n->o.s->val->type;
		if(n->o.t==TID)
			return typeoftid(n);
		return n;
	case NMk:
		if(n->l==0)
			panic("etypeof mk no type");
		return n->l;
	case NNum:
		return &inttype;
	case NProg:
		return n->l;
	case NString:
		return &arychartype;
	case NStructref:
		if(n->r->t==NID)
			fixstructref(n);
		return n->r;
	case NUnit:
		return &unittype;
	case NVal:
		if(n->o.n)
			return n->o.n;
		break;
	}
	panic("etypeof: bad node %n", n);
	return 0;
}

/*
 * n is a type id
 */
Node *
typeoftid(Node *n)
{
	int i=0;
	Value *v;
    loop:
	v=n->l->o.s->val;
	if(v==0 || v->type==0 || v->type->l==0)
		error("undefined type %s", n->l->o.s->name);
	n=v->type->l;
	if(n->o.t==TID){
		if(++i>5)
			lerror(n, "typeoftid: recursion");
		goto loop;
	}
	return n;
}

/*
 * etypeof, but if type is TID, give more information
 */
Node *
etypeoft(Node *n)
{
	n=etypeof(n);
	if(n->o.t==TID)
		n=typeoftid(n);
	return n;
}
int
argcheck(Node *prog, Node *args, Node *formals, int argno, Node *prognode)
{
	if(args==0 && formals==0)
		return 0;
	if(args==0 || formals==0
	|| (args->t==NExprlist && formals->t!=NList)
	|| (args->t!=NExprlist && formals->t==NList))
		lerror(prognode, "wrong number of arguments to %m", prog);
	if(args->t==NExprlist && formals->t==NList){
		argno=argcheck(prog, args->l, formals->l, argno, prognode);
		return argcheck(prog, args->r, formals->r, argno, prognode);
	}
	if(!compatmktype(formals->r, args))
		lerror(prognode, "type mismatch (%t, %t) on arg. %d of %m",
			etypeof(args), formals->r, argno, prog);
	return argno+1;
}

int
eqtype(Node *n1, Node *n2)
{
	if(n1==0 && n2==0)
		return 1;
	if(n1==0 || n2==0)
		return 0;
	if(n1->t!=n2->t)
		return 0;
	if(n1->t==NElem || n1->t==NFormal)
		return eqtype(n1->r, n2->r);
	if(n1->t==NList)
		return eqtype(n1->l, n2->l) && eqtype(n1->r, n2->r);
	if(n1->t!=NType)
		panic("eqtype: not a type\n");
	if(n1->o.t==TAny || n2->o.t==TAny)
		return 1;
	if(n1->o.t==TID)
		return eqtypeid(n1, n2);
	if(n2->o.t==TID)
		return eqtypeid(n2, n1);
	if(n1->o.t != n2->o.t)
		return 0;
	if(n1->o.t==TStruct)
		return eqtype(n1->l, n2->l);
	/* size of array is not part of its type */
	if(n1->o.t!=TArray && !eqtype(n1->l, n2->l))
		return 0;
	return eqtype(n1->r, n2->r);
}

int
eqtypeid(Node *n1, Node *n2)
{
	/* n1 is known to be a TID */
	if(n2->o.t==TID){
		if(n1->l->o.s==n2->l->o.s)
			return 1;
		n2=typeoftid(n2);
	}
	n1=typeoftid(n1);
/*	if(n1==n2)			this is legal, but let's do it the hard way anyway
		return 1;*/
	/* have to do it the hard way */
	if(n1->o.t!=n2->o.t)
		return 0;
	return eqtype(n1, n2);
}

int
compattype(Node *n1, Node *n2)
{
	if(eqtype(n1, n2))
		return 1;
	if(isinttype(n1) && isinttype(n2))
		return 1;
	return 0;
}

int
esizeof(Node *n)
{
	if(n==0)
		panic("esizeof empty");
	if(n->t==NList)
		return esizeof(n->l)+esizeof(n->r);
	return WS;
}

void
structcheck(Node *t, Node *n)
{
	int nt, nn;
	if(n==0)
		return;
	nt=length(t);
	nn=length(n);
	if(nt<nn)
		lerror(n, "too many initializers for struct");
	while(nt>nn){
		t=t->l;
		--nt;
	}
	if(t->t==NList){
		structcheck(t->l, n->l);
		structcheck(t->r, n->r);
		return;
	}
	if(!compatmktype(t->r, n))
		lerror(n, "type mismatch (%t,%t) initializing struct element",
			t->r, etypeof(n));
}

void
arraycheck(Node *t, Node *n)
{
	if(n==0)
		return;
	if(n->t==NExprlist){
		arraycheck(t, n->l);
		arraycheck(t, n->r);
		return;
	}
	if(!compatmktype(t, n))
		lerror(n, "type mismatch (%t,%t) initializing array element",
			t, etypeof(n));
}

void
mkcheck(Node *mk, Node *want)
{
	Node *typ;
	typ=mk->l;
	if(want->o.t==TID)
		want=typeoftid(want);
	if(want->o.t==TStruct){
		if(compattype(typ, want)){
			if(mk->r->t==NMk && !compatmktype(want, mk->r))
				lerror(mk, "type mismatch (%t) initializing struct",
					etypeof(mk->r));
			return;
		}
		structcheck(want->l, mk->r);
		return;
	}
	if(want->o.t==TArray){
		if(compattype(typ, want)){
    Checkarray:
			if(mk->r->t==NMk && !compatmktype(want, mk->r))
				lerror(mk, "type mismatch (%t) initializing array of %t",
					etypeof(mk->r), want->r);
			return;
		}
		if(compattype(typ, want->r)){
			freenode(mk->l);
			mk->l=dupnode(want);
			goto Checkarray;
		}
		arraycheck(want->r, mk->r);
		return;
	}
	if(!compattype(typ, want))
		lerror(mk, "type mismatch (%t,%t) in mk", typ, want);
}

int
compatmktype(Node *typ, Node *val)
{
	if(val->t==NMk && val->r){
		mkcheck(val, typ);
		if(!eqtype(typ, val->l)){
			freenode(val->l);
			val->l=dupnode(typ);
		}
		return 1;
	}
	return compattype(etypeof(val), typ);
}

int
isptrtype(Node *n)
{
	if(n==0)
		panic("isptrtype empty");
	if(n->t!=NType)
		panic("isptrtype");
	if(n->o.t==TID)
		n=typeoftid(n);
	return n->o.t==TArray || n->o.t==TProg || n->o.t==TChan || n->o.t==TStruct;
}

int
isinttype(Node *n)
{
	if(n->t!=NType)
		panic("isinttype");
	if(n->o.t==TID)
		n=typeoftid(n);
	return n->o.t==TInt || n->o.t==TChar;
}


void
array(Node *n)
{
	if(etypeoft(n->l)->o.t!=TArray)
		lerror(n, "left operand of '[' (%m) not array (is %t)",
			n->l, etypeof(n->l));
	if(n->r && !isinttype(etypeof(n->r)))
		lerror(n, "subscript of array %m must be of type int (is %t)",
			n->l, etypeof(n->r));
}

void
chan(Node *n, char *s)
{
	if(etypeoft(n)->o.t!=TChan)
		lerror(n, "operand of %s (%m) not a chan (is %t)",
			s, n, etypeof(n));
}

int
identifier(Node *n)
{
    top:
	if(n->t==NID)
		return 1;
	if(n->t==NArrayref || n->t==NStructref){
		n=n->l;
		goto top;
	}
	return 0;
}

void
lvalue(Node *n, Node *t)
{
    loop:
	if(n->t==NID){
		if(n->o.s->val->stclass&SCconst)
			lerror(n, "%e involves constant %s", t, n->o.s->name);
		if(n->o.s->val->type->o.t==TType)
			lerror(n, "%e involves type variable %s", t, n->o.s->name);
	}else if(n->t==NArrayref || n->t==NStructref){
		n=n->l;
		goto loop;
	}else
		error("%m is not an lvalue", n);
}


Node *
elemtype(Node *t, Symbol *id, long *off)
{
	if(t->t==NList){
		Node *n;
		n=elemtype(t->l, id, off);
		if(!n)
			n=elemtype(t->r, id, off);
		return n;
	}
	if(id==t->l->o.s)
		return t->r;
	(*off)++;
	return 0;
}

void
fixstructref(Node *n)
{
	long off;
	Node *t;
	if(n->r->t!=NID)
		return;
	t=etypeoft(n->l);
	off=(length(t->l)+BPW-1)/BPW;
	t=elemtype(t->l, n->r->o.s, &off);
	if(!t)
		lerror(n, "%s not element of struct %m", n->r->o.s->name, n->l);
	n->o.l=off;
	freenode(n->r);
	n->r=dupnode(t);
}

/*
 * id may be a list; distribute t and join the result to old
 */
Node *
elemrewr(Node *id, Node * t, Node *old, Ntype nt)
{
	if(old==0){
		if(id->t==NID)
			return new(nt, id, t, (Node *)0);
		id->l=elemrewr(id->l, dupnode(t), (Node *)0, nt);
		id->r=elemrewr(id->r, dupnode(t), (Node *)0, nt);
		return id;
	}
	id=elemrewr(id, t, (Node *)0, nt);
	return concat(old, id);
}
