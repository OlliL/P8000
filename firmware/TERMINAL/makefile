AS=u8as
LD=ld
STRIP=strip -h
FIRMWARE=p8t_1_5.0
OBJECTS=p8t.init.o \
	p8t.main.o \
	p8t.up.o \
	p8t.esc.o \
	p8t.vt100.o

all: $(OBJECTS)
	$(LD) -x -o $(FIRMWARE) -bt 0 $(OBJECTS)
	$(STRIP) $(FIRMWARE)

clean:
	rm -f $(FIRMWARE) $(OBJECTS)

