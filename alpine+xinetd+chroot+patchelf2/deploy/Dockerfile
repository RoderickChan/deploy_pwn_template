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

RUN apk update && \
    apk add --no-cache bash coreutils && \
    ln -sf `which bash` /bin/sh && \
	adduser ctf -u 1000 -s /bin/sh -D

WORKDIR /home/ctf

COPY --from=patchelf /root/ctf/pwn .

COPY ./glibc ./glibc
COPY ./flag ./
COPY ./run_pwn.sh /
COPY ./ctf.xinetd /etc/ctf.xinetd
COPY ./bin/ls ./bin/ls
COPY ./bin/ls ./bin/ls
COPY ./bin/xinetd /bin/xinetd

RUN chmod +x ./bin/* && \
    chmod +x /bin/xinetd && \
    cp /bin/bash ./bin/sh

RUN mkdir ./dev && \
    mknod ./dev/null c 1 3 && \
    mknod ./dev/zero c 1 5 && \
    mknod ./dev/random c 1 8 && \
    mknod ./dev/urandom c 1 9 && \
    chmod 666 ./dev/*

RUN mkdir -p ./lib ./usr/lib && \
    cp -r -L /lib/*.so* ./lib && \
    cp -R /usr/lib/*.so* ./usr/lib

RUN chmod 644 ./flag  && \
	chmod 711 /run_pwn.sh && \
    chown -R root:root .

EXPOSE 1337

CMD xinetd -f /etc/ctf.xinetd -pidfile /run/xinetd.pid -limit 1000 -reuse;sleep infinity;


