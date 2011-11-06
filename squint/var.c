#include "store.h"
#include "comm.h"
#include <u.h>
#include <lib9.h>

#include "fns.h"

void	chkary(Store **);
void	chkaryunique(Store **);
int	elemisptr(Store *, long);
void	pprint(Proc *, char *, ...);

char	*tname[]={
	"array of int",
	"array of compound objects",
	"array of char",
	"struct",
	"chan",
	"prog",
	"data",
};

/*
 * Push
 */

int
ipush(Proc *proc)
{
	*proc->sp++=*(SWord *)*++proc->pc;
	return 1;
}

int
ipushauto(Proc *proc)
{
	*proc->sp++=proc->fp[(SWord)*++proc->pc/WS];
	return 1;
}

int
ipushdata(Proc *proc)
{
	Store *s;
	s=(Store *)*++proc->pc;
	*proc->sp++=(SWord)s;
	s->ref++;
	return 1;
}

int
ipushptr(Proc *proc)
{
	Store *s;
	s=*(Store **)*++proc->pc;
	if(s==0)
		rerror("uninitialized global variable");
	*proc->sp++=(SWord)s;
	s->ref++;
	return 1;
}

int
ipushary(Proc *proc)
{
	SWord index;
	Store **addr;
	addr=(Store **)*++proc->pc;
	chkary(addr);
	index=*--proc->sp;
	if(index<0 || index>=(*addr)->len)
		rerror("index of global %s out of bounds (is %ld len %ld)",
			tname[(*addr)->type], index, (*addr)->len);
	*proc->sp++=(*addr)->data[index];
	return 1;
}

int
ipusharychar(Proc *proc)
{
	SWord index;
	Store **addr;
	addr=(Store **)*++proc->pc;
	chkary(addr);
	index=*--proc->sp;
	if(index<0 || index>=(*addr)->len)
		rerror("index of global %s out of bounds (is %ld len %ld)",
			tname[(*addr)->type], index, (*addr)->len);
	*proc->sp++=((unsigned char *)((*addr)->data))[index];
	return 1;
}

int
ipusharyptr(Proc *proc)
{
	SWord index;
	Store **addr;
	Store *s;
	addr=(Store **)*++proc->pc;
	chkary(addr);
	index=*--proc->sp;
	if(index<0 || index>=(*addr)->len)
		rerror("index of global %s out of bounds (is %ld len %ld)",
			tname[(*addr)->type], index, (*addr)->len);
	s=(Store *)((*addr)->data)[index];
	if(s==0)
		rerror("undefined element of %s", tname[(*addr)->type]);
	s->ref++;
	*proc->sp++=(SWord)s;
	return 1;
}

int
ipusharyexpr(Proc *proc)
{
	SWord index;
	Store *s;
	s=(Store *)*--proc->sp;
	chkary(&s);
	index=*--proc->sp;
	if(index<0 || index>=s->len)
		rerror("index of global %s out of bounds (is %ld len %ld)",
			tname[s->type], index, s->len);
	*proc->sp++=s->data[index];
	decref(&s);
	return 1;
}

int
ipusharycharexpr(Proc *proc)
{
	SWord index;
	Store *s;
	s=(Store *)*--proc->sp;
	chkary(&s);
	index=*--proc->sp;
	if(index<0 || index>=s->len)
		rerror("index of %s expression out of bounds (is %ld len %ld)",
			tname[s->type], index, s->len);
	*proc->sp++=((unsigned char *)(s->data))[index];
	decref(&s);
	return 1;
}

int
ipusharyptrexpr(Proc *proc)
{
	SWord index;
	Store *sa, *se;
	sa=(Store *)*--proc->sp;
	chkary(&sa);
	index=*--proc->sp;
	if(index<0 || index>=sa->len)
		rerror("index of %s expression out of bounds (is %ld len %ld)",
			tname[sa->type], index, sa->len);
	se=(Store *)(sa->data)[index];
	if(se==0)
		rerror("undefined element of %s expression", tname[se->type]);
	se->ref++;
	*proc->sp++=(SWord)se;
	decref(&sa);
	return 1;
}

int
ipushptrauto(Proc *proc)
{
	Store *s;
	s=(Store *)proc->fp[(SWord)*++proc->pc/WS];
	if(s==0)
		rerror("undefined variable (auto %ld)", (SWord)*proc->pc/WS);
	*proc->sp++=(SWord)s;
	s->ref++;
	return 1;
}

int
ipusharyauto(Proc *proc)
{
	SWord index;
	Store **addr;
	addr=(Store **)&proc->fp[(SWord)*++proc->pc/WS];
	chkary(addr);
	index=*--proc->sp;
	if(index<0 || index>=(*addr)->len)
		rerror("index of %s (auto %ld) out of bounds (is %ld len %ld)",
			tname[(*addr)->type], (SWord)*proc->pc/WS, index, (*addr)->len);
	*proc->sp++=(*addr)->data[index];
	return 1;
}

int
ipusharycharauto(Proc *proc)
{
	SWord index;
	Store **addr;
	addr=(Store **)&proc->fp[(SWord)*++proc->pc/WS];
	chkary(addr);
	index=*--proc->sp;
	if(index<0 || index>=(*addr)->len)
		rerror("index of %s (auto %ld) out of bounds (is %ld len %ld)",
			tname[(*addr)->type], (SWord)*proc->pc/WS, index, (*addr)->len);
	*proc->sp++=((unsigned char *)(*addr)->data)[index];
	return 1;
}

int
ipusharyptrauto(Proc *proc)
{
	SWord index;
	Store **addr;
	Store *s;
	addr=(Store **)&proc->fp[(SWord)*++proc->pc/WS];
	chkary(addr);
	index=*--proc->sp;
	if(index<0 || index>=(*addr)->len)
		rerror("index of %s (auto %ld) out of bounds (is %ld len %ld)",
			tname[(*addr)->type], (SWord)*proc->pc/WS, index, (*addr)->len);
	s=(Store *)((*addr)->data)[index];
	if(s==0)
		rerror("undefined element of %s (auto %ld)",
			tname[(*addr)->type], (SWord)*proc->pc/WS);
	s->ref++;
	*proc->sp++=(SWord)s;
	return 1;
}

int
ipushuniq(Proc *proc)
{
	Store **addr, *s;
	addr=(Store **)*++proc->pc;
	chkaryunique(addr);
	s=*addr;
	s->ref++;
	*proc->sp++=(SWord)s;
	return 1;
}

int
ipushuniqauto(Proc *proc)
{
	Store **addr, *s;
	addr=(Store **)&proc->fp[(SWord)*++proc->pc/WS];
	chkaryunique(addr);
	s=*addr;
	s->ref++;
	*proc->sp++=(SWord)s;
	return 1;
}

int
ipushuniqary(Proc *proc)
{
	Store **addr, *s;
	int index;
	s=(Store *)*--proc->sp;
	index=*--proc->sp;
	if(index<0 || index>=s->len)
		rerror("index of %s out of bounds (is %d len %ld)",
			tname[s->type], index, s->len);
	addr=(Store **)&s->data[index];
	chkaryunique(addr);
	s->ref--;
	if(s->ref<=0)
		panic("ipushuniqary pointless");
	s=*addr;
	s->ref++;
	*proc->sp++=(SWord)s;
	return 1;
}

/*
 * Store
 */

int
istore(Proc *proc)
{
	*(SWord *)*++proc->pc=*--proc->sp;
	return 1;
}

int
istoreauto(Proc *proc)
{
	proc->fp[(SWord)*++proc->pc/WS]=*--proc->sp;
	return 1;
}

int
istorechar(Proc *proc)
{
	*(SWord *)*++proc->pc=*--proc->sp&0xFF;
	return 1;
}

int
istorecharauto(Proc *proc)
{
	proc->fp[(SWord)*++proc->pc/WS]=*--proc->sp&0xFF;
	return 1;
}

int
istoreptr(Proc *proc)
{
	Store **addr;
	Store *old;
	addr=(Store **)*++proc->pc;
	old=*addr;
	*addr=(Store *)*--proc->sp;	/* move ref from stack to var */
	if(old)
		decref(&old);
	return 1;
}

int
istoreptrauto(Proc *proc)
{
	Store **addr;
	Store *old;
	addr=(Store **)&proc->fp[(SWord)*++proc->pc/WS];
	old=*addr;
	*addr=(Store *)*--proc->sp;	/* move ref from stack to var */
	if(old)
		decref(&old);
	return 1;
}

int
istoreary(Proc *proc)
{
	SWord index;
	Store *s, *old, *new;
	s=(Store *)*--proc->sp;
	index=*--proc->sp;
	if(index<0 || index>=s->len)
		rerror("index of %s out of bounds in assignment (is %ld len %ld)",
			tname[s->type], index, s->len);
	if(s->type==Sarychar){
		((unsigned char *)s->data)[index]=*--proc->sp;
		s->ref--;
		if(s->ref<=0)
			panic("istoreary char");
		return 1;
	}
	if(!elemisptr(s, index)){
		s->data[index]=*--proc->sp;
		s->ref--;
		if(s->ref<=0)
			panic("istoreary int");
		return 1;
	}
	old=(Store *)s->data[index];
	new=(Store *)*--proc->sp;
	s->data[index]=(SWord)new;
	/* ref of new moves from stack to here, so it's right */
	decref(&old);
	s->ref--;
	if(s->ref<=0 || new->ref<=0)
		panic("istoreary ptr");
	return 1;
}

/*
 * Def
 */

int
idef(Proc *proc)
{
	Store **addr;
	addr=(Store **)*++proc->pc;
	*proc->sp++=(*addr!=0);
	return 1;
}

int
idefauto(Proc *proc)
{
	Store **addr;
	addr=(Store **)&proc->fp[(SWord)*++proc->pc/WS];
	*proc->sp++=(*addr!=0);
	return 1;
}

int
idefary(Proc *proc)
{
	SWord index;
	Store *s;
	s=(Store *)*--proc->sp;
	index=*--proc->sp;
	if(index<0 || index>=s->len)
		rerror("index of %s out of bounds in def (is %ld len %ld)",
			tname[s->type], index, s->len);
	if(s->type==Saryptr || (s->type==Sstruct && elemisptr(s, index)))
		*proc->sp++=(s->data[index]!=0);
	else
		*proc->sp++=1;
	decref(&s);
	return 1;
}

/*
 * Inc and Dec
 */

int
iinc(Proc *proc)
{
	*proc->sp++=*(SWord *)*++proc->pc;
	(*(SWord *)*proc->pc)++;
	return 1;
}

int
iincauto(Proc *proc)
{
	*proc->sp++=proc->fp[(SWord)*++proc->pc/WS];
	proc->fp[(SWord)*proc->pc/WS]++;
	return 1;
}

int
idec(Proc *proc)
{
	*proc->sp++=--(*(SWord *)*++proc->pc);
	return 1;
}

int
idecauto(Proc *proc)
{
	*proc->sp++=--proc->fp[(SWord)*++proc->pc/WS];
	return 1;
}

int
iincary(Proc *proc)
{
	SWord index;
	Store *s;
	s=(Store *)*--proc->sp;
	if(s->ref--==1)
		rpanic("iincary pointless");
	index=*--proc->sp;
	if(index<0 || index>=s->len)
		rerror("index of %s out of bounds in ++ (is %ld len %ld)",
			tname[s->type], index, s->len);
	*proc->sp++=s->data[index]++;
	return 1;
}

int
idecary(Proc *proc)
{
	SWord index;
	Store *s;
	s=(Store *)*--proc->sp;
	if(s->ref--==1)
		rpanic("iincary pointless");
	index=*--proc->sp;
	if(index<0 || index>=s->len)
		rerror("index of %s out of bounds in -- (is %ld len %ld)",
			tname[s->type], index, s->len);
	*proc->sp++=--s->data[index];
	return 1;
}

/*
 * Cat and del
 */

int
icat(Proc *proc)
{
	Store *s1, *s2, *r;
	int n, n1, n2;
	s2=(Store *)*--proc->sp;
	s1=(Store *)*--proc->sp;
	n2=s2->len;
	n1=s1->len;
	r = 0;	/* set */
	switch(s1->type){
	case Saryint:
		r=mk(Saryint, n1+n2);
		memcpy(r->data, s1->data, n1*WS);
		memcpy(r->data+n1, s2->data, n2*WS);
		break;
	case Saryptr:
		r=mk(Saryptr, n1+n2);
		memcpy(r->data, s1->data, n1*WS);
		memcpy(r->data+n1, s2->data, n2*WS);
		for(n=0; n<n1+n2; n++)
			if(((Store **)r->data)[n])
				((Store **)r->data)[n]->ref++;
		break;
	case Sarychar:
		r=mk(Sarychar, ((n1+n2)+WS)/WS);
		memcpy(r->data, s1->data, n1);
		memcpy(((char *)r->data)+n1, s2->data, n2);
		r->len=n1+n2;
		((char *)r->data)[n1+n2]=0;
		break;
	}
	decref(&s1);
	decref(&s2);
	*proc->sp++=(long)r;
	return 1;
}

int
idel(Proc *proc)
{
	Store *s, *r;
	int m, n;
	n=*--proc->sp;
	s=(Store *)*--proc->sp;
	m=s->len;
	if(n>m || m<-n)
		rerror("del: len error %ld %d", s->len, n);
	r = 0;	/* set */
	switch(s->type){
	case Saryint:
		if(n>=0){
			r=mk(Saryint, m-n);
			memcpy(r->data, s->data+n, (m-n)*WS);
		}else{
			r=mk(Saryint, m+n);
			memcpy(r->data, s->data, (m+n)*WS);
		}
		break;
	case Saryptr:
		if(n>=0){
			r=mk(Saryptr, m-n);
			memcpy(r->data, s->data+n, (m-n)*WS);
		}else{
			r=mk(Saryptr, m+n);
			memcpy(r->data, s->data, (m+n)*WS);
		}
		for(n=0; n<r->len; n++)
			((Store **)r->data)[n]->ref++;
		break;
	case Sarychar:
		if(n>=0){
			r=mk(Sarychar, ((m-n)+WS)/WS);
			memcpy(r->data, ((char *)s->data)+n, (m-n));
			m-=n;
		}else{
			r=mk(Sarychar, ((m+n)+WS)/WS);
			memcpy(r->data, s->data, (m+n));
			m+=n;
		}
		r->len=m;
		((char *)r->data)[m]=0;
		break;
	}
	decref(&s);
	*proc->sp++=(long)r;
	return 1;
}

/*
 * Print
 */

int
isprint(Proc *proc)
{
	Store *s;
	s=emalloc(SHSZ+proc->nprbuf+1);
	strcpy((char *)s->data, proc->prbuf);
	s->ref=1;
	s->len=proc->nprbuf;
	s->type=Sarychar;
	proc->nprbuf=0;
	*proc->sp++=(long)s;
	return 1;
}

int
iprint(Proc *proc)
{
	Store *s;
	s=(Store *)*--proc->sp;
	write(1, (char *)s->data, s->len);
	decref(&s);
	return 1;
}

void
pprint(Proc *proc, char *fmt, ...)
{
	va_list va;
	char buf[4096];
	long n;
	va_start(va, fmt);
	n=doprint(buf, buf+sizeof buf, fmt, va)-buf;
	va_end(va);
	if(n+proc->nprbuf+1>proc->maxprbuf){
		proc->prbuf=erealloc(proc->prbuf, proc->maxprbuf+64+n);
		proc->maxprbuf+=64+n;
	}
	strcpy(proc->prbuf+proc->nprbuf, buf);
	proc->nprbuf+=n;
}

void
printary(Proc *proc, Store *s)
{
	long i;
	if(s==0){
		pprint(proc, "{}");
		return;
	}
	switch(s->type){
	case Sarychar:
		if(s->len==0)
			pprint(proc, "");	/* sigh */
		for(i=0; i<s->len; i+=4096)	/* 4096 is known in doprint.c */
			pprint(proc, "%.4096s", (char *)(s->data)+i);
		return;
	case Schan:
		pprint(proc, "(chan %lux)", s);
		return;
	case Sprog:
		pprint(proc, "{(prog %lux)\n", s);
		for(i=1; i<s->len; i++)
			pprint(proc, "  %lux:\t%i\n", &s->data[i], s->data[i]);
		pprint(proc, "}\n");
		return;
	case Sdata:
		pprint(proc, "(data %lux)", s->data[0]);
		return;
	}
	pprint(proc, "{");
	for(i=s->sbits; i<s->len; i++){
		if(elemisptr(s, i))
			printary(proc, (Store *)s->data[i]);
		else
			pprint(proc, "%ld", s->data[i]);
		if(i!=s->len-1)
			pprint(proc, ", ");
	}
	pprint(proc, "}");
}

int
iprintary(Proc *proc)
{
	Store *s=(Store *)*--proc->sp;
	printary(proc, s);
	decref(&s);
	return 1;
}

/*
 * Mk
 */

int
imalloc(Proc *proc)
{
	Store *s;
	int type=*--proc->sp;
	SWord n=*--proc->sp;
	if(n<0)
		rerror("negative size in mk()");
	s=emalloc(WS*(2+n));
	memset((char *)s, 0, WS*(2+n));
	s->ref=1;
	s->len=n;
	s->type=type;
	*proc->sp++=(SWord)s;
	return 1;
}

int
imallocstruct(Proc *proc)
{
	Store *s;
	SWord n=*--proc->sp;
	int nbits=(n+BPW-1)/BPW;
	int i;
	n+=nbits;
	if(n<0)
		rerror("negative size in mk()");
	s=emalloc(WS*(2+n));
	memset((char *)s, 0, WS*(2+n));
	s->ref=1;
	s->type=Sstruct;
	s->sbits=nbits;
	s->len=n;
	for(i=0; i<nbits; i++)
		s->data[i]=*--proc->sp;
	*proc->sp++=(SWord)s;
	return 1;
}

int
imallocarychar(Proc *proc)
{
	Store *s;
	SWord n=*--proc->sp;
	if(n<0)
		rerror("negative size in mk()");
	s=emalloc(WS*2+n+1);
	memset((char *)s, 0, WS*2+n+1);
	s->ref=1;
	s->type=Sarychar;
	s->len=n;
	*proc->sp++=(SWord)s;
	return 1;
}

int
imemcpy(Proc *proc)
{
	SWord *l, *ol;
	SWord len;
	l=((SWord *)*--proc->sp);
	len=*--proc->sp;
	ol=l;
	l+=2;
	while(len-->0)
		*l++=*--proc->sp;
	*proc->sp++=(SWord)ol;
	return 1;
}

int
imemcpychar(Proc *proc)
{
	char *l, *ol;
	SWord len;
	Store *s;
	l=((char *)*--proc->sp);
	len=*--proc->sp;
	ol=l;
	l+=2*WS;
	s=(Store *)*--proc->sp;
	memcpy(l, (char *)s->data, len);
	decref(&s);
	*proc->sp++=(SWord)ol;
	return 1;
}

int
imemcpycharint(Proc *proc)
{
	char *l, *ol;
	SWord len;
	l=((char *)*--proc->sp);
	len=*--proc->sp;
	ol=l;
	l+=2*WS;
	while(len-->0)
		*l++=*--proc->sp;
	*proc->sp++=(SWord)ol;
	return 1;
}

int
imemcpystruct(Proc *proc)
{
	SWord *l, *ol;
	SWord len;
	l=((SWord *)*--proc->sp);
	len=*--proc->sp;
	ol=l;
	l+=2+((Store *)l)->sbits;
	while(len-->0)
		*l++=*--proc->sp;
	*proc->sp++=(SWord)ol;
	return 1;
}

/*
 * Ref counts
 */

int
idecref(Proc *proc)
{
	Store *s;
	s=(Store *)*++proc->pc;
	if(s)
		decref(&s);
	return 1;
}

int
idecrefauto(Proc *proc)
{
	Store **s;
	s=(Store **)&proc->fp[(long)*++proc->pc/sizeof(long)];
	if(*s)
		decref(s);
	*s=0;	/* deallocate this automatic */
	return 1;
}

/*
 * Support for compound objects
 */

void
decref(Store **sp)
{
	Store *s=*sp;
	int i;
	if(s==0)
		return;
	if(s->ref<=0)
		rpanic("decref &s=%p s=%p s->ref=%d\n", sp, s, s->ref);
	if(s->ref==1){
		switch(s->type){
		case Saryint:
			break;
		case Saryptr:
			for(i=0; i<s->len; i++)
				decref((Store **)&s->data[i]);
			break;
		case Sarychar:
			break;
		case Sstruct:
			for(i=0; i<s->len-s->sbits; i++)
				if(s->data[i/BPW]&(1L<<(i&(BPW-1))))
					decref((Store **)&s->data[i+s->sbits]);
			break;
		case Schan:
			break;	/* release communication??? */
		case Sprog:
			decref((Store **)&s->data[0]);
			break;
		case Sdata:
			free((void *)s->data[0]);
			break;
		case Sbitmapid:
			libgbfree(s->data[0]);
			break;
		default:
			panic("decref unknown %d\n", s->type);
		}
	}
	s->ref--;
	if(s->ref==0){
		free((char *)s);
		*sp=0;
	}
	return;
}

void
chkary(Store **addr)
{
	if(*addr==0)
		rerror("uninitialized compound object");
	if((*addr)->ref==0)
		rerror("array ref count == 0");
}

void
chkaryunique(Store **addr)
{
	Store *naddr, **p, **endp;
	long len;
	if(*addr==0)
		rerror("uninitialized compound object");
	if((*addr)->ref<=0)
		rerror("array ref count <= 0");
	if((*addr)->ref==1)
		return;
	len=(*addr)->len;
	if((*addr)->type!=Sarychar)
		len*=WS;
	len+=sizeof(Store);
	naddr=emalloc(len);
	memcpy(naddr, *addr, len);
	(*addr)->ref--;
	*addr=naddr;
	naddr->ref=1;
	if(naddr->type==Saryptr){
		p=(Store **)naddr->data;
		endp=(Store **)&naddr->data[naddr->len];
		for(; p<endp; p++)
			if(*p)
				(*p)->ref++;
	}else if(naddr->type==Sstruct){
		int i;
		p=(Store **)(naddr->data+naddr->sbits);
		for(i=0; i<naddr->len-naddr->sbits; i++, p++)
			if(naddr->data[i/BPW]&(1L<<(i&(BPW-1))) && *p)
				(*p)->ref++;
	}
}

int
elemisptr(Store *s, long i)
{
	if(s->type==Saryptr)
		return 1;
	if(s->type==Sstruct){
		i-=s->sbits;
		return s->data[i/BPW]&(1L<<(i&(BPW-1)));
	}
	return 0;
}

