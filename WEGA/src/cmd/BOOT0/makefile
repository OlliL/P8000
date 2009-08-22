all:		pb.image boot0.fd boot0.ud boot0.rm boot

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
