#include "store.h"
#include "node.h"
#include "symbol.h"
#include "ydefs.h"
#include <u.h>
#include <lib9.h>
#include "errjmp.h"

#include "fns.h"

Node		*doconst(Node *);
int		isconst(Node *);
extern int	Cflag;

Node *
constants(Node *n)
{
	if(n==0)
		return 0;
	if(Cflag || !executeallowed)
		return n;
	switch(n->t){
	case NArrayref:
		if(isconst(n))
			return doconst(n);
		break;
	case NArraycom:
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
		n->r=constants(n->r);
		n->o.n=constants(n->o.n);
		declare(n, 0, 0, 0);
		return n;
	case NDeclsc:
		break;
	case NDefault:
		return n;
	case NElem:
		n->r=constants(n->r);
		return n;
	case NExpr:
		switch(n->o.i){
		case GE:
		case LE:
		case NE:
		case EQ:
		case '>':
		case '<':
		case '+':
		case '-':
		case '*':
		case '/':
		case '%':
		case '&':
		case '|':
		case '^':
		case ANDAND:
		case OROR:
		case LSH:
		case RSH:
		case CAT:
		case DEL:
			if(isconst(n->l) && isconst(n->r))
				return doconst(n);
			break;
		case DEF:
		case REF:
		case LEN:
		case UMINUS:
		case '~':
		case '!':
			if(isconst(n->l))
				return doconst(n);
			break;
		case PRINT:
		case RCV:
		case SND:
		case INC:
		case DEC:
			break;
		case '=':
			break;
		default:
			fprint(2, "%Zcan't const expression %e\n", n->line, n);
			return n;
		}
		break;
	case NExprlist:
		break;
	case NFormal:
		n->r=constants(n->r);
		return n;
	case NID:
		if(isconst(n))
			return doconst(n);
		break;
	case NIf:
		n->l=constants(n->l);
		n->r=constants(n->r);
		n->o.n=constants(n->o.n);
		if(isconst(n->o.n)){
			Node *m;
			istart();
			gen(n->o.n, 1);
			execute();
			if(topofstack()){
				m=n->l;
				n->l=0;
			}else{
				m=n->r;
				n->r=0;
			}
			freenode(n);
			return m;
		}
		return n;
	case NLabel:
		break;
	case NList:
		break;
	case NLoop:
		break;
	case NLoopexpr:
		n->o.n=constants(n->o.n);
		break;
	case NMk:
		n->l=constants(n->l);
		n->r=constants(n->r);
		if(isconst(n->l) && isconst(n->r))
			n=doconst(n);
		return n;
		break;
	case NNum:
		return n;
	case NProg:
		pushscope();
		dclformals(n->l->l);
		n->r=constants(n->r);
		popscope();
		return n;
	case NResult:
		break;
	case NScope:
		pushscope();
		n->l=constants(n->l);
		popscope();
		return n;
	case NSelect:
		break;
	case NSmash:
		return n;
	case NString:
		return n;
	case NSwitch:
		break;
	case NStructref:
		if(isconst(n))
			return (n);
		break;
	case NType:
		break;
	case NUnit:
		break;
	case NVal:
		if(isconst(n->l))
			return doconst(n);
		break;
	default:
		fprint(2, "%Zcan't const node %n\n", n->line, n);
		return n;
	}
	n->l=constants(n->l);
	n->r=constants(n->r);
	return n;
}

int
isconst(Node *n)
{
	if(n==0)
		return 1;
	switch(n->t){
	case NArrayref:
		return isconst(n->l) && isconst(n->r);
	case NCall:
		return 0;
	case NComplete:
		return isconst(n->l);
	case NElem:
		return isconst(n->r);
	case NExpr:
		switch(n->o.i){
		case GE:
		case LE:
		case NE:
		case EQ:
		case '>':
		case '<':
		case '+':
		case '-':
		case '*':
		case '/':
		case '%':
		case '&':
		case '|':
		case '^':
		case ANDAND:
		case OROR:
		case LSH:
		case RSH:
		case CAT:
		case DEL:
			return isconst(n->l) && isconst(n->r);
		case DEF:
		case LEN:
		case UMINUS:
		case '~':
		case '!':
		case PRINT:
			return isconst(n->l);
		case REF:
		case '=':
		case RCV:
		case SND:
		case INC:
		case DEC:
			return 0;
		}
		fprint(2, "%Zcan't isconst expression %e", n->line, n);
		return 0;
	case NExprlist:
		return isconst(n->l) && isconst(n->r);
	case NFormal:
		return 0;
	case NID:
		return n->o.s->val->scope==0 && (n->o.s->val->stclass&SCconst);
	case NIf:
		return isconst(n->o.n) && isconst(n->l) && isconst(n->r);
	case NList:
		return 0;
	case NLoop:
		return 0;
	case NMk:
		return isconst(n->l) && isconst(n->r);
	case NNum:
		return 1;
	case NProg:
		return 0;
	case NResult:
		return isconst(n->l);
	case NScope:
		return isconst(n->l);
	case NString:
		return 1;
	case NStructref:
		return isconst(n->l);
	case NType:
		return isconst(n->l) && isconst(n->r);
	case NVal:
		return isconst(n->l);
	case NUnit:
		return 1;
	}
	fprint(2, "%Zcan't isconst node %n\n", n->line, n);
	return 0;
}

Node *
doconst(Node *n)
{
	Node *t;
	Errjmp x;
	if(n->t==NNum || n->t==NString || n->t==NUnit)
		return n;	/* already const */
	t=etypeoft(n);
	errsave(x);
	if(errmark()){
		errrest(x);
		lerror(n, "compile-time evaluation error");
	}
	switch(t->o.t){
	case TChar:
	case TInt:
		istart();
		gen(n, 1);
		freenode(n);
		execute();
		n=new(NNum, (Node *)0, (Node *)0, (Node *)topofstack());
		break;
	case TUnit:
		n=new(NUnit, (Node *)0, (Node *)0, (Node *)0);
		break;
	case TArray:
		if(t->r->o.t==TChar){
			istart();
			gen(n, 1);
			freenode(n);
			execute();
			n=newst(NString, (Node *)0, (Node *)0, (Store *)topofstack());
		}
		break;
	}
	errrest(x);
	return n;
}
