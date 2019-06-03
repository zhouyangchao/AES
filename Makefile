
CC = gcc
OPS = -std=c99 -Wall -Werror -Wextra -O2
LIB = -lcunit
TRG = libaes.so
PREFIX = /usr/local/

CUNIT_VER = $(shell cat /usr/include/CUnit/CUnit.h | awk '$$0~/^\#define CU_VERSION/{print $$NF}')

.PHONY: all test
all: aes test

aes: aes.c
	$(CC) $(OPS) -shared -fPIC -o $(TRG) aes.c

test: aes test_case.c
ifeq ($(CUNIT_VER), "2.1-2")
	$(CC) $(OPS) -o test_case test_case.c $(LIB) -DCUNIT_VER=2
else
	$(CC) $(OPS) -o test_case test_case.c $(LIB) -DCUNIT_VER=1
endif
	./test_case

sample: install sample.c
	$(CC) $(OPS) -o sample sample.c -L./ -laes

.PHONY: install uninstall clean
install: aes
	install libaes.so $(PREFIX)/lib/
	ln -sf $(PREFIX)/lib/libaes.so /usr/lib/libaes.so
	install aes.h /usr/include/aes.h
	
uninstall:
	rm -f $(PREFIX)/lib/libaes.so /usr/lib/libaes.so /usr/include/aes.h

clean:
	rm -f *.o libaes.so test_case sample
