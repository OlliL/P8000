# libl: /lib/libl.a and /lib/slibl.a (the lex library, nonsegmented and segmented) and /usr/lib/lex/ncform, from the
# System III sources (tmp/sys3/usr/src/lib/libl). Both libraries are made from the same sources (their Library.SC are equal,
# 02/11/83) by the compiler of that time, v1 (ccv1, sccv1 from OLDROOT); the archives are byte-identical (headers:
# arhdr_libl.s, arhdr_slibl.s, mkar.sh). oldcc/allprint, oldcc/allprint.s: allprint cut from the original members
# (HANDOFF.md).
#
#   make        libl.a slibl.a
#   make install PREFIX=dir   dir/lib/libl.a, dir/lib/slibl.a, dir/usr/lib/lex/ncform
#   make clean

CCV1 = ccv1
SCCV1 = sccv1
CAS = cas
PREFIX = /

all: libl.a slibl.a

obj:
	-mkdir obj
sobj:
	-mkdir sobj

LOBJS = obj/allprint.o obj/main.o obj/reject.o obj/yyless.o obj/yywrap.o
SOBJS = sobj/allprint.s.o sobj/main.s.o sobj/reject.s.o sobj/yyless.s.o sobj/yywrap.s.o

libl.a: obj $(LOBJS) arhdr_libl.s mkar.sh Library.SC
	rm -f libl.a
	cp Library.SC obj/Library.SC
	$(CAS) -u -o obj/arhdr.o arhdr_libl.s
	cd obj ; sh ../mkar.sh ../libl.a arhdr.o allprint.o main.o reject.o yyless.o yywrap.o Library.SC

slibl.a: sobj $(SOBJS) arhdr_slibl.s mkar.sh Library.SC
	rm -f slibl.a
	cp Library.SC sobj/Library.SC
	$(CAS) -u -o sobj/arhdr.o arhdr_slibl.s
	cd sobj ; sh ../mkar.sh ../slibl.a arhdr.o allprint.s.o main.s.o reject.s.o yyless.s.o yywrap.s.o Library.SC

obj/allprint.o: oldcc/allprint/0_1.s
	cd oldcc/allprint ; $(CAS) -u -o ../../obj/allprint.o 0_1.s
obj/main.o: main.c
	$(CCV1) -O -c main.c ; mv main.o obj/main.o
obj/reject.o: reject.c
	$(CCV1) -O -c reject.c ; mv reject.o obj/reject.o
obj/yyless.o: yyless.c
	$(CCV1) -O -c yyless.c ; mv yyless.o obj/yyless.o
obj/yywrap.o: yywrap.c
	$(CCV1) -O -c yywrap.c ; mv yywrap.o obj/yywrap.o

sobj/allprint.s.o: oldcc/allprint.s/0_1.s
	cd oldcc/allprint.s ; $(CAS) -u -o ../../sobj/allprint.s.o 0_1.s
sobj/main.s.o: main.c
	$(SCCV1) -O -c main.c ; mv main.o sobj/main.s.o
sobj/reject.s.o: reject.c
	$(SCCV1) -O -c reject.c ; mv reject.o sobj/reject.s.o
sobj/yyless.s.o: yyless.c
	$(SCCV1) -O -c yyless.c ; mv yyless.o sobj/yyless.s.o
sobj/yywrap.s.o: yywrap.c
	$(SCCV1) -O -c yywrap.c ; mv yywrap.o sobj/yywrap.s.o

install: all
	cp libl.a $(PREFIX)/lib/libl.a
	cp slibl.a $(PREFIX)/lib/slibl.a
	-mkdir $(PREFIX)/usr/lib $(PREFIX)/usr/lib/lex
	cp ncform $(PREFIX)/usr/lib/lex/ncform

clean:
	rm -rf obj sobj libl.a slibl.a

.PHONY: all install clean
