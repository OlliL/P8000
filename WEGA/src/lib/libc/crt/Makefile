AS=	as
ASFLAGS=
OBJ=	cerror.o mcount.o

all: $(OBJ)

clean:
	rm -f $(OBJ) *.o crt0-size.s crt0-strip.s

# the members of libc.a (called by ../Makefile)
objs: ../obj/cerror.o ../obj/mcount.o

../obj/cerror.o: cerror.s
	$(AS) -o ../obj/cerror.o cerror.s

../obj/mcount.o: mcount.s
	$(AS) -o ../obj/mcount.o mcount.s
