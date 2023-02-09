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
    apk add --no-cache socat runuser bash && \
    ln -sf `which bash` /bin/sh && \
	adduser ctf -u 1000 -s /bin/sh -D

WORKDIR /home/ctf

COPY --from=patchelf /root/ctf/pwn .
COPY ./glibc ./glibc

COPY ./flag ./flag
COPY ./run_pwn.sh /

RUN chmod 644 ./flag  && \
	chmod 711 /run_pwn.sh && \
    chown -R root:root .

EXPOSE 1337

CMD socat tcp-l:1337,reuseaddr,fork exec:/run_pwn.sh


