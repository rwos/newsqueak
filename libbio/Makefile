
LIB=../lib/libbio.a
OFILES=\
	bbuffered.o\
	bfildes.o\
	bflush.o\
	bgetrune.o\
	bgetc.o\
	bgetd.o\
	binit.o\
	boffset.o\
	bprint.o\
	bputrune.o\
	bputc.o\
	brdline.o\
	bread.o\
	bseek.o\
	bwrite.o\

HFILES=/sys/include/bio.h

INCLUDES=-I../include

CFLAGS += $(INCLUDES)

CC=gcc

$(LIB): $(OFILES)
	ar r $(LIB) $(OFILES)

clean:
	rm -rf $(TARG) $(OFILES)
	rm -f $(LIB)

world:;		make depend; make $(LIB)

depend::;	makedepend $(CPPFLAGS) $(INCLUDES) $(OFILES:.o=.c)

# DO NOT DELETE

bbuffered.o: ../include/u.h ../include/libc.h ../include/bio.h
bfildes.o: ../include/u.h ../include/libc.h ../include/bio.h
bflush.o: ../include/u.h ../include/libc.h ../include/bio.h
bgetrune.o: ../include/u.h ../include/libc.h ../include/bio.h
bgetc.o: ../include/u.h ../include/libc.h ../include/bio.h
bgetd.o: ../include/u.h ../include/libc.h ../include/bio.h
binit.o: ../include/u.h ../include/libc.h ../include/bio.h
boffset.o: ../include/u.h ../include/libc.h ../include/bio.h
bprint.o: ../include/u.h ../include/libc.h ../include/bio.h
bputrune.o: ../include/u.h ../include/libc.h ../include/bio.h
bputc.o: ../include/u.h ../include/libc.h ../include/bio.h
brdline.o: ../include/u.h ../include/libc.h ../include/bio.h
bread.o: ../include/u.h ../include/libc.h ../include/bio.h
bseek.o: ../include/u.h ../include/libc.h ../include/bio.h
bwrite.o: ../include/u.h ../include/libc.h ../include/bio.h
