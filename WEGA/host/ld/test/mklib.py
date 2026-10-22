#!/usr/bin/env python3
# mklib.py OUT member.o ...: an archive with a __.SYMDEF in the format ld reads (the original ranlib loses
# most long names, see KNOWLEDGE.md), used by check.py
import sys,struct,time,os
sys.path.insert(0,os.path.join(os.path.dirname(os.path.abspath(__file__)),'..','..','tools'))
from sout import SOut
out=sys.argv[1]; mem=sys.argv[2:]
def hdr(name,data,date):
    return struct.pack('>14sIBBHI',name.encode(),date,0,0,0o100644,len(data))
bodies=[open(m,'rb').read() for m in mem]
names=[os.path.basename(m) for m in mem]
# first pass: sizes of symdef
def entries(offs):
    ents=[]
    for b,o in zip(bodies,offs):
        s=SOut(b)
        for y in s.symbols:
            if (y['type']&0x20) and (y['type']&0x1f)!=0:
                n=y['name'].encode()
                if len(n)<=8: ents.append(struct.pack('>I8s',o,n))
                else:
                    ln=len(n)+1
                    raw=bytes([0x80|ln])+n
                    raw+=b'\0'*((-(4+len(raw)))%12)
                    e=struct.pack('>I',o)+raw
                    # entry occupies 12*k bytes
                    ents.append(e)
    return b''.join(ents)
# offsets depend on the symdef size: iterate
offs=[0]*len(bodies)
for _ in range(3):
    sd=entries(offs)
    pos=2+26+len(sd)+(len(sd)&1)
    offs=[]
    for b in bodies:
        offs.append(pos); pos+=26+len(b)+(len(b)&1)
sd=entries(offs)
date=int(time.time())+3600
f=b'\xff\x65'+hdr('__.SYMDEF',sd,date)+sd+(b'\0' if len(sd)&1 else b'')
for n,b in zip(names,bodies):
    f+=hdr(n,b,int(time.time()))+b+(b'\0' if len(b)&1 else b'')
open(out,'wb').write(f)
