boot:		bstart.o BOOTLIB
		ld -s -e start -b 0x8000 -o boot bstart.o BOOTLIB

