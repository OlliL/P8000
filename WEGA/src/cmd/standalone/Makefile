FILES=  	sa.cat sa.format sa.verify sa.mkfs sa.install sa.timer sa.shipdisk
CLEANFILES=	libb.a sa.wdc.o bmisc.o sa.function.o sa.cat.o sa.format.o \
		sa.verify.o sa.mkfs.o sa.install.o sa.timer.o sa.shipdisk.o \
		$(FILES)
BMISC_C=	../../uts/conf/bmisc.c

all:		$(FILES)

clean:
		rm -f $(CLEANFILES)

bmisc.o:	$(BMISC_C)
		cc -O -c -o bmisc.o $(BMISC_C)
		
libb.a:		sa.function.o bmisc.o
		rm -f libb.a
		ar qc libb.a sa.function.o bmisc.o
		chmod 644 libb.a

sa.mkfs:	sa.mkfs.o libb.a
		ld -s -o $@ sa.mkfs.o libb.a
		chmod 400 $@

sa.cat:		sa.cat.o libb.a
		ld -s -o $@ sa.cat.o libb.a
		chmod 400 $@

sa.timer:	sa.timer.o libb.a
		ld -s -o $@ sa.timer.o libb.a
		chmod 400 $@

sa.install:	sa.install.o libb.a
		ld -s -o $@ sa.install.o libb.a
		chmod 400 $@

sa.format:	sa.format.o sa.wdc.o libb.a
		ld -s -o $@ sa.format.o sa.wdc.o libb.a
		chmod 400 sa.format

sa.verify:	sa.verify.o sa.wdc.o libb.a
		ld -s -o $@ sa.verify.o sa.wdc.o libb.a
		chmod 400 sa.format

sa.shipdisk:	sa.shipdisk.o sa.wdc.o libb.a
		ld -s -o $@ sa.shipdisk.o sa.wdc.o libb.a
		chmod 400 sa.format
