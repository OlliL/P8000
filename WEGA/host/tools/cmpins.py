import sys,re,difflib
import os; sys.path.insert(0,os.path.dirname(os.path.abspath(__file__)))
import z8k
from sout import SOut
def ins(f,lo,hi):
    s=SOut(open(f,'rb').read()); tsz=s.segs[0]['code']
    buf=z8k.Buf(s.image[:tsz],0); out=[]; pc=lo
    while pc<hi:
        i=z8k.decode(buf,pc,False)
        if not i: out.append((pc,'.word %04x'%buf.w(pc))); pc+=2; continue
        out.append((pc,z8k.text(i))); pc=i.end
    return out
start=int(sys.argv[5],0) if len(sys.argv)>5 else 0x42
a=ins(sys.argv[1],start,int(sys.argv[3],0))
b=ins(sys.argv[2],start,int(sys.argv[4],0))
norm=lambda t: re.sub(r'%[0-9A-F]{3,4}','ADDR',t)
A=[norm(t) for p,t in a]; B=[norm(t) for p,t in b]
sm=difflib.SequenceMatcher(None,A,B,autojunk=False)
n=0
for tag,i1,i2,j1,j2 in sm.get_opcodes():
    if tag=='equal': continue
    n+=1
    print('--- %s  new@%04x..  orig@%04x..'%(tag,a[i1][0],b[j1][0]))
    for k in range(i1,i2): print('  new ',a[k][1])
    for k in range(j1,j2): print('  orig',b[k][1])
print(n,'diff blocks')
