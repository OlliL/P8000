# shrink.sh FILE.s: what WEGA did to the assembler output of each module of ex, between the compiler and cas. ex fills
# nearly all of the 64K text space of a nonsegmented program; both steps make it smaller (not documented: found in
# /bin/ex, compare/ex/findings.json). The file is written over in place.
#
# 1. Every external function that the module calls three times or more gets a stub "jp _name" at the start of the
#    code, and the calls go to the stub (calr, 2 bytes, instead of call, 4 bytes). The table is in the order of
#    "sort | uniq -c | sort -n": by the number of calls, then by the name.
# 2. A frame operand with the offset 0 becomes @fp (2 bytes shorter than 0(fp)): ~L1(fp) in a function with
#    "~L1 := 0" (lda rN,~L1(fp) there becomes ld rN,fp), and 0(fp) beside the registers r0 ... r7 (the stores of
#    the parameters; the saves of r8 ... r14 stay 0(fp)).
#    Not so the temporary of an indirect call (ld ~L1(fp),r2 ... ld r1,0(fp); call @r1): its store keeps the offset,
#    and of its loads only the last one of the function becomes @fp (listchar, normchar, vgotab: 6 places, the only
#    ones in ex; a rule read off the binary, its mechanism is not known). Written ~L1+0(fp), which step 2 leaves alone.
s=$1
LC_ALL=C ; export LC_ALL	# sort in ASCII order (_Copy before _column), as on WEGA
awk '{ b[++m] = $0 }
	$0 == "\tcall\t@r1" && b[m - 1] == "\tld\tr1,0(fp)" {
		n++; l[n] = m - 1
		for (i = m - 2; i > m - 5 && i > 0; i--)
			if (b[i] == "\tld\t~L1(fp),r2") { b[i] = "\tld\t~L1+0(fp),r2"; break }
	}
	/^\t}\t\/\*/ {
		for (i = 1; i < n; i++)
			b[l[i]] = "\tld\tr1,~L1+0(fp)"
		for (i = 1; i <= m; i++)
			print b[i]
		m = 0; n = 0
	}
	END { for (i = 1; i <= m; i++) print b[i] }' $s > icall.s
mv icall.s $s
sed -n 's/^\(_[A-Za-z0-9_]*\)::*$/\1/p' $s > defs.tmp
grep '	callr	_' $s | sed 's/.*callr	//' | sort | uniq -c | sort -n | awk '$1 >= 3 { print $2 }' > stubs.tmp
awk 'FILENAME == "defs.tmp" { d[$1] = 1; next }
	FILENAME == "stubs.tmp" && d[$1] != 1 { s[$1] = 1; n++; l[n] = $1; next }
	FILENAME == "stubs.tmp" { next }
	done != 1 && $1 == ".code" { m++; b[m] = $0; for (i = 1; i <= n; i++) { m++; b[m] = sprintf("S%s:\tjp\t%s", l[i], l[i]) } done = 1; next }
	$1 == "callr" && s[$2] == 1 { line = "\tcallr\tS" $2 } $1 != "callr" || s[$2] != 1 { line = $0 }
	{ m++; b[m] = line; if ($1 == "~L1" && $2 == ":=" && $3 == "0") z = 1 }
	$1 == "}" || $1 == ".code" || $1 == ".data" || $1 == ".bss" {
		for (i = 1; i <= m; i++) {
			if (z == 1 && index(b[i], "~L1(fp)") > 0)
				print "Z" b[i]
			else
				print b[i]
		}
		m = 0; z = 0
	}
	END { for (i = 1; i <= m; i++) print b[i] }' defs.tmp stubs.tmp $s |
sed -e '/^Z	lda	/s/lda	\(r[0-9]*\),~L1(fp)/ld	\1,fp/' -e '/^Z/s/~L1(fp)/@fp/' -e 's/^Z//' \
	-e 's/	0(fp),r\([0-7]\)$/	@fp,r\1/' -e 's/	0(fp),rr\([0-7]\)$/	@fp,rr\1/' -e 's/	0(fp),r\([0-7]\),/	@fp,r\1,/' \
	-e 's/	r\([0-7]\),0(fp)$/	r\1,@fp/' -e 's/	rr\([0-7]\),0(fp)$/	rr\1,@fp/' > shrink.s
mv shrink.s $s
rm -f defs.tmp stubs.tmp
