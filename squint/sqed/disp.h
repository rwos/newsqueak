const Offset:=4;
const Fheight:=16;
const Fwidth:=8;

Instring := 0;

disp:=prog(
	r	: Rectangle,
	Ds	: chan of Dcomm,
	nM	: chan of Token,
	Mn	: chan of char)
{
	mx := mk(Point);	# maximum cursor position
	wp := mk(Point);	# cursor position
	lc := mk(Point);	# last newline position
	c  := mk(char);
	s  := mk(array[1] of char);
	ri := rectinset(r, 3, 3);
	bm := mkbitmap(ri, 0);
	fr : Frame;
	com: Token;

	solidbox(r);
	jn:=mk(Frame);
	jn.r = r;
	bitblt(bm, bm.r.min, bm, bm.r, 0);
	jn.name = "~~sqed~~";
	jn.n  = -1;
	jn.ch = nM;

	req := mk(Dcomm);
	req.res = mk(chan of Frame);
	req.tok = INSERT;
	req.fr = jn;
	Ds<- = req;

	lc = wp = {Offset,Offset};
	mx.x = (r.max.x-r.min.x-2*Offset-Fwidth);
	mx.y = (r.max.y-r.min.y-2*Offset-2*Fheight);

	for(;;)
	{	select {
		case com=<-nM:
			switch (com.tok) {
			case RESHAPED:
				req.tok = FIND;
				req.fr = jn;
				Ds<- = req; jn = <-req.res;

				ri = rectinset(jn.r, 3, 3);
				{	bm2 := mkbitmap(ri, 0);
					bitblt(bm2, bm2.r.min, bm2, bm2.r, 0);
					bitblt(bm2, bm2.r.min, bm, bm.r, SRC);
					bm = bm2;
				}
				mx.x = (jn.r.max.x-jn.r.min.x-2*Offset-Fwidth);
				mx.y = (jn.r.max.y-jn.r.min.y-2*Offset-Fheight);
				begin paint(jn, bm, Ds, TOTOP);
			case REPAINTED:
				begin paint(jn, bm, Ds, 0);
			case REPAINT:
				begin paint(jn, bm, Ds, TOTOP);
			}
		case c=<-Mn:
			if (c == '\b')
			{	s = " ";
				if (wp.x >= Offset+Fwidth)
				{	wp.x = wp.x - Fwidth;
					solidbox(jn.r);
					pt := addpt(jn.r.min, wp);
					string(bm, pt, font, s, SRC);
					begin paint(jn, bm, Ds, 0);
				} else
				if (wp.y >= Offset+Fheight && wp.y > lc.y+Fheight)
				{	wp.x = Offset + (mx.x/Fwidth)*Fwidth;
					wp.y = wp.y - Fheight;
					solidbox(jn.r);
					pt := addpt(jn.r.min, wp);
					string(bm, pt, font, s, SRC);
					begin paint(jn, bm, Ds, 0);
				}
			} else
			{	if (wp.x >= mx.x || c == '\n')
				{	lc = wp;
					wp.x = Offset;
					wp.y = wp.y + Fheight;
				}
				if (wp.y >= mx.y)	# scroll
				{	rt := bm.r;
					rt.min.y = rt.min.y + Fheight;
					bitblt(bm, bm.r.min, bm, rt, SRC);
					rt.min.y = rt.max.y - Fheight;
					bitblt(bm, rt.min, bm, rt, 0);
					wp.y = wp.y - Fheight;
					begin paint(jn, bm, Ds, 0);
				}
				if (c != '\n')
				{	s[0] = c;
					pt := addpt(jn.r.min, wp);
					string(bm, pt, font, s, SRC);
					if (!Instring)
						begin paint(jn, bm, Ds, 0);
					wp.x = wp.x + Fwidth;
			}	}
		}
	}
};

strout=prog(s: array of char, Mn: chan of char)
{	i:=0;
	j:=len(s);

	Instring = 1;
	for (i=0; i<j-1; i++)
		Mn<-=s[i];
	Instring = 0;
	Mn<-=s[j-1];
};
