# cpp: the C preprocessor of WEGA (/lib/cpp, SCCS cpp.c 1.5, cpy.y 1.2, yylex.c 1.1), reconstructed after the cpp of
# System III (cmd/cc/vax/cpp). /lib/cpp is a combined program (code and data together, no -i). With strip byte-identical.
#
#   make             cpp
#   make clean

CC = ccv0p
CAS = cas
LD = ld
YACC = yacc
CFLAGS = -O
CRT = ../../lib/libc/crt
LIBC = ../../lib/libc-v2.a
COBJS = cpp.o cpy.o

all: cpp

cpp: crt0.o $(COBJS) $(LIBC)
	$(LD) -e start crt0.o $(COBJS) $(LIBC) -o $@

cpp.o: cpp.c
	$(CC) $(CFLAGS) -c cpp.c

cpy.o: cpy.c yylex.c
	$(CC) $(CFLAGS) -c cpy.c

cpy.c: cpy.y
	$(YACC) cpy.y
	sed -e 's/^YYSTYPE yylval, yyval;/extern YYSTYPE yylval; YYSTYPE yyval;/' -e 's/^\(YYSTYPE yyv\[.*\)$$/\1\
YYSTYPE yylval;/' y.tab.c > cpy.c
	rm -f y.tab.c

crt0.s: $(CRT)/mkcrt0.sh $(CRT)/crt0.s.in $(CRT)/crt0.list
	sh $(CRT)/mkcrt0.sh cpp > $@

crt0.o: crt0.s
	$(CAS) -u -o $@ crt0.s

clean:
	rm -f crt0.s cpy.c cpp *.o

.PHONY: all clean
