#!/bin/sh
# Compare the host ranlib, strip and size with the originals (run in z8k-emu).
# A corpus (objects and libraries of the WEGA tree, byte swapped copies, damaged
# files) is copied twice per case; the original and the host program run on the
# copies. The output, the exit code and every file afterwards must be identical
# (ranlib: apart from the date it stores, now + 5 s).
#   check.sh   (WEGA=dir with bin/ lib/; E=emulator)
HERE=$(cd "$(dirname "$0")" && pwd)
E=${E:-$HERE/../z8k-emu/z8k-emu}; R=${WEGA:?set WEGA to the WEGA file tree}
HAR=$HERE/../ar/ar
T=$(mktemp -d); trap 'rm -rf "$T"' EXIT
export LC_ALL=C
unset TZ
C=$T/corpus; mkdir -p $C/o $C/s $C/b $C/w $C/a $C/j

# objects: the members of libc.a and slibc.a, binaries, libraries
(cd $C/o && "$HAR" x "$R/lib/libc.a")
(cd $C/s && "$HAR" x "$R/lib/slibc.a")
rm -f $C/o/__.SYMDEF $C/s/__.SYMDEF
for f in bin/ar bin/size bin/cc bin/cas bin/ld bin/sld bin/gld lib/cpp lib/cparse lib/gopt lib/crt0.o lib/scrt0.o; do
	[ -f "$R/$f" ] && cp "$R/$f" "$C/b/$(echo $f | tr / _)"
done
for f in $R/lib/*.a $R/usr/lib/*.a; do cp "$f" "$C/a/$(basename $f)"; done
# byte swapped copies: swab of the header and the segment table (what swab does in the tools)
python3 - "$C" <<'PY'
import sys,os,glob
C=sys.argv[1]
def swab(b): 
    b=bytearray(b)
    for i in range(0,len(b)-1,2): b[i],b[i+1]=b[i+1],b[i]
    return bytes(b)
srcs=sorted(glob.glob(C+'/o/*.o'))[:6]+sorted(glob.glob(C+'/s/*.o'))[:6]+sorted(glob.glob(C+'/b/*'))[:3]
for p in srcs:
    d=open(p,'rb').read()
    n=24+int.from_bytes(d[10:12],'big')
    open(C+'/w/'+os.path.basename(p),'wb').write(swab(d[:n])+d[n:])
d=open(srcs[0],'rb').read()
open(C+'/j/text','wb').write(b'just some text\n')
open(C+'/j/empty','wb').write(b'')
open(C+'/j/one','wb').write(b'x')
open(C+'/j/short.o','wb').write(d[:30])
open(C+'/j/hdronly.o','wb').write(d[:24])
open(C+'/j/zeros','wb').write(bytes(100))
PY

# more archives for ranlib: swapped, stripped and damaged members, text members, too many symbols
mkdir -p $C/x $C/big $T/ex
(cd $C/w && "$HAR" r ../x/swapped.a $(ls | head -8) >/dev/null 2>&1)
mkdir $T/st && cp $C/o/read.o $C/o/write.o $C/o/open.o $C/o/close.o $T/st/
"$HERE/strip" $T/st/read.o $T/st/write.o
(cd $T/st && cp $C/j/text . && "$HAR" r $C/x/stripped.a read.o write.o open.o >/dev/null 2>&1 && "$HAR" r $C/x/mixed.a text read.o open.o $C/j/short.o $C/j/zeros close.o >/dev/null 2>&1)
(cd $C/j && "$HAR" r $C/x/textonly.a text empty one >/dev/null 2>&1)
(cd $C/x && "$HAR" r onlyone.a $C/o/read.o >/dev/null 2>&1)
for f in $C/s/*.o; do cp "$f" "$C/big/s_$(basename $f)"; done
for f in $C/o/*.o; do cp "$f" "$C/big/o_$(basename $f)"; done
for l in libcurses.a libtermlib.a slibcurses.a libPW.a slibPW.a libp.a libmp.a slibmp.a slibtermlib.a slibscreen.a; do
	(cd $T/ex && rm -f ./*.o && "$HAR" x "$R/usr/lib/$l" 2>/dev/null; for f in *.o; do [ -f "$f" ] && cp "$f" "$C/big/${l%.a}_$f"; done)
done
(cd $C/big && "$HAR" r ../x/big.a * >/dev/null 2>&1); rm -rf $C/big

# one case: the program name and its arguments (relative to the corpus)
n=0; bad=0
case_() {
	prog=$1; shift
	n=$((n+1))
	for who in orig host; do
		rm -rf "$T/$who"; cp -a "$C" "$T/$who"
		cd "$T/$who" || exit 1
		if [ $who = orig ]; then
			case $prog in ranlib) p=/usr/bin/ranlib;; strip) p=/usr/bin/strip;; size) p=/bin/size;; esac
			"$E" -r "$R" $p "$@" > "$T/$who.out" 2>&1; echo "exit=$?" >> "$T/$who.out"
		else
			PATH="$HERE/../ar:$PATH" "$HERE/$prog" "$@" > "$T/$who.out" 2>&1; echo "exit=$?" >> "$T/$who.out"
		fi
		# ranlib stores the time of the run: clear the date of the first member
		if [ $prog = ranlib ]; then
			find . -name '*.a' | while read f; do
				python3 - "$f" <<'PY'
import sys
p=sys.argv[1]; d=bytearray(open(p,'rb').read())
if d[:2]==b'\xff\x65' and d[2:11]==b'__.SYMDEF': d[16:20]=b'\0\0\0\0'; open(p,'wb').write(d)
PY
			done
		fi
		find . -type f | sort | while read f; do echo "$f $(cksum < "$f")"; done > "$T/$who.files"
		cd "$T"
	done
	if cmp -s "$T/orig.out" "$T/host.out" && cmp -s "$T/orig.files" "$T/host.files"; then :
	else
		bad=$((bad+1)); echo "DIFF: $prog $*"
		diff "$T/orig.out" "$T/host.out" | head -6; diff "$T/orig.files" "$T/host.files" | head -6
	fi
}

# size
case_ size b/bin_ar
case_ size b/bin_ar b/bin_size b/lib_cpp
case_ size b/*
case_ size o/*.o
case_ size s/*.o
case_ size w/*
case_ size j/*
case_ size nonexistent
case_ size b/bin_ar nonexistent j/text
case_ size a/libc.a
case_ size
# strip
case_ strip
case_ strip -h
case_ strip -x o/read.o
case_ strip nonexistent
case_ strip j/text
case_ strip j/empty j/one j/short.o j/hdronly.o j/zeros
case_ strip b/bin_ar
case_ strip o/*.o
case_ strip -h o/*.o
case_ strip s/*.o
case_ strip -h s/*.o
case_ strip w/*
case_ strip -h w/*
case_ strip b/lib_cpp b/lib_crt0.o b/lib_scrt0.o
case_ strip -h b/lib_cpp b/lib_scrt0.o
case_ strip o/read.o nonexistent o/write.o
case_ strip o/read.o j/short.o o/write.o
case_ strip o/read.o o/read.o
# ranlib
case_ ranlib
case_ ranlib nonexistent
case_ ranlib j/text
case_ ranlib j/empty
case_ ranlib j/one
for a in a/*.a; do case_ ranlib $a; done
case_ ranlib a/libc.a a/slibc.a
case_ ranlib a/libm.a nonexistent a/liby.a
for a in x/*.a; do case_ ranlib $a; done
case_ ranlib x/swapped.a x/mixed.a x/stripped.a x/swapped.a
case_ size x/swapped.a
[ -n "$KEEP" ] && cp -a "$C" "$KEEP"
echo "binutils: $n cases, $bad different"
[ $bad = 0 ]
