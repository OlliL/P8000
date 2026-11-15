# oldslibc: the libraries slibc-vN.a of the time of the old segmented programs, in .. (README.md)
#
# NAME/0_n.s cut from the original program, NAME/0_n.c a source; the other members are those of /lib/slibc.a (tmp/). The order
# of a library is the order of the members in the original program. Version numbers of the modules are invented.
#
#   slibc-v9.a   sld (07/83)
#   slibc-v11.a  cas
#
#   make        (needs sccvc in OLDROOT for errlst)
#   make clean

CAS = cas
AR = ar

LIBS =  ../slibc-v9.a ../slibc-v11.a
# slibc-v11.a: cas; NAME/0_2.s are cut from /bin/cas (host/tools/oldslibc_cas.py), the other members are those of /lib/slibc.a.
# The members were made by the shipped assembler (bit 31 of an address constant clear, not as the modules of cas itself): the
# 0_2.s go through $(CAS), not through the assembler of sccvc. errlst is ../libc/common/gen/errlst.c by sccvc (the strings behind
# both tables, the data order of the old compiler), assembled by $(CAS) too.
SCCVC = sccvc

all: $(LIBS)


LIBv9 = tmp/putchar.o tmp/scanf.o tmp/doscan.o printf/0_1.o \
        	doprnt/0_1.o tmp/ffltioc.o tmp/ungetc.o tmp/filbuf.o tmp/flsbuf.o tmp/data.o \
        	tmp/__callex.o tmp/cuexit.o tmp/atoi.o tmp/cnvt.o tmp/decimal.o tmp/length.o \
        	tmp/malloc.o tmp/isatty.o tmp/mktemp.o tmp/ctype_.o tmp/stty.o tmp/strchr.o \
        	tmp/strrchr.o tmp/swab.o tmp/rindex.o tmp/strcat.o tmp/strncmp.o tmp/strcpy.o \
        	tmp/swap.o tmp/access.o tmp/tolower.o tmp/chmod.o tmp/close.o tmp/creat.o \
        	tmp/fstat.o tmp/getpid.o tmp/ioctl.o tmp/link.o tmp/lseek.o tmp/mkseg.o tmp/open.o \
        	tmp/read.o tmp/ssgbrk.o tmp/signal.o tmp/umask.o tmp/unlink.o tmp/write.o \
        	tmp/cerror.o

../slibc-v9.a: tmp/created $(LIBv9)
	sh -x mklib.sh $@ $(AR) $(LIBv9)

LIBv11 = tmp/freopen.o fseek/0_2.o rdwr/0_2.o tmp/fopen.o printf/0_2.o doprnt/0_2.o tmp/ungetc.o \
	 filbuf/0_2.o flsbuf/0_2.o endopen/0_2.o tmp/data.o findiop/0_2.o tmp/__callex.o tmp/cuexit.o \
	 perror/0_2.o cnvt/0_2.o tmp/decimal.o length/0_2.o malloc/0_2.o errlst/0_2.o tmp/isatty.o \
	 mktemp/0_2.o tmp/ctype_.o tmp/stty.o tmp/strcmp.o strcpy/0_2.o strncpy/0_2.o strlen/0_2.o \
	 tmp/access.o tmp/tolower.o tmp/chmod.o tmp/close.o tmp/creat.o tmp/getpid.o tmp/ioctl.o tmp/lseek.o \
	 tmp/mkseg.o tmp/open.o tmp/read.o tmp/ssgbrk.o tmp/signal.o tmp/umask.o tmp/unlink.o tmp/write.o \
	 tmp/cerror.o

../slibc-v11.a: tmp/created $(LIBv11)
	sh -x mklib.sh $@ $(AR) $(LIBv11)

tmp/created:
	-mkdir -p tmp
	cd tmp ; ar x /lib/slibc.a
	touch tmp/created

doprnt/0_1.o: doprnt/0_1.s
	cd doprnt ; $(CAS) -o 0_1.o -u 0_1.s

printf/0_1.o: printf/0_1.s
	cd printf ; $(CAS) -o 0_1.o -u 0_1.s

fseek/0_2.o: fseek/0_2.s
	cd fseek ; $(CAS) -o 0_2.o -u 0_2.s

rdwr/0_2.o: rdwr/0_2.s
	cd rdwr ; $(CAS) -o 0_2.o -u 0_2.s

printf/0_2.o: printf/0_2.s
	cd printf ; $(CAS) -o 0_2.o -u 0_2.s

doprnt/0_2.o: doprnt/0_2.s
	cd doprnt ; $(CAS) -o 0_2.o -u 0_2.s

filbuf/0_2.o: filbuf/0_2.s
	cd filbuf ; $(CAS) -o 0_2.o -u 0_2.s

flsbuf/0_2.o: flsbuf/0_2.s
	cd flsbuf ; $(CAS) -o 0_2.o -u 0_2.s

endopen/0_2.o: endopen/0_2.s
	cd endopen ; $(CAS) -o 0_2.o -u 0_2.s

findiop/0_2.o: findiop/0_2.s
	cd findiop ; $(CAS) -o 0_2.o -u 0_2.s

perror/0_2.o: perror/0_2.s
	cd perror ; $(CAS) -o 0_2.o -u 0_2.s

cnvt/0_2.o: cnvt/0_2.s
	cd cnvt ; $(CAS) -o 0_2.o -u 0_2.s

length/0_2.o: length/0_2.s
	cd length ; $(CAS) -o 0_2.o -u 0_2.s

malloc/0_2.o: malloc/0_2.s
	cd malloc ; $(CAS) -o 0_2.o -u 0_2.s

mktemp/0_2.o: mktemp/0_2.s
	cd mktemp ; $(CAS) -o 0_2.o -u 0_2.s

strcpy/0_2.o: strcpy/0_2.s
	cd strcpy ; $(CAS) -o 0_2.o -u 0_2.s

strncpy/0_2.o: strncpy/0_2.s
	cd strncpy ; $(CAS) -o 0_2.o -u 0_2.s

strlen/0_2.o: strlen/0_2.s
	cd strlen ; $(CAS) -o 0_2.o -u 0_2.s

errlst/0_2.o: errlst/0_2.c
	cd errlst ; $(SCCVC) -S -O 0_2.c
	cd errlst ; $(CAS) -o 0_2.o -u 0_2.s

clean:
	rm -f */0_*.o */*.o $(LIBS) errlst/0_2.s
	rm -rf *.d
	rm -f tmp/*.o tmp/created tmp/Library.SC

.PHONY: all clean
