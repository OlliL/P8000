#!/bin/sh
# tests for conf.c; run by "make check"
T=$(mktemp -d); trap 'rm -rf $T' EXIT
ok=0; bad=0; CONF_TEST_HOME=/h; export CONF_TEST_HOME
t() {		# name expected-output expected-status file-contents key...
	name=$1; want=$2; st=$3; body=$4; shift 4
	printf '%s' "$body" > $T/f
	got=$(./test_conf $T/f "$@" 2>/dev/null); rc=$?
	if [ "$got" = "$want" ] && [ $rc = $st ]; then ok=$((ok+1))
	else bad=$((bad+1)); echo "FAIL $name: got [$got] rc=$rc"; fi
}
t plain      'a=[x]'                    0 'a = x
' a
t comment    'a=[x]'                    0 '# c
  a=x   # trailing
' a
t spaces     'a=[x y]'                  0 'a =   x y  
' a
t quoted     'a=[  x # y ]'             0 'a = "  x # y "
' a
t last-wins  'a=[2]'                    0 'a=1
a=2
' a
t unset      'b=[(unset)]'              0 'a=1
' b
t empty      'a=[]'                     0 'a =
' a
t ref        'b=[/r/bin/cas]'           0 '_root = /r
b = ${_root}/bin/cas
' b
t env        'b=[/h/x]'                 0 'b = ${CONF_TEST_HOME}/x
' b
t nokey      ''                         3 'just words
' a
t badkey     ''                         3 'a b = c
' a
t undefref   ''                         3 'a = ${nothing_here}
' a
t openquote  ''                         3 'a = "x
' a
t afterquote ''                         3 'a = "x" y
' a
rm -f $T/f
./test_conf $T/f a >/dev/null 2>&1; [ $? = 2 ] && ok=$((ok+1)) || { bad=$((bad+1)); echo "FAIL missing file"; }
echo "conf: $ok ok, $bad failed"
[ $bad = 0 ]
