LIB=../lib/lib9.a

OFILES=\
	charstod.o\
	doprint.o\
	fatal.o\
	misc.o\
	nan.o\
	print.o\
	rune.o\
	utflen.o\
	wait.o\

INCLUDES=-I../include

CFLAGS += $(INCLUDES)

CC=gcc

all: $(LIB)

$(LIB): $(OFILES)
	ar r $(LIB) $(OFILES)

clean:
	rm -rf $(OFILES)

world:;		make depend; make $(LIB)

depend::;	makedepend $(CPPFLAGS) $(INCLUDES) $(OFILES:.o=.c)

# DO NOT DELETE

charstod.o: ../include/u.h ../include/lib9.h
doprint.o: ../include/u.h ../include/lib9.h
fatal.o: ../include/u.h ../include/lib9.h
getcallerpc.o: ../include/u.h ../include/lib9.h
lock.o: ../include/u.h ../include/lib9.h
malloc.o: ../include/u.h ../include/lib9.h 
nan.o: ../include/u.h ../include/lib9.h
pool.o: ../include/u.h ../include/lib9.h pool.h
print.o: ../include/u.h ../include/lib9.h
qlock.o: ../include/u.h ../include/lib9.h
rendezvous.o: ../include/u.h ../include/lib9.h ../include/fatal.h
rendezvous.o: ../include/thread.h 
rune.o: ../include/u.h ../include/lib9.h
utflen.o: ../include/u.h ../include/lib9.h
wait.o: ../include/u.h ../include/lib9.h
zalloc.o: ../include/u.h ../include/lib9.h ../include/fatal.h
