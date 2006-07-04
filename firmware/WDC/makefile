VERSION=4.2

AS=u80as
LD=ld
STRIP=strip -h
OBJECTS=wdc.firm.o

all: $(OBJECTS)
	$(LD) -o wdc.firm $(OBJECTS)
	$(STRIP) wdc.firm
	dd if=wdc.firm of=WDC_1_$(VERSION) bs=1 count=4096 >&/dev/null
	dd if=wdc.firm of=WDC_2_$(VERSION) bs=1 skip=4096 >&/dev/null

clean:
	rm -f WDC_1_$(VERSION) WDC_2_$(VERSION) wdc.firm $(OBJECTS)

