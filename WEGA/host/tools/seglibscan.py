#!/usr/bin/env python3
"""seglibscan.py: compile every source of src/lib/libc/common and seg with sccvc -O -DSEG and look for it in the libc part of /lib/gopt (instruction level, addresses masked): the members that fit (diff 0) and the near ones. WEGA=dir. Worked out for gopt (2026-11-19); the ranges and the compiler are in the text."""
import sys,os,glob,shutil,subprocess,tempfile,difflib
sys.path.insert(0,'host/tools'); sys.path.insert(0,'host/recon')
import seglibfit as S
from sout import SOut
W=os.environ['WEGA']
s=SOut(open(W+'/lib/gopt','rb').read()); img=bytes(s.image)
regs={'5':S.listing(img[65534:65534+50432],0x9bc0,0xc414),'4':S.listing(img[0:65534],0xf5fe,0xff78)}
K={k:[t for _,t in v] for k,v in regs.items()}
srcs=sorted(glob.glob('src/lib/libc/common/**/*.c',recursive=True)+glob.glob('src/lib/libc/seg/**/*.c',recursive=True))
if len(sys.argv)>1: srcs=sys.argv[1:]    # other sources, e.g. src/lib/oldlibc/*/0_*.c
for src in srcs:
    base=os.path.splitext(os.path.basename(src))[0]
    label=os.path.basename(os.path.dirname(src))+'/'+base
    w=tempfile.mkdtemp(dir=S.TMP)
    shutil.copy(src,w)
    for h in glob.glob(os.path.dirname(src)+'/*.h'): shutil.copy(h,w)
    shutil.copy('src/head/oldv7/stdio.h',w)    # char *_ptr: the header of the old programs
    r=subprocess.run([S.EMU,'-r',S.OLDROOT,'/bin/sccvc','-O','-DSEG','-I.','-c',base+'.c'],cwd=w,capture_output=True,text=True,timeout=300)
    obj=os.path.join(w,base+'.o')
    if not os.path.exists(obj): shutil.rmtree(w,ignore_errors=True); continue
    o=SOut(open(obj,'rb').read()); code=bytes(o.image[:o.segs[0]['code']])
    m=[t for _,t in S.listing(code,0,len(code))]
    n=len(m)
    best=None
    if n>=4:
        for k,L in K.items():
            for i in range(len(L)-n+1):
                if L[i]==m[0]:
                    sm=difflib.SequenceMatcher(None,L[i:i+n],m,autojunk=False)
                    d=n-sum(b.size for b in sm.get_matching_blocks())
                    if best is None or d<best[0]: best=(d,k,regs[k][i][0])
    print('%-14s %5d bytes  %s'%(label,len(code),('diff %d at %s:%04x'%best) if best else 'no start match'),flush=True)
    shutil.rmtree(w,ignore_errors=True)
