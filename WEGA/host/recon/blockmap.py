#!/usr/bin/env python3
"""blockmap.py - the blocks of the original function mapped onto the blocks of ours (configuration: progcfg.py).

Usage: blockmap.py NAME...      (BV=-v: print our blocks too)

A block ends with an unconditional jump, a jp @r or a ret, and starts at the target of a jump. Both sides are taken in
the strict form of strictcmp.py; each block of the original is looked up among ours (exact, or `~n` when all but the
last instruction are equal, `?` when not found). The printed order is the order in which the original lays out our
blocks: it shows the block moves of the old popt (D11, D15, D20) without guessing the rule.
"""
import sys,os,json,re
import progcfg,zcode
CFG=progcfg.load(); sys.path.insert(0,progcfg.TOOLS)
from sout import SOut
g=SOut(open(os.path.join(CFG.ROOT,CFG.ORIG_BIN),'rb').read()); gc=bytes(g.image)[:g.segs[0]['code']]
o=SOut(open(CFG.STRICT_INPUT,'rb').read()); oc=bytes(o.image)[:o.segs[0]['code']]
sym={s['name']:s['value'] for s in o.symbols}
text=sorted(set(s['value'] for s in o.symbols if s['type'] in (31,34)))
F=json.load(open(CFG.FINDINGS))['functions']
ofn=set(s['value'] for s in o.symbols if s['type']==34); gfn=set(int(f['orig'],16) for f in F.values())
def blocks(ins):
    # ins: list of (addr, text); targets of jumps start blocks too
    tg=set()
    for a,t in ins:
        m=re.search(r'(jr|jp|djnz|calr|call)\t(?:\w+,)?%([0-9A-F]{4})$',t)
        if m and m.group(1) in ('jr','jp','djnz'): tg.add(int(m.group(2),16))
    bl=[]; cur=[]
    for a,t in ins:
        if a in tg and cur: bl.append(cur); cur=[]
        cur.append((a,t))
        if re.match(r'(jr|jp)\t[^,]*$',t) or re.match(r'ret',t) or re.match(r'jp\t@',t):
            bl.append(cur); cur=[]
    if cur: bl.append(cur)
    return bl
def norm(t, fns, d):
    t=zcode.norm_strict(t, fns, d18=CFG.D18)
    t=re.sub(r'\b(jr|jp)\b','J',t)
    return t
for n in sys.argv[1:]:
    f=F[n]; lo=int(f['orig'],16)
    a=list(zcode.insns(gc,lo,lo+f['size_orig'],CFG.SEG))
    s=sym['_'+n]; e=min([t for t in text if t>s]+[len(oc)])
    b=list(zcode.insns(oc,s,e,CFG.SEG))
    ba=blocks(a); bb=blocks(b)
    ka=[tuple(norm(t,gfn,1) for _,t in x) for x in ba]
    kb=[tuple(norm(t,ofn,1) for _,t in x) for x in bb]
    used=set(); order=[]
    for k in ka:
        j=next((j for j,x in enumerate(kb) if x==k and j not in used),None)
        if j is None:
            # partial: same block without its last instruction
            j=next((j for j,x in enumerate(kb) if j not in used and x[:-1]==k[:-1] and len(k)>1),None)
            order.append('~%s'%j if j is not None else '?')
        else: order.append(str(j))
        if j is not None: used.add(j)
    print(n, 'orig blocks', len(ba), 'ours', len(bb))
    print('  orig order as our block numbers:', ' '.join(order))
    if '-v' in os.environ.get('BV',''):
        for i,x in enumerate(bb): print('  ours %2d: %s'%(i, ' ; '.join(t for _,t in x)[:110]))
