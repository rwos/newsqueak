#include	<u.h>
#include	<lib9.h>
#include	<bio.h>

int
Bputc(Biobuf *bb, int c)
{
	int i;
	Biobufhdr *bp = &bb->_h;

	for(;;) {
		i = bp->ocount;
		if(i) {
			bp->ebuf[i++] = c;
			bp->ocount = i;
			return 0;
		}
		if(Bflush(bb) == Beof)
			break;
	}
	fprint(2, "Bputc: write error\n");
	return Beof;
}
