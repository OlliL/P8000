#	Makefile	1.8	83/07/27
#
# The following computer are implemented
# p8000:		P8000 mit WEGA
# a7150:		A7150 mit MUTOS
#
COMPUTER      =	p8000
DEST	      = /z/bin
HDEST         = /usr/new/lib
MDEST         = /usr/man/man1
HELP          = cpm.hlp
MAN           = cpm.1
CFLAGS	      = -O -D$(COMPUTER) ${DFLAG}
EXTHDRS	      = /usr/include/ctype.h \
		/usr/include/stdio.h

HDRS	      = command.h \
		cpmfio.h \
		cpmio.h

LDFLAGS	      = -i

LIBLIST       =

LIST	      = ls -C

LOADER	      = cc

MAKEFILE      = Makefile

OBJS	      = bitmap.o \
		blockio.o \
		blk_no_hdl.o \
		cclose.o \
		ccreat.o \
		cfillbuf.o \
		cflsbuf.o \
		cmdhdl.o \
		copen.o \
		copy.o \
		cpm.o \
		delete.o \
		dirhdl.o \
		disk.o \
		extent.o \
		ffc.o \
		gensktab.o \
		hexdmp.o \
		interact.o \
		physio.o \
		pip.o \
		rename.o

PRINT	      = pr

PROGRAM	      = cpm

SRCS	      = bitmap.c \
		blockio.c \
		blk_no_hdl.c \
		cclose.c \
		ccreat.c \
		cfillbuf.c \
		cflsbuf.c \
		cmdhdl.c \
		copen.c \
		copy.c \
		cpm.c \
		delete.c \
		dirhdl.c \
		disk.c \
		extent.c \
		ffc.c \
		gensktab.c \
		hexdmp.c \
		interact.c \
		physio.c \
		pip.c \
		rename.c

$(PROGRAM):     $(OBJS) $(LIBLIST)
		@echo -n "Loading $(PROGRAMM) ..."
		@$(LOADER) $(LDFLAGS) $(OBJS) $(LIBLIST) -o $(PROGRAM)
		@echo "done"

debug:;		@echo DFLAG = -DDEBUG > Makefile.tmp00
		@cat Makefile >>Makefile.tmp00
		@make -f Makefile.tmp00
		@rm -f Makefile.tmp00

clean:;		@rm -f $(OBJS)

cp:		$(PROGRAM)
		@echo Installing $(PROGRAM) in $(DEST)
		cp $(PROGRAM) $(DEST); strip $(DEST)/$(PROGRAM)
		@echo Installing $(HELP) in $(HDEST)
		cp ../$(HELP) $(HDEST)
		@echo Installing $(MAN) in $(MDEST)
		cp ../$(MAN) $(MDEST)

depend:;	@mkmf PROGRAM=$(PROGRAM) DEST=$(DEST)

index:;		@ctags -wx $(HDRS) $(SRCS)

print:;		@$(PRINT) $(HDRS) $(SRCS+

program:        $(PROGRAM)

strip:;		@strip $(PROGRAM)

tags:           $(SRCS); @ctags $(SRCS)

update:		$(DEST)/$(PROGRAM)

$(DEST)/$(PROGRAM): $(SRCS) $(LIBLIST)
		@make -f $(MAKEFILE) DEST=$(DEST) cp
###
bitmap.o: /usr/include/stdio.h cpmio.h
blockim.o: /usr/include/stdio.h cpmio.h
blk_no_hdl.o:	/usr/include/stdio.h cpmio.h cpmfio.h
cclose.o: /usr/include/stdio.h cpmio.h cpmfio.h
ccreat.o: /usr/include/stdio.h cpmio.h cpmfio.h
cfillbuf.o: /usr/include/stdio.h cpmio.h cpmfio.h
cflsbuf.o: /usr/include/stdio.h cpmio.h cpmfio.h
cmdhdl.o: /usr/include/ctype.h /usr/include/stdio.h cpmio.h
copen.o: /usr/include/stdio.h cpmio.h cpmfio.h
copy.o: /usr/include/stdio.h cpmio.h cpmfio.h
cpm.o: /usr/include/stdio.h cpmio.h cpmfio.h
delete.o: /usr/include/stdio.h cpmio.h cpmfio.h
dirhdl.o: /usr/include/stdio.h cpmio.h
disk.o: /usr/include/stdio.h cpmio.h
extent.o: /usr/include/stdio.h cpmio.h cpmfio.h
gensktab.o: /usr/include/stdio.h cpmio.h
hexdmp.o: /usr/include/stdio.h cpmio.h cpmfio.h
interact.o: /usr/include/stdio.h command.h
physio.o: /usr/include/stdio.h cpmio.h
pip.o: /usr/include/stdio.h cpmio.h cpmfio.h
rename.o: /usr/include/stdio.h cpmio.h cpmfio.h
