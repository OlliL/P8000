# uucp: the files of uucp in /usr/lib/uucp that System III's uucp.mk copies (cp $(UPATH)/L* $(UPATH)/USERFILE $(INSDIR); chmod 400 L.sys USERFILE); the files are those of the WEGA root. The programs of uucp are not here yet.
#
#   make install PREFIX=dir    install below dir/ (the modes of the WEGA root)

PREFIX = /

all:

install:
	mkdir -p $(PREFIX)/usr/lib/uucp
	rm -f $(PREFIX)/usr/lib/uucp/L-cmd ; cp L-cmd $(PREFIX)/usr/lib/uucp/L-cmd ; chmod 400 $(PREFIX)/usr/lib/uucp/L-cmd
	rm -f $(PREFIX)/usr/lib/uucp/L-devices ; cp L-devices $(PREFIX)/usr/lib/uucp/L-devices ; chmod 400 $(PREFIX)/usr/lib/uucp/L-devices
	rm -f $(PREFIX)/usr/lib/uucp/L.sys ; cp L.sys $(PREFIX)/usr/lib/uucp/L.sys ; chmod 400 $(PREFIX)/usr/lib/uucp/L.sys
	rm -f $(PREFIX)/usr/lib/uucp/USERFILE ; cp USERFILE $(PREFIX)/usr/lib/uucp/USERFILE ; chmod 400 $(PREFIX)/usr/lib/uucp/USERFILE

clean:

.PHONY: all install clean
