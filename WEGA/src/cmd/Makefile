CAS=		cas
CC=		cc
LD=		ld

FILES=		getud putud
STRIPFILES=	copydisk dump dumpdir format restor sync tar verify
CLEANFILES=	compat1crt.o sync.o

all: $(FILES) $(STRIPFILES) strip

strip:
	strip $(STRIPFILES)

clean:
	rm -f $(FILES) $(STRIPFILES)

compat1crt.o:	compat1crt.s
	$(CAS) -u -o $@ $*.s

sync:	compat1crt.o sync.o
	$(LD) -o $@ -e start compat1crt.o sync.o -lc
