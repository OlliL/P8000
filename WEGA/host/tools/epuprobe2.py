#!/usr/bin/env python3
"""EPU probe step 2 (see epugen.py): run in an empty work directory,
WEGA=<original tree> python3 epuprobe2.py"""
import os
import json,subprocess,sys,binascii
exec(open(os.path.join(os.path.dirname(os.path.abspath(__file__)),'epuprobe1.py')).read().split("valid={}")[0])
valid=json.load(open('valid.json'))
F=['f%d'%i for i in range(8)]; Rr=['r%d'%i for i in range(16)]
MK=bytes.fromhex('1402DEADBEEF')
def expand(p):
    out=[]
    if p=='f0': return F
    if p=='f0,f1': return [a+','+b for a in F for b in F]
    if p=='f0,r4': return [a+','+r for a in F for r in Rr]
    if p=='r4,f0': return [r+','+a for a in F for r in Rr]
    if p=='f0,@r4': return [a+',@'+r for a in F for r in Rr]
    if p=='@r4,f0': return ['@'+r+','+a for a in F for r in Rr]
    return []
tab={}
for n,pl in valid.items():
    lines=[]
    for p in dict.fromkeys(pl):
        for o in expand(p): lines.append(n+'\t'+o)
    for k in range(0,len(lines),150):
        ch=lines[k:k+150]
        out=assemble([l+"\n\tldl\trr2,#%DEADBEEF" for l in ch])
        s=SOut(open('c.o','rb').read())
        off,size=[(o,z) for si,kk,o,z in s.section_ranges() if kk=='text'][0]
        parts=s.image[off:off+size].split(MK)[:-1]
        if len(parts)!=len(ch): print('mismatch',n,len(parts),len(ch)); continue
        for t,r in zip(ch,parts):
            tab.setdefault(binascii.hexlify(r).decode().upper(),[]).append(t)
json.dump(tab,open('tab.json','w'))
print(len(tab))
