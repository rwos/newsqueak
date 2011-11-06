#include "store.h"
#include "node.h"
#include "symbol.h"
#include "comm.h"
#include "inst.h"
#include <u.h>
#include <lib9.h>

#include "fns.h"

#define	FNS
#include "slib.h"
#undef	FNS

#define	C	0x40000000
#define	I	0x20000000
#define	F	0x10000000
#define	M(x)	((x)&~(C|I|F))

long call0[]={	/* plain function, 0 arguments */
	I+Ipushfp,	C+0,	F,	I+Iret,	C+0*WS,	I+Idone,	0
};
long call1[]={	/* plain function, 1 argument */
	I+Ipushfp,	C+0,	F,	I+Iret,	C+1*WS,	I+Idone,	0
};
long call2[]={	/* plain function, 2 arguments */
	I+Ipushfp,	C+0,	F,	I+Iret,	C+2*WS,	I+Idone,	0
};
long call3[]={	/* plain function, 3 arguments */
	I+Ipushfp,	C+0,	F,	I+Iret,	C+3*WS,	I+Idone,	0
};
long call4[]={	/* plain function, 4 arguments */
	I+Ipushfp,	C+0,	F,	I+Iret,	C+4*WS,	I+Idone,	0
};
long call5[]={	/* plain function, 5 arguments */
	I+Ipushfp,	C+0,	F,	I+Iret,	C+5*WS,	I+Idone,	0
};
long call2_0[]={/* two-step function, 0 arguments */
	I+Ipushfp,	C+0,	F+0,	F+1,	I+Iret,	C+0*WS,	I+Idone,	0
};
long call2_2[]={/* two-step function, 2 arguments */
	I+Ipushfp,	C+0,	F+0,	F+1,	I+Iret,	C+2*WS,	I+Idone,	0
};
long call2_3[]={/* two-step function, 3 arguments */
	I+Ipushfp,	C+0,	F+0,	F+1,	I+Iret,	C+3*WS,	I+Idone,	0
};

struct{
	char	*name;
	int	(*fn[3])(Proc *);
	int	nargs;
	long	*template;
}bltin[]={
#include "slib.h"
	{0,	{0,	0,	0},	0,	0},
};

int
bltinlookup(char *s)
{
	int i;

	for(i=0; bltin[i].name; i++)
		if(strcmp(s, bltin[i].name)==0)
			return i;
	error("%s not a builtin", s);
	return -1;
}

long
bltinval(char *name, Node *t)
{
	int i, nargs, len;
	long *template, *p;
	Store *s;
	SWord *d;

	if(t->o.t != TProg)
		error("builtin %s not a function", name);
	i = bltinlookup(name);
	nargs = bltin[i].nargs;
	if(nargs != length(t->l))	/* necessary but not sufficient */
		error("wrong #args to builtin %s: %d (should be %d)", name, length(t->l), nargs);
	template = bltin[i].template;
	p = template;
	for(len=0; *p; p++)
		len++;
	len++;	/* for locals pointer */
	s = emalloc(SHSZ+len*WS);
	s->ref = 1;
	s->type = Sprog;
	s->sbits = 0;
	s->len = len;
	d = s->data;
	*d++ = 0;	/* locals pointer */
	for(p=template; *p; p++)
		if(*p & C)
			*d++ = (SWord)M(*p);
		else if(*p & I)
			*d++ = (SWord)insttab[M(*p)].fp;
		else if(*p & F)
			*d++ = (SWord)bltin[i].fn[M(*p)];
	return (long)s;
}

Store *
mk(int type, int len)
{
	Store *s;

	if(type == Sstruct)
		len++;
	s = emalloc(SHSZ+len*WS);
	s->ref = 1;
	s->type = type;
	if(type == Sstruct){
		s->sbits = 1;
		s->data[0] = 0;
	}else
		s->sbits = 0;
	s->len = len;
	return s;
}

Store *
mkarraychar(char *c, int n)
{
	Store *s;

	s = mk(Sarychar, (n+WS)/WS);	/* include NUL */
	s->len = n;
	memcpy(s->data, c, n);
	((char *)s->data)[n] = 0;
	return s;
}
