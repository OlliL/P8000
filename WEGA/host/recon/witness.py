#!/usr/bin/env python3
"""witness.py - test bench of an old compiler version: System III sources of WEGA programs built in its time.

Usage: witness.py [-c "EMUARGS"] [-s N] [-v FUNC] [PROG ...]
  -s N     also print the N most frequent differing hunks (registers hidden where only they differ)
  -v FUNC  print the hunks of the function FUNC (prog:func or func)
  -n       use the stdio.h of the WEGA tree (default: the stdio.h of the v1 time, src/cmd/cparse/stdio.h: char *_ptr,
           the V7 getc and putc)

The single-file System III sources (SYS3SRC: the directory usr/src/cmd of a System III tree, not in git) of WEGA
programs whose crt0 stamp is from 01/83 .. 07/83 (the time of the old compiler v1, host/OLDCC.md) are compiled with the compiler under test (default: z8k-emu -r OLDROOT /bin/ccv1 -O -c; -c gives other emulator
arguments, e.g. "-r WEGA /bin/cc") and their code is aligned with the original binary (difflib over the whole own code,
absolute addresses masked, frame offsets kept). Per function: = when every instruction is in an equal block, else the
number of its lines that are not. WEGA changed some of these sources, so a difference is not a compiler difference by
itself; the bench is read by comparison: the same sources with and without a switch (more = functions means the switch
fits more of the old compiler's output). A source restored to the WEGA version in src/cmd (same file name) is used
instead of the System III file. Work directory RECON_TMP/w3. Measurement only; nothing of it goes into git.
"""
import sys, os, re, subprocess, difflib, shutil
HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', 'tools')); sys.path.insert(0, HERE)
from sout import SOut; import zcode
S3 = os.environ.get('SYS3SRC') or sys.exit('witness.py: set SYS3SRC to usr/src/cmd of a System III tree')
R = os.path.abspath(os.environ.get('WEGA', os.path.join(HERE, '..', '..', '..', 'tmp', 'wega31_root')))
OLDROOT = os.path.abspath(os.environ.get('OLDROOT', os.path.join(HERE, '..', '..', '..', 'tmp', 'oldroot')))
EMU = os.path.join(HERE, '..', 'z8k-emu', 'z8k-emu')
PROGS=[('bin/chgrp','chgrp.c'),('bin/chown','chown.c'),('bin/mv','mv.c'),('bin/date','date.c'),('bin/df','df.c'),
 ('bin/od','od.c'),('bin/login','login.c'),('bin/ls','ls.c'),('bin/nice','nice.c'),('bin/passwd','passwd.c'),
 ('bin/regcmp','regcmp.c'),('bin/rmdir','rmdir.c'),('bin/rm','rm.c'),('bin/sort','sort.c'),('bin/su','su.c'),
 ('bin/tee','tee.c'),('bin/time','time.c'),('bin/touch','touch.c'),('bin/tr','tr.c'),('bin/uname','uname.c'),
 ('bin/write','write.c'),('usr/bin/checkeq','checkeq.c'),('usr/bin/ct','ct.c'),('usr/bin/hyphen','hyphen.c'),
 ('usr/bin/id','id.c'),('usr/bin/logname','logname.c'),('usr/bin/units','units.c'),('usr/bin/xargs','xargs.c')]
def norm(t):
    return re.sub(r'%[0-9A-F]{4}(?!\(r15\))','A',t)
def code(path):
    o=SOut(open(path,'rb').read()); return o,bytes(o.image)[:o.segs[0]['code']]
def main():
    a=sys.argv[1:]
    nshape=0; vf=set()
    if '-s' in a:
        i=a.index('-s'); nshape=int(a[i+1]); del a[i:i+2]
    while '-v' in a:
        i=a.index('-v'); vf.add(a[i+1]); del a[i:i+2]
    import collections
    oldhdr='-n' not in a; a=[x for x in a if x!='-n']
    shapes=collections.Counter(); where=collections.defaultdict(set)
    cc=['-r',OLDROOT,'/bin/ccv1']
    if '-c' in a:
        i=a.index('-c'); cc=a[i+1].split(); del a[i:i+2]
    sel=set(a)
    work=os.path.join(os.environ.get('RECON_TMP','/mnt/share/cvs/P8000/tmp/tmp'),'w3'); os.makedirs(work,exist_ok=True)
    tot_same=tot=0; tot_d=0
    for b,src in PROGS:
        name=os.path.basename(b)
        if sel and name not in sel: continue
        mine=os.path.join(HERE,'..','..','src','cmd',src)	# a WEGA version restored in src/cmd comes first
        shutil.copy(mine if os.path.exists(mine) else os.path.join(S3,src),os.path.join(work,src))
        obj=src[:-2]+'.o'
        if os.path.exists(os.path.join(work,obj)): os.remove(os.path.join(work,obj))
        hdr=os.path.join(work,'stdio.h')
        if oldhdr:
            shutil.copy(os.path.join(HERE,'..','..','src','cmd','cparse','stdio.h'),hdr)
        elif os.path.exists(hdr):
            os.remove(hdr)
        r=subprocess.run(['timeout','300',EMU]+cc+['-O']+(['-I.'] if oldhdr else [])+['-c',src],cwd=work,capture_output=True,text=True)
        if not os.path.exists(os.path.join(work,obj)):
            print('%-8s compile failed: %s'%(name,(r.stdout+r.stderr).strip().split('\n')[-1][:80])); continue
        o,c=code(os.path.join(work,obj)); g,gc=code(os.path.join(R,b))
        fs=sorted((s['value'],s['name']) for s in o.symbols if s['type'] in (34,2,31) and s['value']<len(c) and s['name'].startswith('_'))
        B=[];own=[]
        for i,(v,n) in enumerate(fs):
            e=fs[i+1][0] if i+1<len(fs) else len(c)
            ins=[norm(t) for _,t in zcode.insns(c,v,e)]
            own.append((n,len(B),len(B)+len(ins))); B+=ins
        A=[norm(t) for _,t in zcode.insns(gc,0,min(len(gc),2*len(c)+0x200))]
        sm=difflib.SequenceMatcher(None,A,B,autojunk=False)
        eq=[0]*len(B)
        for t,i1,i2,j1,j2 in sm.get_opcodes():
            if t=='equal':
                for j in range(j1,j2): eq[j]=1
        ops=sm.get_opcodes()
        for t,i1,i2,j1,j2 in ops:
            if t=='equal': continue
            fn=[n for n,b1,b2 in own if b1<=j1<b2 or (j1==j2 and b1<=j1<=b2)]
            fn=fn[0][1:] if fn else '?'
            x=A[i1:i2]; y=B[j1:j2]
            if len(x)>12 or len(y)>12: k='(long hunk)'
            else:
                hx=[zcode.hide_regs(t) for t in x]; hy=[zcode.hide_regs(t) for t in y]
                k=(' ; '.join(x) or '-')+'  =>  '+(' ; '.join(y) or '-')
                if hx==hy: k='[regs] '+k
            shapes[k]+=1; where[k].add(name+':'+fn)
            if fn in vf or name+':'+fn in vf:
                print('  %s:%s\n     - %s\n     + %s'%(name,fn,'\n     - '.join(x) or '(none)','\n     + '.join(y) or '(none)'))
        res=[]
        for n,j1,j2 in own:
            d=j2-j1-sum(eq[j1:j2])
            res.append((n,d)); tot+=1; tot_same+=(d==0); tot_d+=d
        print('%-8s %s'%(name,' '.join('%s:%s'%(n[1:],'=' if d==0 else d) for n,d in res)))
    print('functions %d, same %d, differing lines %d'%(tot,tot_same,tot_d))
    for k,c in shapes.most_common(nshape):
        print('%3d %3d  %s   [%s]'%(c,len(where[k]),k[:160],' '.join(sorted(where[k])[:4])))
if __name__ == '__main__':
    main()
