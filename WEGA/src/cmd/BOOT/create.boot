all:		pb.image boot0.fd boot0.ud boot0.rm boot libb.a sa.cat sa.format sa.verify sa.mkfs sa.install sa.timer

pb.image:	boot0.md.s
		@echo pb.image:
		as -u -o boot0.md.o boot0.md.s
		ld -s -o pb.image boot0.md.o
		strip -h pb.image
		chmod 400 pb.image
		rm boot0.md.o
		@echo

boot0.fd:	boot0.fd.s
		@echo boot0.fd:
		as -u -o boot0.fd.o boot0.fd.s
		ld -s -o boot0.fd boot0.fd.o
		strip -h boot0.fd
		chmod 400 boot0.fd
		rm boot0.fd.o
		@echo
 
boot0.ud:	boot0.ud.s
		@echo boot0.ud:
		as -u -o boot0.ud.o boot0.ud.s
		ld -s -o boot0.ud boot0.ud.o
		strip -h boot0.ud
		chmod 400 boot0.ud
		rm boot0.ud.o
		@echo

boot0.rm:	boot0.rm.s
		@echo boot0.rm:
		as -u -o boot0.rm.o boot0.rm.s
		ld -s -o boot0.rm boot0.rm.o
		strip -h boot0.rm
		chmod 400 boot0.rm
		rm boot0.rm.o
		@echo
 
boot:		bstart.c BOOTLIB
		@echo boot:
		cc -c -O bstart.c
		ld -s -e start -b 0x8000 -o boot bstart.o BOOTLIB
		chmod 400 boot
		rm bstart.o
		@echo
 
BOOTLIB:	bmch.s boot.c bsys.c bconf.c bmisc.c bmd.c bfd.c bud.s brm.s
		@echo BOOTLIB:
		as -u -o bmch.o bmch.s
		cc -c -O boot.c
		cc -c -O bsys.c
		cc -c -O bconf.c
		cc -c -O bmisc.c
		cc -c -O bmd.c
		cc -c -O bfd.c
		as -u -o bud.o bud.s
		as -u -o brm.o brm.s
		rm -f BOOTLIB
		ar qc BOOTLIB bmch.o boot.o bsys.o bconf.o bmisc.o bmd.o bfd.o bud.o brm.o
		chmod 644 BOOTLIB
		rm bmch.o boot.o bsys.o bconf.o bmisc.o bmd.o bfd.o bud.o brm.o
		@echo
 
libb.a:		sa.function.s bmisc.c
		@echo libb.a:
		as -u -o sa.function.o sa.function.s
		cc -c -O bmisc.c
		rm -f libb.a
		ar qc libb.a sa.function.o bmisc.o
		chmod 644 libb.a
		rm sa.function.o bmisc.o
		@echo

sa.mkfs:	sa.mkfs.c libb.a
		@echo sa.mkfs:
		cc -c -O sa.mkfs.c
		ld -s -o sa.mkfs sa.mkfs.o libb.a
		chmod 400 sa.mkfs
		rm sa.mkfs.o
		@echo

sa.cat:		sa.cat.c libb.a
		@echo sa.cat:
		cc -c -O sa.cat.c
		ld -s -o sa.cat sa.cat.o libb.a
		chmod 400 sa.cat
		rm sa.cat.o
		@echo

sa.timer:	sa.timer.c libb.a
		@echo sa.timer:
		cc -c -O sa.timer.c
		ld -s -o sa.timer sa.timer.o libb.a
		chmod 400 sa.timer
		rm sa.timer.o
		@echo

sa.install:	sa.install.c libb.a
		@echo sa.install:
		cc -c -O sa.install.c
		ld -s -o sa.install sa.install.o libb.a
		chmod 400 sa.install
		rm sa.install.o
		@echo

sa.format:	sa.format.c sa.wdc.s libb.a
		@echo sa.format:
		cc -c -O sa.format.c
		as -u -o sa.wdc.o sa.wdc.s
		ld -s -o sa.format sa.format.o sa.wdc.o libb.a
		chmod 400 sa.format
		rm sa.format.o sa.wdc.o
		@echo

sa.verify:	sa.verify.c sa.disk.s libb.a
		@echo sa.verify:
		cc -c -O sa.verify.c
		as -u -o sa.wdc.o sa.wdc.s
		ld -s -o sa.verify sa.verify.o sa.wdc.o libb.a
		chmod 400 sa.verify
		rm sa.verify.o sa.wdc.o
		@echo
