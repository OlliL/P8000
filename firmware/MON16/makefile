OBJS=   p.init.o p.term.o p.comm.o p.brk.o p.test.o p.gesa.o \
        p.ldsd.o p.boot.o p.dummy.o p.disk.o p.crc.o p.ram.o

all:	MON16

clean:
	rm -f MON16 ${OBJS}
MON16:  ${OBJS}
	ld ${OBJS} -o MON16
	nm MON16 >symbols
	strip -h MON16

