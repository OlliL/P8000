#!/usr/bin/env python3
"""mamerun.py - the P8000 in MAME as an independent measuring instrument for the models of increment 117 (the 8-bit part,
NDOS, the coupling): a session starts MAME without picture and sound, its console on a socket, floppies as MFI images
(MAME writes them back when it ends), and is driven from Python.

    from mamerun import Session
    with Session('p8000', work='/tmp/x', flop={1: 'udossys.raw', 2: 'empty.raw'}) as m:
        m.expect('...')            # wait until the console has printed it
        m.send('CAT D=1\\r')        # type (WEGA: parity=True, 7E1)
    # afterwards: m.floppy_raw(2) is the RAW image of drive 2 as MAME left it

MAME: /mnt/share/git/mame/mame (MAME_BIN), its floptool beside it; ROMs: MAME_ROMS (the roms directory of that MAME
tree: a directory per machine) and MAME_ROMS8 (the owner's ROM files, flat: linked as WORK/roms/p8000). The floppies go in as RAW (host/tools/imd.py: RAW -> IMD -> floptool -> MFI) and come back the same way.
Machines (mame/LOG.md): p8000 (8-bit only, UDOS), p8000_16 (with the 16-bit board: WEGA from a hard disk image)."""
import os
import shutil
import socket
import subprocess
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import imd  # noqa: E402

MAME = os.environ.get('MAME_BIN', '/mnt/share/git/mame/mame')
FLOPTOOL = os.path.join(os.path.dirname(MAME), 'floptool')
ROMS = os.environ.get('MAME_ROMS', '/mnt/share/git/mame/roms')	# a directory per machine (p8000_16 is there)
ROMS8 = os.environ.get('MAME_ROMS8', '/home/olivleh1/Downloads/P8000/mame/roms')	# the ROMs flat, among them the
# *_nur8bit of the machine p8000 (the 8-bit board alone): a session links them into WORK/roms/p8000

LUA = r'''
-- the control of a session: polls CTL for "nmi", "reset", "swap N FILE", "exit"
local ctl = "%s"
local nmi
for k,v in pairs(manager.machine.ioport.ports[":NMI"].fields) do nmi = v end
local st = {t=0, down=nil}
emu.register_periodic(function()
  local t = manager.machine.time:as_double()
  if st.down and t >= st.down then nmi:set_value(0); st.down=nil end
  if t - st.t < 0.2 then return end
  st.t = t
  local f = io.open(ctl, "r")
  if not f then return end
  local l = f:read("*l"); f:close()
  os.remove(ctl)
  if not l then return end
  local cmd, a, b = l:match("^(%%S+)%%s*(%%S*)%%s*(.*)$")
  if cmd == "nmi" then nmi:set_value(1); st.down = t + 0.3
  elseif cmd == "reset" then manager.machine:soft_reset()
  elseif cmd == "swap" then
    local im = manager.machine.images[":i8272:" .. a .. ":525qd"]
    im:unload(); im:load(b)
  elseif cmd == "exit" then manager.machine:exit()
  end
end)
'''


def raw_to_mfi(raw, mfi, nsec=16, size=256):
    tmp = mfi + '.imd'
    imd.toimd(raw, tmp, nsec, size)
    subprocess.run([FLOPTOOL, 'flopconvert', 'auto', 'mfi', tmp, mfi], check=True, capture_output=True)
    os.remove(tmp)


def mfi_to_raw(mfi, raw):
    tmp = raw + '.imd'
    subprocess.run([FLOPTOOL, 'flopconvert', 'auto', 'imd', mfi, tmp], check=True, capture_output=True)
    imd.toraw(tmp, raw)
    os.remove(tmp)


class Session:
    def __init__(self, machine, work, flop=None, hard=None, seconds=3000, extra=()):
        self.machine, self.work = machine, work
        os.makedirs(work, exist_ok=True)
        self.flop = {}
        for n, src in (flop or {}).items():	# drive number 1, 2 (MAME -flop1, -flop2) -> image
            mfi = os.path.join(work, 'flop%d.mfi' % n)
            if src.endswith('.cqm') or src.endswith('.mfi'):
                subprocess.run([FLOPTOOL, 'flopconvert', 'auto', 'mfi', src, mfi], check=True, capture_output=True)
            else:
                geo = (9, 512) if os.path.getsize(src) == 737280 else (16, 256)
                raw_to_mfi(src, mfi, *geo)
            self.flop[n] = mfi
        self.ctl = os.path.join(work, 'ctl.txt')
        lua = os.path.join(work, 'ctl.lua')
        with open(lua, 'w') as f:
            f.write(LUA % self.ctl)
        rd = os.path.join(work, 'roms', 'p8000')
        os.makedirs(rd, exist_ok=True)
        for f in os.listdir(ROMS8):
            if not os.path.exists(os.path.join(rd, f)):
                os.symlink(os.path.join(ROMS8, f), os.path.join(rd, f))
        self.srv = socket.socket()
        self.srv.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.srv.bind(('127.0.0.1', 0))
        self.srv.listen(1)
        port = self.srv.getsockname()[1]
        args = [MAME, machine, '-rompath', os.path.join(work, 'roms') + ';' + ROMS, '-video', 'none', '-sound', 'none', '-seconds_to_run', str(seconds),
                '-rs232', 'null_modem', '-bitb', 'socket.127.0.0.1:%d' % port, '-autoboot_script', lua, '-nothrottle']
        for n, mfi in self.flop.items():
            args += ['-flop%d' % n, mfi]
        if hard:
            args += ['-hard', hard]
        args += list(extra)
        env = dict(os.environ, SDL_VIDEODRIVER='dummy', SDL_AUDIODRIVER='dummy')
        self.log = open(os.path.join(work, 'mame.out'), 'wb')
        self.proc = subprocess.Popen(args, cwd=work, stdout=self.log, stderr=subprocess.STDOUT, env=env)
        self.srv.settimeout(120)
        self.conn, _ = self.srv.accept()
        self.conn.settimeout(0.2)
        self.buf = b''
        self.console = open(os.path.join(work, 'console.log'), 'wb', buffering=0)

    def _pump(self):
        try:
            d = self.conn.recv(65536)
            if d:
                self.buf += d
                self.console.write(d)
        except socket.timeout:
            pass

    def expect(self, text, timeout=300):
        """wait until text (bytes or str) has appeared since the last expect; returns what came before it"""
        if isinstance(text, str):
            text = text.encode('latin-1')
        end = time.time() + timeout
        while time.time() < end:
            i = self.strip7().find(text)
            if i >= 0:
                got = self.strip7()[:i + len(text)]
                self.buf = self.buf[i + len(text):]
                return got.decode('latin-1')
            if self.proc.poll() is not None:
                break
            self._pump()
        raise TimeoutError('not seen: %r; last output: %r' % (text, self.strip7()[-300:]))

    def strip7(self):
        return bytes(c & 0x7F for c in self.buf)

    def send(self, text, parity=False, delay=0.03):
        """type text; what the console printed before is dropped (expect then sees only the answer)"""
        self._pump()
        self.buf = b''
        for ch in text.encode('latin-1'):
            if parity and bin(ch).count('1') % 2:
                ch |= 0x80
            self.conn.send(bytes([ch]))
            time.sleep(delay)
            self._pump()

    def control(self, cmd):
        with open(self.ctl, 'w') as f:
            f.write(cmd + '\n')
        while os.path.exists(self.ctl):
            time.sleep(0.05)
            self._pump()

    def insert(self, n, raw):
        """put the RAW image into drive n (1, 2: MAME's -flop1, -flop2) while the machine runs"""
        self.ninsert = getattr(self, 'ninsert', 0) + 1
        mfi = os.path.join(self.work, 'flop%d_%d.mfi' % (n, self.ninsert))
        geo = (9, 512) if os.path.getsize(raw) == 737280 else (16, 256)
        raw_to_mfi(raw, mfi, *geo)
        self.flop[n] = mfi
        self.control('swap %d %s' % (n - 1, mfi))
        return mfi	# mfi_to_raw(it, RAW) after the session: the disk as MAME left it

    def floppy_raw(self, n, raw):
        """the image of drive n as MAME left it, as RAW (after the session)"""
        mfi_to_raw(self.flop[n], raw)

    def close(self):
        if self.proc.poll() is None:
            self.control('exit')
            try:
                self.proc.wait(60)
            except subprocess.TimeoutExpired:
                self.proc.kill()
        self.conn.close()
        self.srv.close()

    def __enter__(self):
        return self

    def __exit__(self, *a):
        self.close()
