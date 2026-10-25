
make \
    CC="/mnt/share/cvs/P8000/WEGA/host/cc/cc" \
   SCC="/mnt/share/cvs/P8000/WEGA/host/cc/scc" \
    LD="/mnt/share/cvs/P8000/WEGA/host/ld/ld" \
   GLD="/mnt/share/cvs/P8000/WEGA/host/ld/gld" \
   SLD="/mnt/share/cvs/P8000/WEGA/host/ld/sld" \
  YACC="/mnt/share/cvs/P8000/WEGA/host/z8k-emu/z8k-emu -r /mnt/users/olivleh1/BACKUPs/P8000/harddisks/oliver_lehmann_robk5504.50_1/extracted/ /usr/bin/yacc" \
   CAS="/mnt/share/cvs/P8000/WEGA/host/cas/cas" \
 STRIP="/mnt/share/cvs/P8000/WEGA/host/binutils/strip" \
    AS="/mnt/share/cvs/P8000/WEGA/host/z8k-emu/z8k-emu -r /mnt/users/olivleh1/BACKUPs/P8000/harddisks/oliver_lehmann_robk5504.50_1/extracted/ /bin/as" \
    AR="/mnt/share/cvs/P8000/WEGA/host/ar/ar" \
$*
