COMSRC= .
INSTDIR= /z/LCC/lib
CC=	lcc
CFLAGS=	-O
DEFLIST= -Dunix -Du8k
INCLIST= -I$(COMSRC)

FLEX=	-DFLEXNAMES

CC_CMD=	$(CC) -c $(CFLAGS) $(DEFLIST) $(FLEX) $(INCLIST)

OFILES=	cpp.o cpy.o

#-------------------------
lcpp:	$(OFILES) makefile
	lcc -o lcpp $(OFILES)
	chmod 751 lcpp

install:	lcpp
	cp lcpp $(INSTDIR)
	chmod 751 $(INSTDIR)/lcpp

cpy.c :	$(COMSRC)/cpy.y
	$(YACC) $(COMSRC)/cpy.y
	mv y.tab.c cpy.c

cpy.o:	cpy.c $(COMSRC)/yylex.c
	$(CC_CMD) ./cpy.c

cpp.o:	$(COMSRC)/cpp.c
	$(CC_CMD) $(COMSRC)/cpp.c
