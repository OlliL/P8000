-- polls /home/claude/runs/ctl.txt: "nmi" or "swap <file>" or "swap0 <file>"
local nmi
for k,v in pairs(manager.machine.ioport.ports[":NMI"].fields) do nmi = v end
local imgs = {}
for tag,im in pairs(manager.machine.images) do print("IMG "..tag) end
local state = {t=0, down=nil}
emu.register_periodic(function()
  local t = manager.machine.time:as_double()
  if state.down and t >= state.down then nmi:set_value(0); state.down=nil end
  if t - state.t < 0.5 then return end
  state.t = t
  local f = io.open("/home/claude/runs/ctl.txt","r")
  if not f then return end
  local l = f:read("*l"); f:close()
  if not l then return end
  os.remove("/home/claude/runs/ctl.txt")
  local cmd, arg = l:match("^(%S+)%s*(.*)$")
  if cmd == "nmi" then nmi:set_value(1); state.down = t + 0.3; print("NMI")
  elseif cmd == "reset" then manager.machine:soft_reset(); print("RESET")
  elseif cmd == "swap" or cmd == "swap0" then
    local im = manager.machine.images[(cmd=="swap") and ":i8272:1:525qd" or ":i8272:0:525qd"]
    im:unload(); local r = im:load(arg); print("SWAP "..arg.." -> "..tostring(r))
  end
end)
local cpu = manager.machine.devices[":maincpu"]
local sp = cpu.spaces["io_std"] or cpu.spaces["io"]
local lf = io.open("/home/claude/runs/fdc_trace.txt","w")
local on = false
sp:install_write_tap(0x20, 0x21, "fdcw", function(off, data, mask) if on then lf:write(string.format("W %02x %02x\n", off, data & 0xff)); lf:flush() end end)
sp:install_read_tap(0x20, 0x21, "fdcr", function(off, data, mask) if on and off==1 then lf:write(string.format("R %02x %02x\n", off, data & 0xff)); lf:flush() end end)
local sp2 = sp
emu.register_periodic(function()
  local f = io.open("/home/claude/runs/trace_on","r")
  if f then f:close(); on = true else on = false end
end)
-- dasm / pc requests
local cpu16 = manager.machine.devices[":cpu16"] or manager.machine.devices[":maincpu"]
emu.register_periodic(function()
  local f = io.open("/home/claude/runs/sc/req.txt","r")
  if not f then return end
  local l = f:read("*l"); f:close(); os.remove("/home/claude/runs/sc/req.txt")
  local a,n = l:match("^(%S+)%s+(%S+)")
  if a == "pc" then
    local o = io.open("/home/claude/runs/sc/pc.txt","w")
    local st = cpu16.state
    o:write(string.format("PC=%x SEG=%s FCW=%x\n", st["CURPC"] and st["CURPC"].value or 0, "", st["FCW"] and st["FCW"].value or 0)); o:close()
  else
    manager.machine.debugger:command("dasm /home/claude/runs/sc/dasm.txt,"..a..","..n..",0,:cpu16")
  end
end)
emu.register_periodic(function()
  local f = io.open("/home/claude/runs/sc/req2.txt","r")
  if not f then return end
  local l = f:read("*l"); f:close(); os.remove("/home/claude/runs/sc/req2.txt")
  local a,n = l:match("^(%S+)%s+(%S+)")
  a=tonumber(a); n=tonumber(n)
  local sp = cpu16.spaces["program"]
  local o = io.open("/home/claude/runs/sc/mem.txt","w")
  for i=0,n-1,2 do o:write(string.format("%04x: %04x\n", a+i, sp:read_u16(a+i))) end
  o:close()
  local st=cpu16.state
  local o2=io.open("/home/claude/runs/sc/regs.txt","w")
  for k,v in pairs(st) do o2:write(k.."="..string.format("%x",v.value).."\n") end o2:close()
end)
local tl = io.open("/home/claude/runs/sc/tap.txt","w")
local pspace = cpu16.spaces["data"]
pspace:install_write_tap(0x9100,0x911f,"w9100",function(off,data,mask) tl:write(string.format("W %04x=%04x pc=%x\n",off,data,cpu16.state["CURPC"].value)); tl:flush() end)
pspace:install_write_tap(0x9200,0x921f,"w9200",function(off,data,mask) tl:write(string.format("W %04x=%04x pc=%x\n",off,data,cpu16.state["CURPC"].value)); tl:flush() end)
pspace:install_read_tap(0x9000,0x9003,"r9000",function(off,data,mask) tl:write(string.format("F %04x pc=%x\n",off,cpu16.state["CURPC"].value)); tl:flush() end)
pspace:install_read_tap(0x9080,0x9083,"r9080",function(off,data,mask) tl:write(string.format("H %04x pc=%x\n",off,cpu16.state["CURPC"].value)); tl:flush() end)
