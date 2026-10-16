# Commented assembler listings for the real-hardware tests (tests/01 .. tests/04).
# Run: python3 listing.py   -> prints the listings; with "--write" puts them into the test READMEs.
import sys, os, re
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

class L:
    def __init__(self, base): self.base=base; self.rows=[]; self.pc=base
    def op(self, words, text, comment="", label=""):
        self.rows.append((self.pc, words, text, comment, label)); self.pc += 2*len(words)
    def comment_line(self, c): self.rows.append((None,[],"",c,"#"))
    def text(self):
        out=[]
        for pc,w,t,c,lab in self.rows:
            if lab=="#": out.append("; "+c); continue
            ws=" ".join("%04X"%x for x in w)
            out.append("%04X  %-20s %-25s %s"%(pc,ws,t,("; "+c) if c else ""))
        return "\n".join(out)

REG={0x00:"MR (mode register)",0x01:"SAR (segment address register)",0x20:"DSCR (descriptor selector)",
     0x0A:"attribute byte of the selected descriptor",0x0E:"attribute byte, then SAR := SAR+1",
     0x0F:"descriptor field, DSC+1 (SAR+1 after the attribute byte)",0x02:"VTR (violation type)",
     0x15:"set CPU-inhibit in all descriptors",0x16:"set DMA-inhibit in all descriptors",
     0x10:"reset MR, VTR and DSCR"}

def z8010(base,res,steps,tail=None):
    """steps: list of ('o',cmd,val,comment) / ('i',cmd,comment) / ('c',comment)"""
    l=L(base); n=0
    for s in steps:
        if s[0]=='c': l.comment_line(s[1]); continue
        if s[0]=='o':
            _,cmd,val,c=s
            l.op([0xC800|val],"ldb  rl0,#%%%02X"%val,c)
            l.op([0x3A87,(cmd<<8)|0xF6],"soutb %%%04X,rl0"%((cmd<<8)|0xF6),"stack MMU, cmd %%%02X: %s"%(cmd,REG.get(cmd,"")))
        else:
            _,cmd,c=s
            l.op([0x3A85,(cmd<<8)|0xF6],"sinb  rl0,%%%04X"%((cmd<<8)|0xF6),"cmd %%%02X: %s"%(cmd,REG.get(cmd,"")))
            l.op([0x6E08,res+n],"ldb  %%%04X,rl0"%(res+n),"result byte %d: %s"%(n,c)); n+=1
    if tail: tail(l)
    l.op([0x7F00],"sc   #0","BREAK: back to the monitor")
    return l

def progA():
    s=[('c',"clear the attribute byte of descriptors 5 and 63")]
    for cmd,nm in ((0x15,"%15"),(0x16,"%16")):
        for d in (5,63):
            s.append(('o',0x01,d,"SAR := %d"%d)); s.append(('o',0x0A,0,"attribute := 00"))
        s.append(('c',"command %s with data byte 00"%nm))
        s.append(('o',cmd,0,"data byte 00 (must not matter)"))
        for d in (5,63):
            s.append(('o',0x01,d,"SAR := %d"%d)); s.append(('i',0x0A,"attribute of descriptor %d after %s"%(d,nm)))
    return z8010(0x9000,0x9100,s)
def progB():
    s=[('o',0x01,63,"SAR := 63"),('i',0x0E,"attribute of 63 (SAR is incremented afterwards)"),('i',0x01,"SAR after 0E (00 = wrapped)"),
       ('o',0x01,63,"SAR := 63"),('i',0x0A,"attribute of 63, select attribute byte"),('i',0x0F,"read again, DSC passes the attribute: SAR+1"),('i',0x01,"SAR after 0F (00 = wrapped)")]
    return z8010(0x9200,0x9300,s)
def progC():
    s=[('i',0x00,"MR at start (saved for the end)"),('o',0x00,0x38,"MR := 38 (master enable and translate stay off)"),
       ('o',0x01,7,"SAR := 7"),('o',0x20,2,"DSCR := 2"),
       ('i',0x00,"MR set"),('i',0x20,"DSC set"),('i',0x01,"SAR set"),
       ('o',0x10,0,"command %10"),
       ('i',0x00,"MR after %10"),('i',0x20,"DSC after %10"),('i',0x01,"SAR after %10")]
    def tail(l):
        l.op([0x6008,0x9500],"ldb  rl0,%9500","restore the MR saved at start")
        l.op([0x3A87,0x00F6],"soutb %00F6,rl0","MR := saved value")
    return z8010(0x9400,0x9500,s,tail)

def prog01():
    main=L(0x9000)
    main.comment_line("main program, runs in segmented mode (sc #1); rr6 points into the program status area")
    main.op([0x7F01],"sc   #1","MON16: switch to segmented mode")
    main.op([0x2106,0x8000],"ld   r6,#%8000","segment word of rr6: segment 0 in long form")
    main.op([0x7D15],"ldctl r1,PSAPOFF","offset of the program status area (PSAP)")
    main.op([0xA117],"ld   r7,r1","")
    main.op([0x0107,0x001C],"add  r7,#%1C","rr6 -> PC segment word of the system call entry (PSAP+1C)")
    main.op([0x2160],"ld   r0,@rr6","old PC segment word of the sc vector")
    main.op([0x6F00,0x8000,0x9110],"ld   %9110,r0","keep it")
    main.op([0x0D65,0x8000],"ld   @rr6,#%8000","new vector: PC segment word %8000 (bit 15 set)")
    main.op([0xA971],"inc  r7,#2","-> PC offset word of the vector")
    main.op([0x2160],"ld   r0,@rr6","old PC offset")
    main.op([0x6F00,0x8000,0x9112],"ld   %9112,r0","keep it")
    main.op([0x0D65,0x9080],"ld   @rr6,#%9080","new vector: PC offset = handler at %9080")
    main.op([0x4D05,0x8000,0x9100,0x8000],"ld   %9100,#%8000","frame buffer pointer, segment word")
    main.op([0x4D05,0x8000,0x9102,0x9200],"ld   %9102,#%9200","frame buffer pointer, offset: frames go to %9200")
    for n in (0x11,0x12,0x13,0x14):
        main.op([0x7F00|n],"sc   #%%%02X"%n,"system call %d: the handler stores the frame, flips bit 15, IRET"%(n-0x10))
    main.op([0x2106,0x8000],"ld   r6,#%8000","restore the vector")
    main.op([0x7D15],"ldctl r1,PSAPOFF","")
    main.op([0xA117],"ld   r7,r1","")
    main.op([0x0107,0x001C],"add  r7,#%1C","")
    main.op([0x6100,0x8000,0x9110],"ld   r0,%9110","")
    main.op([0x2F60],"ld   @rr6,r0","old PC segment word back")
    main.op([0xA971],"inc  r7,#2","")
    main.op([0x6100,0x8000,0x9112],"ld   r0,%9112","")
    main.op([0x2F60],"ld   @rr6,r0","old PC offset back")
    main.op([0x7F02],"sc   #2","MON16: back to non segmented mode")
    main.op([0x7F00],"sc   #0","BREAK: back to the monitor")
    h=L(0x9080)
    h.comment_line("handler for the system call vector, runs on the system stack (rr14)")
    h.op([0x97E0],"pop  r0,@rr14","identifier word (7F11 ... 7F14)")
    h.op([0x97E1],"pop  r1,@rr14","saved FCW")
    h.op([0x97E2],"pop  r2,@rr14","saved PC segment word (bit 15 is what we look at)")
    h.op([0x97E3],"pop  r3,@rr14","saved PC offset")
    h.op([0x5406,0x8000,0x9100],"ldl  rr6,%9100","rr6 := frame buffer pointer")
    h.op([0x2F60],"ld   @rr6,r0","store identifier")
    h.op([0xA971],"inc  r7,#2","")
    h.op([0x2F61],"ld   @rr6,r1","store FCW")
    h.op([0xA971],"inc  r7,#2","")
    h.op([0x2F62],"ld   @rr6,r2","store PC segment word as it was pushed")
    h.op([0xA971],"inc  r7,#2","")
    h.op([0x2F63],"ld   @rr6,r3","store PC offset")
    h.op([0xA971],"inc  r7,#2","")
    h.op([0x5D06,0x8000,0x9100],"ldl  %9100,rr6","advance the frame buffer pointer")
    h.op([0x0902,0x8000],"xor  r2,#%8000","FLIP bit 15 of the saved PC segment word")
    h.op([0x93E3],"push @rr14,r3","rebuild the frame (reverse order)")
    h.op([0x93E2],"push @rr14,r2","")
    h.op([0x93E1],"push @rr14,r1","")
    h.op([0x93E0],"push @rr14,r0","")
    h.op([0x7B00],"iret","return; the next trap shows whether bit 15 survived")
    return main,h

def prog05():
    """test 05: PC offset wraps at FFFF (segment 1 -> must not continue in segment 2)"""
    main=L(0x9000)
    main.comment_line("main program, runs in segmented mode; installs the system call handler like test 01")
    main.op([0x7F01],"sc   #1","MON16: switch to segmented mode")
    main.op([0x2106,0x8000],"ld   r6,#%8000","segment word of rr6: segment 0 in long form")
    main.op([0x7D15],"ldctl r1,PSAPOFF","offset of the program status area (PSAP)")
    main.op([0xA117],"ld   r7,r1","")
    main.op([0x0107,0x001C],"add  r7,#%1C","rr6 -> PC segment word of the system call entry")
    main.op([0x2160],"ld   r0,@rr6","")
    main.op([0x6F00,0x8000,0x9110],"ld   %9110,r0","keep the old PC segment word")
    main.op([0x0D65,0x8000],"ld   @rr6,#%8000","new vector: segment 0 ...")
    main.op([0xA971],"inc  r7,#2","")
    main.op([0x2160],"ld   r0,@rr6","")
    main.op([0x6F00,0x8000,0x9112],"ld   %9112,r0","keep the old PC offset")
    main.op([0x0D65,0x9080],"ld   @rr6,#%9080","... offset %9080 = the handler")
    main.op([0x4D05,0x8000,0x9100,0x8000],"ld   %9100,#%8000","frame buffer pointer, segment word")
    main.op([0x4D05,0x8000,0x9102,0x9200],"ld   %9102,#%9200","frame buffer pointer, offset: the frame goes to %9200")
    main.comment_line("code in the two segments (physical %10000 and %20000, main memory)")
    main.op([0x4D05,0x8100,0xFFFE,0xA900],"ld   <<1>>%FFFE,#%A900","segment 1, last word: inc r0,#1 (a one word instruction)")
    main.op([0x4D05,0x8100,0x0000,0x7F21],"ld   <<1>>%0000,#%7F21","segment 1, offset 0: sc #%21  (reached if the offset WRAPS)")
    main.op([0x4D05,0x8200,0x0000,0x7F22],"ld   <<2>>%0000,#%7F22","segment 2, offset 0: sc #%22  (reached if the offset CARRIES into the segment)")
    main.op([0x5E08,0x8100,0xFFFE],"jp   <<1>>%FFFE","run the instruction at the last word of segment 1")
    cont=main.pc
    main.comment_line("the handler continues here (it changes the saved PC to this address)")
    main.op([0x2106,0x8000],"ld   r6,#%8000","restore the vector")
    main.op([0x7D15],"ldctl r1,PSAPOFF","")
    main.op([0xA117],"ld   r7,r1","")
    main.op([0x0107,0x001C],"add  r7,#%1C","")
    main.op([0x6100,0x8000,0x9110],"ld   r0,%9110","")
    main.op([0x2F60],"ld   @rr6,r0","old PC segment word back")
    main.op([0xA971],"inc  r7,#2","")
    main.op([0x6100,0x8000,0x9112],"ld   r0,%9112","")
    main.op([0x2F60],"ld   @rr6,r0","old PC offset back")
    main.op([0x7F02],"sc   #2","MON16: back to non segmented mode")
    main.op([0x7F00],"sc   #0","BREAK: back to the monitor")
    h=L(0x9080)
    h.comment_line("handler for the system call vector, runs on the system stack (rr14)")
    h.op([0x97E0],"pop  r0,@rr14","identifier word: 7F21 or 7F22 tells which sc was executed")
    h.op([0x97E1],"pop  r1,@rr14","saved FCW")
    h.op([0x97E2],"pop  r2,@rr14","saved PC segment word: shows the segment the sc was fetched from")
    h.op([0x97E3],"pop  r3,@rr14","saved PC offset (0002)")
    h.op([0x5406,0x8000,0x9100],"ldl  rr6,%9100","rr6 := frame buffer pointer")
    h.op([0x2F60],"ld   @rr6,r0","store identifier")
    h.op([0xA971],"inc  r7,#2","")
    h.op([0x2F61],"ld   @rr6,r1","store FCW")
    h.op([0xA971],"inc  r7,#2","")
    h.op([0x2F62],"ld   @rr6,r2","store PC segment word")
    h.op([0xA971],"inc  r7,#2","")
    h.op([0x2F63],"ld   @rr6,r3","store PC offset")
    h.op([0xA971],"inc  r7,#2","")
    h.op([0x2102,0x8000],"ld   r2,#%8000","new return segment: 0 (long form)")
    h.op([0x2103,cont],"ld   r3,#%%%04X"%cont,"new return offset: the continue address of the main program")
    h.op([0x93E3],"push @rr14,r3","rebuild the frame (reverse order)")
    h.op([0x93E2],"push @rr14,r2","")
    h.op([0x93E1],"push @rr14,r1","")
    h.op([0x93E0],"push @rr14,r0","")
    h.op([0x7B00],"iret","return to the continue address")
    return main,h

def prog06():
    """test 06: a repeating block instruction (ldir) at the end of a segment restarts in the same segment"""
    HAND=0x9180
    main=L(0x9000)
    main.comment_line("main program, runs in segmented mode; installs the system call handler like test 05")
    main.op([0x7F01],"sc   #1","MON16: switch to segmented mode")
    main.op([0x2106,0x8000],"ld   r6,#%8000","segment word of rr6: segment 0 in long form")
    main.op([0x7D15],"ldctl r1,PSAPOFF","offset of the program status area (PSAP)")
    main.op([0xA117],"ld   r7,r1","")
    main.op([0x0107,0x001C],"add  r7,#%1C","rr6 -> PC segment word of the system call entry")
    main.op([0x2160],"ld   r0,@rr6","")
    main.op([0x6F00,0x8000,0x9110],"ld   %9110,r0","keep the old PC segment word")
    main.op([0x0D65,0x8000],"ld   @rr6,#%8000","new vector: segment 0 ...")
    main.op([0xA971],"inc  r7,#2","")
    main.op([0x2160],"ld   r0,@rr6","")
    main.op([0x6F00,0x8000,0x9112],"ld   %9112,r0","keep the old PC offset")
    main.op([0x0D65,HAND],"ld   @rr6,#%%%04X"%HAND,"... offset %9180 = the handler")
    main.op([0x4D05,0x8000,0x9100,0x8000],"ld   %9100,#%8000","frame buffer pointer, segment word")
    main.op([0x4D05,0x8000,0x9102,0x9200],"ld   %9102,#%9200","frame buffer pointer, offset: the frame goes to %9200")
    main.comment_line("source words at %9300, destination words at %9400 (segment 0)")
    for i in range(3):
        main.op([0x4D05,0x8000,0x9300+2*i,0xA001+i],"ld   %%%04X,#%%%04X"%(0x9300+2*i,0xA001+i),"source word %d"%i)
    for i in range(3):
        main.op([0x4D05,0x8000,0x9400+2*i,0x0000],"ld   %%%04X,#%%0000"%(0x9400+2*i),"destination word %d, cleared"%i)
    main.comment_line("code in segment 2 (physical %20000) and a marker in segment 1 (physical %10000), main memory")
    main.op([0x4D05,0x8200,0xFFFC,0xBB41],"ld   <<2>>%FFFC,#%BB41","segment 2, last two words: ldir @rr2,@rr4,r1 (a two word instruction)")
    main.op([0x4D05,0x8200,0xFFFE,0x0120],"ld   <<2>>%FFFE,#%0120","second word of the ldir")
    main.op([0x4D05,0x8200,0x0000,0x7F21],"ld   <<2>>%0000,#%7F21","segment 2, offset 0: sc #%21  (reached after the ldir if the repeat stays in segment 2)")
    main.op([0x4D05,0x8100,0xFFFC,0x7F22],"ld   <<1>>%FFFC,#%7F22","segment 1, offset FFFC: sc #%22  (reached if the repeat borrows from the segment)")
    main.op([0x2101,0x0003],"ld   r1,#3","word count")
    main.op([0x2102,0x8000],"ld   r2,#%8000","destination rr2 = segment 0 ...")
    main.op([0x2103,0x9400],"ld   r3,#%9400","... offset %9400")
    main.op([0x2104,0x8000],"ld   r4,#%8000","source rr4 = segment 0 ...")
    main.op([0x2105,0x9300],"ld   r5,#%9300","... offset %9300")
    main.op([0x5E08,0x8200,0xFFFC],"jp   <<2>>%FFFC","run the ldir at the end of segment 2")
    cont=main.pc
    main.comment_line("the handler continues here (it changes the saved PC to this address)")
    main.op([0x2106,0x8000],"ld   r6,#%8000","restore the vector")
    main.op([0x7D15],"ldctl r1,PSAPOFF","")
    main.op([0xA117],"ld   r7,r1","")
    main.op([0x0107,0x001C],"add  r7,#%1C","")
    main.op([0x6100,0x8000,0x9110],"ld   r0,%9110","")
    main.op([0x2F60],"ld   @rr6,r0","old PC segment word back")
    main.op([0xA971],"inc  r7,#2","")
    main.op([0x6100,0x8000,0x9112],"ld   r0,%9112","")
    main.op([0x2F60],"ld   @rr6,r0","old PC offset back")
    main.op([0x7F02],"sc   #2","MON16: back to non segmented mode")
    main.op([0x7F00],"sc   #0","BREAK: back to the monitor")
    h=L(HAND)
    h.comment_line("handler for the system call vector, runs on the system stack (rr14)")
    h.op([0x97E0],"pop  r0,@rr14","identifier word: 7F21 or 7F22 tells which sc was executed")
    h.op([0x97E1],"pop  r1,@rr14","saved FCW")
    h.op([0x97E2],"pop  r2,@rr14","saved PC segment word: shows the segment the sc was fetched from")
    h.op([0x97E3],"pop  r3,@rr14","saved PC offset (0002)")
    h.op([0x5406,0x8000,0x9100],"ldl  rr6,%9100","rr6 := frame buffer pointer")
    h.op([0x2F60],"ld   @rr6,r0","store identifier")
    h.op([0xA971],"inc  r7,#2","")
    h.op([0x2F61],"ld   @rr6,r1","store FCW")
    h.op([0xA971],"inc  r7,#2","")
    h.op([0x2F62],"ld   @rr6,r2","store PC segment word")
    h.op([0xA971],"inc  r7,#2","")
    h.op([0x2F63],"ld   @rr6,r3","store PC offset")
    h.op([0xA971],"inc  r7,#2","")
    h.op([0x2102,0x8000],"ld   r2,#%8000","new return segment: 0 (long form)")
    h.op([0x2103,cont],"ld   r3,#%%%04X"%cont,"new return offset: the continue address of the main program")
    h.op([0x93E3],"push @rr14,r3","rebuild the frame (reverse order)")
    h.op([0x93E2],"push @rr14,r2","")
    h.op([0x93E1],"push @rr14,r1","")
    h.op([0x93E0],"push @rr14,r0","")
    h.op([0x7B00],"iret","return to the continue address")
    return main,h

def words(l): return [w for _,ws,_,_,lab in l.rows if lab!="#" for w in ws]

if __name__=="__main__":
    import gen, gen_z8010
    m,h=prog01()
    assert words(m)==gen.main and words(h)==gen.hand
    for k,f in (("A",progA),("B",progB),("C",progC)):
        assert words(f())==gen_z8010.PROGS[k]["words"],k
    m5,h5=prog05()
    m6,h6=prog06()
    LISTS={"06-z8000-block-instruction-restart":m6.text()+"\n\n"+h6.text(),"05-z8000-pc-wrap-ffff":m5.text()+"\n\n"+h5.text(),"01-saved-pc-segment-bit15":m.text()+"\n\n"+h.text(),
           "02-z8010-set-all-inhibit-flags":progA().text(),
           "03-z8010-sar-wrap":progB().text(),
           "04-z8010-reset-command-10":progC().text()}
    if "--write" in sys.argv:
        for d,t in LISTS.items():
            p=os.path.join(os.path.dirname(os.path.abspath(__file__)),"..","tests",d,"README.md")
            if not os.path.exists(p): continue
            s=open(p).read()
            blk="## Assembler listing\n\nAddress, machine words, instruction, comment (hand assembled; the words are what you type).\n\n```\n"+t+"\n```\n\n"
            s=re.sub(r"## Assembler listing\n.*?```\n.*?\n```\n\n","",s,flags=re.S)
            s=s.replace("## What comes out",blk+"## What comes out",1)
            open(p,"w").write(s)
    else:
        for d,t in LISTS.items(): print("=====",d); print(t)
