#include "nodenames.h"
#include "typenames.h"
#include "store.h"
#include "node.h"
#include "symbol.h"
#include "ydefs.h"
#include <u.h>
#include <lib9.h>
#include "errjmp.h"
#include <stdarg.h>

#include "fns.h"

extern	int	eflag;

void
lerror(Node *n, char *s, ...)
{
	va_list va;
	char buf[4096];

	va_start(va, s);
	doprint(buf, buf+sizeof buf, s, va);
	va_end(va);
	fprint(2, "%Z%s\n", n->line, buf);
	executeinhibit();
	if(eflag)
		abort();
	errflush();
	errjmp();
}

void
error(char *s, ...)
{
	va_list va;
	char buf[4096];

	va_start(va, s);
	doprint(buf, buf+sizeof buf, s, va);
	va_end(va);
	fprint(2, "%z%s\n", buf);
	executeinhibit();
	if(eflag)
		abort();
	errflush();
	errjmp();
}

void
rerror(char *s, ...)
{
	va_list va;
	char buf[4096];

	va_start(va, s);
	doprint(buf, buf+sizeof buf, s, va);
	va_end(va);
	fprint(2, "%z%s\n", buf);
	executeinhibit();
	processes(0);
	if(eflag)
		abort();
	errflush();
	errjmp();
}

void
warn(char *s, ...)
{
	va_list va;
	char buf[4096];

	va_start(va, s);
	doprint(buf, buf+sizeof buf, s, va);
	va_end(va);
	fprint(2, "warning:%z%s\n", buf);
	executeinhibit();
	errflush();
	errjmp();
}

void
panic(char *s, ...)
{
	va_list va;
	char buf[4096];

	va_start(va, s);
	doprint(buf, buf+sizeof buf, s, va);
	va_end(va);
	fprint(2, "internal error:%z%s\n", buf);
	abort();
}

void
rpanic(char *s, ...)
{
	va_list va;
	char buf[4096];

	processes(0);
	va_start(va, s);
	doprint(buf, buf+sizeof buf, s, va);
	va_end(va);
	fprint(2, "internal error:%z%s\n", buf);
	abort();
}

int
bconv(va_list *va, Fconv *f)
{
	int o;
	extern int printcol;
	o = va_arg(*va, int);
	while(printcol<o-8)
		strconv("\t", f);
	strconv("        "+(8-(o-printcol)), f);
	return sizeof(int);
}

int
nconv(va_list *va, Fconv *f)
{
	Node *n;
	int t;
	char buf[32];
	n = va_arg(*va, Node*);
	t = n->t;
	if(t<0 || sizeof(Ntypename)/sizeof(Ntypename[0])<=t){
		sprint(buf, "mystery node(%d)", t);
		strconv(buf, f);
	}else
		strconv(Ntypename[t], f);
	return sizeof (Node *);
}

int
tconv(va_list *va, Fconv *f)
{
	int t;
	char buf[1024];
	Node *n;
	n = va_arg(*va, Node*);
	t = n->o.t;
	if(t<0 || sizeof(Ttypename)/sizeof(Ttypename[0])<=t){
		sprint(buf, "mystery type(%d)", t);
	}else if(t==TArray)
		sprint(buf, "TArray of %t", n->r);
	else if(t==TChan)
		sprint(buf, "TChan of %t", n->r);
	else
		strcpy(buf, Ttypename[t]);
	strconv(buf, f);
	return sizeof (Node *);
}

int
econv(va_list *va, Fconv *f)
{
	char buf[16], *x;
	int t;
	Node *n;
	n = va_arg(*va, Node*);
	t = n->o.i;
	if(t<128 && strchr("+-*/%|&^~?!><=", t)){
		sprint(buf, "%c", t);
		strconv(buf, f);
		return sizeof(int);
	}
	switch(t){
	case GE:
		x=">=";
		break;
	case LE:
		x="<=";
		break;
	case NE:
		x="!=";
		break;
	case EQ:
		x="==";
		break;
	case ANDAND:
		x="&&";
		break;
	case OROR:
		x="||";
		break;
	case CAT:
		x="cat";
		break;
	case DEL:
		x="del";
		break;
	case REF:
		x="ref";
		break;
	case LEN:
		x="len";
		break;
	case UMINUS:
		x="unary -";
		break;
	case RCV:
		x="rcv";
		break;
	case SND:
		x="send";
		break;
	case LSH:
		x="<<";
		break;
	case RSH:
		x=">>";
		break;
	case DEC:
		x="--";
		break;
	case INC:
		x="++";
		break;
	case PRINT:
		x="print";
		break;
	default:
		x="mystery expression";
		break;
	}
	strconv(x, f);
	return sizeof(int);
}

int
mconv(va_list *va, Fconv *f)
{
	char buf[4096];
	Node *n;
	n = va_arg(*va, Node*);
	switch(n->t){
	case NID:
		strcpy(buf, n->o.s->name);
		break;
	case NArrayref:
		while(n->t==NArrayref)
			n=n->l;
		sprint(buf, "%m[]", n);
		break;
	case NCall:
		sprint(buf, "%m()", n->l);
		break;
	case NExpr:
		if(n->o.i=='=')
			sprint(buf, "(assignment to %m)", n->l);
		else
			sprint(buf, "(%e expr)", n);
		break;
	case NMk:
		sprint(buf, "mk()");
		break;
	case NNum:
		sprint(buf, "%ld", n->o.l);
		break;
	case NProg:
		sprint(buf, "prog(){}");
		break;
	case NString:
		sprint(buf, "\"%s\"", (char *)n->o.st->data);
		break;
	case NStructref:
		sprint(buf, "%m.%s", n->l, n->r->t==NID? n->r->o.s->name : "tag");
		break;
	case NUnit:
		sprint(buf, "unit");
		break;
	default:
		strcpy(buf, "(expression)");
		break;
	}
	strconv(buf, f);
	return sizeof(Node *);
}
