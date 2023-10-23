CC := x86_64-w64-mingw32-gcc
CFLAGS = -Wall -Wextra -ansi -s -O3
LDFLAGS = -lkernel32 -nostdlib

uptime.exe: uptime.c
	$(CC) $(CFLAGS) $? -o $@ $(LDFLAGS)

clean:
	rm -f uptime.exe

.PHONY: clean
