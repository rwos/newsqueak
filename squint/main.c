#include <u.h>
#include <lib9.h>
#include "errjmp.h"
#include <bio.h>
#include "fns.h"

extern int	bconv(va_list *, Fconv*);
extern int	econv(va_list *, Fconv*);
extern int	iconv(va_list *, Fconv*);
extern int	mconv(va_list *, Fconv*);
extern int	nconv(va_list *, Fconv*);
extern int	tconv(va_list *, Fconv*);
extern int	Aconv(va_list *, Fconv*);
extern int	Cconv(va_list *, Fconv*);
extern int	Uconv(va_list *, Fconv*);
extern int	zconv(va_list *, Fconv*);

int		bflag;
int		cflag;
int		Cflag;
int		eflag;
int		iflag;
int		mflag;
int		pflag;
int		tflag;
int		xflag;
int		initializing;
extern long	Nproc;
int		interactive=0;
int		executeallowed=1;

Errjmp	err;

int
main(int argc, char *argv[])
{
	int i;
	char *s;
	while(argc>1 && argv[1][0]=='-'){
		s=argv[1]+1;
		while(*s)
			switch(*s++){
			case 'b':	/* suppress become flattening */
				bflag=1;
				break;
			case 'c':	/* print constants */
				cflag=1;
				break;
			case 'C':	/* suppress constant compilation */
				Cflag=1;
				break;
			case 'e':	/* dump core on errors */
				eflag=1;
				break;
			case 'i':	/* print compiled instructions */
				iflag=1;
				break;
			case 'm':	/* trace message passing */
				mflag=1;
				break;
			case 'P':	/* set number of procs */
				if(*s==0){
					--argc, argv++;
					s=argv[1];
					if(s==0)
						goto Usage;
				}
				Nproc=atol(s);
				if(Nproc<=0)
					goto Usage;
				goto Out;
			case 'p':	/* trace process creation */
				pflag++;
				break;
			case 't':	/* dump parse trees */
				tflag=1;
				break;
			case 'x':	/* trace execution */
				xflag=1;
				break;
			default:
			Usage:
				fprint(2, "usage: squint [-ixpb -PNPROC] <files>\n");
				return 1;
			}
		Out:
		--argc; argv++;
	}
	interactive=argc==1;
	procinit();
	fmtinstall('b', bconv);
	fmtinstall('e', econv);
	fmtinstall('m', mconv);
	fmtinstall('n', nconv);
	fmtinstall('t', tconv);
	fmtinstall('i', iconv);
	fmtinstall('A', Aconv);
	fmtinstall('C', Cconv);
	fmtinstall('U', Uconv);
	fmtinstall('z', zconv);
	fmtinstall('Z', zconv);
	lexinit();
	typeinit();
	initializing=1;
	if(errmark()){
		fprint(2, "squint: error during initialization; exiting\n");
		exits("initialization error");
	}
	if(argc==1)
		newfile("<stdin>", 1);
	for(i=argc-1; i>0; --i)
		newfile(argv[i], 0);
	initializing=0;
	errmark();
/*	Fflush(1); */
	do; while(yyparse());
	return 0;
}
