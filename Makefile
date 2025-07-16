GMP_CFLAGS = $(shell pkg-config --cflags gmp)
GMP_LFLAGS = $(shell pkg-config --libs gmp)

CC = clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic $(GMP_CFLAGS)

all: keygen encrypt decrypt

keygen: keygen.c randstate.c libss.a libnumtheory.a
	$(CC) $(CFLAGS) keygen.c randstate.c -o keygen -L./ $(GMP_LFLAGS) -lss -lnumtheory -lgmp

encrypt: encrypt.c randstate.c libss.a libnumtheory.a
	$(CC) $(CFLAGS) encrypt.c randstate.c -o encrypt -L./ $(GMP_LFLAGS) -lss -lnumtheory -lgmp

decrypt: decrypt.c randstate.c libss.a libnumtheory.a
	$(CC) $(CFLAGS) decrypt.c randstate.c -o decrypt -L./ $(GMP_LFLAGS) -lss -lnumtheory -lgmp

libss.a: ss.h ss.c
	$(CC) $(CFLAGS) -c ss.c -o ss.o
	ar rvs libss.a ss.o

libnumtheory.a: numtheory.h numtheory.c
	$(CC) $(CFLAGS) -c numtheory.c -o numtheory.o
	ar rvs libnumtheory.a numtheory.o

clean:
	rm -f *.o keygen encrypt decrypt libss.a libnumtheory.a

format:
	clang-format -i -style=file *.[ch]
