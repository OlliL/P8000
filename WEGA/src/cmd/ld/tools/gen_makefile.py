#!/usr/bin/env python3
"""gen_makefile.py - write src/cmd/ld/Makefile from tools/ld.link (the libc part of /bin/ld in link order).

The libc objects are the shipped members of /lib/libc.a (extracted into shipped/ with ar x) and the old modules of
src/lib/oldlibc (assembled with cas -u). The own code of ld is not in this Makefile yet (step 3 of HOWTO_reconstruct.md).
Model: src/cmd/sld/tools/gen_makefile.py. The make of the WEGA does not know pattern rules: one rule per object.
"""
import os

HERE = os.path.dirname(os.path.abspath(__file__))
LD = os.path.join(HERE, '..')


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
    for line in open(os.path.join(HERE, 'ld.link')):
        if line.startswith('#') or not line.strip():
            continue
        f = line.split()
        rows.append((f[0], f[1], f[2] if len(f) > 2 else None))
    shipped = [n for k, n, v in rows if k == 'shipped']
    old = [(n, v) for k, n, v in rows if k == 'old']
    libc = []
    for k, n, v in rows:
        libc.append('shipped/%s.o' % n if k == 'shipped' else '$(OLD)/%s/%s.o' % (n, v))
    mk = '''# ld: the nonsegmented loader of WEGA (/bin/ld, E711, SCCS ld.c 1.18), reconstructed from /bin/ld.
#
#   make             the libc part of ld: the shipped members (ar x from /lib/libc.a) and the old modules (cas -u of
#                    ../../lib/oldlibc), in the link order of /bin/ld (tools/ld.link, tools/ld.spec)
#   make check       tools/libc_verify.py: every module against the original bytes of /bin/ld (needs the file tree of the
#                    original system, WEGA=dir)
#   make clean
#   (this file is written by tools/gen_makefile.py; the own code of ld is not in it yet: step 3)
#
# The own code (copyname.c, printname.c, symbols.c, ld.c, crt0 from mkcrt0.sh) comes later. Relocated words of the
# old modules point to the own code and the data of ld; they are checked when the link exists.

CRT = ../../lib/libc/crt
OLD = ../../lib/oldlibc
CAS = cas
AR = ar
LIBC = /lib/libc.a
WEGA = ../../../tmp/wega31_root

# shipped members in link order (the members of /lib/libc.a), extracted into shipped/
SHIPPED = %s

# link order of the libc part of /bin/ld
%s

all: $(LIBCOBJS)

shipped/stamp:
	mkdir shipped ; cd shipped ; $(AR) x $(LIBC) %s ; touch stamp

''' % (' '.join(n + '.o' for n in shipped), wrap('LIBCOBJS = ', libc), ' '.join(n + '.o' for n in shipped))
    for n in shipped:
        mk += 'shipped/%s.o: shipped/stamp\n' % n
    mk += '\n'
    for n, v in old:
        mk += '$(OLD)/%s/%s.o: $(OLD)/%s/%s.s\n\t$(CAS) -u -o $@ $(OLD)/%s/%s.s\n\n' % (n, v, n, v, n, v)
    mk += '''check:
	cd tools ; WEGA=$(WEGA) python3 libc_verify.py

clean:
	rm -rf shipped
	rm -f $(LIBCOBJS)
'''
    open(os.path.join(LD, 'Makefile'), 'w').write(mk)


if __name__ == '__main__':
    main()
