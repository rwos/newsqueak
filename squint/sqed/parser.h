lex:=prog(Lexin : chan of Keyboard, Lexout: chan of Token)
{	ln	:= mk(Keyboard);
	t	:= mk(Token);
	word	:= mk(array[128] of char);
	ln.n 	 = 0;
	i	:= 0;

	for(;;)
	{	j:=0;
		if (i >= ln.n)
		{	t.tok = EOL;
			Lexout<- = t;
			ln=<-Lexin;
			i = 0;
		}
		while (i < ln.n
		    && ln.line[i] != ' '
		    && ln.line[i] != '\t')
			word[j++] = ln.line[i++];
		word[j] = '\0'; i++;
		if (isdigit(word[0]) || word[0] == '-')
		{	t.tok = VALUE; t.valu = atoi(word); t.txt = word;
			Lexout<- = t;
		} else if (j > 0)
		{	t.tok = NAME; t.valu = 0; t.txt = word;
			Lexout<- = t;
		}
	}
};

parse:=prog(Kw	: chan of Token,
	Ms_evt	: chan of Mouse,
	Ds	: chan of Dcomm,
	Mn	: chan of char)
{
	i, n	:  int;
	ms	:  Mouse;
	ln	:  Token;
	Off	:= mk(Point);
	rc	:  Rectangle;
	fr	:= mk(Frame);
	nframes	:= 1;
	curframe:=-1;
	req	:= mk(Dcomm);
	req.fr   = fr;
	req.res  = mk(chan of Frame);

	for (;;)
	{	ln=<-Kw;
		if (ln.tok == EOL)
			;	# ignore
		else if (ln.tok != NAME)
			strout(print("?bad token ", ln.valu, "\n"), Mn);
		else {
		fr = findframe(curframe, Ds, Mn);
		switch (ln.txt) {
		case ".=":
			ms=<-Ms_evt;
			if (fr.n != 0 && ptinrect(ms.xy, fr.r))
			{	dx, dy : int;
				if (fr.zoom)
				{	dx = (ms.xy.x-fr.r.min.x)/fr.reso;
					dx = dx + fr.offset.x;
					dy = (ms.xy.y-fr.r.min.y)/fr.reso;
					dy = dy + fr.offset.y;
				} else
				{	dx = fr.offset.x+ms.xy.x-fr.r.min.x;
					dy = fr.offset.y+ms.xy.y-fr.r.min.y;
				}
				strout(print(dx, ",", dy, "\n"), Mn);
			} else
				strout("no frame\n", Mn);
		case "become":
			ln = <-Kw;
			if (ln.tok != VALUE)
				strout("?become .N\n", Mn);
			else
			{	curframe = ln.valu;
				fr = findframe(curframe, Ds, Mn);
				if (fr.n != 0)
				{	ln.tok = REPAINT;
					fr.ch<- = ln;
				}
			}
		case "close":
			ln = <-Kw;
			if (ln.tok != VALUE)
				strout("?close .N\n", Mn);
			else 
			{	fr = findframe(ln.valu, Ds, Mn);
				if (fr.n > 0)
				{	ln.tok   = ZAP;
					fr.ch<-  = ln;
					curframe = -1;
			}	}
		case "e":
			ln = <-Kw;
			if (ln.tok != NAME)
				strout("?e filename\n", Mn);
			else if (fr.n > 0)
				fr.ch<- = ln;
		case "f":
			if (fr.n == 0)
				strout("?no frame\n", Mn);
			else
			{	strout(print(fr.n," ",fr.name, "\n"),Mn);
				if (fr.rawmode)
				{	strout("rawmode, ", Mn);
					if (fr.zoom) strout("zoomed, ", Mn);
					strout(print("reso: ", fr.reso, "\n"), Mn);
				}
				a := fr.offset;
				strout(print("off: ", a.x, ",", a.y, "\n"), Mn);
			}
		case "flip":
			ln = <-Kw;
			if (ln.tok != VALUE)
				strout("?close .N\n", Mn);
			else 
			{	cnt := ln.valu;
				while (--cnt > 0)
				{	req.tok = FLIP;
					Ds<- = req;
				}
			}
		case "new":
			curframe = nframes++;
			fr.r = getrect(Kw);
			if (fr.r.max.x != 0)
			{	fr = { fr.r,mk(chan of Token),"no name",curframe };
				begin framehandler(fr, Ds);
			}
		case "r":
			ln = <-Kw;
			if (ln.tok != NAME)
				strout("?e filename\n", Mn);
			else if (fr.n != 0)
			{	ln.tok = RAW;
				fr.ch<- = ln;
			}
		case "refresh":
			{	rc := rectinset(Sc.r, 3, 3);
				bitblt(Sc, rc.min, Sc, rc, 0);
				req.tok = REFRESH;
				Ds<- = req;
			}
		case "reshape":
			ln = <-Kw;
			if (ln.tok != VALUE)
				strout("?reshape .N rect\n", Mn);
			else 
			{	r := getrect(Kw);
				if (r.max.y != 0)
				{	curframe = ln.valu;
					fr = findframe(curframe, Ds, Mn);
					bitblt(Sc, fr.r.min, Sc, fr.r, 0);
					fr.r = r;
					replace(fr, Ds);	# in display list
					ln.tok = RESHAPED;
					fr.ch<- = ln;
					req.tok = REFRESH;
					Ds<- = req;
				}
			}
		case "reso":
			ln = <-Kw;
			if (ln.tok != VALUE)
				strout("?reso N\n", Mn);
			else
			{	if (fr.n > 0)
				{	ln.tok = RESO;
					fr.ch<- = ln;
				}
			}
		case "stack":
			if (fr.n > 0)
			{	rc := rectinset(Sc.r, 3, 3);
				bitblt(Sc, rc.min, Sc, rc, 0);
				req.fr  = fr;
				req.tok = STACK;
				Ds<- = req;
			}
		case "q":
			Exit();
		case "quit":
			Exit();
		case "to":
			ln = <-Kw;
			if (ln.tok != VALUE)
				strout("?to .X Y\n", Mn);
			else 
			{	Off.x = ln.valu;
				ln = <-Kw;
				if (ln.tok != VALUE)
					strout("?to X .Y\n", Mn);
				else
				{	Off.y = ln.valu;
					if (fr.n > 0)
					{	ln.tok = POSITION;
						ln.pt = Off;
						fr.ch<- = ln;
			}	}	}
		case "unzoom":
			if (fr.n > 0)
			{	ln.tok = ZOOM; ln.valu = 0;
				fr.ch<- = ln;
			}
		case "zoom":
			if (fr.n > 0)
			{	ln.tok = ZOOM; ln.valu = 1;
				fr.ch<- = ln;
			}
		default:
			if (len(ln.txt) > 0)
			strout(print("?syntax ", ln.txt, "\n"), Mn);
			}
		}
	}
};
