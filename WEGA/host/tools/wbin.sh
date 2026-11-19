#!/bin/sh
# wbin.sh DIR WEGA OLDROOT EMU: scripts under the names of the WEGA tools that run the originals under the emulator, for the PATH
# of make: the Makefiles of src use the plain names (cc, ld, cas, ccv0p, ...). ccvN run in OLDROOT, the others in WEGA.
d=$1; wega=$2; old=$3; emu=$4
mkdir -p $d
for t in cc scc cas as ld sld gld ar yacc chkout; do
	case $t in yacc) g=/usr/bin/$t ;; *) g=/bin/$t ;; esac
	case $t in
	# chkout writes the time of the checkout into ver.c ($D$): the kernel's date as in the shipped wega, 10.03.1989 09:57:01 (guest zone EST)
	chkout) printf '#!/bin/sh\nZ8K_TIME=605545021 exec %s -r %s %s "$@"\n' $emu $wega $g > $d/$t ;;
	*) printf '#!/bin/sh\nexec %s -r %s %s "$@"\n' $emu $wega $g > $d/$t ;;
	esac
	chmod +x $d/$t
done
# wrun PROGRAM [ARG ...]: a program of the WEGA system that a Makefile runs itself (a generator: maketerms), under the emulator;
# the Makefiles call it as $(RUN) ./PROGRAM with RUN=wrun (exported by the callers: host/Makefile, checkall.sh), RUN empty on WEGA itself
# the guest's /tmp and /usr/tmp are a directory of their own for each call (Z8K_OVERLAY): lint1 leaves files in /usr/tmp, which must not land in WEGA;
# a caller that sets Z8K_OVERLAY itself (a layer over the root: the install tree) gets that one
{
	echo '#!/bin/sh'
	echo 'if [ -n "$Z8K_OVERLAY" ]; then exec' $emu -r $wega '"$@"; fi'
	echo 'o=`mktemp -d` || exit 1'
	echo 'mkdir -p "$o/tmp" "$o/usr/tmp"'
	echo "Z8K_OVERLAY=\"\$o\" $emu -r $wega \"\$@\""
	echo 'rc=$?'
	echo 'rm -rf "$o"'
	echo 'exit $rc'
} > $d/wrun
chmod +x $d/wrun
# strip [-h] FILE ...: each file by its name in its own directory (the guest sees only WEGA and the working directory;
# make install strips below PREFIX)
{
	echo '#!/bin/sh'
	echo 'o='
	echo 'for a; do'
	echo '	case $a in'
	echo '	-*) o="$o $a" ;;'
	echo "	*) (cd \`dirname \$a\` && $emu -r $wega /usr/bin/strip \$o \`basename \$a\`) || exit 1 ;;"
	echo '	esac'
	echo 'done'
} > $d/strip
chmod +x $d/strip
for t in sccv1 sccv1c sccvk sccvc sccvl ccv1 ccv1c ccv1q ccv1r ccv0 ccv0p ccvl; do
	printf '#!/bin/sh\nexec %s -r %s /bin/%s "$@"\n' $emu $old $t > $d/$t
	chmod +x $d/$t
done
