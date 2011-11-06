type Keyboard: struct of {
	n	: int;		# number of characters in line
	line	: array of char;
};
type Token: struct of {
	tok,valu: int;
	pt	: Point;
	txt	: array of char;
	bm	: Bitmap;
};
type Frame: struct of {		# used in parse to access frame handlers
	r	: Rectangle;	# frame, screen coordinates
	ch	: chan of Token;# command channel
	name	: array of char;# file name of bitmap
	n	: int;		# a tag, n=0 means last element in display list
	zoom	: int;
	reso	: int;
	offset	: Point;
	rawmode	: int;
};
rec
type Dlist: struct of {		# display list
	el:	Frame;
	nxt:	Dlist;
};
type Dcomm: struct of {		# talk to the display handler
	tok:	int;
	fr:	Frame;
	res:	chan of Frame;	# result
	nms:	chan of array of array of char;
	xy:	Point;
	bm:	Bitmap;		# used for BITBLT command
};

const NAME	:= 1;
const VALUE	:= 2;
const REPAINT	:= 3;
const ZAP	:= 4;
const RESHAPED	:= 5;
const EOL	:= 6;
const BMAP	:= 7;

const INSERT	:= 10;
const DELETE	:= 11;
const FIND	:= 12;
const REPLACE	:= 13;
const TOTOP	:= 14;
const MATCH	:= 15;
const REFRESH	:= 17;
const MENU	:= 18;
const RAW	:= 19;
const PUT	:= 20;
const BITBLT	:= 21;
const POSITION	:= 22;
const ZOOM	:= 23;
const RESO	:= 24;
const REPAINTED	:= 25;
const STACK	:= 26;
const FLIP	:= 27;

strout:prog(s: array of char, Mn: chan of char);
paint:prog(fr: Frame, bm: Bitmap, Ds: chan of Dcomm, how: int);
