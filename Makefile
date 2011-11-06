DIRS	=	lib9 libbio squint

CFLAGS	= -g -Wall $(INCLUDES)

all:
	mkdir -p lib
	for i in $(DIRS); do cd $$i; CFLAGS="$(CFLAGS)" make -$(MAKEFLAGS); cd ..; done

clean:
	for i in $(DIRS); do cd $$i; CFLAGS="$(CFLAGS)" make -$(MAKEFLAGS) clean; cd ..; done

%:
	for i in $(DIRS); do cd $$i; CFLAGS="$(CFLAGS)" make -$(MAKEFLAGS) $@; cd ..; done
