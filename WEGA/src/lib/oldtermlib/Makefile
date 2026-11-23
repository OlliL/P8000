# oldtermlib: libtermlib-v1.a in .., the termcap library of the time of the old programs (ex 10/82), by v0 (ccv0).
#
# The sources are those of ../libtermlib (2.11BSD, the forms of WEGA's libtermlib.a) in an older state, which the shipped
# libtermlib.a (v1, 01/83) no longer has: termcap.c tnchktc computes tbuf + (strlen(tbuf) - 2); tgoto.c sets its locals
# behind a label (start:) so that ccv0 reads CM and destline from their frame homes, as the original does (a form of the
# compiler, not of the authors: host/OLDCC_PARAMREGS.md). tputs.c is that of ../libtermlib.
#
#   make        ../libtermlib-v1.a
#   make clean

CCV0 = ccv0
AR = ar
CFLAGS = -O -DCM_N -DCM_GT -DCM_B -DCM_D
OBJS = termcap.o tgoto.o tputs.o

all: ../libtermlib-v1.a

../libtermlib-v1.a: $(OBJS)
	rm -f $@
	$(AR) r $@ $(OBJS)

termcap.o: termcap.c
	$(CCV0) $(CFLAGS) -c termcap.c
tgoto.o: tgoto.c
	$(CCV0) $(CFLAGS) -c tgoto.c
tputs.o: tputs.c
	$(CCV0) $(CFLAGS) -c tputs.c

clean:
	rm -f ../libtermlib-v1.a $(OBJS)

.PHONY: all clean
