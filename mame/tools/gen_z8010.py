# Z8010 tests for MON16 (non-segmented mode). Three independent programs.
import json
def build(base, res, body):
    P=[]; S=[]
    def outb(cmd,val):                  # ldb rl0,#val ; soutb port,rl0  (special I/O, stack MMU)
        P.extend([0xC800|val,0x3A87,(cmd<<8)|0xF6])
    def inb(cmd,name):                  # sinb rl0,port ; ldb %res,rl0
        P.extend([0x3A85,(cmd<<8)|0xF6,0x6E08,res+len(S)]); S.append(name)
    body(outb,inb,P)
    P.append(0x7F00)                    # sc #0 -> back to the monitor
    return {"base":base,"res":res,"words":P,"slots":S}
def progA(outb,inb,P):                  # commands %15 / %16
    for cmd,nm in ((0x15,"15"),(0x16,"16")):
        for d in (5,63):
            outb(0x01,d); outb(0x0A,0x00)       # SAR=d, attribute byte := 00
        outb(cmd,0x00)                          # command with data byte 00
        for d in (5,63):
            outb(0x01,d); inb(0x0A,"attr desc%d after %%%s"%(d,nm))
def progB(outb,inb,P):                  # SAR wraps 63 -> 0
    outb(0x01,63); inb(0x0E,"attr desc63 (read 0E, SAR+1)"); inb(0x01,"SAR after 0E")
    outb(0x01,63); inb(0x0A,"attr desc63 (read 0A)"); inb(0x0F,"attr desc63 (read 0F, SAR+1)"); inb(0x01,"SAR after 0F")
def progC(outb,inb,P):                  # command %10
    inb(0x00,"MR at start")
    outb(0x00,0x38); outb(0x01,7); outb(0x20,2)
    inb(0x00,"MR set"); inb(0x20,"DSC set"); inb(0x01,"SAR set")
    outb(0x10,0x00)
    inb(0x00,"MR after %10"); inb(0x20,"DSC after %10"); inb(0x01,"SAR after %10")
    P.extend([0x6008,0x9500, 0x3A87,0x00F6])    # ldb rl0,%9500 ; soutb 00F6,rl0 : restore MR
PROGS={"A":build(0x9000,0x9100,progA),"B":build(0x9200,0x9300,progB),"C":build(0x9400,0x9500,progC)}
if __name__=="__main__":
    for k,p in PROGS.items():
        assert p["base"]+2*len(p["words"])<=p["res"]-0x20 or k=="C", k
        print(k,"words",len(p["words"]),"results",len(p["slots"]))
        L=["D %04X\\r"%p["base"]]+["%04X\\r"%w for w in p["words"]]+["Q\\r","G %04X\\r"%p["base"],"D %04X %X W\\r"%(p["res"],len(p["slots"]))]
        open("enter_z8010_%s.txt"%k,"w").write("\n".join(L)+"\n")
    json.dump(PROGS,open("prog_z8010.json","w"),indent=1)
