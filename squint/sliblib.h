#ifdef	FNS
extern	int bi_abort(Proc *);
extern	int bi_suspend(Proc *);
extern	int bi_rand(Proc *);
extern	int bi_strchr(Proc *);
#else
	{"abort",	{bi_abort,	0,	0},	1,	call1},
	{"suspend",	{bi_suspend,	0,	0},	1,	call1},
	{"rand",	{bi_rand,	0,	0},	0,	call0},
	{"strchr",	{bi_strchr,	0,	0},	2,	call2},
#endif
