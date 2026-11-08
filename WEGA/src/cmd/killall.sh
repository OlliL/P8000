: @[$]killall	1.3  04/13/83 13:53:53 - 87wega3.2
a=`who am i`
b=`expr "$a" : '.* \(co\)nsole' \| "$a" : '.* tty\(..\)' \| noterm`
nice --20 ps -ef^sed -e '1d' \
 -e '/INIT/d' \
 -e '2s/.*/kill -'${1-9}' \\/' \
 -e '/^.*[0-9]*  *[0-9]*.* '$b' /d' \
 -e '/wega * '$$' /d' \
 -e '/\/etc\/rc/d' \
 -e '/killall/d' \
 -e '/wega .* ps -ef/d' \
 -e '/wega .* \[ sed \]/d' \
 -e 's/....... *\([0-9][0-9]*\).*/ \1 \\/' \
 -e '$s/\\$//' ^ sh >/dev/null 2>&1
echo /etc/cron\\n/etc/update\\n/etc/dqueuer > /tmp/cud
nice --20 ps -ef^fgrep -f /tmp/cud^sed \
 -e 'i\
kill -'${1-9}' \\' \
 -e 's/....... *\([0-9][0-9]*\).*/ \1 /' \
 -e '$s/\\$//' ^ sh >/dev/null 2>&1
rm -f /tmp/cud
