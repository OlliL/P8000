#!/bin/sh
# wbin.sh DIR WEGA OLDROOT EMU: scripts under the names of the WEGA tools that run the originals under the emulator, for the PATH
# of make: the Makefiles of src use the plain names (cc, ld, cas, ccv0p, ...). ccvN run in OLDROOT, the others in WEGA.
d=$1; wega=$2; old=$3; emu=$4
mkdir -p $d
for t in cc scc cas as ld sld gld ar strip yacc; do
	case $t in yacc|strip) g=/usr/bin/$t ;; *) g=/bin/$t ;; esac
	printf '#!/bin/sh\nexec %s -r %s %s "$@"\n' $emu $wega $g > $d/$t
	chmod +x $d/$t
done
for t in sccv1 ccv1 ccv1q ccv1r ccv0 ccv0p; do
	printf '#!/bin/sh\nexec %s -r %s /bin/%s "$@"\n' $emu $old $t > $d/$t
	chmod +x $d/$t
done
