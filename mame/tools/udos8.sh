#!/bin/bash
cd /home/claude/runs
for p in $(ps -eo pid,args | grep -a "[m]ame p8000" | awk '{print $1}'); do kill -9 $p; done
for p in $(ps -eo pid,args | grep -a "[c]onsole.py 5630" | awk '{print $1}'); do kill -9 $p; done
sleep 1
rm -f u.log u.in; mkfifo u.in
export LD_LIBRARY_PATH=/home/claude/sdl2/lib SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy
(setsid nohup python3 console.py 5630 u.log u.in > u_console.out 2>&1 &)
sleep 2
cd /home/claude/mamedev/mame
S=/home/claude/sw/p8000_flop
(setsid nohup ./mame p8000 -rompath /home/claude/roms -video none -sound none -seconds_to_run 900 -rs232 null_modem -bitb socket.127.0.0.1:5630 -flop1 $S/udos31/udossys.cqm -autoboot_script /home/claude/runs/lua/ctl2.lua > /home/claude/runs/u_mame.out 2>&1 &)
