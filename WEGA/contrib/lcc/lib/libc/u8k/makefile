#
# makefile for libc/u8k
#

SHELL=/bin/sh
PROT=644
GRP=system
OWN=wega

all:
#
# compile crt runtime routines, a makefile exists in the directory
#
	cd crt ; make
#
# compile csu start up routines, a makefile exists in the directory
#
	cd csu ; make
#
# compile gen routines, a makefile exists in the directory
#
	cd gen ; make
#
# compile sys system call routines, a makefile exists in the directory
#
	cd sys ; make

archive:
#
# Note that "archive" is invoked with libc/object as current directory.
#
# figure out the correct ordering for all the archive modules except
#	cuexit and fakcu, which must go at end of archive.
	chmog $(PROT) $(OWN) $(GRP) *.o
	mv cuexit.o cuexit.x
	mv fakcu.o fakcu.x
	mv signal.o signal.x
# $(LORDER) *.o | tsort >objlist #
	ar q scratch.a *.o && lorder scratch.a | tsort > objlist; rm scratch.a
	mv cuexit.x cuexit.o
	mv fakcu.x fakcu.o
	mv signal.x signal.o
#
# build the archive with the modules in correct order.
	$(AR) q ../lib.Llibc signal.o `cat objlist` cuexit.o fakcu.o
	$(AR) t ../lib.Llibc >../Llib.contents
	rm -f *.o

clean:
	-rm -f *.o

cleanc:
	-rm -f crt/chrtab.o gen/malloc.o

clobber: clean
	-rm -f */*.o

