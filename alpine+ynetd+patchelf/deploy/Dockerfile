# dockerfile for pwn
FROM ubuntu:20.04 as patchelf

WORKDIR /root/ctf

COPY ./bin/patchelf /bin/patchelf
COPY ./bin/pwn ./pwn

RUN chmod +x ./pwn /bin/patchelf && \
    patchelf --set-rpath ./glibc ./pwn && \
    patchelf --set-interpreter ./glibc/ld-linux-x86-64.so.2 ./pwn

FROM alpine:latest as runner

USER root

RUN adduser ctf -u 1000 -s /bin/sh -D

WORKDIR /home/ctf

COPY --from=patchelf /root/ctf/pwn .
COPY ./glibc ./glibc
COPY ./flag ./flag
COPY ./bin/ynetd /bin/ynetd

RUN chmod 644 ./flag  && \
    chmod +x ./pwn  && \
	chmod +x /bin/ynetd && \
	chmod +x ./glibc/* && \
    chown -R root:root .

EXPOSE 1337

CMD ynetd -p 1337 -lp 500 -se y -u ctf "timeout 300 ./pwn"

