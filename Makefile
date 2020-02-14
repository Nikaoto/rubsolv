# Makefile

CFLAGS=-Wall -Werror

all: rubsolv

rubsolv: rubsolv.c sides.c colors.h config.h
	gcc $(CFLAGS) -O3 rubsolv.c sides.c -o rubsolv

rubsolv_slow: rubsolv.c sides.c colors.h config.h
	gcc $(CFLAGS) rubsolv.c sides.c -o rubsolv_slow

rubsolv_g: rubsolv.c sides.c colors.h config.h
	gcc $(CFLAGS) -g rubsolv.c sides.c -o rubsolv_g
