# dockerfile for pwn
FROM ubuntu:18.04 as compiler

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


FROM alpine:latest as runner

USER root

RUN apk update && \
    apk add --no-cache socat runuser bash && \
    ln -sf `which bash` /bin/sh && \
	adduser ctf -u 1000 -s /bin/sh -D

WORKDIR /home/ctf

COPY --from=compiler /root/ctf/pwn .
COPY --from=compiler /root/ctf/glibc ./glibc

COPY ./flag ./flag
COPY ./run_pwn.sh /

RUN chmod 644 ./flag  && \
	chmod 711 /run_pwn.sh && \
    chown -R root:root /home/ctf


EXPOSE 1337

CMD socat tcp-l:1337,reuseaddr,fork exec:/run_pwn.sh


