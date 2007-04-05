VERSION=5

AS=u8as
LD=ld
STRIP=strip -h
FIRMWARE=p8t_1_$(VERSION)
OBJECTS=p8t.$(VERSION).init.o \
	p8t.$(VERSION).main.o \
	p8t.$(VERSION).up.o \
	p8t.$(VERSION).esc.o \
	p8t.$(VERSION).vt100.o

all: $(OBJECTS)
	$(LD) -x -o $(FIRMWARE) -bt 0 $(OBJECTS)
	$(STRIP) $(FIRMWARE)

clean:
	rm -f $(FIRMWARE) $(OBJECTS)

