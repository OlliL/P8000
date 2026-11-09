# mku.sh IN OUT: u.o from IN, the object that "as -u" makes from u.s.
#
# The user structure lies at segment 62 (%3E), offset %F600 (UBASE in sys/param.h). The shipped u.o (98 bytes) holds only the symbol
# _u with this address. as makes every byte of it from u.s except two, the segment of _u: the high byte of its value (offset 84) and
# its segment byte (offset 89), %01 instead of %3E. This script sets them. %3E is the character >. dd and echo also exist on WEGA.
#
# Why the segment cannot come from a source (measured with /bin/as and /bin/cas):
# - The source was for as (PLZ/ASM), not cas: the symbols u_p and u_d of the object are the default sections of a PLZ/ASM module named
#   u, which cas does not write. In cas, "_u ::= <<62>>%f600" gives the value %3E00F600, but with symbol type %21 instead of %61.
# - $ABS takes a 16-bit offset (Z8000 assembler manual, 4.3.2 and appendix C: a constant expression); a longer value is cut.
# - The segment of the symbol is the index of the current section: 1 for the data section u_d, 2 behind "$SECTION foo".
#   An index of 62 would need 61 more sections, and each of them has a symbol in the table; the shipped object has only u_p, u_d and _u.
# - <<62>> in $ABS or in a constant is an error or ignored ($abs <<62>>0: offset 0, segment 1). $SECTION takes no segment.
# - The as of ZEUS 3.21 has the same size and gives the same results. sld -r -M changes the structure of the object.
# So the two bytes were probably patched after assembling in the original too (a guess; only the difference itself is measured).
dd if=$1 bs=1 count=84 > $2 2>/dev/null
echo '>' | dd bs=1 count=1 >> $2 2>/dev/null
dd if=$1 bs=1 skip=85 count=4 >> $2 2>/dev/null
echo '>' | dd bs=1 count=1 >> $2 2>/dev/null
dd if=$1 bs=1 skip=90 >> $2 2>/dev/null
