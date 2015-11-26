CC=g++
CFLAGS=-ggdb -pipe -Wall -O0 -Wextra -Werror -march=native
CFLAGS+=-mtune=native -masm=intel -Wconversion -Wsign-conversion
CFLAGS+=-Wformat-security -fstack-protector-all -Wstack-protector
CFLAGS+=--param ssp-buffer-size=4 -fPIE
CFLAGS+=-ftrapv -Wl,-z,relro,-z,now

PCFLAGS=-fomit-frame-pointer -O2 -D_FORTIFY_SOURCE=2
SHELL=/bin/bash

#CFLAGS+=$(PCFLAGS)

all: monteCarlo run

monteCarlo: monteCarlo.cpp
	$(CC) $(CFLAGS) -o monteCarlo monteCarlo.cpp

test: valgrind

cpplint:
	cpplint --filter=-legal/copyright *.cpp *.h

cppcheck:
	cppcheck --enable=all --error-exitcode=1 *.cpp *.h

valgrind: monteCarlo
	valgrind --leak-check=full --error-exitcode=1 -v ./monteCarlo

cloc:
	cloc *.{cpp,h}

run: test
	time ./monteCarlo

clean:
	rm -f *.o monteCarlo
