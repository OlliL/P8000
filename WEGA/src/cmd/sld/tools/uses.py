import re,sys
files=['copyname.c','dump.c','ext.c','ld.c','ldm.c','printname.c','swap.c','symbols.c']
src={f:open('../'+f).read().split('\n') for f in files}
def func_of(lines,i):
    for j in range(i,-1,-1):
        m=re.match(r'^(?:struct \w+ \*+|[a-z]+ \**)?([a-z_0-9]+)\(.*\)$',lines[j])
        if m and not lines[j].startswith('\t'): return m.group(1)
    return '-'
for v in sys.argv[1:]:
    out=[];fs=set()
    for f in files:
        for i,l in enumerate(src[f]):
            if re.search(r'\b%s\b'%re.escape(v),l) and not l.startswith('/*') and not l.startswith(' *'):
                fn=func_of(src[f],i)
                fs.add(fn)
                if len(out)<4 and fn!='-': out.append('    %s: %s'%(fn,l.strip()[:90]))
    print('%s  [%s]'%(v,' '.join(sorted(fs))[:100]))
    print('\n'.join(out))
