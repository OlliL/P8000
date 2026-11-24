"""v7sim.py: the V7 malloc of src/lib/oldlibc/malloc/malloc.c on a simulated arena (word addresses as byte addresses,
WORD 2). Memory: a dict word-address -> pointer value (with the busy bit)."""
BLOCK=1024; WORD=2
class Heap:
    def __init__(self, brk):
        self.m={}            # address -> value (pointer words of the arena)
        self.data={}         # address -> byte (contents written by the user, for sfldc)
        self.allocs=0x10000  # the static allocs[2]: below the arena in the real program; any low address works
        self.brk=brk
        self.allocp=None
    def ptr(self,a): return self.m.get(a,0)
    def malloc(self,nbytes):
        A=self.allocs
        if self.allocp is None:
            A=self.allocs=self.brk-8   # allocs[0], allocs[1] lie in front of the arena (static data)
            self.m[A]=(A+2)|1; self.m[A+2]=A|1; self.alloct=A+2; self.allocp=A
        nw=(nbytes+WORD+WORD-1)//WORD
        p=self.allocp; last=None
        while True:
            temp=0
            while True:
                if not self.ptr(p)&1:
                    last=p
                    while True:
                        q=self.ptr(p)
                        if self.ptr(q)&1: break
                        self.m[p]=self.ptr(q)
                    if q>=p+2*nw: return self._found(p,q,nw)
                q=p; p=self.ptr(p)&~1
                if p>q: continue
                elif q!=self.alloct or p!=A: return None
                else:
                    temp+=1
                    if temp>1: break
            x = nw-(self.alloct-last)//2 if last is not None and self.ptr(last)==self.alloct else nw
            t=((x+BLOCK//WORD)//(BLOCK//WORD))*(BLOCK//WORD)
            q=self.brk; self.brk+=t*WORD
            self.m[self.alloct]=q
            if q!=self.alloct+2: self.m[self.alloct]=q|1
            self.alloct=q+2*(t-1); self.m[q]=self.alloct
            self.m[self.alloct]=A|1
            p=self.allocp  # the search goes on from where it was (p is the last p of the loop: allocs)
            p=A
    def _found(self,p,q,nw):
        self.allocp=p+2*nw
        if q>self.allocp:
            self.m[self.allocp]=self.ptr(p)
        self.m[p]=self.allocp|1
        return p+2
    def free(self,ap):
        p=ap-2; self.allocp=p; self.m[p]=self.ptr(p)&~1
