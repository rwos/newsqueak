#include	<u.h>
#include	<lib9.h>
#include	<bio.h>

int	printcol;

int
Bprint(Biobuf *bb, char *fmt, ...)
{
	va_list arg;
	int n, pcol;
	char *ip, *ep, *out;
	Biobufhdr *bp = &bb->_h;

	va_start(arg, fmt);
	ep = (char*)bp->ebuf;
	ip = ep + bp->ocount;
	pcol = printcol;
	out = doprint(ip, ep, fmt, arg);
	if(out >= ep-UTFmax-1) {
		Bflush(bb);
		ip = ep + bp->ocount;
		printcol = pcol;
		out = doprint(ip, ep, fmt, arg);
		if(out >= ep-UTFmax-1) {
			va_end(arg);
			return Beof;
		}
	}
	n = out-ip;
	bp->ocount += n;
	va_end(arg);
	return n;
}
