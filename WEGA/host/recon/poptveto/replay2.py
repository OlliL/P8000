import sys
src=open('/tmp/SCRATCH/pathsearch.py').read().replace("if __name__ == '__main__':\n    main()",'')
exec(src)
raw,path=sys.argv[1],eval(sys.argv[2])
st=cleanup(parse(raw))
NAMES={'ldk r9,#0':'INIT','sub r2,r2':'PLAIN','cpl rr12,#0':'T','cp r2,#54':'E','ldl rr2,rr12(#6)':'F','cp r2,#3':'d2','ld r2,rr10(#8)':'G','cp r8,#7':'H','ldl rr2,rr10(#8)':'H2','ldl rr2,rr12(#2)':'D','cp |_stkseg+~L1+6|(fp),#81':'A','inc r9,#1':'A2','ldk r7,#2':'err','cp r8,#1':'I','sub fp,#~L2':'PRO','ldm r8,_stkseg+18(fp),#6':'EXIT','cp r2,#25':'cp25','cp r2,#7':'cp7','cp r2,#86':'cp86'}
def short(st):
    out=[]
    for n in st:
        if n[0]=='L': out.append('L%d:'%n[1])
        elif n[0]=='J': out.append('J%d'%n[1])
        elif n[0]=='C': out.append('C%s%d'%(n[1],n[2]))
        elif n[0]=='R': out.append('ret')
        elif n[1] in NAMES: out.append('['+NAMES[n[1]]+']')
    s=' '.join(out)
    return s[:s.index('[I]')+3] if '[I]' in s else s
print('start', short(st))
for d,(kind,i) in enumerate(path):
    n=st[i]
    cand=[s2 for k,j,s2 in moves(st,30000+d) if k==kind and j==i]
    st=cleanup(cand[0])
    print(d,kind,n,'\n   ',short(st))
