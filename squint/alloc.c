#include <u.h>
#include <lib9.h>

#include "fns.h"

void *
emalloc(unsigned long n)
{
	void *p;
	p = malloc((unsigned)n);
	if(p == 0){
		warn("out of memory; exiting");
		exits("out of memory");
	}
	return p;
}
void *
erealloc(void *p, unsigned long n)
{
	p = realloc(p, (unsigned)n);
	if(p == 0){
		warn("out of memory; exiting");
		exits("out of memory");
	}
	return p;
}
