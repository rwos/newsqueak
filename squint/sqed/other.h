normalize:=prog(r:Rectangle) of Rectangle
{	tmp:int;

	if (r.min.x > r.max.x)
	{	tmp = r.min.x;
		r.min.x = r.max.x;
		r.max.x = tmp;
	}
	if (r.min.y > r.max.y)
	{	tmp = r.min.y;
		r.min.y = r.max.y;
		r.max.y = tmp;
	}
	if (r.max.x - r.min.x < 20)
		r.max.x = r.min.x+20;
	if (r.max.y - r.min.y < 20)
		r.max.y = r.min.y+20;
	become r;
};

sanitize:=prog(r: Rectangle) of Rectangle
{
	if (r.max.x <= r.min.x || r.max.y <= r.min.y)
		become {{0,0},{0,0}};
	become r;
};

getrect:=prog(Kw: chan of Token) of Rectangle
{	r,z : Rectangle={ {0,0}, {0,0} };
	ln : Token;

	ln = <-Kw; if (ln.tok != VALUE) become z; r.min.x = ln.valu;
	ln = <-Kw; if (ln.tok != VALUE) become z; r.min.y = ln.valu;
	ln = <-Kw; if (ln.tok != VALUE) become z; r.max.x = ln.valu;
	ln = <-Kw; if (ln.tok != VALUE) become z; r.max.y = ln.valu;
	become normalize(r);
};

addpt:=prog(p1: Point, p2: Point) of Point
{
	p1.x = p1.x+p2.x;
	p1.y = p1.y+p2.y;
	become p1;
};

subpt:=prog(p1: Point, p2: Point) of Point
{
	p1.x = p1.x-p2.x;
	p1.y = p1.y-p2.y;
	become p1;
};

itoa:=prog(n:int) of array of char
{	s:=mk(array[128] of char);
	i:=0; j:=0;

	while(n>10)
	{	s[i++]=(n%10)+'0';
		n = n/10;
	}
	s[i++] = n+'0';
	t:=mk(array[i] of char);
	for (j = 0; i > 0; )
		t[j++] = s[--i];
	become t;
};

isdigit:=prog(c: char) of int
{	become (c >= '0' && c <= '9');
};

atoi:=prog(s: array of char) of int
{	i:=0;
	n:=0;
	sign:=1;

	if (s[0] == '-')
	{	i++;
		sign = -1;
	}
	while (s[i] >= '0' && s[i] <= '9' && i < len(s))
		n = 10*n + s[i++] - '0';
	become (sign*n);
};

rectclip:=prog(r1: Rectangle, r2: Rectangle) of Rectangle
{
	if (r2.min.x > r1.min.x) r1.min.x = r2.min.x;
	if (r2.min.y > r1.min.y) r1.min.y = r2.min.y;
	if (r2.max.x < r1.max.x) r1.max.x = r2.max.x;
	if (r2.max.y < r1.max.y) r1.max.y = r2.max.y;
	if (r1.max.y < r1.min.y) r1.max.y = r1.min.y;
	if (r1.max.x < r1.min.x) r1.max.x = r1.min.x;
	become sanitize(r1);
};

rectouch:=prog(r1: Rectangle, r2: Rectangle) of int
{
	r := rectclip(sanitize(r1), sanitize(r2));
	become (r.max.x!=0);
};

rawbox:=prog(bm: Bitmap, rc:Rectangle, code: int, bar: int) of Rectangle
{
	rc = rectclip(normalize(rc), bm.r);
	top := rc; top.max.y = top.min.y+bar; top.max.x = top.max.x-bar;
	bot := rc; bot.min.y = bot.max.y-bar; bot.min.x = bot.min.x+bar;
	lft := rc; lft.max.x = lft.min.x+bar; lft.min.y = lft.min.y+bar;
	rgt := rc; rgt.min.x = rgt.max.x-bar; rgt.max.y = rgt.max.y-bar;
	bitblt(bm, top.min, bm, top, code);
	bitblt(bm, bot.min, bm, bot, code);
	bitblt(bm, lft.min, bm, lft, code);
	bitblt(bm, rgt.min, bm, rgt, code);
	become rc;
};

drawbox:=prog(rc:Rectangle) of Rectangle
{	become rawbox(Sc, rc, XXX&~DST, 1);
};

solidbox:=prog(rc:Rectangle)
{	rawbox(Sc, rc, XXX, 3);
};

Solidbox:=prog(bm: Bitmap, rc:Rectangle)
{	rawbox(bm, rc, XXX, 3);
};

cannot_happen:=prog()
{	i,j:int;
	print("cannot happen\n");
	i=i/j;	# divide by zero
};

Exit:=prog()
{	rc := rectinset(Sc.r, 3, 3);
	bitblt(Sc, rc.min, Sc, rc, 0);
	abort("quit\n");
};
