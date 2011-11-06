#include	<u.h>
#include	<lib9.h>
#include	<bio.h>

int
Bfildes(Biobuf *b)
{
	return b->_h.fid;
}
