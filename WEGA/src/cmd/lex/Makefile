# lex: the lexical analyzer generator, System III lex (cmd/lex, main.c as lmain.c), built by ccv1l, the compiler of lex (01/14/83; host/OLDCC.md, v1l).
# See compare/lex/findings.json.
#
#   make              build ./lex
#   make install PREFIX=dir
#   make clean

CC = ccv1l
CAS = cas
LD = ld
YACC = yacc
CFLAGS = -O -I../../head/oldv7
PREFIX = /
STRIP = strip
LIBY = ../../lib/liby/build/nonseg/liby.a
LIBC = ../../lib/libc-v3.a
CRT = ../../lib/libc/nonseg/crt
OBJS = lmain.o y.tab.o sub1.o sub2.o header.o

all: lex

lex: crt0.o $(OBJS)
	$(LD) -i -e start -o $@ crt0.o $(OBJS) $(LIBY) $(LIBC)

crt0.s: $(CRT)/crt0.s.in $(CRT)/crt0.list $(CRT)/mkcrt0.sh
	sh $(CRT)/mkcrt0.sh lex > $@

crt0.o: crt0.s
	$(CAS) -u -o $@ crt0.s

y.tab.c: parser.y
	$(YACC) parser.y

lmain.o: lmain.c once.c ldefs.c
	$(CC) $(CFLAGS) -c lmain.c
y.tab.o: y.tab.c ldefs.c
	$(CC) $(CFLAGS) -c y.tab.c
sub1.o: sub1.c ldefs.c
	$(CC) $(CFLAGS) -c sub1.c
sub2.o: sub2.c ldefs.c
	$(CC) $(CFLAGS) -c sub2.c
header.o: header.c ldefs.c
	$(CC) $(CFLAGS) -c header.c

install: lex
	mkdir -p $(PREFIX)/usr/bin
	cp lex $(PREFIX)/usr/bin/lex ; $(STRIP) $(PREFIX)/usr/bin/lex

clean:
	rm -f lex *.o y.tab.c crt0.s

.PHONY: all install clean
