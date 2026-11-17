: '@[$]sdisk.sh	1.1  01/04/83 09:17:57 - 87wega3.2'
: 'start periodic disk accounting'
cd /usr/adm
nohup sh /usr/lib/acct/dodisk > diskdiag 2>&1 &
