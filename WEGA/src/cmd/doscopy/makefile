#DOS-Kommando Makefile

OBJECTS = dosdev.o dosplit.o dosopn.o  dosdel.o dosfat.o dosin.o dosout.o \
          doscat.o dosrm.o doscp.o dosdir.o dosls.o dosmkdir.o \
          dosrmdir.o dosformat.o
I = /usr/include
LD = cc

all:	doscat dosrm doscp dosdir dosls dosmkdir dosrmdir dosformat

doscat: doscat.o dosplit.o dosdev.o dosopn.o dosout.o \
        ; $(LD) doscat.o dosplit.o dosdev.o dosopn.o dosout.o -o doscat -s
           
dosrm: dosrm.o dosplit.o dosdev.o dosopn.o dosdel.o \
        ; $(LD) dosrm.o dosplit.o dosdev.o dosopn.o dosdel.o -o dosrm -s

doscp: doscp.o dosplit.o dosdev.o dosopn.o dosdel.o dosfat.o dosin.o dosout.o \
        ; $(LD) doscp.o dosplit.o dosdev.o dosopn.o dosdel.o dosfat.o dosin.o \
        dosout.o -o doscp -s

dosdir: dosdir.o dosplit.o dosdev.o dosopn.o \
        ; $(LD) dosdir.o dosplit.o dosdev.o dosopn.o -o dosdir -s

dosls:  dosls.o dosplit.o dosdev.o dosopn.o \
        ; $(LD) dosls.o dosplit.o dosdev.o dosopn.o -o dosls -s

dosmkdir:dosmkdir.o dosplit.o dosdev.o dosopn.o dosdel.o dosfat.o\
        ; $(LD) dosmkdir.o dosplit.o dosdev.o dosopn.o dosin.o dosdel.o \
        dosfat.o -o dosmkdir -s

dosrmdir:dosrmdir.o dosplit.o dosdev.o dosopn.o dosdel.o \
        ; $(LD) dosrmdir.o dosplit.o dosdev.o dosopn.o dosdel.o -o dosrmdir -s

dosformat: dosformat.o dosplit.o \
        ; $(LD) dosformat.o dosplit.o -o dosformat -s

$(OBJECTS): dos.h
dosdev.o:  dosdev.c
dosplit.o: dosplit.c $I/stdio.h $I/ctype.h
dosopn.o:  dosopn.c
dosdel.o:  dosdel.c
dosfat.o:  dosfat.c
dosin.o:   dosin.c $I/time.h $I/sys/types.h $I/sys/stat.h
dosout.o:  dosout.c
doscat.o:  doscat.c $I/stdio.h $I/sys/types.h $I/sys/stat.h
dosrm.o:   dosrm.c $I/stdio.h $I/sys/types.h $I/sys/stat.h
doscp.o:   doscp.c $I/stdio.h $I/ctype.h $I/sys/types.h $I/sys/stat.h
dosdir.o:  dosdir.c $I/stdio.h $I/sys/types.h $I/sys/stat.h
dosls.o:   dosls.c $I/stdio.h $I/sys/types.h $I/sys/stat.h
dosmkdir.o: dosmkdir.c $I/time.h $I/stdio.h $I/sys/types.h $I/sys/stat.h
dosrmdir.o: dosrmdir.c $I/stdio.h $I/sys/types.h $I/sys/stat.h
dosformat.o: dosformat.c $I/stdio.h $I/sys/types.h $I/sys/stat.h $I/fcntl.h
 
