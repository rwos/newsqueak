#ifdef	FNS
extern	int bi_read(Proc*);
extern	int bi_bread(Proc*);
extern	int bi_write(Proc*);
extern	int bi_open(Proc*);
extern	int bi_close(Proc*);
#else
	{"read",	{bi_read,	0,	0},	1,	call1},
	{"bread",	{bi_bread,	0,	0},	2,	call2},
	{"write",	{bi_write,	0,	0},	2,	call2},
	{"open",	{bi_open,	0,	0},	2,	call2},
	{"close",	{bi_close,	0,	0},	1,	call1},
#endif
