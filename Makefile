CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra -O2 -Iinclude
LDFLAGS ?= -lm

.PHONY: all test dataset clean esp32-notes

all: howl test_host

howl: src/howl.c src/main.c include/howl.h
	$(CC) $(CFLAGS) src/howl.c src/main.c -o howl $(LDFLAGS)

test_host: src/howl.c tests/test_host.c include/howl.h
	$(CC) $(CFLAGS) src/howl.c tests/test_host.c -o test_host $(LDFLAGS)

test: test_host
	./test_host

dataset: howl
	./howl dataset datasets/wav

esp32-notes:
	@sed -n '1,80p' esp32/README.md

clean:
	rm -rf howl test_host howl-test.wav datasets/wav
