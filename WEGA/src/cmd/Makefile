# cmd: WEGA system commands. Every command is built with the cc, cas and ld
# named by the variables below (default: the WEGA tools in PATH). The
# subdirectories are the commands that have their own Makefile. The cc
# driver and popt are Linux-side tools and live in ../../host.
#
#   make                       build everything
#   make install PREFIX=dir    install below dir (default /)
#   make clean                 remove the objects and the programs

CAS ?= cas
CC  ?= cc
LD  ?= ld
STRIP ?= strip
PREFIX ?= /

SUBDIRS = ar ranlib size strip

FILES=		getud putud
STRIPFILES=	copydisk dump dumpdir format restor sync tar verify
CLEANFILES=	compat1crt.o sync.o

all: $(FILES) $(STRIPFILES) strip-programs $(SUBDIRS:%=%.dir)

%.dir:
	@$(MAKE) -C $* all CC="$(CC)" CAS="$(CAS)" LD="$(LD)"

strip-programs:
	$(STRIP) $(STRIPFILES)

clean:
	rm -f $(FILES) $(STRIPFILES) $(CLEANFILES)
	@for d in $(SUBDIRS); do $(MAKE) -C $$d clean || exit 1; done

compat1crt.o:	compat1crt.s
	$(CAS) -u -o $@ $*.s

sync:	compat1crt.o sync.o
	$(LD) -o $@ -e start compat1crt.o sync.o -lc

.PHONY: all clean strip-programs
