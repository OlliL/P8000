#!/usr/bin/env python3
"""regdecl.py FILE FUNC SPEC [OCC]: toggle `register` on the K&R parameter and local declarations of FUNC.
SPEC: integer k = the first variables (params, then locals) up to k registers (long = 2), others not register;
      or comma list of names = exactly those are register (no reordering)."""
import re,sys
fn,func,spec=sys.argv[1],sys.argv[2],sys.argv[3]
occ=int(sys.argv[4]) if len(sys.argv)>4 else 0
lines=open(fn).read().split('\n')
idx=[i for i,l in enumerate(lines) if re.match(r'^'+re.escape(func)+r'\(.*\)$',l)]
h=idx[occ]
j=h+1
while lines[j]!='{': j+=1
k=j+1
declre=re.compile(r'^\t(register |static |unsigned |struct |union |long |int |char |short |float |double )')
while k<len(lines) and (declre.match(lines[k]) or lines[k].startswith('#')): k+=1
rng=list(range(h+1,j))+list(range(j+1,k))
def split_decls(rest):
    parts=[];d=0;cur=''
    for c in rest:
        if c in '([': d+=1
        if c in ')]': d-=1
        if c==',' and d==0: parts.append(cur.strip());cur=''
        else: cur+=c
    parts.append(cur.strip());return parts
def vname(p):
    p=p.split('=')[0]
    return re.sub(r'[\*\s]','',re.sub(r'\[.*','',p)),'[' in p
info={}
order=[]
for i in rng:
    l=lines[i]
    if l.startswith('#') or 'static' in l: continue
    m=re.match(r'^\t(register )?((?:unsigned |struct \w+ |union \w+ |long |int |char |short |float |double )+)\s*(.*);(\s*/\*.*\*/)?\s*$',l)
    if not m: print('cannot parse',repr(l));continue
    typ=m.group(2).strip();parts=split_decls(m.group(3))
    info[i]=(typ,parts,m.group(4) or '')
    for p in parts:
        n,a=vname(p);order.append((n,typ,p,a))
def width(t,p):
    if '*' in p.split('=')[0]: return 1
    return 2 if t.endswith('long') else 1
if spec.isdigit():
    k0=int(spec);tot=0;names=set()
    for n,t,p,a in order:
        if a: continue
        w=width(t,p)
        if tot+w>k0: break
        tot+=w;names.add(n)
else: names=set(x for x in spec.split(',') if x)
new={}
for i,(typ,parts,cm) in info.items():
    r=[p for p in parts if vname(p)[0] in names]
    nr=[p for p in parts if vname(p)[0] not in names]
    out=[]
    if r: out.append('\tregister %s %s;'%(typ,', '.join(r)))
    if nr: out.append('\t%s %s;'%(typ,', '.join(nr)))
    if cm and out: out[-1]+=cm
    new[i]=out
res=[]
for i,l in enumerate(lines):
    if i in new: res.extend(new[i])
    else: res.append(l)
open(fn,'w').write('\n'.join(res))
print('ok',func,sorted(names))
