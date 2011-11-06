typedef struct Errbuf{
	jmp_buf	x;
}Errjmp;
extern	Errjmp	err;

#define	errjmp()	longjmp(err.x, 1)
#define	errmark()	setjmp(err.x)
#define	errsave(s)	s=err
#define	errrest(s)	err=s
