#include "store.h"
#include "node.h"
#include "symbol.h"
#include "ydefs.h"
#include <u.h>
#include <lib9.h>

#include "fns.h"

enum{
	Nscope=20,
	Nhash=255	/* 2^n - 1 */
};

int	nscope=0;
Symbol	*scope[Nscope];
Symbol	*symtab[Nhash+1];

extern long	autooffset;

/*
 * All names have at most Namesize chars (this is done when they are read)
 */
int
hash(char *s)
{
	int h=0;
	while(*s){
		h+=*s++;
		h<<=1;
		if(h&(Nhash+1))
			h|=1;
	}
	return h&Nhash;
}

Symbol *
install(char *s, int tok)
{
	Symbol *sp;
	int h=hash(s);
	sp=alloc(Symbol);
	sp->token=tok;
	sp->hnext=symtab[h];
	symtab[h]=sp;
	strcpy(sp->name, s);
	sp->val=0;
	return sp;
}

Symbol *
lookup(char *s, int tok)
{
	int h=hash(s);
	Symbol *sp;
	for(sp=symtab[h]; sp; sp=sp->hnext)
		if(strcmp(s, sp->name)==0)
			return sp;
	return install(s, tok);
}

void
pushval(Symbol *sp, Node *typ)
{
	Value *v;
	if(sp->val && sp->val->scope==nscope)
		lerror(typ, "redeclaration of %s", sp->name);
	v=alloc(Value);
	v->scope=nscope;
	v->next=sp->val;
	sp->val=v;
	v->type=typ;
	if(nscope>0){
		autooffset+=WS;
		v->store.off=autooffset;
	}else
		v->store.l=0;
	v->snext=scope[nscope];
	v->isauto=(nscope>0);
	scope[nscope]=sp;
}

void
pushscope(void){
	nscope++;
	if(nscope>=Nscope)
		error("scope overflow");
	scope[nscope]=0;
}

void
popscope(void)
{
	Symbol *s, *ns;
	Value *v;
	for(s=scope[nscope]; s; s=ns){
		ns=s->val->snext;
		v=s->val->next;
		freenode(s->val->type);
		free((char *)s->val);
		s->val=v;
	}
	--nscope;
	if(nscope<0)
		panic("scope underflow");
}

void
scopedecrefgen(void){
	Symbol *s;
	for(s=scope[nscope]; s; s=s->val->snext)
		if(nscope>0)
			genfreeauto(s);
}

void
topscope(void)	/* fix state left by errors */
{
	while(nscope)
		popscope();
}

int
istopscope(void){
	return nscope==0;
}

long
sbind(Node *formals, long offset)
{
	Symbol *s;
	if(formals==0)
		return 0;
	if(formals->t==NList){
		offset=sbind(formals->l, offset);
		return sbind(formals->r, offset);
	}
	s=formals->l->o.s;
	pushval(s, dupnode(formals->r));
	s->val->store.off=offset;
	s->val->stclass=0;
	return offset-WS;
}

void
dclformals(Node *fl)
{
	sbind(fl, -4L*(sizeof (long *)));
}

int
argtypwalk(Node *l, char *al)
{
	int n;
	if(l==0)
		return 0;
	if(l->t==NList){
		n=argtypwalk(l->l, al);
		return n+argtypwalk(l->r, al+n);
	}
	switch(l->r->o.t){
	case TInt:
		*al=0;
		break;
	case TChar:
		*al=1;
		break;
	case TUnit:
		*al=2;
		break;
	case TArray:
		if(l->r->r->o.t==TChar)
			*al=3;
		else
			*al=4;
		break;
	case TStruct:
		*al=4;
		break;
	default:
		*al=5;
	}
	return 1;
}

int
nargs(char *s, char al[])
{
	int h=hash(s);
	Symbol *sp;
	Value *v;
	for(sp=symtab[h]; sp; sp=sp->hnext)
		if(strcmp(s, sp->name)==0){
			for(v=sp->val; v; v=v->next)
				if(v->type->o.t==TProg)
					return argtypwalk(v->type->l, al);
			break;
		}
	/* no idea; just guess */
	al[0]=0;
	return 1;
}
