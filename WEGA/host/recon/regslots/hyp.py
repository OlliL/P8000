"""hyp.py: score hypotheses (transformations of the event streams) on the test set with replay.run"""
import os
HERE=os.path.dirname(os.path.abspath(__file__))
WEGA=os.path.normpath(os.path.join(HERE,'..','..','..'))
WORK=os.environ.get('RS_WORK', os.path.normpath(os.path.join(WEGA,'..','tmp','tmp','regslots')))
import sys,collections,glob,os
sys.path.insert(0,HERE)
from replay import parse, run
S=WORK
EXP={'make/main':{'getmflgs':1},'make/misc':{'colontrans':1,'straightrans':3,'setvar':1,'eqsign':1,'appendq':1,'mkqlist':2,'sindex':1,'is_sccs':1,'trychk':1,'addstars':2,'setenv':2,'readenv':1},
 'make/files':{'eqstr':1},'make/gram':{'yyparse':1},'diff/diff':{'prepare':1,'equiv':1,'main':1,'stone':1},'mail/mail':{'isfrom':1,'system':1},
 'nl/nl':{'main':1},'comm/comm':{'compare':2},'csplit/csplit':{'fatal':1,'re_arg':1},'tsort/tsort':{'findloop':1},'uniq/uniq':{'skip':2},
 'ar/ar':{'rcmd':1,'notfound':1,'longt':1},'cpio/cpio':{'gmatch':3,'missdir':1}}
FILES=[]
for l in open(S+'/ts/list'):
    w=l.split()
    for f in w[2:]: FILES.append('%s/%s'%(w[0],f[:-2]))
EV={f:parse('%s/logs/%s.log'%(S,f)) for f in FILES}
def score(transform=None, verbose=False, **kw):
    bad=0; lst=[]
    for f in FILES:
        ev=EV[f]
        if transform: ev=transform(ev)
        try: out,chk=run(ev,**kw)
        except Exception as x:
            return 999,['%s: %s'%(f,x)]
        c=collections.Counter(); seen=set()
        for fn,n,s in out:
            seen.add(fn); c[fn]+= 1 if s else 0
        e=EXP.get(f,{})
        for fn in seen|set(e):
            if c[fn]!=e.get(fn,0): bad+=1; lst.append('%s:%s %d/%d'%(f,fn,c[fn],e.get(fn,0)))
    return bad,lst
if __name__=='__main__':
    b,l=score(); print(b); print(' '.join(l))
