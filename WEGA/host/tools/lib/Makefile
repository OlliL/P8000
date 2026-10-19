# conf: reader for the configuration files of the host tools (cc.conf, ...)
#
#   make check     build the test and run it

HOSTCC ?= cc
CFLAGS ?= -O2 -Wall

check: test_conf
	sh test_conf.sh

test_conf: test_conf.c conf.c conf.h
	$(HOSTCC) $(CFLAGS) -I. -o $@ test_conf.c conf.c

clean:
	rm -f test_conf

.PHONY: check clean
