#define	MNTDIR	"/tmp"
#define	DEBUG	0
#include "store.h"
#include "comm.h"
#include <u.h>
#include <lib9.h>
#include "space.h"
#include "libargs.h"
#include <libg.h>
#include <auth.h>
#include <fcall.h>

#include "fns.h"
#include "slibgfns.h"

enum{
	Nfid=		512,
	Nslot=		512,
};
typedef struct Mfile Mfile;
struct Mfile{
	int	slot;
	short	busy;
	short	open;
	Bind	*bind;
};

Mfile	*mfile;
int	nmfile;
int	ref[Nslot];
int	fmounted;
int	fmountactive;
int	sfd;
int	cfd;
char	user[NAMELEN];
int	mypid;
char	*fmdata;
Pend	*freepend;
short	spaceofslot[Nslot];
Fcall	rhdr;
Fcall	thdr;
void	rflush(Mfile*);
void	rattach(Mfile*);
void	rclone(Mfile*);
void	rwalk(Mfile*);
void	rclwalk(Mfile*);
void	ropen(Mfile*);
void	rcreate(Mfile*);
void	rread(Mfile*);
void	rwrite(Mfile*);
void	rclunk(Mfile*);
void	rremove(Mfile*);
void	rstat(Mfile*);
void	rwstat(Mfile*);
void	sendmsg(int);
void	mountinit(char*);

void	rdpend(Mfile *, int, int, int);
void	wrpend(Mfile *, int, int, int, char *);
void	wrdrain(Queue *, int, int);
void	rddrain(Queue *, int, int);
int	fork2(void);
Store	*mkmouse(int, int, int);

enum
{
	Enone,
	Emtabfull,
	Emntclosed,
	Enonexist,
	Eperm,
	Eio,
	Eisdir,
	Espace,
	Egreg,
};

char	*errs[]=
{
	[Enone]		"squint: no error",
	[Emtabfull]	"squint: slot table full",
	[Emntclosed]	"squint: mount shut down",
	[Enonexist]	"squint: file does not exist",
	[Eperm]		"squint: permission denied",
	[Eio]		"squint: i/o error (alignment)",
	[Eisdir]	"squint: is a directory",
	[Espace]	"squint: bad name space",
	[Egreg]		"squint: all greg's fault",
};

moreio(void){
	return fmountactive;
}

int
fmready(void)
{
	Dir d;
	if(dirfstat(sfd, &d)<0)
		return 0;
	if(d.length>0)
		return 1;
	return 0;
}

void
newfid(int fid)
{
	if(nmfile <= fid){
		nmfile = fid + 100;
		mfile = erealloc(mfile, nmfile*sizeof mfile[0]);
	}
}

void
schedio(int block)
{
	long n;
	Mfile *mf;

	if(!fmountactive)
		return;
	if(!block && !fmready())
		return;
	libgflush();
	n=read(sfd, fmdata, MAXMSG+MAXFDATA);
	if(n<=0){
		if(fmounted==0)
			rerror("nsinit() not called");
		fmountactive=0;
		rerror("fmount read");
	}
	if(convM2S(fmdata, &rhdr, n) == 0)
		error("format error");
	if(rhdr.fid<0)
		rerror("fid %d out of range", rhdr.fid);
	newfid(rhdr.fid);
	mf = &mfile[rhdr.fid];

	if(DEBUG)
		fprint(2, "recv: %F\n", &rhdr);
	switch(rhdr.type){
	default:
		rerror("fmount unknown: %F", &rhdr);
		break;
	case Tflush:
		rflush(mf);
		break;
	case Tattach:
		rattach(mf);
		break;
	case Tclone:
		rclone(mf);
		break;
	case Twalk:
		rwalk(mf);
		break;
	case Topen:
		ropen(mf);
		break;
	case Tcreate:
		rcreate(mf);
		break;
	case Tread:
		rread(mf);
		break;
	case Twrite:
		rwrite(mf);
		break;
	case Tclunk:
		rclunk(mf);
		break;
	case Tremove:
		rremove(mf);
		break;
	case Tstat:
		rstat(mf);
		break;
	case Twstat:
		rwstat(mf);
		break;
	}
}

void
fmountinit(void)
{
	int i, p[2];

	if(fmounted)
		return;
	fmounted++;
	if(pipe(p) < 0)
		error("pipe failed");
	sfd = p[0];
	cfd = p[1];
	fmdata = emalloc(MAXMSG+MAXFDATA);
	i = open("/dev/user", 0);
	if(i<0 || read(i, user, sizeof user)<=0)
		rerror("can't read /dev/user");
	mypid = getpid();
	close(i);
	fmtinstall('F', fcallconv);
}

fmountproc(int ns, char *argv[])
{
	int i;
	char buf[NAMELEN];
	tospace(ns);	/* just to verify it */
	sprint(buf, "%d.%d", mypid, ns);
	switch(rfork(RFFDG|RFPROC|RFNOWAIT)){
	case -1:
		return -1;
	case 0:
		break;
	default:
		fmountactive=1;
		return 1;
	}
	/* child here */
	for(i=3; i<20; i++)
		if(i != cfd)
			close(i);
	if(rfork(RFNAMEG)<0){
		fprint(2, "newpgrp failed:");
		goto die;
	}
	if(mount(cfd, MNTDIR, MREPL|MCREATE, buf)<0){
		fprint(2, "child mount /mnt/ns failed:");
		goto die;
	}
	exec(argv[0], argv);
	fprint(2, "exec %s failed:", argv[0]);
    die:
	perror("exec");
	_exits((char *)0);
	return 0;
}

void
rflush(Mfile *mf)
{
	USED(mf);
	rerror("rflush");	/* can't handle just now */
	sendmsg(0);
}

void
rattach(Mfile *mf)
{
	int s, sp, err;
	char *name;

	err=0;
	for(s=0; ref[s]; s++)
		if(s==Nslot){
			err=Emtabfull;
			goto send;
		}
	name = rhdr.aname;
	if(atoi(name)!=mypid){
		err = Eperm;
		goto send;
	}
	do; while(*name && *name++!='.');
	sp=atoi(name);
	if(sp<1 || !ckspace(sp)){
		err=Espace;
		goto send;
	}
	spaceofslot[s]=sp;
	mf->bind=spacelookup(sp, "ns");
	if(mf->bind == 0)
		mf->bind=spaceinstallb(sp, "ns", CHDIR|0777);
	mf->bind->nclient++;
	ref[mf->slot]--;
	mf->slot=s;
	ref[s]=1;
	mf->busy=1;
   send:
	thdr.qid.path = mf->bind->qid;
	thdr.qid.vers = 0;
	sendmsg(err);
}

void
rclone(Mfile *mf)
{
	int nfid=rhdr.newfid;
	Mfile *nmf;
	int err=0;

	USED(mf);
	if(nfid<0)
		rerror("clone nfid out of range");
	newfid(nfid);
	mf = &mfile[rhdr.fid];	/* in case of reallocation */
	nmf = &mfile[nfid];
	if(nmf->busy)
		rerror("clone to used channel");
	if(mf->bind->removed){
		err=Emntclosed;
		goto send;
	}
	*nmf=*mf;
	/* some ref++, space? bind? ..................*/
	ref[mf->slot]++;
	mf->bind->nclient++;
	if(mf->open)
		panic("rclone already open\n");
    send:
	sendmsg(err);
}

void
rwalk(Mfile *mf)
{
	int err;
	long qid;
	char *name=rhdr.name;
	Bind *bp;

	err=0;
	qid=0;
	if(mf->bind->removed){
		err=Emntclosed;
		goto send;
	}
	bp=spacelookup(spaceofslot[mf->slot], (char *)name);
	if(bp==0)
		err=Enonexist;
	else if(bp->removed)
		err=Emntclosed;
	else{
		if(mf->open)
			panic("rwalk");
		mf->bind->nclient--;
		mf->bind=bp;
		qid=bp->qid;
		bp->nclient++;
	}
    send:
	thdr.qid.path = qid;
	thdr.qid.vers = 0;
	sendmsg(err);
}

void
ropen(Mfile *mf)
{
	int err, mode;
	Bind *bp=mf->bind;
	long qid=bp->qid;
	long perm=bp->perm;

	err=0;
	if(mf->bind->removed){
		err=Emntclosed;
		goto send;
	}
	mode=rhdr.mode&15;
	if(qid&CHDIR){
		if(mode!=0)
			err=Eperm;
	}else{
		if(mode<0 || mode>2){
			err=Eperm;
			goto send;
		}
		if(((mode==0 || mode==2) && !(perm&0400))
		|| ((mode==1 || mode==2) && !(perm&0200))){
			err=Eperm;
			goto send;
		}
	}
	if(bp->nopen++==0){
		wrpend(mf, WCtl, 4, 0, "open");
		if(bp->q[WCtl].proc)
			wrdrain(&bp->q[WCtl], 1, 1);
	}
	mf->open=1;
    send:
	thdr.qid.path = qid;
	thdr.qid.vers = 0;
	sendmsg(err);
}

void
rcreate(Mfile *mf)
{
	USED(mf);
	sendmsg(Eperm);
}

void
rread(Mfile *mf)
{
	int n, err, cnt;
	long off, clock;
	Dir dir;
	Bind *bp;
	char buf[MAXFDATA];

	n=0;
	err=0;
	if(mf->bind->removed){
		err=Emntclosed;
		goto send;
	}
	off=rhdr.offset;
	cnt=rhdr.count;
	if(mf->bind->qid&CHDIR){
		if(off%sizeof(Dir) || cnt%sizeof(Dir)){
			err=Eio;
			goto send;
		}
		bp=tospace(spaceofslot[mf->slot])->bind;
		for(; off; off-=sizeof(Dir), bp=bp->next)
			if(bp==0)
				goto send;
		clock=time(0);
		for(; bp && cnt; cnt-=DIRLEN, n+=DIRLEN, bp=bp->next){
			memcpy(dir.name, bp->name, NAMELEN);
			dir.qid.path = bp->qid;
			dir.qid.vers = 0;
			dir.mode = bp->perm;
			dir.length=0;
			memcpy(dir.uid, user, NAMELEN);
			memcpy(dir.gid, user, NAMELEN);
			dir.atime = clock;
			dir.mtime = clock;
			convD2M(&dir, buf+n);
		}
		thdr.data = buf;
    send:
		thdr.count = n;
		sendmsg(err);
		return;
	}
	rdpend(mf, RData, cnt, rhdr.tag);
	bp=mf->bind;
	if(bp->q[RData].proc)
		rddrain(&bp->q[RData], 1, 0);
}

void
rwrite(Mfile *mf)
{
	int n, err, cnt;

	Bind *bp;
	n=0;
	if(mf->bind->removed){
		err=Emntclosed;
		goto send;
	}
	cnt=rhdr.count;
	if(mf->bind->qid&CHDIR){
		err=Eisdir;
    send:
		thdr.count = n;
		sendmsg(err);
		return;
	}
	wrpend(mf, WData, cnt, rhdr.tag, rhdr.data);
	bp=mf->bind;
	if(bp->q[WData].proc)
		wrdrain(&bp->q[WData], 1, 0);
}

void
rclunk(Mfile *mf)
{
	Bind *bp=mf->bind;

	if(mf->open)
		if(--bp->nopen==0){
			wrpend(mf, WCtl, 5, 0, "close");
			if(bp->q[WCtl].proc)
				wrdrain(&bp->q[WCtl], 1, 1);
		}
	mf->open=0;
	mf->busy=0;
	bp->nclient--;
	if(bp->nclient==0 && bp->removed)
		binduninstallb(bp);
	ref[mf->slot]--;
    send:
	sendmsg(0);
}

void
rremove(Mfile *mf)
{
	USED(mf);
	sendmsg(Eperm);
}

void
rstat(Mfile *mf)
{
	Dir dir;
	long clock;
	Bind *bp=mf->bind;
	int err;

	err = 0;
	if(mf->bind->removed){
		err = Emntclosed;
		goto send;
	}
	clock = time(0);
	memcpy(dir.name, bp->name, NAMELEN);
	dir.qid.path = bp->qid;
	dir.qid.vers = 0;
	dir.mode = bp->perm;
	dir.length=0;
	memcpy(dir.uid, user, NAMELEN);
	memcpy(dir.gid, user, NAMELEN);
	dir.atime = clock;
	dir.mtime = clock;
	convD2M(&dir, thdr.stat);
    send:
	sendmsg(err);
}

void
rwstat(Mfile *mf)
{
	USED(mf);
	sendmsg(Eperm);
}

long
getclock(void)
{
	static int clockfd=0;
	long x;
	if(clockfd==0)
		clockfd=open("/dev/clock", 0);
	if(clockfd<0 || read(clockfd, (char *)&x, sizeof x)!=sizeof x)
		return 0;
	return x;
}

void
sendmsg(int err)
{
	int n;

	if(err){
		thdr.type = Rerror;
		strncpy(thdr.ename, errs[err], ERRLEN);
	}else{
		thdr.type = rhdr.type+1;
		thdr.fid = rhdr.fid;
	}
	thdr.tag = rhdr.tag;
	n = convS2M(&thdr, fmdata);
	if(DEBUG)
		fprint(2, "repl: %F\n", &thdr);
	if(write(sfd, fmdata, n)!=n)
		error("mount write");
}

/*
 * Linkage to newsqueak
 */

void
rdpend(Mfile *mf, int n, int cnt, int tag){
	Pend *p, *fp;
	Queue *q=&mf->bind->q[n];
	if(freepend==0){
		freepend=alloc(Pend);
		memset(freepend, 0, sizeof(Pend));
	}
	fp=freepend;
	freepend=fp->next;
	fp->next=0;
	p=q->pend;
	if(p==0)
		q->pend=fp;
	else{
		while(p->next)
			p=p->next;
		p->next=fp;
	}
	fp->fid=mf-mfile;
	fp->cnt=cnt;
	fp->tag = tag;
}

void
rddrain(Queue *q, int dorun, int isctl)	/* there is known to be a proc */
{
	Store *s=q->buf;
	int n, cnt;
	Pend *p;
	if(isctl)	/* this is a control channel */
		panic("rddrain");
    loop:
	p=q->pend;
	if(p==0)
		return;
	if(q->synch){
		RETVAL(q->proc, 4)=0;
		goto out;
	}
	cnt=p->cnt;
	n=s->len-q->off;
	if(n>cnt)
		n=cnt;
	thdr.data = (char *)s->data+q->off;
	thdr.type = Tread+1;
	thdr.fid = p->fid;
	thdr.tag = p->tag;
	thdr.count = n;
	sendmsg(0);
	q->pend=p->next;
	p->next=freepend;
	freepend=p;
	q->off+=n;
	if(q->off<s->len)
		goto loop;
	RETVAL(q->proc, 4)=n;
    out:
	if(dorun)
		erun(q->proc);
	DECREF(q->proc, 3);
	q->proc=0;
}

int
fmread(Proc *proc, Bind *bp, int n, Store *s, int synch)
{
	Queue *q=&bp->q[n];
	if(n<0 || n>=NQ)
		rerror("fmread");
if(n==RCtl) panic("fmread ctl");
	q->proc=proc;
	q->buf=s;
	q->off=0;
	q->synch=synch;
	rddrain(q, 0, 0);
	if(q->proc)		/* not finished yet */
		return 0;
	return 1;
}

int
fmwrite(Proc *proc, Bind *bp, int n, int synch)
{
	Queue *q=&bp->q[n];
	if(n<0 || n>=NQ)
		panic("fmwrite");
	q->proc=proc;
	q->synch=synch;
	wrdrain(q, 0, n==WCtl);
	if(q->proc)		/* not finished yet */
		return 0;
	return 1;
}

void
wrpend(Mfile *mf, int n, int cnt, int tag, char *s)	/* there is known to be a proc */
{
	Pend *p, *fp;
	Queue *q=&mf->bind->q[n];
	if(freepend==0){
		freepend=alloc(Pend);
		memset(freepend, 0, sizeof(Pend));
	}
	fp=freepend;
	freepend=fp->next;
	fp->next=0;
	p=q->pend;
	if(p==0)
		q->pend=fp;
	else{
		while(p->next)
			p=p->next;
		p->next=fp;
	}
	fp->fid=mf-mfile;
	fp->cnt=cnt;
	fp->tag=tag;
	fp->wrbuf=mkarraychar(s, cnt);
}

void
wrdrain(Queue *q, int dorun, int isctl)
{
	int n;
	Pend *p;
	p=q->pend;
	if(p==0)
		return;
	if(q->synch)
		RETVAL(q->proc, 3)=0;
	else{
		if(!isctl){
			n=p->cnt;
			thdr.type = Twrite+1;
			thdr.fid = p->fid;
			thdr.tag = p->tag;
			thdr.count = n;
			sendmsg(0);
		}
		RETVAL(q->proc, 3)=(SWord)p->wrbuf;
		q->pend=p->next;
		p->next=freepend;
		freepend=p;
	}
	if(dorun)
		erun(q->proc);
	q->proc=0;
}

void
drain(Bind *bp)
{
	Pend *p;
	int i;
	Queue *q;
	for(i=WData; i<=WCtl; i++){
		q=&bp->q[i];
		if(i==WData) while(p=q->pend){
			thdr.type = Twrite+1;
			thdr.tag = p->tag;
			thdr.fid = p->fid;
			thdr.count = 0;
			sendmsg(Emntclosed);
			q->pend=p->next;
			p->next=freepend;
			freepend=p;
		}
		if(q->proc){
			RETVAL(q->proc, 3)=(SWord)mkarraychar("", 0);
			((Store *)RETVAL(q->proc, 3))->len=-1;
			erun(q->proc);
			q->proc=0;
		}
	}
	for(i=RData; i<RCtl; i++){
		q=&bp->q[i];
		if(i==RData) while(p=q->pend){
			thdr.type = Tread+1;
			thdr.tag = p->tag;
			thdr.fid = p->fid;
			thdr.count = 0;
			sendmsg(Emntclosed);
			q->pend=p->next;
			p->next=freepend;
			freepend=p;
		}
		if(q->proc){
			RETVAL(q->proc, 4)=-1;
			DECREF(q->proc, 3);
			erun(q->proc);
			q->proc=0;
		}
	}
}
