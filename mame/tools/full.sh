#!/bin/bash
cd /home/claude/runs/sc
python3 gen.py >/dev/null
python3 - <<'PY'
import json
p=json.load(open("prog.json"))
def lines(addr,words):
    return ["D %04X\\r"%addr]+["%04X\\r"%x for x in words]+["Q\\r"]
L=lines(0x9000,p["main"])+lines(0x9080,p["hand"])+["D 9000 30 W\\r","D 9080 1A W\\r"]
open("enter.txt","w").write("\n".join(L)+"\n")
PY
bash start.sh
(printf '\\r\n' > mon.in &)
until grep -a -q "Press RETURN" mon.log 2>/dev/null; do sleep 2; done
sleep 1; printf '\\r\n' > mon.in; sleep 4; printf 'X\\r\n' > mon.in
until grep -a -q "Press NMI" mon.log; do sleep 2; done
sleep 1; printf '\\r\n' > mon.in; sleep 3; echo nmi > /home/claude/runs/ctl.txt
until grep -a -q "^NMI" mon.log; do sleep 2; done; sleep 3
cat enter.txt > mon.in
until grep -a -q "^<00>90B0 7B00" mon.log; do sleep 3; done
sleep 3; echo READY >> /home/claude/runs/sc/state.txt
