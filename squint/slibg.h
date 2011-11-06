/* graphics and multiplexed i/o */
#ifdef	FNS
extern	int bi_getscreen(Proc *);
extern	int bi_bitblt(Proc *);
extern	int bi_string(Proc *);
extern	int bi_strsize(Proc *);
extern	int bi_readbitmap(Proc *);
extern	int bi_mkbitmap(Proc *);
extern	int bi_cursorswitch(Proc *);
extern	int bi_ptinrect(Proc *);
extern	int bi_rectinset(Proc *);
extern	int bi_border(Proc *);
extern	int bi_texture(Proc *);
extern	int bi_nsnew(Proc *);
extern	int bi_nsinstall(Proc *);
extern	int bi_nsuninstall(Proc *);
extern	int bi_nslookup(Proc *);
extern	int bi_nsproc(Proc *);
extern	int bi_nsread(Proc *);
extern	int bi_nswrite(Proc *);
extern	int bi_nsreadctl(Proc *);
extern	int bi_nswritectl(Proc *);
extern	int bi_BtoMouse(Proc *);
extern	int bi_MousetoB(Proc *);
#else
	"abort",	{bi_abort,	0,		0},	1,	call1,
	"getscreen",	{bi_getscreen,	0,		0},	0,	call0,
	"bitblt",	{bi_bitblt,	0,		0},	5,	call5,
	"string",	{bi_string,	0,		0},	5,	call5,
	"strsize",	{bi_strsize,	0,		0},	2,	call2,
	"readbitmap",	{bi_readbitmap,	0,		0},	1,	call1,
	"mkbitmap",	{bi_mkbitmap,	0,		0},	2,	call2,
	"cursorswitch",	{bi_cursorswitch,0,		0},	1,	call1,
	"ptinrect",	{bi_ptinrect,	0,		0},	2,	call2,
	"rectinset",	{bi_rectinset,	0,		0},	3,	call3,
	"border",	{bi_border,	0,		0},	4,	call4,
	"texture",	{bi_texture,	0,		0},	4,	call4,
	"nsnew",	{bi_nsnew,	0,		0},	0,	call0,
	"nsinstall",	{bi_nsinstall,	0,		0},	3,	call3,
	"nsuninstall",	{bi_nsuninstall,0,		0},	2,	call2,
	"nslookup",	{bi_nslookup,	0,		0},	2,	call2,
	"nsproc",	{bi_nsproc,	0,		0},	2,	call2,
	"nsread",	{bi_nsread,	0,		0},	4,	call4,
	"nswrite",	{bi_nswrite,	0,		0},	3,	call3,
	"nsreadctl",	{bi_nsreadctl,	0,		0},	4,	call4,
	"nswritectl",	{bi_nswritectl,	0,		0},	3,	call3,
	"BtoMouse",	{bi_BtoMouse,	0,		0},	1,	call1,
	"MousetoB",	{bi_MousetoB,	0,		0},	1,	call1,
#endif
