#!/usr/bin/env python3
"""gen_makefile.py - write src/cmd/ld/Makefile (link of sld from the C files, the members of slibc.a and the two old objects).

The link order is the order of /bin/sld: scrt0.o, the C files (tools/module_ranges.txt), then the libc objects in the order of
host/tools/oldslibc_cut.py (OBJECTS) and the data-only members. All libc objects are members of /lib/slibc.a except printf and
doprnt (another revision), which come from src/lib/oldslibc as cas sources.
"""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', '..', '..', 'host', 'tools'))
sys.path.insert(0, HERE)
import oldslibc_cut as C  # noqa: E402
import ldsrc  # noqa: E402

# data-only members of slibc.a (no code in sld); their data lies behind the data of the object named here (measured in the data of sld)
DATAOBJS = [('data', 'filbuf'), ('ctype_', 'malloc')]


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
    mods = [f[:-2] for f, _, _ in ldsrc.modules()]
    members = [o for o, a, b, k in C.OBJECTS if k != 'old'] + [d for d, _ in DATAOBJS]
    old = [o for o, a, b, k in C.OBJECTS if k == 'old']
    libc = []
    for o, a, b, k in C.OBJECTS:
        libc.append(('$(OLD)/%s/$(REV).o' % o) if k == 'old' else 'slibc/%s.o' % o)
        libc += ['slibc/%s.o' % d for d, after in DATAOBJS if after == o]
    mk = '''# ld: the segmented loader of WEGA (sld), reconstructed from /bin/sld.
#
#   make             compile the C files, assemble the start code and the old libc objects (printf, doprnt)
#   make link        sld.new: scrt0.o, the C files, the members of /lib/slibc.a and the two old objects, linked by the original
#                    sld (SLDBIN: segmented program, -i -z -e start)
#   make linkown     sld.own: the same with the cut own code (sld_own.s, 0x56..0x71ca of /bin/sld) instead of the C files
#   make clean
#   (this file is written by tools/gen_makefile.py)
#
# The libc part of sld (0x71ca..0x9730) consists of members of /lib/slibc.a in the order of /bin/sld; only printf and doprnt are
# another revision than the shipped members: their assembler sources are in ../../lib/oldslibc/<object>/<revision>.s (the
# revision number REV is invented). The code of the other objects equals the members (host/tools/oldslibc_cut.py checks that).

CRT = ../../lib/libc/crt
OLD = ../../lib/oldslibc
REV = 0_1
SLDBIN = /bin/sld
CAS = cas
AR = ar
SCC = scc
CFLAGS = -O
SLIBC = /lib/slibc.a

# the C files of sld (code ranges: tools/module_ranges.txt); each one is compiled alone
%s
# members of slibc.a (extracted into slibc/)
MEMBERS = %s
OLDOBJS = %s

# link order of /bin/sld
%s
LINKOBJS = scrt0.o $(COBJS) $(LIBCOBJS)
OWNOBJS = scrt0.o sld_own.o $(LIBCOBJS)

all: $(COBJS) scrt0.o $(OLDOBJS)

link: sld.new

sld.new: $(LINKOBJS)
	$(SLDBIN) -i -z -e start $(LINKOBJS) -o $@

linkown: sld.own

sld.own: $(OWNOBJS)
	$(SLDBIN) -i -z -e start $(OWNOBJS) -o $@

slibc/stamp:
	mkdir slibc ; cd slibc ; $(AR) x $(SLIBC) $(MEMBERS) ; touch stamp

$(MEMBERS:%%=slibc/%%): slibc/stamp

scrt0.s: $(CRT)/mkcrt0.sh $(CRT)/scrt0.s.in $(CRT)/crt0.list
	sh $(CRT)/mkcrt0.sh sld > $@

scrt0.o: scrt0.s
	$(CAS) -u -o $@ scrt0.s

sld_own.o: sld_own.s
	$(CAS) -u -o $@ sld_own.s

''' % (wrap('COBJS = ', [m + '.o' for m in mods]),
       ' '.join(o + '.o' for o in [o for o, a, b, k in C.OBJECTS if k != 'old'] + [d for d, _ in DATAOBJS]),
       ' '.join('$(OLD)/%s/$(REV).o' % o for o in old),
       wrap('LIBCOBJS = ', libc))
    # the make of the WEGA does not know pattern substitution: write the dependency of every member
    mk = mk.replace('$(MEMBERS:%=slibc/%): slibc/stamp\n\n', ''.join('slibc/%s.o: slibc/stamp\n' % o for o in members) + '\n')
    for m in mods:
        mk += '%s.o: %s.c ld.h\n\t$(SCC) $(CFLAGS) -c %s.c\n\n' % (m, m, m)
    for o in old:
        mk += '$(OLD)/%s/$(REV).o: $(OLD)/%s/$(REV).s\n\t$(CAS) -u -o $@ $(OLD)/%s/$(REV).s\n\n' % (o, o, o)
    mk += 'clean:\n\trm -f $(COBJS) $(OLDOBJS) scrt0.s scrt0.o sld_own.o sld.new sld.own\n\trm -rf slibc\n'
    open(os.path.join(HERE, '..', 'Makefile'), 'w').write(mk)


if __name__ == '__main__':
    main()
