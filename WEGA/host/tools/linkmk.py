#!/usr/bin/env python3
"""linkmk.py DIR NAME COMPILER SOURCES...: write DIR/Makefile for a program linked from its own objects and a libc part.

DIR/NAME.link holds the libc part in link order, one module per line: `shipped NAME` (a member of /lib/libc.a, ar x)
or `old NAME 0_n` (src/lib/oldlibc, cas -u). COMPILER names the old compiler the program needs (ccv1, ccv0, ...).
The make of WEGA does not know pattern rules: one rule per object. Called once per program, the Makefile is kept in git.
"""
import os
import sys


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
    d, name, cc = sys.argv[1:4]
    srcs = sys.argv[4:]
    rows = []
    for line in open(os.path.join(d, name + '.link')):
        line = line.split('#')[0].strip()
        if line:
            f = line.split()
            rows.append((f[0], f[1], f[2] if len(f) > 2 else None))
    own = [s.rsplit('.', 1)[0] + '.o' for s in srcs]
    libc = ['shipped/%s.o' % n if k == 'shipped' else '$(OLD)/%s/%s.o' % (n, v) for k, n, v in rows]
    shipped = [n + '.o' for k, n, v in rows if k == 'shipped']
    old = [(n, v) for k, n, v in rows if k == 'old']
    mk = """# %(name)s: linked from its own objects and the libc part of the original (%(name)s.link, ../../lib/oldlibc): no libc is linked.
# With the old compiler (make CC=%(cc)s) and strip the result is byte-identical with the original.
# Written by host/tools/linkmk.py.
#
#   make              build ./%(name)s
#   make install PREFIX=dir
#   make clean

CC = cc
CAS = cas
LD = ld
AR = ar
CFLAGS = -O -I../cparse
PREFIX = /
LIBC = /lib/libc.a
CRT = ../../lib/libc/crt
OLD = ../../lib/oldlibc

%(ownl)s
%(libcl)s

all: %(name)s

%(name)s: crt0.o $(OWN) $(LIBCOBJS)
	$(LD) -s -e start -o $@ crt0.o $(OWN) $(LIBCOBJS)

crt0.s: $(CRT)/crt0.s.in $(CRT)/crt0.list $(CRT)/mkcrt0.sh
	sh $(CRT)/mkcrt0.sh %(name)s > $@

crt0.o: crt0.s
	$(CAS) -u -o $@ crt0.s

""" % dict(name=name, cc=cc, ownl=wrap('OWN = ', own), libcl=wrap('LIBCOBJS = ', libc))
    for s, o in zip(srcs, own):
        mk += '%s: %s\n\t$(CC) $(CFLAGS) -c %s\n\n' % (o, s, s)
    mk += wrap('# members of the shipped libc\nSHIPPED = ', shipped).replace('SHIPPED =  ', 'SHIPPED = ') + '\n\n'
    mk += 'shipped/stamp:\n\t-mkdir shipped\n\tcd shipped ; $(AR) x $(LIBC) $(SHIPPED) ; touch stamp\n\n'
    for k, n, v in rows:
        if k == 'shipped':
            mk += 'shipped/%s.o: shipped/stamp\n' % n
    mk += '\n# modules of the old libc\n'
    for n, v in old:
        mk += '$(OLD)/%s/%s.o: $(OLD)/%s/%s.s\n\t$(CAS) -u -o $@ $(OLD)/%s/%s.s\n\n' % (n, v, n, v, n, v)
    mk += 'install: %s\n\tcp %s $(PREFIX)/bin/%s\n\nclean:\n\trm -rf *.o crt0.s shipped %s\n\n.PHONY: all install clean\n' % (name, name, name, name)
    open(os.path.join(d, 'Makefile'), 'w').write(mk)


if __name__ == '__main__':
    main()
