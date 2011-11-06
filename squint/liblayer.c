#include "store.h"
#include "comm.h"
#include "libargs.h"
#include <u.h>
#include <lib9.h>
#include <libg.h>
#include <layer.h>

#include "fns.h"

Cover	**cover;
int	ncover;
Layer	**layer;
int	nlayer;
void	inrectangle(Store *, Rectangle *);
Store	*mkdata(long);
Store	*mkrectangle(int, int, int, int);

/*
 * Garbage collecting bitmaps
 */

int	nrefid;
short	*refid;

void
libgballoc(int id)
{
	if(id >= nrefid){
		refid = erealloc(refid, (id+10)*sizeof(short));
		memset(refid+nrefid, 0, ((id+10)-nrefid)*sizeof(short));
		nrefid = id+10;
	}
	refid[id]++;
}

void
libgbfree(int id)
{
	uchar *buf;

	if(--refid[id] == 0){
		buf = bneed(3);
		buf[0] = 'f';
		buf[1] = id;
		buf[2] = id>>8;
		bneed(0);	/* make sure the memory's freed before continuing */
	}
}

Bitmap *
inbitmap(Store *s, int doalloc)
{
	static Bitmap b[2];
	static int x;
	Bitmap *bp, *nbp;
	int n;
	x^=1;	/* bitblt needs two bitmaps */
	bp=&b[x];
	inrectangle((Store *)s->data[1], &bp->r);
	bp->clipr = bp->r;
	bp->ldepth=(long)s->data[2];
	bp->id=((Store*)s->data[3])->data[0];
	n=s->data[4];
	if(n==0){
		bp->cache=0;
		if(doalloc){
			nbp=alloc(Bitmap);
			*nbp=*bp;
			bp=nbp;
		}
	}else{
		nbp = 0;	/* set */
		if(n<0 || n>=nlayer || (nbp=(Bitmap *)layer[n])==0)
			rerror("unknown layer");
		if(doalloc)
		if(!eqrect(nbp->r, bp->r) || ((Layer *)nbp)->user!=n)
			rerror("corrupt layer");
		bp=nbp;
	}
	return bp;
}

Store *
mkbitmap(Bitmap *b)
{
	Store *s, *s1;

	s=mk(Sstruct, 4);
	s->data[0]=0x05;
	s->data[1]=(long)mkrectangle(b->r.min.x, b->r.min.y,
			b->r.max.x, b->r.max.y);
	s->data[2]=b->ldepth;
	s1 = mk(Sbitmapid, 1);
	libgballoc(b->id);
	s1->data[0] = b->id;
	s->data[3]=(long)s1;
	if(b->cache==0)
		s->data[4]=0;
	else
		s->data[4]=((Layer *)b)->user;
	return s;
}

Store *
mkcover(Cover *c, int seq, Store *cov)
{
	Store *cs, *s;
	int i;
	Layer *lp;
	cs=mk(Sstruct, 5);
	cs->data[0]=0x0E;
	cs->data[1]=seq;
	cs->data[2]=(long)cov;
	cov->ref++;
	cs->data[3]=(long)mkbitmap(c->ground);
	for(i=0,lp=c->front; lp; lp=lp->next,i++)
		;
	s=mk(Saryptr, i);
	for(i=0,lp=c->front; lp; lp=lp->next,i++)
		s->data[i]=(long)mkbitmap((Bitmap *)lp);
	cs->data[4]=(long)s;
	return cs;
}

Cover *
incover(Store *s, int *seqp)
{
	int seq=s->data[1];
	if(seq<1 || seq>ncover || cover[seq]==0)
		rerror("unknown cover");
	*seqp=seq;
	return cover[seq];
}

int
bi_mkcover(Proc *proc)
{
	enum{
		Delta=10
	};
	int i;
	Cover *c;
	if(cover==0){
		cover=emalloc(Delta*sizeof(Cover *));
		memset(cover, 0, Delta*sizeof(Cover *));
		ncover=Delta;
	}
	for(i=1; cover[i]; i++)
		if(i==ncover-1){
			cover=erealloc(cover, (ncover+Delta)*sizeof(Cover *));
			memset(cover+ncover, 0, Delta*sizeof(Cover *));
			ncover+=Delta;
		}
	c=cover[i]=alloc(Cover);
	c->layer=(Layer *)inbitmap(SARG(proc, 1), 1);
	c->ground=inbitmap(SARG(proc, 2), 1);
	c->front=0;
	RETVAL(proc, 2)=(SWord)mkcover(c, i, SARG(proc, 1));
	DECREF(proc, 1);
	DECREF(proc, 2);
	return 1;
}

int
bi_lalloc(Proc *proc)
{
	enum{
		Delta=10
	};
	Cover *c;
	Rectangle r;
	Layer *l;
	int i, seq;
	c=incover(SARG(proc, 1), &seq);
	inrectangle(SARG(proc, 2), &r);
	l=lalloc(c, r);
	if(l==0)
		rerror("lalloc failed");
	if(layer==0){
		layer=emalloc(Delta*sizeof(Layer *));
		memset(layer, 0, Delta*sizeof(Layer *));
		nlayer=Delta;
	}
	for(i=1; layer[i]; i++)
		if(i==nlayer-1){
			layer=erealloc(layer, (nlayer+Delta)*sizeof(Layer *));
			memset(layer+nlayer, 0, Delta*sizeof(Layer *));
			nlayer+=Delta;
		}
	l->user=i;
	layer[i]=l;
	RETVAL(proc, 2)=(SWord)mkcover(c, seq, (Store *)SARG(proc, 1)->data[2]);
	DECREF(proc, 1);
	DECREF(proc, 2);
	return 1;
}

void
lop(Proc *proc, void (*lfn)(Layer *))
{
	Cover *c;
	Layer *lp;
	Store *s;
	int seq, lno, i;
	c=incover(SARG(proc, 1), &seq);
	s=(Store *)SARG(proc, 1)->data[4];	/* cover.layer */
	lno=LARG(proc, 2);
	if(lno<0)
   bad:
		rerror("layer with seq %d not in cover", lno);
	for(i=0,lp=c->front; lp && i<s->len; i++,lp=lp->next)
		if(lp==layer[lno]){
			if(((Store *)s->data[i])->data[4]!=lno)
				goto bad;
			(*lfn)(lp);
			RETVAL(proc, 2)=(SWord)mkcover(c, seq, (Store *)SARG(proc, 1)->data[2]);
			DECREF(proc, 1);
			return;
		}
	goto bad;
}

int
bi_ltofront(Proc *proc)
{
	lop(proc, ltofront);
	return 1;
}

int
bi_ltoback(Proc *proc)
{
	lop(proc, ltoback);
	return 1;
}

int
bi_lfree(Proc *proc)
{
	lop(proc, lfree);
	layer[LARG(proc, 2)]=0;
	return 1;
}
