type Sreq: struct of {
	tok:	int;
	r:	Rectangle;
	bm:	Bitmap;
	res:	chan of Rectangle;

	mch:	chan of Mouse;
	m:	Mouse;
	menu:	Menu;
	ans:	chan of int;
};
screen:=prog(Sc: Bitmap, Rq: chan of Sreq)	# hides the screen bitmap
{
	for(;;)
	{	req := <-Rq;
		switch(req.tok) {
		case CLR:
			bitblt(Sc, req.r.min, Sc, req.r, 0);
		case PUT:
			bitblt(Sc, req.r.min, req.bm, req.r, SRC);
		case XBOX:
			rawbox(Sc, req.r, XXX&~DST, 1);
		case BOX:
			rawbox(Sc, rc, XXX, 3);
		case RECT:
			req.res<- = Sc.r;
		case MENU:
			req.ans<- = menuhit(Sc, req.mch, req.m, req.menu, 4);
		}
	}
};
