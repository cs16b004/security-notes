CC = gcc
CFLAGS = -g -fno-stack-protector -z execstack -mpreferred-stack-boundary=2

SRC_VULN1 = vuln1.c

SRC_VULN2 = vuln2.c

SRC_VULN3 = vuln3.c

all: VUL1 VUL2 VUL3

VUL1: vuln1.c
	$(CC) $(CFLAGS) -o vuln1 $(SRC_VULN1)
	sudo chown root vuln1
	sudo chgrp root vuln1
	sudo chmod +s vuln1
VUL2: vuln2.c
	$(CC) $(CFLAGS) -o vuln2 $(SRC_VULN2)
	sudo chown root vuln2
	sudo chgrp root vuln2
	sudo chmod +s vuln2
VUL3: vuln3.c
	$(CC) $(CFLAGS) -o vuln3 $(SRC_VULN3)
	sudo chown root vuln3
	sudo chgrp root vuln3
	sudo chmod +s vuln3
clean:
	rm -f vuln1 vuln2 vuln3
