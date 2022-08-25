# Copyright 2020 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


# dockerfile for pwn
# stage1: compiler
FROM ubuntu:20.04 as compiler

RUN apt update && apt install -y gcc g++ make

WORKDIR /root/ctf
COPY ./src .

RUN make pwn


# stage2: chroot
FROM ubuntu:20.04 as chroot

WORKDIR /home/ctf

RUN useradd ctf -u 1000 -s /bin/sh

COPY --from=compiler /root/ctf/pwn .
COPY ./flag .
COPY ./run_pwn.sh /

RUN chmod 644 ./flag && \
	chmod 755 /run_pwn.sh


# stage3: kctf nsjail
FROM anjia0532/kctf-docker.challenge:latest

USER root

COPY --from=chroot / /chroot

COPY ./nsjail.cfg /
COPY ./start.sh /

RUN chmod +x /start.sh

EXPOSE 1337

CMD ["/start.sh"]