: '@[$]dodisk.sh	1.1  01/04/83 09:17:49 - 87wega3.2'
: 'perform disk accounting'
PATH=:/bin:/usr/lib/acct/bin:/usr/bin:/etc
export PATH
dir=/usr/adm
pickup=acct/nite
PATH=/usr/lib/acct:/bin:/usr/bin:/etc
cd ${dir}
date
find / -print | acctdusg >dtmp
date
sort +0n +1 -o dtmp dtmp
acctdisk <dtmp >disktmp
chmod 644 disktmp
chown adm disktmp
mv disktmp ${pickup}/disktacct
