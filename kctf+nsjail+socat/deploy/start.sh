#!/bin/bash

# override flag from env
# if environmental variable FLAG is not empty string
if [ ! -z $FLAG ]
then
    if [ "$(cat /chroot/home/ctf/flag)" != "$FLAG" ]
    then
        echo $FLAG > /chroot/home/ctf/flag
        chmod 644 /chroot/home/ctf/flag
    fi
fi

# the env will not pass to ctf
unset FLAG

kctf_setup

exec kctf_drop_privs socat \
        TCP-LISTEN:1337,reuseaddr,fork \
        EXEC:"nsjail --config /nsjail.cfg -- /run_pwn.sh"