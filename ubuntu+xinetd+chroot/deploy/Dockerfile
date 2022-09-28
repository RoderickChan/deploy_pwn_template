# dockerfile for pwn
# stage1: compiler
FROM ubuntu:20.04 as compiler

RUN apt update && apt install -y gcc g++ make

WORKDIR /root/ctf
COPY ./src .

RUN make pwn


# stage2: runner
FROM ubuntu:20.04 as runner

RUN apt update && apt install -y xinetd && \
	useradd ctf -u 1000 -s /bin/sh

USER root

WORKDIR /home/ctf

COPY --from=compiler /root/ctf/pwn .

COPY ./flag ./flag
COPY ./ctf.xinetd /etc/xinetd.d/ctf
COPY ./run_pwn.sh /

RUN chmod 644 ./flag && \
	chmod 711 /run_pwn.sh && \
    chown -R root:root .

RUN cp -R /usr/lib* . && \
	mkdir ./dev && \
    mknod ./dev/null c 1 3 && \
    mknod ./dev/zero c 1 5 && \
    mknod ./dev/random c 1 8 && \
    mknod ./dev/urandom c 1 9 && \
    chmod 666 ./dev/*

RUN mkdir ./bin && \
    cp /bin/sh ./bin && \
    cp /bin/ls ./bin && \
    cp /bin/cat ./bin

EXPOSE 1337

CMD /etc/init.d/xinetd start;sleep infinity;




