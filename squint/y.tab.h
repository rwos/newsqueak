
typedef union {
	Node	*n;
	Symbol	*s;
	Store	*st;
	long	l;
	int	i;
}	YYSTYPE;
extern	YYSTYPE	yylval;
#define	ID	57346
#define	NUM	57347
#define	STRING	57348
#define	UNIT	57349
#define	INT	57350
#define	CHAR	57351
#define	CONST	57352
#define	BUILTIN	57353
#define	ARRAY	57354
#define	OF	57355
#define	CHAN	57356
#define	STRUCT	57357
#define	PROG	57358
#define	TYPE	57359
#define	IF	57360
#define	ELSE	57361
#define	FOR	57362
#define	WHILE	57363
#define	DO	57364
#define	SELECT	57365
#define	SWITCH	57366
#define	CASE	57367
#define	DEFAULT	57368
#define	PRINT	57369
#define	REC	57370
#define	BEGIN	57371
#define	BECOME	57372
#define	RESULT	57373
#define	MK	57374
#define	LEN	57375
#define	REF	57376
#define	DEF	57377
#define	BREAK	57378
#define	CONTINUE	57379
#define	VAL	57380
#define	ANDAND	57381
#define	OROR	57382
#define	CAT	57383
#define	DEL	57384
#define	EQ	57385
#define	NE	57386
#define	LE	57387
#define	GE	57388
#define	LSH	57389
#define	RSH	57390
#define	UMINUS	57391
#define	ARROW	57392
#define	SND	57393
#define	RCV	57394
#define	DEC	57395
#define	INC	57396
