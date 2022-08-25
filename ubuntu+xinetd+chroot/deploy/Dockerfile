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
    chown -R root:root /home/ctf

RUN cp -R /usr/lib* /home/ctf && \
	mkdir /home/ctf/dev && \
    mknod /home/ctf/dev/null c 1 3 && \
    mknod /home/ctf/dev/zero c 1 5 && \
    mknod /home/ctf/dev/random c 1 8 && \
    mknod /home/ctf/dev/urandom c 1 9 && \
    chmod 666 /home/ctf/dev/*

RUN mkdir /home/ctf/bin && \
    cp /bin/sh /home/ctf/bin && \
    cp /bin/ls /home/ctf/bin && \
    cp /bin/cat /home/ctf/bin

EXPOSE 1337

CMD /etc/init.d/xinetd start;sleep infinity;




