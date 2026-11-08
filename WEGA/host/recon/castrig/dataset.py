#!/usr/bin/env python3
"""castrig/dataset.py OUT.json: the data base for the search of the rule of the old cas. For every module of the seven programs:
the sites (jpr, callr) of the module in order, with what cas knows there and what the source has up to there, and from the original
the long and short forward sites in reach. HC: the instrumented scratch cas (hostcas2.patch over host/cas), CAS_J is set here.
Needs the .s of oldcc.sh model (d12model_PROG). Only the original and the model are read; nothing is changed."""
import os,sys,re,json,subprocess
sys.path.insert(0,'/mnt/share/cvs/P8000/WEGA/host/tools'); sys.path.insert(0,'/mnt/share/cvs/P8000/WEGA/host/recon')
from sout import SOut
import zcode
W='/mnt/share/cvs/P8000/WEGA/compare/'; R='/mnt/share/cvs/P8000/tmp/wega31_root/lib/'; T='/mnt/share/cvs/P8000/tmp/tmp/d12model_%s/'
HC=os.environ['HC']
PROGS='cparse popt codgen cpp spopt scparse scodgen'.split()
INDEP={'spopt':'popt','scparse':'cparse','scodgen':'codgen'}
def jumps(gc,lo,hi):
    r=[]
    for a,x in zcode.insns(gc,lo,hi,False):
        m=re.match(r'(call|calr|jp|jr)\t(?:[a-z]+,)?%([0-9A-F]+)$',x)
        if m: r.append((a,m[1],int(m[2],16)))
    return r
KINDS=['idg','idb','idl','idt','blk','lines','stmts','labl','labg','labn','byte','word','long','even','comm','sect','assign','ins','jpr','callr','chars','names']
def linefeat(path):
    """cumulative counts per line of the source, by what a line is"""
    c=dict.fromkeys(KINDS,0); out=[None]; insec=None
    secs=[]
    seen=set(); local=set()
    MN=re.compile(r'^(r[lh]?\d+|rr\d+|rq\d+|fp|sp|eq|ne|lt|le|gt|ge|c|nc|ugt|uge|ult|ule|z|nz|mi|pl|ov|nov)$')
    for l in open(path,errors='replace'):
        c['lines']+=1; c['chars']+=len(l)
        s=re.sub(r'/\*.*?\*/','',l).split('//')[0].strip()
        if s:
            c['stmts']+=1
            if s=='{': c['blk']+=1
            if s.startswith('}'): local=set()
            if not re.match(r'^\.(comm|lcomm)\b',s):
                body=s
                mm=re.match(r'^[A-Za-z_~.$][\w~.$]*::?(?!=)\s*(.*)$',s)
                head=[]
                if mm: head=[s.split(':')[0]]; body=mm[1]
                tt=body.split(None,1)
                ops=tt[1] if len(tt)>1 and not body.startswith('~') else body
                if body.startswith('~'): ops=body
                for w in head+re.findall(r'[A-Za-z_~$][\w~$]*',ops):
                    if MN.match(w) or re.match(r'^[0-9]',w): continue
                    if w.startswith('~'):
                        if w not in local: local.add(w); c['idt']+=1; c['idb']+=len(w)+1
                    elif w not in seen:
                        seen.add(w); c['idb']+=len(w)+1
                        if re.match(r'^L\d+$',w): c['idl']+=1
                        else: c['idg']+=1
            m=re.match(r'^([A-Za-z_~.$][\w~.$]*)(::?)(?!=)\s*(.*)$',s)
            if m:
                if m[2]=='::': c['labg']+=1
                elif re.match(r'^L\d+$',m[1]): c['labn']+=1
                else: c['labl']+=1
                c['names']+=len(m[1])
                s=m[3]
            t=s.split()
            if t:
                k=t[0]
                if k=='.byte': c['byte']+=1
                elif k=='.word': c['word']+=1
                elif k=='.long': c['long']+=1
                elif k=='.even': c['even']+=1
                elif k in('.comm','.lcomm'): c['comm']+=1
                elif k in('.data','.text','.bss','.psec','.csec','.seg','.nonseg'): c['sect']+=1
                elif ':=' in s: c['assign']+=1
                elif k=='jpr': c['jpr']+=1; c['ins']+=1
                elif k=='callr': c['callr']+=1; c['ins']+=1
                elif not k.startswith('.'): c['ins']+=1
        out.append(dict(c))
    return out
def module(prog,mod,fl,F,gc):
    fn=T%prog+mod+'.s'
    allf=sorted((int(f['orig'],16),f['size_orig'],k,f.get('file')) for k,f in F.items() if isinstance(f.get('orig'),str) and f.get('size_orig') and f.get('file'))
    rng=[(l,l+s) for l,s,k,f2 in allf if f2==fl]
    if not rng: return None
    lo=rng[0][0]; hi=rng[-1][1]
    Sh=[];LL=[]
    for a,f,t in jumps(gc,lo,hi):
        if not(lo<=t<hi and t>a): continue
        rg=4098 if f in('call','calr') else 256
        if t-a>rg: continue
        (LL if f in('call','jp') else Sh).append((a,t))
    bases={k:(l,s) for l,s,k,f2 in allf}
    r=subprocess.run([HC,'-u','-o','/dev/null',fn],capture_output=True,text=True,env=dict(os.environ,CAS_J='1',CAS_D='1'))
    J=[];last=None
    for l in r.stderr.split('\n'):
        p=l.split()
        if p and p[0]=='J':
            last=dict(tok=int(p[1]),it=int(p[2]),st=int(p[3]),nt=int(p[4]),ent=int(p[5]),sec=int(p[6]),lc=int(p[7]),ifp=int(p[8]),sfp=int(p[9]),line=int(p[10]),res=-1)
            J.append(last)
        elif p and p[0]=='R' and last: last['res']=int(p[1])
        elif p and p[0]=='D' and last: last.update(zip(('dep','open','zero','nz','longd','nj','nc'),map(int,p[1:])))
    LF=linefeat(fn)
    L=open(fn,errors='replace').read().split('\n')
    cur=None;seq=[];idx=0;n=0;rows=[]
    for l in L:
        s=l.split('/*')[0].split('//')[0]
        m=re.match(r'\s*([_~\w]+)::?(?!=)',s)
        if m and m[1][1:] in bases:
            cur=m[1][1:]; seq=jumps(gc,bases[cur][0],bases[cur][0]+bases[cur][1]); idx=0
        t=s.split()
        if t and t[0] in('jpr','callr'):
            a=seq[idx][0] if cur and idx<len(seq) else None; idx+=1
            if n<len(J):
                q=dict(J[n]); q['a']=a; q['fn']=cur
                q.update(LF[min(q['line'],len(LF)-1)])
                rows.append(q)
            n+=1
    end=dict(LF[-1])
    return dict(prog=prog,mod=mod,lo=lo,hi=hi,long=LL,short=Sh,rows=rows,end=end,nsites=n,nj=len(J))
out=[]
for prog in PROGS:
    F=json.load(open(W+prog+'/findings.json'))['functions']
    g=SOut(open(R+prog,'rb').read()); gc=bytes(g.image)[:g.segs[0]['code']]
    for fl in sorted({f.get('file') for f in F.values() if f.get('file') and f.get('file')!='-'}):
        mod=fl.rsplit('.',1)[0]
        if not os.path.exists(T%prog+mod+'.s'): continue
        m=module(prog,mod,fl,F,gc)
        if m:
            m['indep']=INDEP.get(prog,prog)
            out.append(m)
json.dump(out,open(sys.argv[1],'w'))
print(len(out),'modules',sum(len(m['long']) for m in out),'long',sum(len(m['short']) for m in out),'short',sum(len(m['rows']) for m in out),'sites')
