# dockerfile for pwn
# stage1: compiler
FROM ubuntu:20.04 as compiler

RUN apt update && apt install -y gcc g++ make

WORKDIR /root/ctf
COPY ./src .

RUN make pwn


# stage2: runner
FROM ubuntu:20.04 as runner

USER root

RUN apt update && apt install -y socat && \
	useradd ctf -u 1000 -s /bin/sh

WORKDIR /home/ctf

COPY --from=compiler /root/ctf/pwn .

COPY ./flag ./flag
COPY ./run_pwn.sh /

RUN chmod 644 ./flag && \
	chmod 711 /run_pwn.sh && \
	chown -R root:root /home/ctf


EXPOSE 1337

CMD socat tcp-l:1337,reuseaddr,fork exec:/run_pwn.sh




