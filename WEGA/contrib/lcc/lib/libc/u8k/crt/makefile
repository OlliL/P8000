#

SHELL=/bin/sh
CC=	lcc
LINT=	lint
CFLAGS=	-O
LDFLAGS=

# FPPO=	addmag.o fladd.o flcmp.o fldiv.o flmul.o flneg.o
#	flsub.o fltst.o ftol.o itof.o ltof.o
#	submag.o uitof.o ultof.o
#	dbadd.o dbcmp.o dbdiv.o dbmul.o dbneg.o dbtofl.o
#	dbtol.o dbtst.o fltodb.o ltodb.o table.o
#	fix.o single.o double.o
FPPO= comp.o conv.o doub.o fix.o mul.o sing.o

# FPPC=	addmag.c fladd.c flcmp.c fldiv.c flmul.c flneg.c
#	flsub.c fltst.c ftol.c itof.c ltof.c
#	submag.c uitof.c ultof.c
FPPC=

# OTHERO=	chrtab.o ldiv.o lmul.o cerror.o sigcode.o spgrow.o mcount.o
OTHERO= cerror.o chrtab.o
OTHERC=

OFILES=		$(FPPO) $(OTHERO)
CFILES=		$(FPPC) $(OTHERC)


all:		$(OFILES)


#-------------------------

.s.o:
	@echo $*.s:
	$(CC) $(CFLAGS) -c $*.s

#--------------------------

clean:
	-rm -f $(OFILES)

#--------------------------

clobber:	clean

