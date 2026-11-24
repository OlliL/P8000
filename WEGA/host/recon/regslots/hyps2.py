"""hyps2.py: the strings of a function freed after its symbols (behind the clearst batch) instead of before"""
import sys,os; sys.path.insert(0,os.path.dirname(os.path.abspath(__file__)))
import hyp
def strlate(rev=False, until='batch'):
    def t(ev):
        sa=set(e[2] for e in ev if e[0]=='M' and e[1]!=26)
        out=[]; pend=[]; inbatch=False
        for i,e in enumerate(ev):
            if e[0]=='f' and e[1] in sa:
                pend.append(e); continue
            if pend:
                isb = e[0]=='XF' or (e[0]=='f' and out and out[-1][0]=='XF')
                if isb: inbatch=True
                elif inbatch or (until=='D' and e[0]=='D') or (until=='M' and e[0]=='M'):
                    out+= list(reversed(pend)) if rev else pend; pend=[]; inbatch=False
            out.append(e)
        out+=pend
        return out
    return t
for name,tr in [('late',strlate()),('late_rev',strlate(True)),('lateD',strlate(until='D')),('lateM',strlate(until='M'))]:
    b,l=hyp.score(tr); print(name,b); print('   ',' '.join(l)[:600])
