# gcc warnings in WARNFILE: removes the variables of "unused variable" warnings from their declarations. usage: rmunused.py WARNFILE (in the current directory)
import re,sys,collections
S=sys.argv[1]
d=collections.defaultdict(list)
for l in open(S):
    m=re.match(r"(\w+\.[cy]):(\d+):(\d+): warning: unused variable .(\w+).",l)
    if m: d[m.group(1)].append((int(m.group(2)),int(m.group(3)),m.group(4)))
for f,items in d.items():
    L=open(f).read().split('\n')
    for ln,col,name in sorted(items,reverse=True):
        s=L[ln-1]
        # remove the declarator `name` (with optional leading * ) from the line
        pat_mid=re.compile(r'\s*\*?\b%s\b(\[[^\]]*\])?\s*,'%name)
        pat_last=re.compile(r',\s*\*?\b%s\b(\[[^\]]*\])?(?=\s*;)'%name)
        pat_only=re.compile(r'^(\s*)(?:register |static )?[\w ]+?[\s\*]+\b%s\b(\[[^\]]*\])?\s*;[ \t]*(/\*.*\*/)?\s*$'%name)
        if pat_only.match(s) and ',' not in s.split(name)[0][-2:]:
            if re.search(r',',s): pass
            else:
                L[ln-1]=None; continue
        n=pat_mid.sub(' ',s,count=1) if pat_mid.search(s) else None
        if n is None and pat_last.search(s): n=pat_last.sub('',s,count=1)
        if n is None: print('MANUAL',f,ln,name,s); continue
        L[ln-1]=n.replace('  ',' ') if False else n
    open(f,'w').write('\n'.join(l for l in L if l is not None))
