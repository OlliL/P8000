# oldlibc: the libraries libc-vN.a of the time of the old programs, in ..
#
# Each original program was linked from a libc of its time. Their modules are here in the versions the originals hold:
# NAME/0_n, from C with the compiler of the rule (a source of ../libc or NAME/...) or, where a compiler or a source is still
# missing, from NAME/0_n.s; the other members are those of ../libc. The order of a library is the order of the members in
# the original programs, ld takes what is referenced. Version numbers of the modules are invented.
#
#   libc-v9.a  sld (07/83)
#
#   make        (needs ../libc made, and ccv1, ccv0, ccv0p)
#   make clean

CAS = cas
AR = ar

LIBS =  ../slibc-v9.a

all: $(LIBS)


LIBv9 = tmp/putchar.o tmp/scanf.o tmp/doscan.o printf/0_1.o \
        	doprnt/0_1.o tmp/ffltioc.o tmp/ungetc.o tmp/filbuf.o tmp/data.o tmp/flsbuf.o \
        	tmp/__callex.o tmp/cuexit.o tmp/atoi.o tmp/cnvt.o tmp/decimal.o tmp/length.o \
        	tmp/malloc.o tmp/ctype_.o tmp/isatty.o tmp/mktemp.o tmp/stty.o tmp/strchr.o \
        	tmp/strrchr.o tmp/swab.o tmp/rindex.o tmp/strcat.o tmp/strncmp.o tmp/strcpy.o \
        	tmp/swap.o tmp/access.o tmp/tolower.o tmp/chmod.o tmp/close.o tmp/creat.o \
        	tmp/fstat.o tmp/getpid.o tmp/ioctl.o tmp/link.o tmp/lseek.o tmp/mkseg.o tmp/open.o \
        	tmp/read.o tmp/ssgbrk.o tmp/signal.o tmp/umask.o tmp/unlink.o tmp/write.o \
        	tmp/cerror.o

../slibc-v9.a: tmp/created $(LIBv9)
	sh -x mklib.sh $@ $(AR) $(LIBv9)

tmp/created:
	-mkdir -p tmp
	cd tmp ; ar x /lib/slibc.a
	touch tmp/created

doprnt/0_1.o: doprnt/0_1.s
	cd doprnt ; $(CAS) -o 0_1.o -u 0_1.s

printf/0_1.o: printf/0_1.s
	cd printf ; $(CAS) -o 0_1.o -u 0_1.s

clean:
	rm -f */0_*.o */*.o $(LIBS)
	rm -rf *.d
	rm -f tmp/*.o tmp/created tmp/Library.SC

.PHONY: all clean
