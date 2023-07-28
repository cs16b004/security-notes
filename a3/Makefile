CC = gcc
CFLAGS = -g -fno-stack-protector -z execstack -mpreferred-stack-boundary=2


SRC_CRACK1 = crack1.c

SRC_CRACK2 = crack2.c


all: CRACK1 CRACK2

CRACK1: crack1.c sol1.c
	$(CC) $(CFLAGS) -o crack1 $(SRC_CRACK1)
	$(CC) $(CFLAGS) -o sol1 sol1.c
CRACK2: crack2.c
	$(CC) $(CFLAGS) -o crack2 $(SRC_CRACK2)
clean:
	rm -f crack1 crack2 sol1
