
LIB = -L/usr/local/lib -lcunit
TRG = libaes.so

all: aes test sample
aes:
	gcc -shared -fPIC -o $(TRG) aes.c
	strip $(TRG)
test:
	gcc -o test test_case.c $(LIB)
sample:
	gcc -o sample sample.c -Wl,-rpath=. -L. -laes

clean:
	rm -f *.o libaes.so test sample