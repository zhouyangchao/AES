CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Werror -O2 -Wl,-rpath=./
LDLAGS = -lcunit
PREFIX = /usr/local/

CUNIT_VER = $(shell cat /usr/include/CUnit/CUnit.h | awk '$$0~/^\#define CU_VERSION/{print $$NF}')

.PHONY: all test sample
all: aes

aes: aes.c
	$(CC) $(CFLAGS) -c -o aes.o $<
	$(CC) $(CFLAGS) -shared -fPIC -o libaes.so aes.o
	$(AR) rcs libaes.a aes.o

test: aes test_case.c
ifeq ($(CUNIT_VER), "2.1-2")
	$(CC) $(CFLAGS) -o test_case test_case.c $(LDLAGS) -DCUNIT_VER=2
else
	$(CC) $(CFLAGS) -o test_case test_case.c $(LDLAGS) -DCUNIT_VER=1
endif
	./test_case

sample: aes sample.c
	$(CC) $(CFLAGS) -o sample sample.c -L./ -laes
	./sample

.PHONY: install uninstall clean
install: aes
	install libaes.so $(PREFIX)/lib/
	ln -sf $(PREFIX)/lib/libaes.so /usr/lib/libaes.so
	install aes.h /usr/include/aes.h
	
uninstall:
	rm -f $(PREFIX)/lib/libaes.so /usr/lib/libaes.so /usr/include/aes.h

clean:
	rm -f *.o *.so *.a test_case sample
