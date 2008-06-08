FILES= libb.a sa.cat sa.format sa.verify sa.mkfs sa.install sa.timer

all:		$(FILES)

clean:
		rm -f *.o $(FILES)

libb.a:		sa.function.s ../../uts/conf/bmisc.c
		@echo libb.a:
		as -u -o sa.function.o sa.function.s
		cc -c -O -o bmisc.o ../../uts/conf/bmisc.c
		rm -f libb.a
		ar qc libb.a sa.function.o bmisc.o
		chmod 644 libb.a
		@echo

sa.mkfs:	sa.mkfs.c libb.a
		@echo sa.mkfs:
		cc -c -O sa.mkfs.c
		ld -s -o sa.mkfs sa.mkfs.o libb.a
		chmod 400 sa.mkfs
		@echo

sa.cat:		sa.cat.c libb.a
		@echo sa.cat:
		cc -c -O sa.cat.c
		ld -s -o sa.cat sa.cat.o libb.a
		chmod 400 sa.cat
		@echo

sa.timer:	sa.timer.c libb.a
		@echo sa.timer:
		cc -c -O sa.timer.c
		ld -s -o sa.timer sa.timer.o libb.a
		chmod 400 sa.timer
		@echo

sa.install:	sa.install.c libb.a
		@echo sa.install:
		cc -c -O sa.install.c
		ld -s -o sa.install sa.install.o libb.a
		chmod 400 sa.install
		@echo

sa.wdc.o:	sa.wdc.s
		as -u -o sa.wdc.o sa.wdc.s

sa.format:	sa.format.c sa.wdc.o libb.a
		@echo sa.format:
		cc -c -O sa.format.c
		ld -s -o sa.format sa.format.o sa.wdc.o libb.a
		chmod 400 sa.format
		@echo

sa.verify:	sa.verify.c sa.wdc.o libb.a
		@echo sa.verify:
		cc -c -O sa.verify.c
		ld -s -o sa.verify sa.verify.o sa.wdc.o libb.a
		chmod 400 sa.verify
		@echo
