TESTDIR = .
FRC =
INS = /etc/install -o
INSDIR = 

all: ed

ed: ed.s
	$(CC) -s -n -o $(TESTDIR)/ed ed.s 

ed.s:	ed.c $(FRC)
	$(CC) -S -O ed.c
	-if pdp11; then ./edfun ed.s; fi

test:
	rtest $(TESTDIR)/ed

install: all
	$(INS) $(TESTDIR)/ed $(INSDIR)
	-rm -f /bin/red
	ln /bin/ed /bin/red

clean:
	-rm -f ed.s

clobber: clean
	-rm -f $(TESTDIR)/ed

FRC:
