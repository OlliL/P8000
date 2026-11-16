#!/usr/bin/env python3
"""mkutmp.py FILE LINE:NAME[:TIME] ...: a WEGA /etc/utmp (struct utmp: char ut_line[8], ut_name[8], long ut_time; the
Z8000 is big endian) for the cases of the corpus of write. A name of '' is an entry that is not in use."""
import struct
import sys

out = open(sys.argv[1], 'wb')
for a in sys.argv[2:]:
    p = a.split(':')
    out.write(struct.pack('>8s8sl', p[0].encode(), p[1].encode(), int(p[2]) if len(p) > 2 else 600000000))
out.close()
