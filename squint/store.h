typedef	long	SWord;
typedef struct	Proc Proc;
#define	WS	(sizeof(SWord))

typedef struct Store Store;
struct Store
{
	short	ref;
	unsigned char	type;
	unsigned char	sbits;		/* SWords of structure type bits */
	long	len;
	SWord	data[1];
};
enum	/* Type */
{
	Saryint=	0,	/* hdr, ints */
	Saryptr=	1,	/* hdr, pointers */
	Sarychar=	2,	/* hdr, bytes */
	Sstruct=	3,	/* hdr, words of pointer bits, ints&pointers */
	Schan=		4,	/* hdr, Pqueue sndq, Pqueue rcvq */
	Sprog=		5,	/* hdr, ptr to Saryptr, function pointers */
	Sdata=		6,	/* hdr, pointer to data */
	Sbitmapid=	7	/* hdr, bitmap id */
};
enum
{
	BPW=32
};
#define	SHSZ	(unsigned long)(sizeof(Store)-WS)

void	decref(Store **);
Store	*mk(int, int);
Store	*mkarraychar(char *, int);
void	emitstore(Store *);
SWord	topofstack(void);
