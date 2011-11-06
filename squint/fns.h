void	*emalloc(unsigned long);
void	emit(int i);
void	emitconst(long);
void	emitspace(long);
void	*erealloc(void *, unsigned long);
void	errflush(void);
void	execute(void);
void	executeinhibit(void);
int	fileline(void);
int	here(void);
void	idump(void);
void	istart(void);
int	istopscope(void);
void	lexinit(void);
void	libgflush(void);
void	libgbfree(int);
int	lfileline(int);
int	nargs(char *, char []);
void	newfile(char *, int);
int	nrand(int n);
void	patch(int, long);
void	popscope(void);
void	processes(int);
void	procinit(void);
void	pushscope(void);
void	scopedecrefgen(void);
void	setprog(long);
long	topofstack(void);
void	topscope(void);
void	typeinit(void);
int	yylex(void);
int	yyparse(void);

void	error(char *, ...);
void	panic(char *, ...);
void	rerror(char *, ...);
void	rpanic(char *, ...);
void	warn(char *, ...);

extern	int	interactive;	/* if interactive, allow execution after error */
extern	int	executeallowed;
#define	alloc(t)	emalloc(sizeof(t))

/*
  #pragma varargck argpos error	1
  #pragma varargck argpos warn	1
  #pragma varargck argpos panic	1
  #pragma varargck argpos rpanic	1
  #pragma varargck argpos rerror	1
  #pragma varargck argpos lerror	2
  #pragma	varargck	type "A"	Store*
  #pragma	varargck	type	"b"	int
  #pragma	varargck	type "C"	Store*
  #pragma	varargck	type	"e"	Node*
  #pragma	varargck	type	"i"	void*
  #pragma	varargck	type	"m"	Node*
  #pragma	varargck	type	"n"	Node*
  #pragma	varargck	type	"t"	Node*
  #pragma	varargck	type	"U"	void
  #pragma	varargck	type	"z"	void
  #pragma	varargck	type	"Z"	int
*/

/* defined away; no fmount these days */
#define	moreio() 0
#define	schedio(x)
#define	fmountinit()
#define	libgflush()
#define	libgbfree(x)
#define	drain(x)
