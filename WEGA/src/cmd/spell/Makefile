# spell: the spelling checker of System III (usr/src/cmd/spell), so far the data of /usr/dict: the hash tables of the word lists, made as
# System III's spell.mk makes them: spellin makes hlist from list, hlista from hlist and american + local, hlistb from hlist and
# british + local, hstop from stop (spellin is a program of the WEGA system that make runs itself: RUN is "wrun" under the emulator, empty
# on WEGA). The word lists are those of System III, changed until the tables of the WEGA root come out (the tables are bit sets, the
# model of the hash found the words in them):
#   list      24000 words: without "network" (hlistb of the WEGA root does not have it)
#   local     the 16 words of System III's local that both tables have, and Zilog and motwork (not in System III's lists)
#   american  System III's, plus 17 words of System III's local that only hlista has, "network", and the words of /usr/dict/words that no other
#             list has (the 5769 words of that file are all in hlista)
#   british   System III's
#   stop      System III's without the word "vis"
# The 123 other words of System III's local (words of Bell Laboratories: Aho, Amdahl, awk, AFIPS, ...) are not in the tables of the WEGA root.
# words and spellhist are copied (no source in System III builds them).
#
#   make install PREFIX=dir    install below dir/usr/dict

CC = cc
PREFIX = /

all: hlista hlistb hstop

spellin: spellin.c spell.h
	$(CC) -O -o spellin spellin.c

hlist: spellin list
	$(RUN) ./spellin < list > hlist
hlista: hlist american local
	cat american local | $(RUN) ./spellin hlist > hlista
hlistb: hlist british local
	cat british local | $(RUN) ./spellin hlist > hlistb
hstop: spellin stop
	$(RUN) ./spellin < stop > hstop

install: all
	mkdir -p $(PREFIX)/usr/dict
	for f in hlista hlistb hstop ; do cp $$f $(PREFIX)/usr/dict/$$f ; chmod 644 $(PREFIX)/usr/dict/$$f ; done
	cp words $(PREFIX)/usr/dict/words ; chmod 664 $(PREFIX)/usr/dict/words
	cp spellhist $(PREFIX)/usr/dict/spellhist ; chmod 666 $(PREFIX)/usr/dict/spellhist

clean:
	rm -f spellin hlist hlista hlistb hstop

.PHONY: all install clean
