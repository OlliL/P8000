# popt, spopt: the peephole optimizer of cc and scc, reconstructed from /lib/popt. spopt is the same source with -DSPOPT, the objects in
# seg. Byte-identical with the originals.
#
#   make              popt
#   make spopt
#   make old          the versions of the old compiler in OLDDIRS (v1/Makefile ...)
#   make clean

CC = ccv0p
LD = ld
CFLAGS = -O
LIBC = ../../lib/libc-v5.a
# the start-up file of popt is the shipped one (same code: start, 42 bytes)
CRT0 = /lib/crt0.o
OBJS = z20.o z21.o z22.o z23.o
OLDDIRS = v1 v0 v0p

all: popt spopt

old:
	for d in $(OLDDIRS); do cd $$d ; $(MAKE) ; cd .. ; done

popt: $(OBJS)
	$(LD) -X -e start -i $(CRT0) $(OBJS) $(LIBC) -o popt

spopt: z20.c z21.c z22.c z23.c z2.h stdio.h
	-mkdir seg
	for f in z20 z21 z22 z23 ; do ( cd seg ; $(CC) $(CFLAGS) -DSPOPT -c ../$$f.c ) || exit 1 ; done
	$(LD) -X -e start -i $(CRT0) seg/z20.o seg/z21.o seg/z22.o seg/z23.o $(LIBC) -o spopt

$(OBJS): z2.h stdio.h

clean:
	-rm -f $(OBJS) popt spopt seg/*.o

.PHONY: all old clean
