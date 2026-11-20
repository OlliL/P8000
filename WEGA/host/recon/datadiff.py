#!/usr/bin/env python3
"""datadiff.py - the data segment of src/cmd/gopt/gopt against /lib/gopt byte by byte; the words that are code addresses
(84 00 / 85 00 in both: segment 4 and 5) are skipped. Prints the count and the groups of differing bytes."""
import sys
sys.path.insert(0,'/home/olivleh1/git/P8000/WEGA/host/tools')
from sout import SOut
o=SOut(open('/home/olivleh1/git/P8000/tmp/wega31_root/lib/gopt','rb').read())
n=SOut(open('/home/olivleh1/git/P8000/WEGA/src/cmd/gopt/gopt','rb').read())
od=bytes(o.image)[65534+50432:]; nd=bytes(n.image)[sum(s['code'] for s in n.segs):]
diffs=[];i=0
while i<len(od):
    if od[i:i+2] in (b'\x84\x00',b'\x85\x00') and nd[i:i+2] in (b'\x84\x00',b'\x85\x00'):
        i+=4; continue
    if od[i]!=nd[i]: diffs.append(i)
    i+=1
print('differing data bytes (code addresses masked):',len(diffs))
grp=[];s=None
for d in diffs:
    if s is None or d>e+8:
        if s is not None: grp.append((s,e))
        s=d
    e=d
if s is not None: grp.append((s,e))
for a,b in grp: print(hex(a),hex(b), od[a:b+1][:16], nd[a:b+1][:16])
