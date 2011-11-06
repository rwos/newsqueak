/* graphics and multiplexed i/o */
#ifdef	FNS
extern	int bi_mkcover(Proc*);
extern	int bi_lalloc(Proc*);
extern	int bi_ltofront(Proc*);
extern	int bi_ltoback(Proc*);
extern	int bi_lfree(Proc*);
#else
	"mkcover",	{bi_mkcover,	0,		0},	2,	call2,
	"lalloc",	{bi_lalloc,	0,		0},	2,	call2,
	"ltofront",	{bi_ltofront,	0,		0},	2,	call2,
	"ltoback",	{bi_ltoback,	0,		0},	2,	call2,
	"lfree",	{bi_lfree,	0,		0},	2,	call2,
#endif
