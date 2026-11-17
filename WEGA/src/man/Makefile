# man: the manual pages of the WEGA root (usr/man/man0 ... manM), as they are, and usr/lib/whatis.
#
# whatis is the file of the WEGA root (the copy here): it is the output of /etc/makewhatis over an older set of pages than these (38 lines of the
# output over the pages here differ, 34 lines of the file have no page any more), so no generator can make it from these pages.
# The target whatis-new makes it from the pages here, the way the system does, into the file whatis.new (it is not installed and not part
# of all or install): makewhatis (../cmd/makewhatis.sh) is run by the csh of the WEGA system (RUN is "wrun" under the emulator) in a layer
# over the root that holds copies of the pages.
#
#   make install PREFIX=dir    install below dir/usr/man and dir/usr/lib/whatis
#   make whatis-new            make whatis.new from the pages here

PREFIX = /
DIRS = man0 man1 man2 man3 man4 man5 man6 man7 manM

all:

install:
	for d in $(DIRS) ; do mkdir -p $(PREFIX)/usr/man/$$d ; for f in $$d/* ; do if [ -f $$f ] ; then cp $$f $(PREFIX)/usr/man/$$f ; chmod 644 $(PREFIX)/usr/man/$$f ; fi ; done ; done
	mkdir -p $(PREFIX)/usr/lib
	cp whatis $(PREFIX)/usr/lib/whatis ; chmod 644 $(PREFIX)/usr/lib/whatis

whatis-new:
	rm -rf whatis.d
	for d in $(DIRS) ; do mkdir -p whatis.d/usr/man/$$d ; for f in $$d/* ; do if [ -f $$f ] ; then cp $$f whatis.d/usr/man/$$f ; fi ; done ; done
	mkdir -p whatis.d/usr/lib whatis.d/tmp
	: > whatis.d/usr/lib/whatis
	Z8K_OVERLAY=`pwd`/whatis.d $(RUN) /bin/csh ../cmd/makewhatis.sh
	cp whatis.d/usr/lib/whatis whatis.new
	rm -rf whatis.d

clean:
	rm -rf whatis.d whatis.new

.PHONY: all install whatis-new clean
