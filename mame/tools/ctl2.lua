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
