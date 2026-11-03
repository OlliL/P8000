#!/usr/bin/env python3
"""gen_makefile.py - write src/cmd/cpp/Makefile from tools/cpp.link (the libc part of /lib/cpp in link order).

The libc objects are the shipped members of /lib/libc.a (extracted into shipped/ with ar x) and the old modules of
src/lib/oldlibc (assembled with cas -u). The own code of ld is not in this Makefile yet (step 3 of HOWTO_reconstruct.md).
Model: src/cmd/sld/tools/gen_makefile.py. The make of the WEGA does not know pattern rules: one rule per object.
"""
import os

HERE = os.path.dirname(os.path.abspath(__file__))


def wrap(head, items, width=100):
    lines, cur = [], head
    for n in items:
        if len(cur) + len(n) + 1 > width:
            lines.append(cur.rstrip() + ' \\')
            cur = '\t'
        cur += n + ' '
    lines.append(cur.rstrip())
    return '\n'.join(lines)


def main():
    rows = []
    for line in open(os.path.join(HERE, 'cpp.link')):
        if line.startswith('#') or not line.strip():
            continue
        f = line.split()
        rows.append((f[0], f[1], f[2] if len(f) > 2 else None))
    shipped = [n for k, n, v in rows if k == 'shipped']
    old = [(n, v) for k, n, v in rows if k == 'old']
    libc = []
    for k, n, v in rows:
        libc.append('shipped/%s.o' % n if k == 'shipped' else '$(OLD)/%s/%s.o' % (n, v))
    mk = '''# cpp: the C preprocessor of WEGA (/lib/cpp, SCCS cpp.c 1.5, cpy.y 1.2, yylex.c 1.1), reconstructed after the cpp of
# System III (cmd/cc/vax/cpp). /lib/cpp is a combined program (code and data together, no -i).
#
#   make             cpp: crt0, the own code, the libc part: the shipped members (ar x from /lib/libc.a) and the old modules
#                    (cas -u of ../../lib/oldlibc), in the link order of /lib/cpp (tools/cpp.link, tools/cpp.spec)
#   make clean
#   (this file is written by tools/gen_makefile.py)
#
# The sources are written for the old compiler: make CC=ccv0p

CC = cc
CRT = ../../lib/libc/crt
OLD = ../../lib/oldlibc
LDBIN = /bin/ld
CAS = cas
AR = ar
YACC = yacc
LIBC = /lib/libc.a
CFLAGS = -O

# shipped members in link order (the members of /lib/libc.a), extracted into shipped/
SHIPPED = %s

# link order of the libc part of /lib/cpp
%s
COBJS = cpp.o cpy.o
LINKOBJS = crt0.o $(COBJS) $(LIBCOBJS)

all: cpp

cpp: $(LINKOBJS)
	$(LDBIN) -e start $(LINKOBJS) -o $@

shipped/stamp:
	-mkdir shipped
	cd shipped ; $(AR) x $(LIBC) %s ; touch stamp

cpp.o: cpp.c
	$(CC) $(CFLAGS) -c cpp.c

cpy.o: cpy.c yylex.c
	$(CC) $(CFLAGS) -c cpy.c

cpy.c: cpy.y
	$(YACC) cpy.y
	sed -e 's/^YYSTYPE yylval, yyval;/extern YYSTYPE yylval; YYSTYPE yyval;/' -e 's/^\\(YYSTYPE yyv\\[.*\\)$$/\\1\\
YYSTYPE yylval;/' y.tab.c > cpy.c
	rm -f y.tab.c

crt0.s: $(CRT)/mkcrt0.sh $(CRT)/crt0.s.in $(CRT)/crt0.list
	sh $(CRT)/mkcrt0.sh cpp > $@

crt0.o: crt0.s
	$(CAS) -u -o $@ crt0.s

''' % (' '.join(n + '.o' for n in shipped), wrap('LIBCOBJS = ', libc), ' '.join(n + '.o' for n in shipped))
    for n in shipped:
        mk += 'shipped/%s.o: shipped/stamp\n' % n
    mk += '\n'
    for n, v in old:
        mk += '$(OLD)/%s/%s.o: $(OLD)/%s/%s.s\n\t$(CAS) -u -o $@ $(OLD)/%s/%s.s\n\n' % (n, v, n, v, n, v)
    mk += '''clean:
	rm -rf shipped
	rm -f $(LIBCOBJS)
	rm -f $(COBJS) crt0.s crt0.o cpy.c cpp
'''
    open(os.path.join(HERE, '..', 'Makefile'), 'w').write(mk)


if __name__ == '__main__':
    main()
