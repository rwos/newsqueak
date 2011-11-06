include "rawmap.h"

replace:=prog(fr: Frame, Ds: chan of Dcomm)
{
	req := mk(Dcomm);
	req.res = mk(chan of Frame);
	req.tok = REPLACE;
	req.fr = fr;
	Ds<- = req;
};

framehandler:=prog(fr: Frame, Ds: chan of Dcomm)
{	ln	: Token;
	fd	: File;
	St	: Bitmap;
	image	: array of char;
	raw	: array of char;
	i, j, k	: int;

	bm := mkbitmap(rectinset(fr.r, 3, 3), 0);
	bitblt(bm, fr.r.min, bm, fr.r, 0);

	req := mk(Dcomm);
	req.res = mk(chan of Frame);
	fr.zoom = 0;
	fr.reso = 6;
	fr.rawmode = 0;
	fr.offset = { 0, 0 };
	req.tok = INSERT;
	req.fr = fr;
	Ds<- = req;

	for (;;)
	{	ln = <-fr.ch;
		switch (ln.tok) {
		case BMAP:
			bm = ln.bm;
			begin paint(fr, bm, Ds, 0);
		case REPAINTED:
			begin paint(fr, bm, Ds, 0);
		case REPAINT:
			begin paint(fr, bm, Ds, TOTOP);
		case POSITION:
			fr.offset = ln.pt;
			begin replace(fr, Ds);
			if (fr.rawmode)
				begin rawpaint(fr, image, Ds, bm);
		case ZOOM:
			fr.zoom = ln.valu;
			begin replace(fr, Ds);
			if (fr.rawmode)
				begin rawpaint(fr, image, Ds, bm);
		case RESO:
			fr.reso = ln.valu;
			begin replace(fr, Ds);
			if (fr.rawmode)
				begin rawpaint(fr, image, Ds, bm);
		case RESHAPED:
			req.tok = FIND;
			req.fr = fr;
			Ds<- = req; fr = <-req.res;
			if (fr.name == "no name")
			{	bm = mkbitmap(rectinset(fr.r, 3, 3), 0);
				bitblt(bm, fr.r.min, bm, fr.r, 0);
			};
			begin paint(fr, bm, Ds, TOTOP);
		case RAW:
			if (ln.txt[len(ln.txt)-1] == '\n')
				ln.txt[len(ln.txt)-1] = '\0';
			fr.name = ln.txt;
			fr.rawmode = 1;
			begin replace(fr, Ds);
		{	f := open(ln.txt, 0);
			image = bread(f, 512*512);
			close(f);
		}	bm = mkbitmap({{0,0},{512,512}}, 0);
			bitblt(bm, {0,0}, bm, bm.r, XXX);
			begin rawpaint(fr, image, Ds, bm);
		case NAME:
			if (ln.txt[len(ln.txt)-1] == '\n')
				ln.txt[len(ln.txt)-1] = '\0';
			fd = fopen(ln.txt, 0);
			fr.name = ln.txt;
			fr.rawmode = 0;
			begin replace(fr, Ds);
			bm = freadbitmap(fd);
			fclose(fd);
			begin paint(fr, bm, Ds, TOTOP);
		case ZAP:
			bitblt(Sc, fr.r.min, Sc, fr.r, 0);
			req.tok = DELETE;
			req.fr = fr;
			Ds<- = req;
			become unit;
		default:
			cannot_happen();
		}
	}
};

findframe:=prog(
	this	: int,
	Ds	: chan of Dcomm,
	Mn	: chan of char) of Frame
{
	req	:= mk(Dcomm);
	req.fr   = mk(Frame);
	req.res  = mk(chan of Frame);
	req.tok  = FIND;
	req.fr.n = this;
	Ds<- = req;
	req.fr = <-req.res;
	if (req.fr.n == 0)
		strout("?no such frame\n", Mn);
	become req.fr;
};

pickframe:=prog(Ms_evt: chan of Mouse, Ds: chan of Dcomm) of Frame
{	ms:Mouse;
	fr: Frame;
	req := mk(Dcomm);
	req.res = mk(chan of Frame);

	cursorswitch(Csight);
	do
		ms=<-Ms_evt;
	while (ms.but!=4);
	do
		ms=<-Ms_evt;
	while (ms.but==4);

	req.tok = MATCH;
	req.xy = ms.xy;
	Ds<- = req; fr = <-req.res;
	cursorswitch(Carrow);

	become fr;
};

paint=prog(fr: Frame, bm: Bitmap, Ds: chan of Dcomm, how: int)
{	req := mk(Dcomm);
	req.res = mk(chan of Frame);
	if (fr.n == 0)
		cannot_happen();

	if (how == TOTOP)
	{	req.tok = TOTOP;
		req.fr = fr;
		Ds<- = req;
	}
	fr = findframe(fr.n, Ds, mk(chan of char));
	Bm := mkbitmap(fr.r, 0);
	bitblt(Bm, Bm.r.min, Bm, Bm.r, XXX);
	bitblt(Bm, addpt(fr.r.min, {3,3}), bm, bm.r, SRC);
	Solidbox(Bm, fr.r);
	req.tok = BITBLT;
	req.xy = fr.r.min;
	req.fr = fr;
	req.bm = Bm;
	Ds<- = req;
};
