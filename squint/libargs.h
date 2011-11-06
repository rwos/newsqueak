#define	SARG(proc, n)		((Store *)proc->fp[-4-(n)+1])	/* args are 1-indexed */
#define	IARG(proc, n)		((int)proc->fp[-4-(n)+1])
#define	LARG(proc, n)		proc->fp[-4-(n)+1]
#define	DECREF(proc, n)		decref((Store **)&LARG(proc, n))
#define	RETVAL(proc, nargs)	proc->fp[-4-nargs]
#define	STRARG(proc, n)		((char *)SARG(proc, n)->data)

extern	Store	*mk(int type, int len);
extern	Store	*mkarraychar(char *s, int len);
