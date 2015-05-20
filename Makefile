
CC = gcc
OPS = -Wall
LIB = -L/usr/local/lib -lcunit
TRG = libaes.so

.PHONY: all
all: aes test sample
aes:
	$(CC) $(OPS) -O2 -shared -fPIC -o $(TRG) aes.c
	strip $(TRG)
test:
	$(CC) -o test_case test_case.c -Wl,-rpath=/usr/local/lib $(LIB)
sample: aes
	$(CC) -o sample sample.c -Wl,-rpath=. -L=. -laes

.PHONY: install
install: uninstall aes
	cp libaes.so /usr/local/lib/
	ln -s /usr/local/lib/libaes.so /usr/lib64/libaes.so
	ln -s /usr/local/lib/libaes.so /usr/lib/libaes.so
	cp aes.h /usr/include/aes.h
	
.PHONY: uninstall
uninstall:
	rm -f /usr/local/lib/libaes.so /usr/lib/libaes.so /usr/lib64/libaes.so /usr/include/aes.h

.PHONY: clean
clean:
	rm -f *.o libaes.so test_case sample
