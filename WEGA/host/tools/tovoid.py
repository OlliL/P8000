# gcc warnings (file:line:col: warning: ...) in WARNFILE: an int function that "reaches the end" or has "return;" and no "return expr" becomes void in the definition and in cpproto.h. usage: tovoid.py WARNFILE (changes the sources in the current directory)
import re,collections,sys
S=sys.argv[1]
fs=collections.defaultdict(set)
for l in open(S):
    m=re.match(r'(\w+\.[cy]):(\d+):\d+: warning: (?:.return. with no value|control reaches end)',l)
    if m: fs[m.group(1)].add(int(m.group(2)))
names=set()
hd=re.compile(r'^(?:int|void|NODE \*|static int|char \*|struct symtab \*|OFFSZ|TWORD|long|double) ?\*?(\w+)\(')
for f,ls in fs.items():
    L=open(f).read().split('\n')
    for n in ls:
        i=n-1
        while i>=0 and not re.match(r'^int \w+\(',L[i]):
            if hd.match(L[i]) and not L[i].startswith('int '): i=-1;break
            i-=1
        if i<0: continue
        j=i+1
        while j<len(L) and not hd.match(L[j]): j+=1
        body='\n'.join(L[i:j])
        if re.search(r'\breturn\s*\(?\s*[^;\s]',re.sub(r'/\*.*?\*/','',body,flags=re.S)):
            print('HAS',f,L[i][:40]); continue
        names.add(re.match(r'^int (\w+)\(',L[i]).group(1))
        L[i]=L[i].replace('int ','void ',1)
    open(f,'w').write('\n'.join(L))
P=open('cpproto.h').read()
for nm in names: P=re.sub(r'^int (%s\()'%nm,r'void \1',P,flags=re.M)
open('cpproto.h','w').write(P)
print(len(names))
