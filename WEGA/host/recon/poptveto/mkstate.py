"""mkstate.py RAW.s SPEC OUT.s : replace jpr's body (L10002 .. before L106) by SPEC; SPEC lines: 'Lnnn' = the chunk of that
label from the raw code (its label line and instructions up to the next label), '-Lnnn' = the chunk without its last
jump, other lines are copied as they are."""
import sys,re
raw=open(sys.argv[1]).read().split('\n')
s=raw.index('_jpr::') if '_jpr::' in raw else [i for i,l in enumerate(raw) if l.strip()=='_jpr::'][0]
a=[i for i in range(s,len(raw)) if raw[i]=='L10002:'][0]
b=[i for i in range(a,len(raw)) if raw[i]=='L106:'][0]
body=[l for l in raw[a:b] if not l.startswith('/*') and l.strip()]
ch={}; cur=None
for l in body:
    if re.match(r'^L\d+:$',l): cur=l[:-1]; ch[cur]=[l]
    else: ch[cur].append(l)
out=[]
for l in open(sys.argv[2]).read().split('\n'):
    if not l.strip(): continue
    m=re.match(r'^(-?)(L\d+)$',l.strip())
    if m and m.group(2) in ch:
        c=ch[m.group(2)][:]
        if m.group(1):
            assert c[-1].strip().startswith('jpr\tL') or c[-1].strip().startswith('jpr'), c[-1]
            c=c[:-1]
        out+=c
    else: out.append(l)
open(sys.argv[3],'w').write('\n'.join(raw[:a]+out+raw[b:]))
