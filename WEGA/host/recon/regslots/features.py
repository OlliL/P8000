"""slots.py: per function with register locals: the register locals with features, the slot count (make: findings; others: unused/spare words)"""
import re,sys,json
import os
W=os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)),'..','..','..'))+'/'
mk=json.load(open(W+'compare/make/findings.json'))['functions']
extra={'getmflgs':1,'straightrans':3,'setvar':1,'eqsign':1,'appendq':1,'mkqlist':2,'sindex':1,'is_sccs':1,'trychk':1,'addstars':2,'setenv':2,'readenv':1,'eqstr':1}
TYPES=r'(register|int|char|long|unsigned|short|CHARSTAR|VARBLOCK|NAMEBLOCK|DEPBLOCK|LINEBLOCK|SHBLOCK|CHAIN|GOTF|OPENDIR|PATTERN|FILE|struct|TIMETYPE|INTSTAR|static|double|float)'
def funcs(path):
    t=open(path,errors='replace').read()
    t=re.sub(r'/\*.*?\*/',lambda m:' ' if 'unused' not in m.group(0) else ' ',t,flags=re.S)
    L=t.split('\n'); i=0
    while i<len(L):
        m=re.match(r'^(?:[A-Za-z_][\w ]*?[\s*])?\**([A-Za-z_]\w*)\(([^)]*)\)\s*$',L[i])
        if not m or L[i].startswith('#'): i+=1; continue
        name=m.group(1); params=[p.strip() for p in m.group(2).split(',') if p.strip()]
        j=i+1; preg=[]
        while j<len(L) and not L[j].startswith('{'):
            if re.match(r'^\s*register',L[j]):
                preg+=[re.sub(r'[\[\]\d*()]','',w).strip() for w in re.sub(r'^\s*register\s+(unsigned\s+|struct\s+\w+\s+)?(\w+\s+)?','',L[j]).rstrip('; \t').split(',')]
            j+=1
        if j>=len(L): break
        e=j+1
        while e<len(L) and not L[e].startswith('}'): e+=1
        body=L[j+1:e]
        decl=[]; k=0
        while k<len(body):
            s=body[k].strip()
            if s=='' or s.startswith('#'): k+=1; continue
            if re.match('^'+TYPES+r'\b',s) and not re.match(r'^\w+\s*\(',s):
                reg=s.startswith('register')
                b=re.sub(r'^(register\s+)?((unsigned|struct)\s+)?(\w+\s+)?','',s) if not re.match(r'^register\s+\w+\s*[,;=]',s) else re.sub(r'^register\s+','',s)
                for w in re.split(r',(?![^(]*\))',b.rstrip(';')):
                    nm=re.sub(r'\[.*|=.*','',w).strip().strip('*()').strip('*')
                    nm=re.sub(r'\W','',nm)
                    decl.append((nm,reg,'=' in w))
                k+=1; continue
            break
        code='\n'.join(body[k:])
        yield name,params,preg,decl,code,e
        i=e+1
def feat(v,code):
    f=[]
    if re.search(r'for\s*\(\s*'+v+r'\s*=',code): f.append('for=')
    n=len(re.findall(r'\b'+v+r'\b',code)); f.append('n%d'%n)
    if re.search(r'\b'+v+r'\s*(\+\+|--)|(\+\+|--)\s*'+v+r'\b',code): f.append('inc')
    if re.search(r'\b'+v+r'\s*=[^=]',code): f.append('asg')
    return ','.join(f)
progs={'make':['src/cmd/make/'+x for x in ['main.c','doname.c','misc.c','files.c','dosys.c','dyndep.c','prtmem.c']],
 'diff':['src/cmd/diff.c'],'mail':['src/cmd/mail.c'],'nl':['src/cmd/nl.c'],'gpasswd':['src/cmd/gpasswd.c']}
for prog,files in progs.items():
    for p in files:
        for name,params,preg,decl,code,e in funcs(W+p):
            regs=[(n,ini) for n,r,ini in decl if r]
            if not regs: 
                if any(n in('unused','spare') for n,_,_ in decl): print(prog,name,'NO-REG but unused')
                continue
            if prog=='make': sl=extra.get(name,0)
            else: sl=sum(1 for n,_,_ in decl if n in ('unused','spare'))
            rl=' '.join('%s%s[%s]'%(n,'=' if ini else '',feat(n,code)) for n,ini in regs)
            print('%-7s %-13s slot=%d preg=%-12s regs: %s'%(prog,name,sl,','.join(preg) or '-',rl))
