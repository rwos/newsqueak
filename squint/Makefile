TARG		= squint
INCLUDES	= -I../include
LIBDIR		= ../lib
LIBS9		= -lbio -l9
LIBS		= $(LIBS9) -lm
LDFLAGS		= -L$(LIBDIR)
CC		= gcc
LD		= gcc

CFLAGS += $(INCLUDES)

BUILTINS=

CODE=arith.c ctl.c proc.c var.c
CO=arith.o ctl.o proc.o var.o
FO=squint.o alloc.o bltin.o compile.o const.o declare.o error.o \
	lex.o main.o node.o symbol.o type.o xec.o
#SO=space.o fmount.o
#LIBO=libio.o liblib.o libg.o liblayer.o
SO=space.o
LIBO=libio.o liblib.o

OFILES		= $(FO) $(CO) $(SO) $(LIBO)

$(TARG): $(FO) $(CO) $(SO) $(LIBO) $(LIBDIR)/*.a
	$(LD) $(LDFLAGS) -o $@ $(FO) $(CO) $(SO) $(LIBO) $(LIBS)

y.tab.h squint.c:	squint.y
	yacc -d squint.y
	mv y.tab.c squint.c

ydefs.h:	y.tab.h
	cmp -s y.tab.h ydefs.h || cp y.tab.h ydefs.h

slib.h:	sliblib.h slibio.h
	cat $< > slib.h

nodenames.h typenames.h:	node.h enumtoname
	enumtoname Ntype < node.h > nodenames.h
	enumtoname Ttype < node.h > typenames.h

inst.h insttab.h:	inst
	( echo 'typedef enum Inst{' ; \
	  sed 's/.*/	&,/' inst ; \
	  echo '	NInst' ; \
	  echo ' } Inst;' ; \
	  sed 's/^I/extern int i/;s/$$/(Proc*);/' inst ; \
	  echo 'extern struct Insttab{int (*fp)(Proc*); char *name;} insttab[];'\
	) > inst.h
	( sed 's/^I/i/;s/.*/extern int &(Proc*);/' inst ; \
	  echo 'struct Insttab insttab[]={' ; \
	  sed 's/.\(.*\)/	{i\1, "&"},/' inst ; \
	  echo '};' ; \
	) > insttab.h
	
inst:	$(CODE)
	cat $(CODE) | grep '^i.*(Proc \*proc)' | sed 's/^i/I/; s/(.*//; s/.*/&/' | sort > ninst
	cmp -s inst ninst || mv ninst inst

regress:	squint
	regress
	touch regress

install:	$(TARG)
	# regress
	install -c $(TARG) /usr/local/bin

clean:
	rm -rf $(OFILES)

world:;		make depend; make $(TARG)

depend:;	makedepend $(CPPFLAGS) $(INCLUDES) $(OFILES:.o=.c)

# DO NOT DELETE

squint.o: store.h node.h symbol.h ../include/u.h ../include/libc.h
squint.o: ../include/bio.h fns.h y.debug
alloc.o: ../include/u.h ../include/libc.h fns.h
bltin.o: store.h node.h symbol.h comm.h inst.h ../include/u.h
bltin.o: ../include/libc.h fns.h slib.h
compile.o: store.h node.h symbol.h ydefs.h comm.h inst.h ../include/u.h
compile.o: ../include/libc.h errjmp.h fns.h
const.o: store.h node.h symbol.h ydefs.h ../include/u.h ../include/libc.h
const.o: errjmp.h fns.h
declare.o: store.h node.h symbol.h ydefs.h ../include/u.h ../include/libc.h
declare.o: fns.h
error.o: nodenames.h typenames.h store.h node.h symbol.h ydefs.h
error.o: ../include/u.h ../include/libc.h errjmp.h fns.h
lex.o: store.h node.h symbol.h ydefs.h ../include/u.h ../include/libc.h
lex.o: ../include/bio.h fns.h
main.o: ../include/u.h ../include/libc.h errjmp.h ../include/bio.h fns.h
node.o: store.h node.h symbol.h ydefs.h ../include/u.h ../include/libc.h
node.o: fns.h
symbol.o: store.h node.h symbol.h ydefs.h ../include/u.h ../include/libc.h
symbol.o: fns.h
type.o: store.h node.h symbol.h ydefs.h ../include/u.h ../include/libc.h
type.o: errjmp.h fns.h
xec.o: store.h node.h comm.h inst.h insttab.h ../include/u.h
xec.o: ../include/libc.h errjmp.h fns.h
arith.o: store.h comm.h ../include/u.h ../include/libc.h fns.h
ctl.o: store.h comm.h ../include/u.h ../include/libc.h fns.h
proc.o: store.h comm.h ../include/u.h ../include/libc.h fns.h
var.o: store.h comm.h ../include/u.h ../include/libc.h fns.h
space.o: store.h comm.h ../include/u.h ../include/libc.h space.h fns.h
libio.o: store.h comm.h libargs.h ../include/u.h ../include/libc.h
libio.o: ../include/bio.h fns.h
liblib.o: store.h comm.h libargs.h ../include/u.h ../include/libc.h fns.h
