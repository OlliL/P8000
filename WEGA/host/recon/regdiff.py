#!/usr/bin/env python3
"""regdiff.py FILE.diff - the diff of fnc.py without register names: what is structurally different (orig-only / ours-only instructions)."""
import re,sys,collections
f=sys.argv[1]
minus=[];plus=[]
for l in open(f):
    if l.startswith('---') or l.startswith('+++') or l.startswith('@@'): continue
    if l[0]=='-': minus.append(l[1:].strip())
    elif l[0]=='+': plus.append(l[1:].strip())
def norm(s): return re.sub(r'\b(rr|rl|rh|r)\d+\b','R',s)
a=collections.Counter(map(norm,minus)); b=collections.Counter(map(norm,plus))
ra=a-b; rb=b-a
print('structural: orig-only',sum(ra.values()),'ours-only',sum(rb.values()))
for k,v in list(ra.items())[:12]: print(' -',v,k)
for k,v in list(rb.items())[:12]: print(' +',v,k)
