# mku.sh IN OUT: u.o from the object of u.s. The user structure lies at segment 62 (%3E), offset %F600 (UBASE in sys/param.h), but as can
# give an absolute symbol only the number of the current section as its segment (TODO.md, README.md). So the segment of the symbol _u is
# set here: the high byte of its value (offset 84) and its segment byte (offset 89). %3E is the character >. dd and cat also exist on WEGA.
dd if=$1 bs=1 count=84 > $2 2>/dev/null
echo '>' | dd bs=1 count=1 >> $2 2>/dev/null
dd if=$1 bs=1 skip=85 count=4 >> $2 2>/dev/null
echo '>' | dd bs=1 count=1 >> $2 2>/dev/null
dd if=$1 bs=1 skip=90 >> $2 2>/dev/null
