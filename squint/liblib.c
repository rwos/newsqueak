#include "store.h"
#include "comm.h"
#include "libargs.h"
#include <u.h>
#include <lib9.h>

#include "fns.h"

int
bi_abort(Proc *proc)
{
	print("squint abort: %s\n", (char *)SARG(proc, 1)->data);
	exits((char *)0);
	return 0;
}

int
bi_suspend(Proc *proc)
{
	print("squint suspend: %s\n", (char *)SARG(proc, 1)->data);
	stacktrace(proc);
	return 0;
}

int
bi_strchr(Proc *proc)
{
	Store *s;
	char *p;
	s=SARG(proc, 1);
	p=strchr((char *)s->data, LARG(proc, 2));
	if(p==0)
		RETVAL(proc, 2)=-1;
	else
		RETVAL(proc, 2)=p-(char *)s->data;
	DECREF(proc, 1);
	return 1;
}

int
bi_rand(Proc *proc)
{
	RETVAL(proc, 0)=nrand(32768);
	return 1;
}
