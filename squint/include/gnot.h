type Point:struct of{
	x:	int;
	y:	int;
};
type Rectangle:struct of{
	min:	Point;
	max:	Point;
};
type Bitmap:struct of{
	r:	Rectangle;
	ldepth:	int;
	id:	int;
	seq:	int;	# 0=> Bitmap, !0=> Layer
};
type Mouse: struct of{
	but:	int;
	xy:	Point;
};
type Cursor: struct of{
	offset:	Point;
	clr:	array[16] of int;
	set:	array[16] of int;
};
type Font: int;		# hack

# known fonts
const font:Font=	0;

# bitblt functions
const DST	:= 0xA;
const SRC	:= 0xC;
const XXX	:= 0xF;

# mouse ctl functions
const Cursorallow:=0;
const Cursorinhibit:=1;

builtin getscreen:prog() of Bitmap;
builtin bitblt:prog(db:Bitmap, p:Point, sb:Bitmap, r:Rectangle, c:int);
builtin string:prog(b:Bitmap, p:Point, f:Font, a:array of char, c:int);
builtin texture:prog(b:Bitmap, r:Rectangle, t:Bitmap, c:int);
builtin strsize:prog(f:Font, a:array of char) of Point;
builtin mkbitmap:prog(r:Rectangle, i:int) of Bitmap;
builtin cursorswitch:prog(c:Cursor);
builtin ptinrect:prog(p:Point, r:Rectangle) of int;
builtin rectinset:prog(r:Rectangle, dx,dy:int) of Rectangle;
builtin border:prog(b:Bitmap, r:Rectangle, i:int, c:int);

builtin BtoMouse:prog(a:array of char) of Mouse;
builtin MousetoB:prog(m:Mouse) of array of char;
