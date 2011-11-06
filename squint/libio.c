#include "store.h"
#include "comm.h"
#include "libargs.h"
#include <u.h>
#include <lib9.h>
#include <bio.h>

#include "fns.h"

enum{
	Niobuf=128
};

Biobuf	*iobuf[Niobuf];

int
bi_read(Proc *proc)
{
	int fd;
	char *s;
	Biobuf *b;
	fd=LARG(proc, 1);
	SET(b);
	if(fd<0 || fd>=Niobuf || (b=iobuf[fd])==0)
		rerror("read from non-open file");
	s=Brdline(b, '\n');
	if(s)
		RETVAL(proc, 1)=(long)mkarraychar(s, BLINELEN(b));
	else
		RETVAL(proc, 1)=(long)mkarraychar("", 0);
	return 1;
}

int
bi_bread(Proc *proc)
{
	int fd;
	int n, an;
	char *buf;
	Biobuf *b;
	fd=LARG(proc, 1);
	an=LARG(proc, 2);
	SET(b);
	if(fd<0 || fd>=Niobuf || (b=iobuf[fd])==0)
		rerror("bread from non-open file");
	buf=emalloc(an);
	n=Bread(b, buf, an);
	if(n<=0){
		free(buf);
		RETVAL(proc, 2)=(long)mkarraychar("", 0);
	}else
		RETVAL(proc, 2)=(long)mkarraychar(buf, n);
	free(buf);
	return 1;
}

int
bi_write(Proc *proc)
{
	int fd;
	long n;
	Biobuf *b;
	fd=LARG(proc, 1);
	SET(b);
	if(fd<0 || fd>=Niobuf || (b=iobuf[fd])==0)
		rerror("write to non-open file");
	n=Bwrite(b, STRARG(proc, 2), SARG(proc, 2)->len);
	DECREF(proc, 2);
	RETVAL(proc, 2)=n;
	return 1;
}

int
bi_open(Proc *proc)
{
	int fd, m;
	char *s;
	s=STRARG(proc, 1);
	m=LARG(proc, 2);
	if(m==0)
		fd=open(s, 0);
	else if(m==1)
		fd=create(s, 0666, 1);
	else{
		fd=0;	/* set */
		rerror("unknown open mode %d", m);
	}
	if(fd<0)
		rerror("can't open file %s", s);
	iobuf[fd]=alloc(Biobuf);
	Binit(iobuf[fd], fd, m);
	DECREF(proc, 1);
	RETVAL(proc, 2)=fd;
	return 1;
}

int
bi_close(Proc *proc)
{
	int fd;
	Biobuf *b;
	fd=LARG(proc, 1);
	SET(b);
	if(fd<0 || fd>=Niobuf || (b=iobuf[fd])==0)
		rerror("file %d is not open", fd);
	Bterm(b);
	close(fd);
	free(b);
	iobuf[fd] = 0;
	return 1;
}
