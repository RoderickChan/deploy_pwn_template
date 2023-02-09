#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
    setbuf(stdin, 0);
    setbuf(stdout, 0);
    setbuf(stderr, 0);
    puts("Welcome to pwn world!");
    printf("please input your name: ");
    char buf[0x80] = {0};
    read(0, buf, 0x10);
    if (strncmp(buf, "admin", 5) == 0) {
        system("/bin/sh");
    } else {
        puts("you are not admin!");
    }
    return 0;
}