Find:=prog(dl: Dlist, tag: int) of Frame
{
	while (dl.el.n != tag && dl.el.n != 0)
		dl = dl.nxt;
	become dl.el;
};

Match:=prog(dl: Dlist, pt: Point) of Frame
{
	while (dl.el.n != 0 && !ptinrect(pt, dl.el.r))
		dl = dl.nxt;
	become dl.el;
};

rec Delete:=prog(dl: Dlist, tag: int) of Dlist
{
	if (dl.el.n == 0)	become dl;
	if (dl.el.n == tag)	become dl.nxt;
	become { dl.el, Delete(dl.nxt, tag) };
};

rec Replace:=prog(dl: Dlist, fr: Frame) of Dlist
{
	if (dl.el.n == 0)	become dl;
	if (dl.el.n == fr.n)	become { fr, dl.nxt };
	become { dl.el, Replace(dl.nxt, fr) };
};

Names:=prog(dl: Dlist) of array of array of char
{	i  := 0;
	dd := dl;

	while (dl.el.n != 0)	# count frames
	{	i++;
		dl = dl.nxt;
	}
	nm := mk(array[i+5] of array of char);
	nm[0] = "new      ";
	nm[1] = "reshape  ";
	nm[2] = "close    ";
	nm[3] = "refresh  ";
	nm[4] = "quit     ";

	i = 5;
	for (dl = dd; dl.el.n != 0; dl = dl.nxt)
		nm[i++] = print(dl.el.n, " ", dl.el.name);
	become nm;
};

refresh:=prog(dl: Dlist)
{	ln := mk(Token);
	ln.tok = REPAINTED;

	while (dl.el.n != 0)
	{	dl.el.ch<- = ln;
		dl = dl.nxt;
	}
};

flip:=prog(dl: Dlist)
{	ln := mk(Token);
	ln.tok = RESHAPED;

	while (dl.el.n != 0)
	{	dl.el.ch<- = ln;
		dl = dl.nxt;
	}
};

rec clipper:=prog(
	comm	: Dcomm,
	dd	: Dlist,	# entry point in display list
	cs	: Bitmap)	# clipped screen (Sc)
{
	if (dd.el.n == 0 || dd.el.n == comm.fr.n)
		become bitblt(cs, comm.xy, comm.bm, comm.bm.r, SRC);

	if (!rectouch(cs.r, dd.el.r))
		become clipper(comm, dd.nxt, cs);

	dim := subpt(comm.bm.r.max, comm.bm.r.min);
	drw : Rectangle = { comm.xy, addpt(comm.xy, dim) };
	side := mk(array[4] of Rectangle);
	side[0] = side[1] = side[2] = side[3] = cs.r;

	side[0].max.y = dd.el.r.min.y;		# top
	side[1].min.y = dd.el.r.max.y;		# bottom
	side[2].min.y = dd.el.r.min.y;		# left
	side[2].max = { dd.el.r.min.x, dd.el.r.max.y };
	side[3].max.y = dd.el.r.max.y;		# right
	side[3].min = { dd.el.r.max.x, dd.el.r.min.y };

	i : int;
	for (i = 0; i < 4; i++)
		side[i] = rectclip(cs.r, side[i]);
	for (i = 0; i < 4; i++)
		if (rectouch(side[i], drw))
		{	cs.r = side[i];
			clipper(comm, dd.nxt, cs);
		}
};

Clipper:=prog(comm: Dcomm, Dl: Dlist)
{	St := Sc; St.r = comm.fr.r;
	clipper(comm, Dl, St);
};

Finder:=prog(comm: Dcomm, Dl: Dlist)
{	comm.res<- = Find(Dl, comm.fr.n);
};

Namer:=prog(comm: Dcomm, Dl: Dlist)
{	comm.nms<- = Names(Dl);
};

Matcher:=prog(comm: Dcomm, Dl: Dlist)
{	comm.res<- = Match(Dl, comm.xy);
};

Insert:=prog(fr: Frame, Dl: Dlist) of Dlist
{	become { fr, Dl };
};

Totop:=prog(fr: Frame, Dl: Dlist) of Dlist
{	become { fr, Delete(Dl, fr.n) };
};

rec Stack:=prog(r: Rectangle, dl: Dlist) of Dlist
{	ln := mk(Token);

	if (dl.el.n == 0)
		become dl;
	if (dl.el.n != -1)
	{	dl.el.r = r;
		ln.tok = RESHAPED;
		dl.el.ch<- = ln;
	}
	become { dl.el, Stack(r, dl.nxt) };
};

d_handler:=prog(Dch: chan of Dcomm)
{	Dh_act	:= mk(chan of Dcomm);
	comm	:  Dcomm;
#	Dl	:= mk(Dlist={ mk(Frame) });
	f:=mk(Frame);		# why do i need to split this into two? 
	Dl:=mk(Dlist=f);
	Dl.el.n  = 0;
	
	for (;;)
	{	comm = <-Dch;
		switch (comm.tok) {
		case INSERT:
			Dl = Insert(comm.fr, Dl);
		case TOTOP:
			Dl = Totop(comm.fr, Dl);
		case REPLACE:
			Dl = Replace(Dl, comm.fr);
		case STACK:
			Dl = Stack(comm.fr.r, Dl);
			begin refresh(Dl);
		case DELETE:
			Dl = Delete(Dl, comm.fr.n);
			begin refresh(Dl);
		case BITBLT:
			begin Clipper(comm, Dl);
		case FIND:
			begin Finder(comm, Dl);
		case MENU:
			begin Namer(comm, Dl);
		case MATCH:
			begin Matcher(comm, Dl);
		case REFRESH:
			begin refresh(Dl);
		case FLIP:
			begin flip(Dl);
		default:
			cannot_happen();
	}	}
};
