# terms.d: the terminal driving tables of nroff (/usr/lib/term). System III makes them: maketerms.c includes the tables of the tabNNN.c files
# and writes the files tabNNN, in three passes (-DPART1 .. -DPART3: the program does not fit in one). maketerms is a program of the
# WEGA system: RUN is what runs it ("wrun" under the emulator, empty on WEGA itself). The names in /usr/lib/term differ from those of
# System III: tab300-12 is installed as tab-12, tab300s as tab300S, tab300s-12 as tab300S-12; tab2631-c is not installed; tab630wp-12
# has no source and is the file of the WEGA root.
#
#   make install PREFIX=dir    install below dir/usr/lib/term (mode 644)

CC = cc
PREFIX = /
FILES = tab2631 tab2631-e tab300 tab300-12 tab300s tab300s-12 tab37 tab382 tab4000A tab450 tab450-12 tab832 taba1 tablp tabtn300 tabX

all: tables

# one rule for all the files, made by one run of maketerms (a rule with several targets would run its commands once for each of them under make -j);
# tables is the stamp
tables: maketerms.c terms.hd tab2631.c tab2631-c.c tab2631-e.c tab300.c tab300-12.c tab300s.c tab300s-12.c tab37.c tab382.c tab4000A.c \
	tab450.c tab450-12.c tab832.c taba1.c tablp.c tabtn300.c tabX.c
	$(CC) -DPART1 -o maketerms maketerms.c
	$(RUN) ./maketerms
	$(CC) -DPART2 -o maketerms maketerms.c
	$(RUN) ./maketerms
	$(CC) -DPART3 -o maketerms maketerms.c
	$(RUN) ./maketerms
	rm -f maketerms
	touch tables

install: all
	mkdir -p $(PREFIX)/usr/lib/term
	for f in tab2631 tab2631-e tab300 tab37 tab382 tab4000A tab450 tab450-12 tab832 taba1 tablp tabtn300 tabX ; do \
		cp $$f $(PREFIX)/usr/lib/term/$$f ; chmod 644 $(PREFIX)/usr/lib/term/$$f ; done
	cp tab300-12 $(PREFIX)/usr/lib/term/tab-12 ; chmod 644 $(PREFIX)/usr/lib/term/tab-12
	cp tab300s $(PREFIX)/usr/lib/term/tab300S ; chmod 644 $(PREFIX)/usr/lib/term/tab300S
	cp tab300s-12 $(PREFIX)/usr/lib/term/tab300S-12 ; chmod 644 $(PREFIX)/usr/lib/term/tab300S-12
	cp tab630wp-12 $(PREFIX)/usr/lib/term/tab630wp-12 ; chmod 644 $(PREFIX)/usr/lib/term/tab630wp-12

clean:
	rm -f maketerms tables $(FILES) tab2631-c

.PHONY: all install clean
