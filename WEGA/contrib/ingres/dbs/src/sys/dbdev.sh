ls -l /dev | sed -n -e "/.*$1, *$2.* \(.*\)/p"
