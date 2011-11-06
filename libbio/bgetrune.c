#include	<u.h>
#include	<lib9.h>
#include	<bio.h>

long
Bgetrune(Biobuf *b)
{
	int c, i;
	Rune rune;
	char str[4];

	c = Bgetc(b);
	if(c < Runeself) {		/* one char */
		b->_h.runesize = 1;
		return c;
	}
	str[0] = c;

	for(i=1;;) {
		c = Bgetc(b);
		if(c < 0)
			return c;
		str[i++] = c;

		if(fullrune(str, i)) {
			b->_h.runesize = chartorune(&rune, str);
			while(i > b->_h.runesize) {
				Bungetc(b);
				i--;
			}
			return rune;
		}
	}
}

int
Bungetrune(Biobuf *bb)
{
	Biobufhdr *bp = &bb->_h;

	if(bp->state == Bracteof)
		bp->state = Bractive;
	if(bp->state != Bractive)
		return Beof;
	bp->icount -= bp->runesize;
	bp->runesize = 0;
	return 1;
}
