ROOT= /z/LCC

LIBDIR= $(ROOT)/lib
MOD=	751
OWN=	wega
GRP=	system
SHELL=	/bin/sh

COMMON=	../common


CC=	lcc
LINT=	lint
YACC=	yacc
LFLAGS=
CFLAGS=	-O
INCLIST=	-I. -I$(COMMON)
#
#	conditional compilation variables are declared 
#	in the DEFLIST and are passed to cpp.
#
#	variables:
#		PROFILE - causes inclusion of profiling code -
#			  conditional compilation can be removed if desired.
#

DEFLIST=
LDFLAGS=
YFLAGS=

CC_CMD=	$(CC) -c $(CFLAGS) $(INCLIST)
YACC_CMD=	$(YACC) $(YFLAGS)

# use M as a short-hand for the awkward COMMON
# (M for Machine independent part)

M=	$(COMMON)


OFILES= cgram.o xdefs.o scan.o pftn.o trees.o optim.o local.o reader.o\
	local2.o debug.o match.o allo.o comm1.o table.o cost.o cgen.o
CFILES=	$M/cgram.c $M/xdefs.c $M/scan.c $M/pftn.c $M/cgen.c $M/cost.c \
	$M/trees.c $M/optim.c local.c $M/reader.c \
	local2.c debug.c $M/match.c $M/allo.c $M/comm1.c \
	table.c

build:	ccom

#-------------------------

ccom:	$(OFILES)
		$(CC) $(LDFLAGS) -o ccom $(OFILES)
		chmog $(MOD) $(OWN) $(GRP) ccom

sty.c:		$M/manifest.h $M/sty.y macdefs.h $M/mfile2.h $M/common
		@echo "Expect 3 reduce/reduce conflicts"
		$(YACC_CMD) $M/sty.y
		mv y.tab.c sty.c

sty:	sty.c
#
# always use the resident compiler to generate sty.
# whether generating a cross-compiler or a resident
# on a non-target machine.
#
		lcc -O -I. -I$M -o sty sty.c

trees.o:	$M/manifest.h macdefs.h $M/mfile1.h sdb.h $M/trees.c
		$(CC_CMD) $M/trees.c

optim.o:	$M/manifest.h macdefs.h $M/mfile1.h sdb.h $M/optim.c
		$(CC_CMD) $M/optim.c

pftn.o:		$M/manifest.h macdefs.h $M/mfile1.h sdb.h $M/pftn.c
		$(CC_CMD) $M/pftn.c

local.o:	$M/manifest.h macdefs.h $M/mfile1.h sdb.h
		$(CC_CMD) local.c

scan.o:		$M/manifest.h macdefs.h $M/mfile1.h sdb.h $M/scan.c
		$(CC_CMD) $M/scan.c

xdefs.o:	$M/manifest.h $M/mfile1.h sdb.h macdefs.h $M/xdefs.c
		$(CC_CMD) $M/xdefs.c

cgram.o:	$M/manifest.h $M/mfile1.h sdb.h macdefs.h $M/cgram.c
		$(CC_CMD) $M/cgram.c

$M/cgram.c:	$M/cgram.y
		@echo "Expect 6 shift/reduce conflicts"
		$(YACC_CMD) $M/cgram.y
		mv y.tab.c $M/cgram.c

comm1.o:	$M/manifest.h $M/mfile1.h sdb.h $M/common macdefs.h \
			$M/comm1.c
		$(CC_CMD) $M/comm1.c

table.o:	$M/manifest.h $M/mfile2.h macdefs.h table.c
		$(CC_CMD) table.c

table.c:	sty stin
		./sty <stin >table.c

cost.o:		$M/manifest.h $M/mfile2.h macdefs.h $M/cost.c
		$(CC_CMD) $M/cost.c

cgen.o:		$M/manifest.h $M/mfile2.h macdefs.h $M/cgen.c
		$(CC_CMD) $M/cgen.c

reader.o:	$M/manifest.h $M/mfile2.h macdefs.h $M/reader.c
		$(CC_CMD) $M/reader.c

local2.o:	$M/manifest.h $M/mfile2.h macdefs.h
		$(CC_CMD) local2.c

debug.o:	$M/mfile1.h macdefs.h $(ROOT)/include/storclass.h
		$(CC_CMD) debug.c

match.o:	$M/manifest.h $M/mfile2.h macdefs.h $M/match.c sdb.h
		$(CC_CMD) $M/match.c

allo.o:		$M/manifest.h $M/mfile2.h macdefs.h $M/allo.c
		$(CC_CMD) $M/allo.c

#-------------------------

install:	ccom save
		cp ccom $(LIBDIR)
#		strip $(LIBDIR)/ccom
		chmog $(MOD) $(OWN) $(GRP) $(LIBDIR)/ccom

#--------------------------

save:
	if test -f $(LIBDIR)/ccom ; \
	then \
		mv $(LIBDIR)/ccom $(LIBDIR)/ccom.back ; \
	fi ;

#--------------------------

uninstall:
	if test -f $(LIBDIR)/ccom.back ; \
	then \
		mv $(LIBDIR)/ccom.back $(LIBDIR)/ccom ; \
	fi ;

#--------------------------

clean:
	-rm -f $(OFILES)

#--------------------------

clobber:	clean
		-rm -f ccom table.c sty.c sty $M/cgram.c

#--------------------------

lint:	$(CFILES)
	$(LINT) $(LFLAGS) -I. -I$M $(CFILES) >lint.out
