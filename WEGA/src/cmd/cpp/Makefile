# cpp: the C preprocessor, reconstructed from /lib/cpp after the cpp of cmd/cc in the Plexus tree. In work: the libc part
# is not split yet, the model of tools/progcfg.py links the shipped libc.a.
#
#   make cpy.c       cpy.c from cpy.y with yacc
YACC = yacc

cpy.c: cpy.y
	$(YACC) cpy.y
	mv y.tab.c cpy.c
