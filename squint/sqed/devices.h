sweep:=prog(Ms_evt: chan of Mouse) of Rectangle
{	rc:=mk(Rectangle);
	re:Rectangle;
	ms,ur,ll:Mouse;

	cursorswitch(Cbox);
	do
		ur=<-Ms_evt;
	while (ur.but!=4);
	ms = ur;
	ll=<-Ms_evt;
	rc.min = ur.xy; rc.max = ll.xy;
	rc = drawbox(rc);		# draw
	do {
		ll=<-Ms_evt;
		if (ll.xy.x != ms.xy.x || ll.xy.y != ms.xy.y)
		{	rc = drawbox(rc);	# erase
			rc.min = ur.xy; rc.max = ll.xy;
			rc = drawbox(rc);	# draw
			ms.xy = ll.xy;
		}
	} while (ll.but==4);
	cursorswitch(Carrow);

	re = rc;
	rc = rectinset(rc, 3, 3);
	solidbox(re);
	bitblt(Sc, rc.min, Sc, rc, 0);	# clear it

	become re;
};

msrv:=prog(
	Ms_evt	: chan of Mouse,
	Dh_req	: chan of Dcomm,
	Kb	: chan of Keyboard)
{
	fr	:= mk(Frame);
	req	:= mk(Dcomm);
	req.res  = mk(chan of Frame);
	req.nms  = mk(chan of array of array of char);

	MA	:= nsbind(gnotions, "mousein", 0, 0666);
	begin nsproc(gnotions, {"/bin/rc", "-c", "exec cat < /dev/mouse > /mnt/ns/mousein"});
	mch	:= mk(chan of Mouse);
	begin prog(){
		for(;;)
			mch<- = BtoMouse(<-MA.rdata);
	}();
	m	:= <-mch;

	for(;;)
	{	select {
		case Ms_evt<- = m:	;
		case m = <-mch:
			if (m.but==7)
				abort("button 123\n");
			if (m.but&1)
			{	kb:=mk(Keyboard);
				req.tok = MATCH;
				req.xy = m.xy;
				Dh_req<- = req; fr = <-req.res;
				if (fr.n != 0)
				{	kb.line = print("become ", fr.n);
					kb.n = len(kb.line);
					Kb<-=kb;
				}
				do
					m = BtoMouse(<-MA.rdata);
				while (m.but&1);
			}
			if (m.but&4)
			{	kb	:= mk(Keyboard);
				menu	:= mk(Menu);
				req.tok  = MENU;
				Dh_req<- = req; nm := <-req.nms;
				menu.lab = nm;
				i := menuhit(Sc, mch, m, menu, 4);
				if (i == 2)	#close
				{	fr := pickframe(mch, Dh_req);
					if (fr.n != 0)
					{	kb.line = print("close ", fr.n);
						kb.n = len(kb.line);
						Kb <-= kb;
					}
				} else
				if (i == 1)	#reshape
				{	fr := pickframe(mch, Dh_req);
					if (fr.n != 0)
					{	r := sweep(mch);
						a := print(" ", r.min.x, " ", r.min.y);
						b := print(" ", r.max.x, " ", r.max.y);
						kb.line = print("reshape ", fr.n, a, b);
						kb.n = len(kb.line);
						Kb <-= kb;
					}
				} else
				if (i == 0)	#new
				{	r := sweep(mch);
					a := print(" ", r.min.x, " ", r.min.y);
					b := print(" ", r.max.x, " ", r.max.y);
					kb.line = print("new", a, b);
					kb.n = len(kb.line);
					Kb <-= kb;
				} else
				if (i != -1)
				{	kb.n = 0;
					while (nm[i][kb.n] != ' ')
						kb.n++;
					nm[i][kb.n] = '\0';
					if (isdigit(nm[i][0]) || nm[i][0] == '-')
						kb.line = print("become ", nm[i]);
					else
						kb.line = print(nm[i]);
					kb.n = len(kb.line);
					m.but=0;
					Kb<-=kb;
			}	}
		}
	}
};

ksrv:=prog(Kb_evt: chan of Keyboard, Cw_req: chan of char)	# keyboard server
{	c	:  char;
	cc	:= mk( chan of char);
	txt	:= mk(Keyboard);
	txt.n	 = 0;
	txt.line = mk(array[128] of char);

	K:= nsbind(gnotions, "kbdin", 0, 0666);
	begin nsproc(gnotions, {"/bin/rc", "-c", "exec cat < /dev/rcons > /mnt/ns/kbdin"});

	kbd	:= K.rdata;

	begin prog(){
		line : array of char;
		i := 0;
		for (;;)
		{	line = <-kbd;
			for (i = 0; i < len(line); i++)
				cc<- = line[i];
		}
	}();

	for(;;)
	{	c = <-cc;
		Cw_req<- = c;		# to command window
		if (c == '\b')
		{	if (txt.n > 0)
				--txt.n;
		} else if (c == '\n')
		{	txt.line[txt.n] = '\0';
			Kb_evt<- = txt;
			txt.n = 0;
		} else
		{	if (txt.n < 128)
				txt.line[txt.n++] = c;
		}
	}
};
