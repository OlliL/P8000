/lib/cpp -P -D$1 AMX.SRC | sed "/^$/d
s/@/*/g" | amxfile $2
