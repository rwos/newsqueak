#	assume 512x512 for now
const X	:= 512;
const Y := 512;

rawpaint:=prog(
	fr	: Frame,
	raw	: array of char,
	Ds	: chan of Dcomm,
	Bm	: Bitmap)
{
	if (fr.n == 0)
		cannot_happen();

	Fr := rectinset(fr.r, 3, 3);
	i, x, y : int;
	z : char;
	step1, step2 : int;

	W := Fr.max.x - Fr.min.x;
	H := Fr.max.y - Fr.min.y;

	if (fr.zoom)
	{	H = H/fr.reso;
		W = W/fr.reso;
		step1 = 1;
		step2 = fr.reso;
	} else
	{	H = H*fr.reso;
		W = W*fr.reso;
		step1 = fr.reso;
		step2 = fr.reso;
	}

	nz	:= mk(array[256] of Rectangle);
	nx	:= mk(array[X] of int);
	fd	:= fopen("/usr/gerard/sqed/Grey", 0);
	bm	:= freadbitmap(fd); fclose(fd);

	lbm	:= mk(array[256] of Bitmap);
	for (i = 0; i < 256; i++)
	{	lbm[i] = mkbitmap({{0,0},{16,16}}, 0);
		bitblt(lbm[i], {0,0}, bm, {{i*16,0},{(i+1)*16,16}}, SRC);
	}
	for (i = 0; i < 256; i++)
		nz[i] = { {i*16, 0}, {i*16+step2, step2} };
	for (i = 0; i < X; i++)
		nx[i] = i%(16-step2);

	r : Rectangle;
	at := Bm.r.min;
	for (y = fr.offset.y; y < fr.offset.y+H; y = y+step1)
	{	if (y < Y && at.y < Y)
		{	i = y*X;
			dy := nx[at.y];
			for (x = fr.offset.x; x < fr.offset.x+W; x = x+step1)
			{	if (x < X && at.x < X)
				{	z = raw[i+x];
					dx := nx[at.x];
					r = { { dx, dy }, { 16, 16 } };
					bitblt(Bm, at, lbm[z], r, SRC);
					at.x = at.x + step2;
				} else
					x = fr.offset.x+W;
			}

			paint(fr, Bm, Ds, 0);
			at.x = Bm.r.min.x;
			at.y = at.y + step2;
		} else
			y = fr.offset.y+H;
	}
	t := mk(Token);
	t.tok = BMAP;
	t.bm = Bm;
	fr.ch<- = t;
};
