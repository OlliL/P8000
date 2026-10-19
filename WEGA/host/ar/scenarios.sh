# scenarios for check.sh; run with sh in an empty directory, AR is the ar to test.
# Every command's output, exit code and the archives are logged by check.sh.
run() { echo "\$ ar $*"; $AR "$@"; echo "exit=$?"; }
fix() { touch -d '2019-05-05 05:05:05 UTC' "$@" 2>/dev/null; }   # extracted files get the time of the extraction
show() { for f in "$@"; do [ -f "$f" ] && { echo "[$f] $(wc -c < $f) $(cksum < $f)"; }; done; }

mkdir sub
printf 'hello\n' > a            # odd size
printf 'world!\n' > b           # even size
: > empty
printf '0123456789abcdefghijklmnopqrstuvwxyz\n' > averyveryverylongfilename.c
head -c 1500 /dev/zero | tr '\0' 'x' > big
printf 'in sub\n' > sub/c
printf 'x\n' > ex.sh; chmod 755 ex.sh
printf 'y\n' > ro; chmod 444 ro
for f in a b empty averyveryverylongfilename.c big sub/c ex.sh ro; do touch -d '2020-03-04 05:06:07 UTC' $f; done
touch -d '2021-01-02 03:04:05 UTC' b

run r l.a a b
show l.a
run rv l.a a b empty
run rc l.a empty
run rv n.a a b big averyveryverylongfilename.c sub/c ex.sh ro
show n.a
run t n.a
run tv n.a
run t n.a a ex.sh
run tv n.a ro
run p n.a a
run pv n.a a b
run x n.a
fix a b big averyveryvery c ex.sh ro
show a b big averyveryverylongfilename.c c ex.sh ro
ls -l a b ex.sh ro c 2>/dev/null | awk '{print $1,$5}'
rm -f a b big c ex.sh ro averyveryverylongfilename.c averyveryvery
run xv n.a b ex.sh
fix a b ex.sh
ls -l b ex.sh | awk '{print $1,$5}'
run d n.a b
run dv n.a ro
run t n.a
run ma n.a a ex.sh
run t n.a
run mb n.a a big
run t n.a
run mi n.a sub/c
run t n.a
run ra n.a a b
run rb n.a a empty
run ri n.a a ex.sh
run t n.a
touch -d '2022-02-02 02:02:02 UTC' b
run ru n.a a b
run ruv n.a a b
run t n.a
run rv n.a nothere
run rc n.a nothere
run q q.a a b
show q.a
run qv q.a empty big
run qc q2.a a
run q n.a ex.sh
run t n.a
run rl l2.a a
show l2.a
run rv n.a
run rua n.a a
run d n.a nothere
run dv n.a
run x n.a nothere
run t nonexist.a
run x nonexist.a
run p nonexist.a
run d nonexist.a
run m nonexist.a a
run r new.a
run r new2.a nothere
run
run r
run r x.a
run zz x.a a
run rd x.a a
run mq x.a a
run a n.a
run ra n.a
run rb n.a a
run qa n.a a
run xx n.a
run u n2.a a b
show n2.a
printf 'not an archive\n' > bad.a
run t bad.a
run r bad.a a
run q bad.a a
run x bad.a
: > zero.a
run t zero.a
run r zero.a a
printf 'AB' > two.a
run t two.a
run vr v.a a
run rvv v.a a b
run tvv v.a
run r dir/../s.a a
mkdir dd
run r dd/d.a a
run t dd/d.a
run t dd//d.a
run r sub/ a
run t sub/
cp n.a n3.a; chmod 444 n3.a
run r n3.a a
# dates for tv: daylight saving rules, 1974/1975, before the epoch, after 2000
i=0
for d in '1969-12-31 12:00:00' '1974-01-10 12:00:00' '1974-07-01 12:00:00' '1974-11-30 12:00:00' '1975-03-01 12:00:00' '1975-10-20 12:00:00' '1983-04-24 06:59:59' '1983-04-24 07:00:00' '1983-04-30 12:00:00' '1983-10-30 05:59:59' '1983-10-30 06:00:00' '2000-02-29 23:59:59' '2001-07-15 12:00:00' '2030-07-15 12:00:00' '2037-12-31 23:59:59'; do
	i=$((i+1)); echo $i > dt$i; touch -d "$d UTC" dt$i
done
run r dates.a dt1 dt2 dt3 dt4 dt5 dt6 dt7 dt8 dt9 dt10 dt11 dt12 dt13 dt14 dt15
run tv dates.a
echo end
