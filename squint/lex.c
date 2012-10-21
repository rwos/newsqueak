#include "store.h"
#include "node.h"
#include "symbol.h"
#include "ydefs.h"
#include <u.h>
#include <lib9.h>
#include <bio.h>

#include "fns.h"

extern int initializing;

enum{
	Nfile=10
};
typedef struct File{
	char		*name;
	unsigned char	*lbuf;
	int		line;
	int		fd;
	Biobuf	*b;
	struct File	*next;
}File;
File	*file;

unsigned char	*lbuf=(unsigned char *)"";
int		lastc;
int		peekc=-1;
int		line;
int		nfile;

int		getc(void);
void		ungetc(void);
int		follow(int, int, int);
int		ishex(int);
int		tohex(int);
int		backslash(int);
void		include(void);
void		newfile(char*, int);
Store		*mkarraychar(char *, int);

#define	INCLUDE	10000

struct{
	char	*word;
	int	val;
}keytab[]={
	{"array",	ARRAY},
	{"become",	BECOME},
	{"begin",	BEGIN},
	{"break",	BREAK},
	{"builtin",	BUILTIN},
	{"case",		CASE},
	{"cat",		CAT},
	{"chan",		CHAN},
	{"char",		CHAR},
	{"const",	CONST},
	{"continue",	CONTINUE},
	{"def",		DEF},
	{"default",	DEFAULT},
	{"del",		DEL},
	{"do",		DO},
	{"else",		ELSE},
	{"for",		FOR},
	{"if",		IF},
	{"include",	INCLUDE},
	{"int",		INT},
	{"len",		LEN},
	{"of",		OF},
	{"mk",		MK},
	{"print",	PRINT},
	{"prog",		PROG},
	{"rec",		REC},
	{"ref",		REF},
	{"result",	RESULT},
	{"select",	SELECT},
	{"sprint",	PRINT},
	{"struct",	STRUCT},
	{"switch",	SWITCH},
	{"type",		TYPE},
	{"unit",		UNIT},
	{"val",		VAL},
	{"while",	WHILE},
};
enum{
	Nkeytab=sizeof keytab/sizeof keytab[0]
};

void
lexinit(void){
	int i;
	for(i=0; i<Nkeytab; i++)
		lookup(keytab[i].word, keytab[i].val);
}

int
yylex(void){
	int c;
	long l;
	char *p;
	char buf[1024];
    loop:
	c=getc();
	if(c<0)
		return 0;
	if(c==' ' || c=='\t' || c=='\n')
		goto loop;
	if(c=='#'){
		do; while((c=getc())>=0 && c!='\n');
		goto loop;
	}
	if(c=='\''){
		l=backslash(getc());
		if(getc()!='\'')
			error("improper character constant");
		yylval.l=l;
		return NUM;
	}
	if(c=='"'){
		l=0;
		while((c=getc())!='"'){
			if(l>=sizeof buf-1)
				error("string too long");
			buf[l++]=backslash(c);
		}
		buf[l]=0;
		yylval.st=mkarraychar(buf, l);
		return STRING;
	}
	if('0'<=c && c<='9'){
		l=c-'0';
		if(l==0){
			c=getc();
			if(c=='x'){
				while((c=getc()) != 0){
					if(!ishex(c))
						break;
					l=16*l+tohex(c);
				}
			}else{
				ungetc();
				while('0'<=(c=getc()) && c<='7')
					l=8*l+c-'0';
			}
		}else
			while('0'<=(c=getc()) && c<='9')
				l=10*l+c-'0';
		ungetc();
		yylval.l=l;
		return NUM;
	}
	if(('a'<=c && c<='z') || ('A'<=c && c<='Z') || c=='_'){
		p=buf;
		l=0;
		do{
			if(++l<Namesize)
				*p++=c;
			c=getc();
		}while(('a'<=c && c<='z') || ('A'<=c && c<='Z')
		 ||    ('0'<=c && c<='9') || c=='_');
		ungetc();
		*p=0;
		yylval.s=lookup(buf, ID);
		if(yylval.s->token==INCLUDE){
			include();
			goto loop;
		}
		return yylval.s->token;
	}
	switch(c){
	case '+':	return follow('+', INC, '+');
	case '-':	return follow('-', DEC, '-');
	case '!':	return follow('=', NE, '!');
	case '&':	return follow('&', ANDAND, '&');
	case '|':	return follow('|', OROR, '|');
	case '=':	return follow('=', EQ, '=');
	case '>':	if(follow('>', 1, 0))
				return RSH;
			return follow('=', GE, '>'); 
	case '<':	if(follow('<', 1, 0))
				return LSH;
			if(follow('-', 1, 0))
				return ARROW;
			return follow('=', LE, '<'); 
	}
	if(c<=' ' || c>=0x7F)
		error("bad character 0x%x", c);
	return c;
}

int
follow(int c, int ifyes, int ifno)
{
	if(getc()==c)
		return ifyes;
	ungetc();
	return ifno;
}

int
backslash(int c)
{
	if(c!='\\'){
		if(c=='\n')
			error("newline in character or string constant");
		return c;
	}
	c=getc();
	if('0'<=c && c<='7'){
		int n;
		n=c-'0';
		c=getc();
		if(n==0 && (c=='x' || c=='X')){
			c=getc();
			if(!ishex(c)){
				ungetc();
				return 0;
			}
			n=tohex(c);
			c=getc();
			if(!ishex(c)){
				ungetc();
				return n;
			}
			n=16*n+tohex(c);
			return n;
		}
		if(c<'0' || '7'<c){
			ungetc();
			return n;
		}
		n=8*n+c-'0';
		c=getc();
		if(c<'0' || '7'<c){
			ungetc();
			return n;
		}
		n=8*n+c-'0';
		return n;
	}
	if(c=='b')
		return '\b';
	if(c=='f')
		return '\f';
	if(c=='n')
		return '\n';
	if(c=='r')
		return '\r';
	if(c=='t')
		return '\t';
	if(c=='\n')
		return backslash(getc());
	return c;
}

int
ishex(int c)
{
	return ('0'<=c && c<='9') || ('a'<=c && c<='f') || ('A'<=c && c<='F');
}

int
tohex(int c)
{
	if('0'<=c && c<='9')
		return c-'0';
	if('a'<=c && c<='f')
		return 10+c-'a';
	return 10+c-'A';
}

int
getc(void){
	if(peekc>=0){
		lastc=peekc;
		peekc=-1;
		return lastc;
	}
	if(lbuf==0){
		File *f;
		if(file->next==0)
			return lastc=-1;
		free(file->name);
		Bterm(file->b);
		free(file->b);
		close(file->fd);
		f=file->next;
		free((char *)file);
		file=f;
		line=file->line;
		lbuf=file->lbuf;
		if(lbuf==0)
			lbuf=(unsigned char *)"";
		--nfile;
	}
	if(*lbuf==0){
		file->line++;
		line=file->line;
		if(file->lbuf){
			free((char *)file->lbuf);
			file->lbuf=0;
		}
		lbuf=(unsigned char *)Brdline(file->b, '\n');
		if(lbuf)
			lbuf[BLINELEN(file->b)-1] = 0;
		return lastc='\n';
	}
	return lastc=*lbuf++;
}

void
ungetc(void){
	if(peekc>=0)
		panic("ungetc");
	peekc=lastc;
}

void
include(void){
	char *s, *f;
	s=(char *)lbuf;
	while(*s==' ' || *s=='\t')
		s++;
	if(*s++!='"')
		error("include syntax");
	f=s;
	while(*s!='"')
		if(*s++==0)
			error("include syntax");
	if(s==f)
		error("include syntax");
	*s=0;
	lbuf=(unsigned char *)s+1;
	if(++nfile>Nfile)
		error("includes nested too deeply");
	file->lbuf=emalloc(strlen((char *)lbuf)+1);
	strcpy((char *)file->lbuf, (char *)lbuf);
	newfile(f, 0);
}

void
newfile(char *s, int stdin)
{
	File *fp;
	int fd;
	if(stdin)
		fd=0;
	else{
		char buf[1024];
		fd=open(s, 0);
		if(fd<0 && s[0]!='/' && s[0]!='.'){
			sprint(buf, PREFIX "/lib/newsqueak/include/%s", s);
			fd=open(buf, 0);
		}
		if(fd<0)
			error("can't open %s (%s)\n", s, buf);
	}
	fp=alloc(File);
	fp->lbuf=0;
	fp->name=emalloc(strlen(s)+1);
	strcpy(fp->name, s);
	fp->fd=fd;
	fp->b = emalloc(sizeof(Biobuf));
	Binit(fp->b, fd, OREAD);
	fp->line=0;
	line=0;
	lbuf=(unsigned char *)"";
	fp->next=file;
	file=fp;
}

char *
printfileline(char *buf, File *f, int l, int top)
{
	*buf=0;
	if(f==0 || (!top && strcmp(f->name, "<stdin>")==0))
		return buf;
	if(f->next)
		buf=printfileline(buf, f->next, f->next->line, 0);
	return buf+sprint(buf, "%s:%d: ", f->name, l);
}

int
zconv(va_list *va, Fconv *f)
{
	int o;
	char buf[4096];

	SET(o);
	if(f->chr == 'Z')
		o = va_arg(*va, int);
	if(initializing)
		strcpy(buf, "squint: ");
	else{
		if(f->chr == 'z')
			printfileline(buf, file, file->line, 1);
		else
			printfileline(buf, file, o, 1);
	}
	strconv(buf, f);
	if(f->chr == 'z')
		return 0;
	return sizeof(int);
}

char *
filename(void){
	return file->name;
};

void
errflush(void)
{
	if(!initializing)
		while(lastc>0 && lastc!='}' && lastc!=';')
			getc();
}
