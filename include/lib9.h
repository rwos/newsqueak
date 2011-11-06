	/* Plan 9 C library interface */

typedef unsigned short Rune;

#define	dup(a,b)			dup2(a,b)
#define	seek(a,b,c)			lseek(a,b,c)
#define	create(name, mode, perm)	creat(name, perm)
#define	exec(a,b)			execv(a,b)
#define	USED(a)
#define SET(a)

#define nil ((void*)0)

#define	_exits(v)			_exit(v!=nil)
#define	exits(v)			exit(v!=nil)

int getopt(int argc, char * const argv[], const char *optstring);
extern char *optarg;
extern int optind, opterr, optopt;

#define ARGBEGIN(s)			{int opt; while ((opt = getopt(argc, argv, s)) != EOF) switch (opt)
#define ARGEND				}
#define ARGF				optarg

#define nelem(a)			(sizeof(a)/sizeof((a)[0]))

#define	OREAD	0	/* open for read */
#define	OWRITE	1	/* write */
#define	ORDWR	2	/* read and write */
#define	OEXEC	3	/* execute, == read but check execute permission */
#define	OTRUNC	16	/* or'ed in (except for exec), truncate file first */
#define	OCEXEC	32	/* or'ed in, close on exec */
#define	ORCLOSE	64	/* or'ed in, remove on close */
#define	OEXCL	0x1000	/* or'ed in, exclusive use */
#define AEXEC	0x1
#define AWRITE	0x2
#define AREAD	0x4
#define ERRLEN	64
#define NAMELEN	28
#define CHDIR	0x80000000

typedef struct Dir Dir;

enum
{
	UTFmax		= 3,		/* maximum bytes per rune */
	Runesync	= 0x80,		/* cannot represent part of a utf sequence (<) */
	Runeself	= 0x80,		/* rune and utf sequences are the same (<) */
	Runeerror	= 0x80		/* decoding error in utf */
};

/*
 * new rune routines
 */
int	runetochar(char*, const Rune*);
int	chartorune(Rune*, const char*);
int	runelen(long);
int	fullrune(char*, int);

/*
 * rune routines from converted str routines
 */
int	utflen(char*);		/* was countrune */
char*	utfrune(char*, long);
char*	utfrrune(char*, long);
char*	utfutf(char*, char*);
/*
 *	Miscellaneous functions
 */
int	notify (void(*)(void *, char *));
int	errstr(char *);
char*	getuser(void);

/* Plan9 library calls reimplemented in libc.a */

/* doprint.c */
typedef	struct	Fconv	Fconv;
struct	Fconv
{
	char*	out;		/* pointer to next output */
	char*	eout;		/* pointer to end */
	int	f1;
	int	f2;
	int	f3;
	int	chr;
};
char*	doprint(char *s, char *es, const char *fmt, va_list argp);
void	strconv(char *s, Fconv *fp);

double	charstod(int (*f)(void *), void *a);
/* print.c */

int	print(const char *fmt, ...);
int	fprint(int f, const char *fmt, ...);
int	sprint(char *buf, const char *fmt, ...);
int	snprint(char *buf, int len, const char *fmt, ...);
char*	seprint(char *buf, char *e, const char *fmt, ...);
#define pow10(n) (pow(10.0, (double)(n)))
extern	int	fmtinstall(int, int (*)(va_list*, Fconv*));

/* nan.c */
double	NaN(void);
int	isNaN(double d);
double	Inf(int sign);
int	isInf(double d, int sign);

#define	ERRLEN	64
#define wait(x) __plan9wait(x)
#define nbwait(x) __plan9nbwait(x)

/* getcallerpc.c */

ulong	getcallerpc(void *firstarg);

/* wait.h */

typedef
struct Waitmsg {
	char	pid[12];         /* of loved one */
	char	time[3*12];      /* of loved one & descendants */
	char	msg[ERRLEN];
} Waitmsg;			/* Plan 9 wait */
int	wait(Waitmsg *w);	/* Plan 9 wait */
int	nbwait(Waitmsg *w);	/* Plan 9 wait */
