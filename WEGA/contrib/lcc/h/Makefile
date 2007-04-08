# created by Oliver Lehmann
# 2007-04-08

PREFIX=/z/LCC

H=	alarm.h assert.h core.h ctype.h dumprestor.h fatal.h fcntl.h ftw.h \
	grp.h macros.h math.h memory.h mon.h nan.h pwd.h regexp.h search.h \
	setjmp.h sgsmacros.h sgtty.h stdio.h storclass.h string.h symbol.h \
	time.h ustat.h utmp.h values.h varargs.h

SYS_H=	sys/sysmacros.h sys/types.h

install:
	-mkdir ${PREFIX}
	-mkdir ${PREFIX}/include
	-mkdir ${PREFIX}/include/sys
	cp ${H}     ${PREFIX}/include
	cp ${SYS_H} ${PREFIX}/include/sys
	chmod 644 ${PREFIX}/include/*.h
	chmod 644 ${PREFIX}/include/sys/*.h
