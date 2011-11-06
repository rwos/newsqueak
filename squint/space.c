#include "store.h"
#include "comm.h"
#include <u.h>
#include <lib9.h>
#include "space.h"

#include "fns.h"

Space	*space=0;
int	nspace=0;

void	ckname(char *);

int
ckspace(int n)
{
	return n>0 && n<nspace && space[n].busy;
}

Space *
tospace(int n)
{
	if(n<0 || nspace<=n || space[n].busy==0)
		rerror("undefined nspace %d", n);
	return &space[n];
}

Bind *
tobind(Space *sp, int n)
{
	Bind *bp;
	for(bp=sp->bind; bp; bp=bp->next)
		if(bp->seq==n)
			return bp;
	return 0;
}

int
spacenew(void)
{
	enum{
		Delta=10
	};
	int i;
	fmountinit();
	if(space==0){
		space=emalloc(Delta*sizeof(Space));
		memset(space, 0, Delta*sizeof(Space));
		nspace=Delta;
	}
	for(i=1; space[i].busy; i++)	/* space 0 is 'undefined' */
		if(i==nspace-1){
			space=erealloc(space, (nspace+Delta)*sizeof(Space));
			memset(space+nspace, 0, Delta*sizeof(Space));
			nspace+=Delta;
		}
	space[i].busy=1;
	return i;
}

Bind *
spacelookup(int n, char *name)
{
	Bind *bp=tospace(n)->bind;
	for(; bp; bp=bp->next)
		if(strcmp(name, bp->name)==0)
			return bp;
	return 0;
}

Bind *
spaceinstallb(int n, char *name, long perm)
{
	Space *sp=tospace(n);
	Bind *bp=sp->bind;
	Bind *obp=0;
	ckname(name);
	for(; bp; obp=bp, bp=bp->next)
		if(strcmp(name, bp->name)==0)
			rerror("duplicate name \"%s\" in nspace %d", name, n);
	bp=alloc(Bind);
	bp->next=0;
	bp->space=sp;
	strcpy(bp->name, name);
	bp->seq=sp->seq++;
	bp->nclient=0;
	bp->nopen=0;
	bp->removed=0;
	bp->qid=(perm&CHDIR)|((n&0xFF)<<8)|(bp->seq&0xFF);
	bp->perm=perm;
	memset(bp->q, 0, sizeof bp->q);
	if(obp)
		obp->next=bp;
	else
		sp->bind=bp;
	return bp;
}

int
spaceinstall(int n, char *name, long perm)
{
	return spaceinstallb(n, name, perm)->seq;
}

void
binduninstallb(Bind *bp)
{
	Bind *xbp;
	Space *sp=bp->space;
	bp->removed=1;
	drain(bp);
	if(bp->nclient==0){
		if(sp->bind==0)
    error:
			rerror("multiple removes of binding");
		if(sp->bind==bp)
			sp->bind=bp->next;
		else{
			for(xbp=sp->bind; xbp->next!=bp; xbp=xbp->next)
				if(xbp->next==0)
					goto error;
			xbp->next=bp->next;
		}
		free(bp);
	}
}

void
binduninstall(int n, int seq)
{
	binduninstallb(tobind(tospace(n), seq));
}

int
badchar(int c){
	static uchar hex[]={
		0xFF, 0xFF, 0xFF, 0xFF, 0x90, 0x01, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	};
	return (c&0x80) || (hex[c>>3]&(0x80>>(c&7)));
}

void
ckname(char *name)
{
	char *s;
	if(strlen(name)>=NAMELEN-1)
		rerror("file name \"%s\" too long", name);
	for(s=name; *s; s++)
		if(badchar(*s))
			rerror("illegal character 0x%x in file name \"%s\"", *s, name);
}
