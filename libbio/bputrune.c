#include	<u.h>
#include	<lib9.h>
#include	<bio.h>

int
Bputrune(Biobuf *bb, long c)
{
	Rune rune;
	char str[4];
	int n;

	rune = c;
	if(rune < Runeself) {
		Bputc(bb, rune);
		return 1;
	}
	n = runetochar(str, &rune);
	if(n == 0)
		return Bbad;
	if(Bwrite(bb, str, n) != n)
		return Beof;
	return n;
}
