#include "store.h"
#include "comm.h"
#include <u.h>
#include <lib9.h>

#include "fns.h"

void	pprint(Proc *, char *, ...);

/*
 * Push constants
 */

int
ipushconst(Proc *proc)
{
	*proc->sp++ = (SWord)*++proc->pc;
	return 1;
}

int
ipush_2(Proc *proc)
{
	*proc->sp++ = -2;
	return 1;
}

int
ipush_1(Proc *proc)
{
	*proc->sp++ = -1;
	return 1;
}

int
ipush0(Proc *proc)
{
	*proc->sp++ = 0;
	return 1;
}

int
ipush1(Proc *proc)
{
	*proc->sp++ = 1;
	return 1;
}

int
ipush2(Proc *proc)
{
	*proc->sp++ = 2;
	return 1;
}

int
ipush3(Proc *proc)
{
	*proc->sp++ = 3;
	return 1;
}

int
ipush4(Proc *proc)
{
	*proc->sp++ = 4;
	return 1;
}

int
ipush5(Proc *proc)
{
	*proc->sp++ = 5;
	return 1;
}

int
ipush6(Proc *proc)
{
	*proc->sp++ = 6;
	return 1;
}

int
ipush7(Proc *proc)
{
	*proc->sp++ = 7;
	return 1;
}

int
ipush8(Proc *proc)
{
	*proc->sp++ = 8;
	return 1;
}

int
ipush9(Proc *proc)
{
	*proc->sp++ = 9;
	return 1;
}

int
ipush10(Proc *proc)
{
	*proc->sp++ = 10;
	return 1;
}

/*
 * Binary operators
 */
int
ige(Proc *proc)
{
	--proc->sp;
	proc->sp[-1] = proc->sp[-1]>=proc->sp[0];
	return 1;
}

int
ile(Proc *proc)
{
	--proc->sp;
	proc->sp[-1] = proc->sp[-1]<=proc->sp[0];
	return 1;
}

int
ine(Proc *proc)
{
	--proc->sp;
	proc->sp[-1] = proc->sp[-1]!=proc->sp[0];
	return 1;
}

int
ieq(Proc *proc)
{
	--proc->sp;
	proc->sp[-1] = proc->sp[-1]==proc->sp[0];
	return 1;
}

int
igt(Proc *proc)
{
	--proc->sp;
	proc->sp[-1] = proc->sp[-1]>proc->sp[0];
	return 1;
}

int
ilt(Proc *proc)
{
	--proc->sp;
	proc->sp[-1] = proc->sp[-1]<proc->sp[0];
	return 1;
}

int
iadd(Proc *proc)
{
	--proc->sp;
	proc->sp[-1] += proc->sp[0];
	return 1;
}

int
isub(Proc *proc)
{
	--proc->sp;
	proc->sp[-1] -= proc->sp[0];
	return 1;
}

int
imul(Proc *proc)
{
	long l0, l1, l;

	--proc->sp;
	l0 = proc->sp[-1];
	l1 = proc->sp[0];
	l = l0*l1;
	if(l1 && l/l1!=l0)
		rerror("product overflow");
	proc->sp[-1] = l;
	return 1;
}

int
idiv(Proc *proc)
{
	--proc->sp;
	if(proc->sp[0]==0)
		rerror("zero divide");
	proc->sp[-1]/=proc->sp[0];
	return 1;
}

int
imod(Proc *proc)
{
	--proc->sp;
	if(proc->sp[0] == 0)
		rerror("zero modulo");
	proc->sp[-1] %= proc->sp[0];
	return 1;
}

int
iand(Proc *proc)
{
	--proc->sp;
	proc->sp[-1] &= proc->sp[0];
	return 1;
}

int
ior(Proc *proc)
{
	--proc->sp;
	proc->sp[-1] |= proc->sp[0];
	return 1;
}

int
ixor(Proc *proc)
{
	--proc->sp;
	proc->sp[-1] ^= proc->sp[0];
	return 1;
}

int
ilsh(Proc *proc)
{
	--proc->sp;
	proc->sp[-1] <<= proc->sp[0];
	return 1;
}

int
irsh(Proc *proc)
{
	--proc->sp;
	proc->sp[-1] >>= proc->sp[0];
	return 1;
}

int
imax(Proc *proc)
{
	SWord l;

	l = *--proc->sp;
	if(l>proc->sp[-1])
		proc->sp[-1] = l;
	return 1;
}

/*
 * Unary operators
 */

int
ineg(Proc *proc)
{
	proc->sp[-1] = -proc->sp[-1];
	return 1;
}

int
inot(Proc *proc)
{
	proc->sp[-1] = ~proc->sp[-1];
	return 1;
}

int
ilnot(Proc *proc)
{
	proc->sp[-1] = !proc->sp[-1];
	return 1;
}

int
iref(Proc *proc)
{
	Store *s=(Store *)*--proc->sp;

	*proc->sp++ = s->ref-1;
	decref(&s);
	return 1;
}

int
ilen(Proc *proc)
{
	Store *s=(Store *)*--proc->sp;

	*proc->sp++ = s->len;
	decref(&s);
	return 1;
}

/*
 * String comparison: put value of strcmp() on stack
 */

int
istrcmp(Proc *proc)
{
	int cmp;
	Store *s1, *s2;

	s1 = (Store *)proc->sp[-2];
	s2 = (Store *)proc->sp[-1];
	cmp = strcmp((char *)s1->data, (char *)s2->data);
	decref(&s1);
	decref(&s2);
	proc->sp--;
	proc->sp[-1] = cmp;
	return 1;
}

/*
 * Print
 */

int
iprintint(Proc *proc)
{
	pprint(proc, "%ld", *--proc->sp);
	return 1;
}

int
iprintnewline(Proc *proc)
{
	pprint(proc, "\n");
	return 1;
}

int
iprintblank(Proc *proc)
{
	pprint(proc, " ");
	return 1;
}

int
iprintunit(Proc *proc)
{
	pprint(proc, "(unit)");
	return 1;
}

int
iprintchar(Proc *proc)
{
	pprint(proc, "%c", *--proc->sp);
	return 1;
}

/*
 * Stack management
 */

int
ipop(Proc *proc)
{
	--proc->sp;
	return 1;
}

int
ipopptr(Proc *proc)
{
	decref((Store **)(proc->sp-1));
	--proc->sp;
	return 1;
}

int
idup(Proc *proc)
{
	proc->sp++;
	proc->sp[-1] = proc->sp[-2];
	return 1;
}

int
idupptr(Proc *proc)
{
	proc->sp++;
	proc->sp[-1] = proc->sp[-2];
	((Store *)(proc->sp[-1]))->ref++;
	return 1;
}
