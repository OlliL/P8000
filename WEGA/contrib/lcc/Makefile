# created by Oliver Lehmann
# 2009-07-29

C_FILES = \
 cmd/cpp/cpy.c \
 cmd/comp/u8k/sty.c \
 cmd/comp/common/cgram.c

BIN_FILES = \
 cmd/cpp/lcpp \
 cmd/c2/lcopt \
 cmd/comp/u8k/ccom \
 cmd/comp/u8k/sty \
 cmd/ar \
 cmd/lcc \
 cmd/lld

LIB_FILES = \
 lib/libc/lcrt0.o \
 lib/libc/lib.Llibc \
 lib/libm/Llibm.a \
 lib/libc/Llib.contents

clean: 
	-rm -f lib/libc/port/gen/*.o
	-rm -f lib/libc/port/print/*.o
	-rm -f lib/libc/port/stdio/*.o
	-rm -f lib/libc/u8k/crt/*.o
	-rm -f lib/libc/u8k/csu/*.o
	-rm -f lib/libc/u8k/gen/*.o
	-rm -f lib/libc/u8k/sys/*.o
	-rm -f cmd/*.o cmd/*/*.o cmd/comp/*/*.o
	-rm -f $(C_FILES)
	-rm -f $(BIN_FILES)
	-rm -f $(LIB_FILES)

bootstrap:
	cd cmd ; make CC=cc CFLAGS= INCLIST="-I. -I/z/LCC/h -I/z/LCC/cmd/comp/common" all
	-mkdir /z/LCC/bin
	cd cmd ; make install
all:
	cd cmd ; make $@
	cd lib/libc ; make $@
	cd lib/libm ; make $@

install: all
	cd cmd ; make $@
	cd lib/libc ; make $@
	cd lib/libm ; make $@
	cd h; make $@
