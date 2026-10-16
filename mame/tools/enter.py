import json,sys
p=json.load(open("prog.json"))
def lines(addr,words):
    out=["D %04X\\r"%addr]
    out+=["%04X\\r"%x for x in words]
    out.append("Q\\r")
    return out
L=lines(0x9000,p["main"])+lines(0x9080,p["hand"])+["D 9200 8 W\\r"]
open("mon.in","w").write("\n".join(L)+"\n")
