# BOOT0: boot images (boot0.*.s) and the pb.image boot program. Assembled
# with the WEGA cas, linked with ld, header-stripped with strip -h as in the
# original build.
#
#   make                       build pb.image, boot0.fd, boot0.ud, boot0.rm
#   make install PREFIX=dir    install pb.image below dir (default /)
#   make clean                 remove the objects and the images

AS ?= as   # the original BOOT0 build uses as, not cas
LD ?= ld
STRIP ?= strip
PREFIX ?= /

PROGS = pb.image boot0.fd boot0.ud boot0.rm

all: $(PROGS)

.s.o:
	$(AS) -u -o $@ $<

pb.image: boot0.md.o
	$(LD) -s -o $@ boot0.md.o
	$(STRIP) -h $@
	chmod 400 $@

boot0.fd: boot0.fd.o
	$(LD) -s -o $@ boot0.fd.o
	$(STRIP) -h $@
	chmod 400 $@

boot0.ud: boot0.ud.o
	$(LD) -s -o $@ boot0.ud.o
	$(STRIP) -h $@
	chmod 400 $@

boot0.rm: boot0.rm.o
	$(LD) -s -o $@ boot0.rm.o
	$(STRIP) -h $@
	chmod 400 $@

install: pb.image
	cp pb.image $(PREFIX)/pb.image

clean:
	rm -f $(PROGS) *.o

.PHONY: all install clean
