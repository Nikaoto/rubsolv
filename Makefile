# Makefile

CFLAGS=-Wall -Werror

all: rubsolv

rubsolv: rubsolv.c colors.h
	gcc $(CFLAGS) -O3 rubsolv.c -o rubsolv

rubsolv_slow: rubsolv.c colors.h
	gcc $(CFLAGS) rubsolv.c -o $@

