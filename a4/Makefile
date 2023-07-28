CC = gcc
CFLAGS = -g  


SRC_MAIN = main.c



all: OS_ISOLATE

OS_ISOLATE: main.c
	$(CC) $(CFLAGS) -o os_isolate $(SRC_MAIN)
	sudo setcap 'cap_sys_admin=eip' os_isolate
clean:
	rm -f os_isolate
