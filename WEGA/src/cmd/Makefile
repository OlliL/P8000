FILES=		getud putud
STRIPFILES=	copydisk dump dumpdir format restor tar verify

all: $(FILES) $(STRIPFILES) strip

strip:
	strip $(STRIPFILES)

clean:
	rm -f $(FILES) $(STRIPFILES)
