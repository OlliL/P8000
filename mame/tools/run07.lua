-- Test 07 (tests/07-p8000-16-fetch-with-mmu-off) runs 7, 8 and 8K in the emulator.
--
--   RUN=8  mame p8000_16 -rs232 terminal -video none -sound none -nothrottle \
--          -autoboot_script run07.lua          (RUN=8, 8k or 7)
--
-- Types RETURN and X at the 8 bit monitor through the generic terminal, then takes over the
-- U8001 while MON16 waits, writes the program and starts it segmented in system mode. The
-- programs end with jr $ instead of sc #0; after one second R5/R6 are printed:
--   run 8  : R5=0000 R6=0001 (fetch at 9100 read 9000: A8-A23 held)
--   run 8k : R5=0001 R6=0000 (control, code MMU left on)
--   run 7  : R5=0001         (nop + two-word outb in the held page)
local run = os.getenv("RUN") or "8"
local cpu = manager.machine.devices[":cpu16"]
local sp = cpu.spaces["program"]
local nat = manager.machine.natkeyboard
local steps = 0
local t0

local main8 = { 0x2105,0x0000, 0x2106,0x0000, 0x2100,0x0000, 0x3A07,0x00FA, 0x3A07,0x00F6,
	0x2101,0xFFC1, 0x2100,0x8000, 0x3A07,0x00FC, 0x2100,0x0200, 0x3A06,0xFFC1,
	0x2100,0x0000, 0x3A07,0x00FC, 0xA950, 0x3E10, 0xE8FF }
local img8 = { 0xA960, 0x3E10, 0xE8FF }
-- run 7 at 9000: nop + two-word outb, all in page 90
local main7 = { 0x2100,0x0000, 0x3A07,0x00FA, 0x3A07,0x00F6, 0x2101,0xFFC1, 0x2100,0x8000,
	0x3A07,0x00FC, 0x2100,0x0200, 0x3A06,0xFFC1, 0x2100,0x0000, 0x3A07,0x00FC,
	0x8D07, 0x3A06,0xFFC1, 0xA950, 0xE8FF }

local function put(addr, words)
	for i, w in ipairs(words) do sp:write_u16(addr + 2 * (i - 1), w) end
end

emu.register_periodic(function()
	local t = manager.machine.time:as_double()
	if steps == 0 and t > 3 then nat:post("\r"); steps = 1
	elseif steps == 1 and t > 6 then nat:post("X\r"); steps = 2
	elseif steps == 2 and t > 14 then
		print(string.format("TAKEOVER PC=%X FCW=%X", cpu.state["PC"].value, cpu.state["FCW"].value))
		local start
		if run == "7" then
			put(0x9000, main7); start = 0x9000
		else
			put(0x9000, img8); put(0x90D0, main8); start = 0x90D0
			if run == "8k" then sp:write_u16(0x90FC, 0x8D07); sp:write_u16(0x90FE, 0x8D07) end
		end
		local st = cpu.state
		st["FCW"].value = 0xC000
		st["PC"].value = start
		steps = 3; t0 = t
	elseif steps == 3 and t > t0 + 1 then
		local st = cpu.state
		print(string.format("RESULT run=%s R5=%04X R6=%04X PC=%X", run, st["R5"].value, st["R6"].value, st["PC"].value))
		manager.machine:exit()
	end
end)
