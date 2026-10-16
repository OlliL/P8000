#!/bin/bash
# usage: ramtest.sh SIZE  -> start the hardware test with -ram SIZE (output in inst.log)
cd /home/claude/runs
for p in $(ps -eo pid,args | grep -a "[m]ame p8000" | awk '{print $1}'); do kill -9 $p; done
for p in $(ps -eo pid,args | grep -a "[c]onsole.py 5620" | awk '{print $1}'); do kill -9 $p; done
sleep 1
rm -f ctl.txt inst.log; cp inst_after_install_done.hd rt.hd
export LD_LIBRARY_PATH=/home/claude/sdl2/lib SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy
(setsid nohup python3 console.py 5620 inst.log inst.in > console.out 2>&1 &)
sleep 2
cd /home/claude/mamedev/mame
S=/home/claude/sw/p8000_flop
(setsid nohup ./mame p8000_16 -rompath /home/claude/roms -video none -sound none -nothrottle -seconds_to_run 20000 -ram $1 -rs232 null_modem -bitb socket.127.0.0.1:5620 -flop1 $S/w31start/w31start.cqm -hard /home/claude/runs/rt.hd -autoboot_script /home/claude/runs/lua/ctl2.lua > /home/claude/runs/rt_mame.out 2>&1 &)
cd /home/claude/runs
sleep 25; echo '\r' > inst.in; sleep 3; echo '\r' > inst.in; sleep 15; echo nmi > ctl.txt
