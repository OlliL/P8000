#WDOS-Kommando Makefile

I = /usr/include

all:	wm wdos bios bios.p8 bios.wy wdos.test bios.test

wm: 	wm.c \
	$I/stdio.h $I/ctype.h $I/curses.h $I/screen.h $I/signal.h \
	$I/fcntl.h $I/sys/types.h \
        ; cc -O -s -o wm wm.c -lscreen -lcurses -ltermlib
           
wdos: 	wdos.c \
	$I/stdio.h $I/termio.h $I/signal.h $I/time.h \
	$I/fcntl.h $I/sys/types.h $I/sys/stat.h \
        ; cc -O -s -i -o wdos wdos.c

wdos.test: wdos.test.c \
	$I/termio.h $I/signal.h $I/time.h \
	$I/fcntl.h $I/sys/types.h $I/sys/stat.h \
        ; cc -O -s -i -o wdos.test wdos.test.c

bios:	dseg.obj bios.obj term.obj disk.obj orgs.obj mdir/mkbios \
	; link3 < mdir/mkbios

bios.p8: dseg.obj bios.obj termp8.obj disk.obj orgs.obj mdir/mkbios.p8 \
	; link3 < mdir/mkbios.p8

bios.wy: dseg.obj bios.obj termwy.obj disk.obj orgs.obj mdir/mkbios.wy \
	; link3 < mdir/mkbios.wy

bios.test: dseg.obj test.obj termp8.obj disk.obj orgs.obj mdir/mkbios.test \
	; link3 < mdir/mkbios.test

dseg.obj:	dseg.asm \
		; asm86 < mdir/mkdseg.obj
	
bios.obj:	bios.asm \
		; asm86 < mdir/mkbios.obj

term.obj:	term.asm \
		; asm86 < mdir/mkterm.obj

termp8.obj:	termp8.asm \
		; asm86 < mdir/mktermp8.obj

termwy.obj:	termwy.asm \
		; asm86 < mdir/mktermwy.obj

disk.obj:	disk.asm \
		; asm86 < mdir/mkdisk.obj

orgs.obj:	orgs.asm \
		; asm86 < mdir/mkorgs.obj

test:		test.asm \
		; asm86 < mdir/mktest.obj
