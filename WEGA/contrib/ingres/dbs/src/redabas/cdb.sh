db=${1:?"usage: $0 db dbi"}
dbi=${2:?"DBI-file-name fehlt"}
cp $dbi /tmp/cdb$$
chmod 600 /tmp/cdb$$
ed - /tmp/cdb$$ << SCRIPT
v/^[TD]/d
g/^T/s//destroyr $db/
g/destroy\(.*\)/s//&\\
 create\1/
g/^D/-s/$/	\\\\/
g/^D	* *[0-9][0-9]*/s///
w
SCRIPT
. /tmp/cdb$$
rm /tmp/cdb$$
