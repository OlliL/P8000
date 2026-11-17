# spell: the spelling checker of System III (usr/src/cmd/spell), so far the data of /usr/dict: the hash tables of the word lists.
# hstop is made the way System III makes it: spellin over the list stop (a program of the WEGA system that make runs itself: RUN is "wrun" under
# the emulator, empty on WEGA). The list stop is that of System III without the word "vis", which the WEGA table does not contain (found by
# the model of the hash in the bits of the table; with that change hstop is byte-identical).
# hlista and hlistb are the files of the WEGA root: spellin over System III's american/british, local and list does not give them. The
# tables of the WEGA root lack 123 and 141 words of those lists (words of Bell Laboratories: Aho, Amdahl, awk, AFIPS, ...), have the
# 5769 words of /usr/dict/words in hlista and 11 bits of words that could not be found (a search over strings up to 7 characters
# found no single word for them): so the inputs of the WEGA build are not known and the tables are copied.
# words and spellhist are copied as well.
#
#   make install PREFIX=dir    install below dir/usr/dict

CC = cc
PREFIX = /

all: hstop

spellin: spellin.c spell.h
	$(CC) -O -o spellin spellin.c

hstop: spellin stop
	$(RUN) ./spellin < stop > hstop

install: all
	mkdir -p $(PREFIX)/usr/dict
	cp hstop $(PREFIX)/usr/dict/hstop ; chmod 644 $(PREFIX)/usr/dict/hstop
	cp hlista $(PREFIX)/usr/dict/hlista ; chmod 644 $(PREFIX)/usr/dict/hlista
	cp hlistb $(PREFIX)/usr/dict/hlistb ; chmod 644 $(PREFIX)/usr/dict/hlistb
	cp words $(PREFIX)/usr/dict/words ; chmod 664 $(PREFIX)/usr/dict/words
	cp spellhist $(PREFIX)/usr/dict/spellhist ; chmod 666 $(PREFIX)/usr/dict/spellhist

clean:
	rm -f spellin hstop

.PHONY: all install clean
