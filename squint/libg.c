#include "store.h"
#include "comm.h"
#include "libargs.h"
#include <u.h>
#include <lib9.h>
#include <libg.h>
#include "space.h"

#include "fns.h"
#include "slibgfns.h"

extern void	fmountinit(void);
extern void	binduninstall(int, int);

int	libgstart;

/*
 * Graphics help
 */


Store*
mkpoint(int a, int b)
{
	Store *s;

	s = mk(Sstruct, 2);
	s->data[1] = a;
	s->data[2] = b;
	return s;
}

Store*
mkrectangle(int a, int b, int c, int d)
{
	Store *s;

	s = mk(Sstruct, 2);
	s->data[0] = 0x03;
	s->data[1] = (long)mkpoint(a, b);
	s->data[2] = (long)mkpoint(c, d);
	return s;
}

Store*
mkdata(long p)
{
	Store *s;

	s = mk(Sdata, 1);
	s->data[0] = p;
	return s;
}

Store*
mkmouse(int but, int x, int y)
{
	Store *s;

	s = mk(Sstruct, 2);
	s->data[0] = 0x02;
	s->data[1] = (long)but;
	s->data[2] = (long)mkpoint(x, y);
	return s;
}

void
inpoint(Store *s, Point *pp)
{
	if(pp == 0)
		rerror("undefined Point");
	pp->x = s->data[1];
	pp->y = s->data[2];
}

void
inrectangle(Store *s, Rectangle *rp)
{
	inpoint((Store *)s->data[1], &rp->min);
	inpoint((Store *)s->data[2], &rp->max);
}

void
incursor(Store *s, Cursor *cp)
{
	int i;
	ulong w;

	if(s->data[1]==0 || s->data[2]==0 || s->data[3]==0)
		rerror("undefined element of Cursor");
	if(((Store *)s->data[2])->len!=16 || ((Store *)s->data[3])->len!=16)
		rerror("Cursor has wrong sized bitmaps");
	inpoint((Store *)s->data[1], &cp->offset);
	for(i=0; i<16; i++){
		w = ((Store *)s->data[2])->data[i];
		cp->clr[2*i+0] = w>>24;
		cp->clr[2*i+1] = w>>16;
		w = ((Store *)s->data[3])->data[i];
		cp->set[2*i+0] = w>>24;
		cp->set[2*i+1] = w>>16;
	}
}

void
infont(long l, Font **fp)
{
	if(l != 0)
		rerror("undefined font");
	*fp = font;
}

void
inmouse(Store *s, Mouse *mp)
{
	mp->buttons = s->data[1];
	inpoint((Store *)s->data[2], &mp->xy);
}

void
libgflush(void)
{
	if(libgstart)
		bflush();
}

/*
 * Graphics
 */

int
bi_bitblt(Proc *proc)
{
	Point p;
	Rectangle r;
	Bitmap *sb, *db;

	db = inbitmap(SARG(proc, 1), 0);
	inpoint(SARG(proc, 2), &p);
	sb = inbitmap(SARG(proc, 3), 0);
	inrectangle(SARG(proc, 4), &r);
	bitblt(db, p, sb, r, LARG(proc, 5));
	DECREF(proc, 1);
	DECREF(proc, 2);
	DECREF(proc, 3);
	DECREF(proc, 4);
	return 1;
}

int
bi_string(Proc *proc)
{
	Point p;
	Bitmap *db;
	Font *f;

	db = inbitmap(SARG(proc, 1), 0);
	inpoint(SARG(proc, 2), &p);
	infont(LARG(proc, 3), &f);
	string(db, p, f, STRARG(proc, 4), LARG(proc, 5));
	DECREF(proc, 2);
	DECREF(proc, 3);
	DECREF(proc, 4);
	return 1;
}

int
bi_texture(Proc *proc)
{
	Rectangle r;
	Bitmap *b, *t;

	b = inbitmap(SARG(proc, 1), 0);
	inrectangle(SARG(proc, 2), &r);
	t = inbitmap(SARG(proc, 3), 0);
	texture(b, r, t, LARG(proc, 4));
	DECREF(proc, 1);
	DECREF(proc, 2);
	DECREF(proc, 3);
	return 1;
}

int
bi_strsize(Proc *proc)
{
	Font *f;
	Point p;

	infont(LARG(proc, 1), &f);
	p = strsize(f, STRARG(proc, 2));
	DECREF(proc, 2);
	RETVAL(proc, 2) = (long)mkpoint(p.x, p.y);
	return 1;
}

int
bi_mkbitmap(Proc *proc)
{
	Bitmap *b;
	Rectangle r;
	int ld;
	char buf[ERRLEN];

	inrectangle(SARG(proc, 1), &r);
	ld = LARG(proc, 2);
	if(ld<0 || ld>5)
		rerror("ldepth %d out of range in mkbitmap", ld);
	b = balloc(r, ld);
	if(b == 0){
		errstr(buf);
		fprint(2, "balloc: %s\n", buf);
		rerror("balloc fail in mkbitmap");
	}
	DECREF(proc, 1);
	RETVAL(proc, 2)=(long)mkbitmap(b);
	free(b);
	return 1;
}

/*
 * I/O
 */

bi_getscreen(Proc *proc)
{
	static Store *sscreen;

	if(!libgstart){
		binit(0, 0, "squint");
		sscreen=mkbitmap(&screen);
		((Store *)sscreen->data[1])->ref++;	/* so it won't get freed */
		libgstart++;
	}
	sscreen->ref++;
	RETVAL(proc, 0)=(long)sscreen;
	return 1;
}

int
bi_readbitmap(Proc *proc)
{
	Bitmap *b;
	extern Bitmap *tobm(int);

	b = rdbitmapfile(LARG(proc, 1));
	if(b == 0)
		error("can't read bitmap");
	RETVAL(proc, 1) = (long)mkbitmap(b);
	return 1;
}


int
bi_cursorswitch(Proc *proc)
{
	static int fd;
	Cursor c;

	incursor(SARG(proc, 1), &c);
	cursorswitch(&c);
	return 1;
}

/*
 * Geometry
 */

int
bi_ptinrect(Proc *proc)
{
	Point p;
	Rectangle r;

	inpoint(SARG(proc, 1), &p);
	inrectangle(SARG(proc, 2), &r);
	RETVAL(proc, 2) = ptinrect(p, r);
	DECREF(proc, 1);
	DECREF(proc, 2);
	return 1;
}

int
bi_rectinset(Proc *proc)
{
	Rectangle r;
	int a, b;

	inrectangle(SARG(proc, 1), &r);
	a = LARG(proc, 2);
	b = LARG(proc, 3);
	RETVAL(proc, 3) = (SWord)mkrectangle(r.min.x+a, r.min.y+b, r.max.x-a, r.max.y-b);
	DECREF(proc, 1);
	return 1;
}

int
bi_border(Proc *proc)
{
	Rectangle r;
	Bitmap *b;
	Fcode c;
	int i;

	b = inbitmap(SARG(proc, 1), 0);
	inrectangle(SARG(proc, 2), &r);
	i = LARG(proc, 3);
	c = LARG(proc, 4);
	bitblt(b, r.min, b, Rect(r.min.x, r.min.y, r.max.x, r.min.y+i), c);
	bitblt(b, Pt(r.min.x, r.max.y-i),
		b, Rect(r.min.x, r.max.y-i, r.max.x, r.max.y), c);
	bitblt(b, Pt(r.min.x, r.min.y+i),
		b, Rect(r.min.x, r.min.y+i, r.min.x+i, r.max.y-i), c);
	bitblt(b, Pt(r.max.x-i, r.min.y+i),
		b, Rect(r.max.x-i, r.min.y+i, r.max.x, r.max.y-i), c);
	DECREF(proc, 1);
	DECREF(proc, 2);
	return 1;
}

int
bi_BtoMouse(Proc *proc)
{
	Mouse m;
	uchar *d;

	d = (uchar*)SARG(proc, 1)->data;
	m.buttons = d[1] & 7;
	m.xy.x = BGLONG(d+2);
	m.xy.y = BGLONG(d+6);
	RETVAL(proc, 1)=(SWord)mkmouse(m.buttons, m.xy.x, m.xy.y);
	DECREF(proc, 1);
	return 1;
}

int
bi_MousetoB(Proc *proc)
{
	Mouse m;
	uchar up[10];
	inmouse(SARG(proc, 1), &m);
	up[0] = 'm';
	up[1] = m.buttons;
	BPLONG(up+2, m.xy.x);
	BPLONG(up+6, m.xy.y);
	RETVAL(proc, 1)=(SWord)mkarraychar((char*)up, 10);
	DECREF(proc, 1);
	return 1;
}

/*
 * Name space
 */

int
bi_nsnew(Proc *proc)
{
	RETVAL(proc, 0) = spacenew();
	return 1;
}

int
bi_nsinstall(Proc *proc)
{
	RETVAL(proc, 3) = spaceinstall(LARG(proc, 1), STRARG(proc, 2), LARG(proc, 3));
	DECREF(proc, 2);
	return 1;
}

int
bi_nsuninstall(Proc *proc)
{
	binduninstall(LARG(proc, 1), LARG(proc, 2));
	return 1;
}

int
bi_nslookup(Proc *proc)
{
	RETVAL(proc, 2) = (SWord)spacelookup(LARG(proc, 1), STRARG(proc, 2));
	DECREF(proc, 2);
	return 1;
}

int
bi_nsproc(Proc *proc)
{
	enum{
		Nargv=500
	};
	char *argv[Nargv];
	int i;
	int ns = LARG(proc, 1);
	Store *s = SARG(proc, 2);

	if(s->len >= Nargv)
		rerror("argument list too long in nsproc (limit %d)", Nargv);
	for(i=0; i<s->len; i++)
		argv[i]=(char *)(((Store *)s->data[i])->data);
	argv[s->len] = 0;
	RETVAL(proc, 2)=fmountproc(ns, argv);
	DECREF(proc, 2);
	return 1;
}

int
bi_nsread(Proc *proc)
{
	Bind *bp = tobind(tospace(LARG(proc, 1)), LARG(proc, 2));

	if(bp == 0){
		RETVAL(proc, 4) = -1;
		DECREF(proc, 3);
		return 1;
	}
	return fmread(proc, bp, RData, SARG(proc, 3), LARG(proc, 4));
}

int
bi_nsreadctl(Proc *proc)
{
	Bind *bp = tobind(tospace(LARG(proc, 1)), LARG(proc, 2));

	if(bp == 0){
		RETVAL(proc, 4) = -1;
		DECREF(proc, 3);
		return 1;
	}
	return fmread(proc, bp, RCtl, SARG(proc, 3), LARG(proc, 4));
}

int
bi_nswrite(Proc *proc)
{
	Bind *bp = tobind(tospace(LARG(proc, 1)), LARG(proc, 2));

	if(bp == 0){
		RETVAL(proc, 3)=(SWord)mkarraychar("", 0);
		((Store *)RETVAL(proc, 3))->len=-1;
		return 1;
	}
	return fmwrite(proc, bp, WData, LARG(proc, 3));
}

int
bi_nswritectl(Proc *proc)
{
	Bind *bp=tobind(tospace(LARG(proc, 1)), LARG(proc, 2));
	if(bp==0){
		RETVAL(proc, 3)=(SWord)mkarraychar("", 0);
		((Store *)RETVAL(proc, 3))->len=-1;
		return 1;
	}
	return fmwrite(proc, bp, WCtl, LARG(proc, 3));
}
