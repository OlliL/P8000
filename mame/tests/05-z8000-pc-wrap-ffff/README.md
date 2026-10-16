# Test 05: the PC offset wraps at FFFF without a carry into the segment

Status: **program ready, not yet run on the real machine** (emulator results below).

## What is tested

Commit `d613b434` (cpu/z8000): when the Z8001 fetches an instruction word at offset `FFFE`,
the PC offset continues at `0000` **in the same segment**; it does not carry into the segment
number. Documents: U8000 CPU description 3.4.2 (instructions cannot cross a segment boundary),
Moore, Z8000 Handbook chapter 2 p. 19 ("past FFFF it wraps to 0000, no carry into the segment
register"). The old emulation added 2 to the whole 23 bit PC, so the next word came from the
next segment.

The program writes a one word instruction (`inc r0,#1`) to the last word of segment 1
(`<<1>>%FFFE`), a system call `sc #%21` to `<<1>>%0000` and another one, `sc #%22`, to
`<<2>>%0000`, and jumps to `<<1>>%FFFE`. After the `inc` the next fetch is at

- segment 1, offset 0 if the offset wraps (executes `sc #%21`),
- segment 2, offset 0 if it carries (executes `sc #%22`).

The system call handler stores the frame (identifier, FCW, saved PC) at `%9200`, so the
identifier and the saved PC segment word tell where the instruction was fetched from. Then it
changes the return address to the end of the main program, which puts the machine back into
the monitor.

Assumption to check on the real machine: with the MMUs off, segment `n` is the physical
memory at `n * 64 KiB`, and main memory exists at `%10000` to `%2FFFF` (at least 192 KiB
installed besides the on-board memory). The emulator has 1 MiB.

## Procedure

Start: reset, Return at the 8 bit monitor, `X`, any key at "Press NMI", then NMI (nothing
else before the NMI, any input stops the boot). At the `*` prompt of MON16: `D <address>`,
then every word on its own line, then `Q`. `G <address>` starts the program, the monitor
answers `BREAK AT ...`. `D <address> <count> W` shows memory (count in words). Hex digits in
upper case only.

Words for `D 9000` (60 words):

```
7F01 2106 8000 7D15 A117 0107 001C 2160
6F00 8000 9110 0D65 8000 A971 2160 6F00
8000 9112 0D65 9080 4D05 8000 9100 8000
4D05 8000 9102 9200 4D05 8100 FFFE A900
4D05 8100 0000 7F21 4D05 8200 0000 7F22
5E08 8100 FFFE 2106 8000 7D15 A117 0107
001C 6100 8000 9110 2F60 A971 6100 8000
9112 2F60 7F02 7F00
```

`Q`, then the words for `D 9080` (24 words, the handler):

```
97E0 97E1 97E2 97E3 5406 8000 9100 2F60
A971 2F61 A971 2F62 A971 2F63 A971 2102
8000 2103 9056 93E3 93E2 93E1 93E0 7B00
```

`Q`, `G 9000`, `D 9200 4 W`.

## Assembler listing

Address, machine words, instruction, comment (hand assembled; the words are what you type).

```
; main program, runs in segmented mode; installs the system call handler like test 01
9000  7F01                 sc   #1                   ; MON16: switch to segmented mode
9002  2106 8000            ld   r6,#%8000            ; segment word of rr6: segment 0 in long form
9006  7D15                 ldctl r1,PSAPOFF          ; offset of the program status area (PSAP)
9008  A117                 ld   r7,r1                
900A  0107 001C            add  r7,#%1C              ; rr6 -> PC segment word of the system call entry
900E  2160                 ld   r0,@rr6              
9010  6F00 8000 9110       ld   %9110,r0             ; keep the old PC segment word
9016  0D65 8000            ld   @rr6,#%8000          ; new vector: segment 0 ...
901A  A971                 inc  r7,#2                
901C  2160                 ld   r0,@rr6              
901E  6F00 8000 9112       ld   %9112,r0             ; keep the old PC offset
9024  0D65 9080            ld   @rr6,#%9080          ; ... offset %9080 = the handler
9028  4D05 8000 9100 8000  ld   %9100,#%8000         ; frame buffer pointer, segment word
9030  4D05 8000 9102 9200  ld   %9102,#%9200         ; frame buffer pointer, offset: the frame goes to %9200
; code in the two segments (physical %10000 and %20000, main memory)
9038  4D05 8100 FFFE A900  ld   <<1>>%FFFE,#%A900    ; segment 1, last word: inc r0,#1 (a one word instruction)
9040  4D05 8100 0000 7F21  ld   <<1>>%0000,#%7F21    ; segment 1, offset 0: sc #%21  (reached if the offset WRAPS)
9048  4D05 8200 0000 7F22  ld   <<2>>%0000,#%7F22    ; segment 2, offset 0: sc #%22  (reached if the offset CARRIES into the segment)
9050  5E08 8100 FFFE       jp   <<1>>%FFFE           ; run the instruction at the last word of segment 1
; the handler continues here (it changes the saved PC to this address)
9056  2106 8000            ld   r6,#%8000            ; restore the vector
905A  7D15                 ldctl r1,PSAPOFF          
905C  A117                 ld   r7,r1                
905E  0107 001C            add  r7,#%1C              
9062  6100 8000 9110       ld   r0,%9110             
9068  2F60                 ld   @rr6,r0              ; old PC segment word back
906A  A971                 inc  r7,#2                
906C  6100 8000 9112       ld   r0,%9112             
9072  2F60                 ld   @rr6,r0              ; old PC offset back
9074  7F02                 sc   #2                   ; MON16: back to non segmented mode
9076  7F00                 sc   #0                   ; BREAK: back to the monitor

; handler for the system call vector, runs on the system stack (rr14)
9080  97E0                 pop  r0,@rr14             ; identifier word: 7F21 or 7F22 tells which sc was executed
9082  97E1                 pop  r1,@rr14             ; saved FCW
9084  97E2                 pop  r2,@rr14             ; saved PC segment word: shows the segment the sc was fetched from
9086  97E3                 pop  r3,@rr14             ; saved PC offset (0002)
9088  5406 8000 9100       ldl  rr6,%9100            ; rr6 := frame buffer pointer
908E  2F60                 ld   @rr6,r0              ; store identifier
9090  A971                 inc  r7,#2                
9092  2F61                 ld   @rr6,r1              ; store FCW
9094  A971                 inc  r7,#2                
9096  2F62                 ld   @rr6,r2              ; store PC segment word
9098  A971                 inc  r7,#2                
909A  2F63                 ld   @rr6,r3              ; store PC offset
909C  A971                 inc  r7,#2                
909E  2102 8000            ld   r2,#%8000            ; new return segment: 0 (long form)
90A2  2103 9056            ld   r3,#%9056            ; new return offset: the continue address of the main program
90A6  93E3                 push @rr14,r3             ; rebuild the frame (reverse order)
90A8  93E2                 push @rr14,r2             
90AA  93E1                 push @rr14,r1             
90AC  93E0                 push @rr14,r0             
90AE  7B00                 iret                      ; return to the continue address
```

## What comes out

One frame (identifier, FCW, PC segment word, PC offset) at `%9200`.

| | Frame | Meaning |
|---|---|---|
| Emulator with the change (and the expected real result) | `7F21 D000 8100 0002` | `sc #%21` executed from segment 1, offset 0: the offset wrapped |
| Emulator without the change (old core) | `7F22 D000 8200 0002` | `sc #%22` executed from segment 2, offset 0: the offset carried into the segment |

The monitor shows `BREAK AT 9076` afterwards (emulator).

## What it means and which claim it supports

`7F21 ... 8100 0002` on the real board means the real U8001 continues at offset 0 of the
**same** segment after the last word of a segment, which is the claim of commit `d613b434`
("PC offset wraps at FFFF without carry into the segment"). `7F22 ... 8200 0002` would show that
the real CPU carries into the segment number and the change would be wrong.

## Photos of the screen (real machine)

- (none yet)

## Result on the real machine

| Date | Board / monitor | Bytes read | Same as the emulator? |
|---|---|---|---|
| | | | |
