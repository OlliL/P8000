#!/usr/bin/env python3
"""cgcmp.py - compare the functions of a rebuilt codgen with the original /lib/codgen.

  cgcmp.py ORIG.dis MINE.dis MINE.nm all          list functions that differ (bag-of-instructions, old-cc effects masked)
  cgcmp.py ORIG.dis MINE.dis MINE.nm ADDR...      show the difference of the functions at the original addresses (hex)

Inputs: ORIG.dis, MINE.dis = `sdis.py -l BINARY` listings; MINE.nm = the output of `nm` of the own binary.
The program part of the original ends at 0xbbc4 (the C library follows). Functions are paired by normalised
instruction text and then through the call graph. Masked: addresses, frame offsets, call targets, condition
codes, scratch registers, argument spills, switch dispatch, extsb/subb, shifts (the effects of the older cc,
OLDCC.md). Used for KNOWLEDGE.md "codgen fidelity".
"""
import sys
ORIG,MINE,NM=sys.argv[1:4]; ARGS=sys.argv[4:]
OUT='/dev/null'
import re,sys,difflib,json,collections
def funcs(fn,limit):
    out=[];cur=[];start=None
    for l in open(fn).read().splitlines():
        if l.startswith('// segment') and 'data,' in l: break
        m=re.match(r'^([0-9a-f]{4}) ([0-9a-f ]+?)\t(.*)$',l)
        if not m: continue
        a=int(m.group(1),16);raw=m.group(3).strip()
        if a>=limit: break
        if start is None: start=a
        cur.append((a,raw))
        if raw=='ret': out.append((start,cur));cur=[];start=None
    return out
def norm(i):
    i=re.sub(r'L0t_[0-9a-f]+','L',i)
    i=re.sub(r'%([0-9A-F]{4})',lambda m:'%'+m.group(1) if int(m.group(1),16)<0x100 else 'X',i)
    return i
def score(f): return [norm(x[1]) for x in f[1]]

LIM=0xbbc4
A=funcs(ORIG,LIM);B=funcs(MINE,LIM)
nm={}
for l in open(NM):
    p=l.split()
    if len(p)==3 and p[1] in 'Tt': nm[int(p[0],16)]=p[2]
nmaddr={v:k for k,v in nm.items()}
def n3(raw):
    m=raw.split()[0]
    if m in('dec','inc','add','sub') and re.match(r'\w+\s+r15,',raw): return None
    if m=='ldm':
        r=re.findall(r'r(\d+)',raw); return 'ldm '+raw.split('#')[-1] if False else None
    if m=='ld' and re.match(r'ld\s+r\d+,r\d+$',raw): return None
    if m=='ldl' and re.match(r'ldl\s+rr\d+,rr\d+$',raw): return None
    s=re.sub(r'L0t_[0-9a-f]+','L',raw)
    s=re.sub(r'%[0-9A-F]{4}\(r15\)','F',s)
    s=re.sub(r'@r15','F',s)
    s=re.sub(r'^(call|calr)\s+.*',r'\1 T',s)
    s=re.sub(r'#%([0-9A-F]{4})',lambda x:'#%'+x.group(1) if int(x.group(1),16)<0x100 else '#X',s)
    s=re.sub(r'(?<!#)%([0-9A-F]{4})(?!\()','X',s)
    s=re.sub(r'%([0-9A-F]{4})\((r\d+)\)',lambda x:'X(rS)' if int(x.group(1),16)>=0x100 else x.group(0),s)
    s=re.sub(r'^(ld\s+r[567]),#%([0-9A-F]{4})',lambda x:x.group(1)+',#X' if int(x.group(2),16)>=0x20 else x.group(0),s)
    s=re.sub(r'\b(r[lh]?|rr|rq)\d+\b',r'\1S',s)
    return s
def seq(f): return [x for x in (n3(r) for a,r in f[1]) if x]
def sv(f):
    s=set()
    for a,raw in f[1][:8]:
        m=re.match(r'ldm\s+(%0000\(r15\)|%\w+\(r15\)|@r15),r(\d+),#(\d+)',raw)
        if m:
            for k in range(int(m.group(3))):
                if int(m.group(2))+k>=8: s.add(int(m.group(2))+k)
    return tuple(sorted(s))
SA={f[0]:seq(f) for f in A};SB={f[0]:seq(f) for f in B}
Af={f[0]:f for f in A};Bm={f[0]:f for f in B}
def tgt(t):
    if t.startswith('%'): return int(t[1:],16)
    if t in nmaddr: return nmaddr[t]
    m=re.match(r'L0t_([0-9a-f]+)',t)
    return int(m.group(1),16) if m else t
def calls(f):
    return [tgt(re.match(r'(?:call|calr)\s+(.*)',raw).group(1).strip()) for a,raw in f[1] if re.match(r'(call|calr)\s',raw)]
CA={a:calls(f) for a,f in Af.items()};CB={b:calls(f) for b,f in Bm.items()}
# ratios
R={}
for a in SA:
    for b in SB:
        x,y=SA[a],SB[b]
        if abs(len(x)-len(y))>max(10,len(x)//2) or not x or not y: continue
        # quick filter on multiset overlap
        R[(a,b)]=difflib.SequenceMatcher(None,x,y,autojunk=False).ratio()
M={}
byA=collections.defaultdict(list)
for (a,b),r in R.items(): byA[a].append((r,b))
for a,l in byA.items():
    l.sort(reverse=True)
    if l[0][0]>=0.9 and (len(l)==1 or l[0][0]>l[1][0]+0.04): M[a]=l[0][1]
for it in range(30):
    new=0
    for a,b in list(M.items()):
        x,y=CA[a],CB[b]
        if len(x)!=len(y): continue
        for p,q in zip(x,y):
            if isinstance(p,int) and isinstance(q,int) and p in Af and q in Bm and p not in M and q not in M.values():
                M[p]=q;new+=1
    if not new: break
used=set(M.values())
# fallback best unused
for a,l in byA.items():
    if a in M: continue
    for r,b in l:
        if b not in used and r>=0.5: M[a]=b;used.add(b);break
rows=[]
for a in sorted(Af):
    b=M.get(a)
    r=R.get((a,b),0) if b is not None else 0
    rows.append((a,b,r))
json.dump(rows,open(OUT,'w'))
h=collections.Counter()
for a,b,r in rows:
    h['unmatched' if b is None else '1.0' if r==1 else '>=.9' if r>=.9 else '>=.8' if r>=.8 else '>=.6' if r>=.6 else '<.6']+=1
print(dict(h))
sm=0
for a,b,r in rows:
    if b is not None and sv(Af[a])!=sv(Bm[b]): sm+=1
print('regvar sets differ:',sm)
if False:
    for a,b,r in rows:
        if b is None: print(hex(a),'-'); continue
        print(hex(a),hex(b),nm.get(b,'?'),round(r,2),sv(Af[a]),sv(Bm[b]), len(SA[a]),len(SB[b]))

rows={a:b for a,b,r in rows}

def n5(l):
    o=[]
    prev=[]
    for x in l:
        prev=(prev+[x])[-2:]
        if re.match(r'^ldl?\t(F,rrS|rrS,F|F,rS|rS,F)$',x): continue
        if x in('neg	rS','add	rS,rS','extsb	rS','subb	rhS,rhS'): continue
        x=x.replace('calr','call')
        x=re.sub(r'^(jr|jp)\t[a-z]+,(L|X)$',r'\1 \2',x)
        if x in('test	rS','testl	rrS','cp	rS,#%0000','cp	F,#%0000','test	F'): continue
        if re.match(r'^ld\trS,(X|%00[0-9A-F]{2})\(rS\)$',x) and len(o)>=2 and o[-1].startswith('jr') and o[-2].startswith('cpir'): continue
        if x in('sub	rS,rS','ldk	rS,#0'): x='ZERO'
        x=re.sub(r'^(sd[al]|s[lr][al])l?\t.*',lambda m:'SHIFT',x) if x.split('\t')[0] in('sdal','sdll','sda','sdl','sla','sll','sra','srl','slal','sral','slll','srll','sdab','sdlb') else x
        x=re.sub(r'^ld\trS,#.*','ldimm',x)
        o.append(x)
    return o
def bag(a):
    b=rows.get(a)
    if b is None: return None
    return collections.Counter(n5(SA[a])),collections.Counter(n5(SB[b]))
if ARGS[0]=='all':
    res=[]
    for a in sorted(rows):
        if rows[a] is None: continue
        x,y=bag(a)
        d=sum(((x-y)+(y-x)).values()); tot=sum(x.values())
        res.append((d,a,tot))
    print('bag-equal',sum(1 for d,a,t in res if d==0),'of',len(res),' <=2:',sum(1 for d,a,t in res if d<=2))
    for d,a,t in sorted(res):
        if d: print(d,hex(a),hex(rows[a]),nm.get(rows[a]),t)
    sys.exit()
for arg in ARGS:
    a=int(arg,16);b=rows[a]
    x=collections.Counter(n5(SA[a]));y=collections.Counter(n5(SB[b]))
    print('=====',arg,hex(b),nm.get(b),'len',len(SA[a]),len(SB[b]))
    for k in sorted(set(x)|set(y)):
        if x[k]!=y[k]: print('%+d %s'%(y[k]-x[k],k))
