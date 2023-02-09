# dockerfile for pwn
FROM ubuntu:20.04 as compiler

WORKDIR /root

RUN apt update && apt install -y gcc g++ make git autoconf automake libtool

RUN git clone https://github.com/NixOS/patchelf.git && \
    cd ./patchelf && \
    ./bootstrap.sh && \
    ./configure && \
    make && \
    make install

WORKDIR /root/ctf

COPY ./src .

RUN make pwn && \
    mkdir glibc && \
    cp /lib/x86_64-linux-gnu/libc.so.6 ./glibc/libc.so.6 && \
    cp /lib/x86_64-linux-gnu/ld-linux-x86-64.so.2 ./glibc/ld-linux-x86-64.so.2 && \
    patchelf --set-rpath ./glibc ./pwn && \
    patchelf --set-interpreter ./glibc/ld-linux-x86-64.so.2 ./pwn



FROM alpine:latest as compiler2

USER root

WORKDIR /root/ctf

RUN apk update && \
    apk add --no-cache wget gcc libc-dev make

RUN  wget -q https://github.com/opensuse/xinetd/releases/download/2.3.15.4/xinetd-2.3.15.4.tar.xz && \
    tar xJf xinetd-2.3.15.4.tar.xz && \
    cd xinetd-2.3.15.4 && \
    ./configure && \
    make -j8 && \
    make install

COPY ./ls.c ./
COPY ./cat.c ./

RUN gcc ./ls.c -o ./ls && \
    gcc ./cat.c -o ./cat


FROM alpine:latest as runner

USER root

RUN apk update && \
    apk add --no-cache bash coreutils && \
    ln -sf `which bash` /bin/sh && \
	adduser ctf -u 1000 -s /bin/sh -D

WORKDIR /home/ctf

COPY --from=compiler /root/ctf/pwn .
COPY --from=compiler /root/ctf/glibc ./glibc


COPY ./flag ./
COPY ./run_pwn.sh /
COPY ./ctf.xinetd /etc/ctf.xinetd

# compile ls && cat
RUN mkdir ./bin && \
    cp /bin/bash ./bin/sh

COPY --from=compiler2 /root/ctf/ls ./bin/ls
COPY --from=compiler2 /root/ctf/cat ./bin/cat
COPY --from=compiler2 /usr/local/sbin/xinetd /bin/xinetd

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


