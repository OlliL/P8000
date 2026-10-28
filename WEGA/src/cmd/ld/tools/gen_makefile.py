#!/usr/bin/env python3
"""gen_makefile.py - write src/cmd/ld/Makefile from the object list of host/tools/oldslibc_cut.py (link order, shipped / cut / old)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', '..', '..', 'host', 'tools'))
sys.path.insert(0, HERE)
import oldslibc_cut as C  # noqa: E402
import ldsrc  # noqa: E402

C_MODULES = ldsrc.modules()


def wrap(head, items, width=100):
    lines, cur = [], head
    for n in items:
        if len(cur) + len(n) + 1 > width:
            lines.append(cur.rstrip() + ' \\')
            cur = '\t'
        cur += n + ' '
    lines.append(cur.rstrip())
    return '\n'.join(lines)


# data-only members of slibc.a that the shipped objects refer to (no code in sld; their place in the data is open)
DATAOBJS = ['data', 'ctype_']


def main():
    objs = C.OBJECTS
    ship = [o for o, a, b, k in objs if k == 'shipped'] + DATAOBJS
    cut = [o for o, a, b, k in objs if k != 'shipped']
    link = [('$(OLD)/%s/$(REV).o' % o) if k != 'shipped' else o + '.o' for o, a, b, k in objs] + [o + '.o' for o in DATAOBJS]
    mk = '''# ld: the segmented loader of WEGA (sld, gld), reconstructed from /bin/sld. This Makefile builds the libc part of sld.
# The libc part of sld (0x71ca..0x9730) is a sequence of members of slibc.a, in the order the original linker took them.
# Objects whose code equals the member of /lib/slibc.a are taken from there (SHIPOBJS, ar x). The others have a directory
# ../../lib/oldslibc/<object>/<revision>.s, cut from /bin/sld and verified with host/tools/oldslibc_cut.py (the revision number REV
# is invented: other programs carry other revisions of the same objects). printf and doprnt are another revision than the member
# of /lib/slibc.a; the other cut objects equal their members (oldslibc_cut.py checks that) and can be dropped when the data is cut.
#
#   make              libc objects of sld, the own code of sld (sld_own.o) and the start-up code (scrt0.o)
#   make cobjs       compile the C files of sld (not linked yet: the data of sld is not cut)
#   make link         sld.new, linked from these objects by the original sld (SLDBIN)
#   make linkslibc    sld.new.slibc: same link, but all objects (except printf, doprnt) are the members of /lib/slibc.a
#   make clean
#   (this file is written by tools/gen_makefile.py)

CRT = ../../lib/libc/crt
OLD = ../../lib/oldslibc
REV = 0_1
# linker for the test link: the original sld (segmented program: -i -z -e start)
SLDBIN = /bin/sld
CAS = cas
AR = ar
SLIBC = /lib/slibc.a
SCC = scc
CFLAGS = -O

# the C files of sld (code ranges: tools/module_ranges.txt); each one is compiled alone
COBJS = %s

# members of slibc.a that are identical to the original code of sld
SHIPOBJS = %s
# link order of sld (= address order in /bin/sld): start code, own code, then the libc objects
%s
# the same with the objects of slibc.a (printf and doprnt: the old revision)
%s
%s

all: libc own start

cobjs: $(COBJS)

libc: $(LIBCOBJS) $(SHIPOBJS)

# the link: not byte identical yet (the data of sld is not cut, the libc data spans are at other offsets)
link: sld.new

sld.new: $(LINKOBJS)
	$(SLDBIN) -i -z -e start $(LINKOBJS) -o $@

linkslibc: sld.new.slibc

slibc/stamp:
	mkdir slibc ; cd slibc ; $(AR) x $(SLIBC) %s ; touch stamp

sld.new.slibc: slibc/stamp $(SLIBCOBJS)
	$(SLDBIN) -i -z -e start $(SLIBCOBJS) -o $@

# own code of sld (0x56..0x71ca of /bin/sld, one block; cut with host/tools/oldslibc_cut.py, verified)
own: sld_own.o

# start-up code (0x18..0x55 of /bin/sld): written by mkcrt0.sh from the template scrt0.s.in (row sld in crt0.list)
start: scrt0.o

scrt0.s: $(CRT)/mkcrt0.sh $(CRT)/scrt0.s.in $(CRT)/crt0.list
	sh $(CRT)/mkcrt0.sh sld > $@

scrt0.o: scrt0.s
	$(CAS) -u -o $@ scrt0.s

sld_own.o: sld_own.s
	$(CAS) -u -o $@ sld_own.s

''' % (' '.join(f[:-2] + '.o' for f, _, _ in C_MODULES), ' '.join(o + '.o' for o in ship),
       wrap('LINKOBJS = ', ['scrt0.o', 'sld_own.o'] + link),
       wrap('SLIBCOBJS = ', ['scrt0.o', 'sld_own.o'] + [('$(OLD)/%s/$(REV).o' % o) if k == 'old' else 'slibc/%s.o' % o
                                                      for o, a, b, k in objs] + ['slibc/%s.o' % o for o in DATAOBJS]),
       wrap('LIBCOBJS = ', ['$(OLD)/%s/$(REV).o' % o for o in cut]),
       ' '.join(o + '.o' for o, a, b, k in objs if k != 'old') + ' ' + ' '.join(o + '.o' for o in DATAOBJS))
    for f, _, _ in C_MODULES:
        mk += '%s.o: %s ld.h\n\t$(SCC) $(CFLAGS) -c %s\n\n' % (f[:-2], f, f)
    for o in ship:
        mk += '%s.o:\n\t$(AR) x $(SLIBC) %s.o\n\n' % (o, o)
    for o in cut:
        mk += '$(OLD)/%s/$(REV).o: $(OLD)/%s/$(REV).s\n\t$(CAS) -u -o $@ $(OLD)/%s/$(REV).s\n\n' % (o, o, o)
    mk += 'clean:\n\trm -f $(COBJS) $(LIBCOBJS) $(SHIPOBJS) sld_own.o scrt0.s scrt0.o sld.new sld.new.slibc\n\trm -rf slibc\n'
    open(os.path.join(HERE, '..', 'Makefile'), 'w').write(mk)


if __name__ == '__main__':
    main()
