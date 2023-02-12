all: pwn pwn_debug

pwn: pwn.c
	gcc ./pwn.c -o pwn -fstack-protector-all -pie -z now -z noexecstack -O0 -w
	strip ./pwn

pwn_debug: pwn.c
	gcc ./pwn.c -o pwn_debug -fstack-protector-all -pie -z now -z noexecstack -O0 -g3

clean:
	rm -rf ./pwn ./pwn_debug

