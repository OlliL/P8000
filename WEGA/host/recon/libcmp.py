#!/usr/bin/env python3
"""libcmp.py - the libc part of src/cmd/gopt/gopt against /lib/gopt instruction by instruction (masked): segment 5 from
5:9bbe (ours from __fltuse) and segment 4 from 4:fc3a (ours from _fseek)."""
import sys,os,re,difflib
os.environ['RECON_PROG']='gopt'
sys.path.insert(0,'/home/olivleh1/git/P8000/WEGA/host/recon'); sys.path.insert(0,'/home/olivleh1/git/P8000/WEGA/host/tools')
import progcfg, zcode
from sout import SOut
CFG=progcfg.load()
o=SOut(open('/home/olivleh1/git/P8000/tmp/wega31_root/lib/gopt','rb').read())
n=SOut(open('/home/olivleh1/git/P8000/WEGA/src/cmd/gopt/gopt','rb').read())
def segs(s):
    out,off={},0
    for g in s.segs:
        if g['code']: out[g['segno']]=bytes(s.image[off:off+g['code']]); off+=g['code']+g['data']
    return out
os_,ns=segs(o),segs(n)
sym={x['name']:x['value']&0xffffff for x in n.symbols}
nm=lambda t: zcode.norm_masked(re.sub(r'<<\d+>>','',t))
def ins(code,lo,hi): return [nm(t) for _,t in zcode.insns(code,lo,hi,True)]
# seg 5 libc
a=ins(os_[5],0x9bbe,len(os_[5])); b=ins(ns[5],sym['__fltuse'],len(ns[5]))
print('seg5 libc: orig',len(a),'ours',len(b))
d=[l for l in difflib.unified_diff(a,b,lineterm='',n=0) if l[:1] in '+-' and l[:3] not in ('---','+++')]
print('diff lines',len(d)); print('\n'.join(d[:30]))
# seg 4 libc
a=ins(os_[4],0xfc3a,len(os_[4])); b=ins(ns[4],sym.get('_fseek',0),len(ns[4]))
print('seg4 libc: orig',len(a),'ours',len(b))
d=[l for l in difflib.unified_diff(a,b,lineterm='',n=0) if l[:1] in '+-' and l[:3] not in ('---','+++')]
print('diff lines',len(d)); print('\n'.join(d[:20]))
