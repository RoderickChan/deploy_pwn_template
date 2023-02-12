#!/bin/bash

set -e

# set pow using sha256 POW by ENABLE_POW
if [ ! -z $ENABLE_POW ]
then
    if [ "$ENABLE_POW" == "1" ]
    then
        echo "=================proof-of-work================="
        echo ""
        rand_str=$(head -c 27 /dev/urandom | base64)
        hash_value=$(echo -n "$rand_str" | sha256sum - | cut -c 1-64)
        frontend=$(echo "$rand_str" | cut -c -4 )
        backend=$(echo "$rand_str" | cut -c 5- )
        prompt="sha256(XXXX + \"${backend}\") == ${hash_value}"
        echo $prompt
        echo -n "Gime me XXXX: "

        read -t 300 -r input_hash

        if [ "$input_hash" != "$frontend" ]
        then
            echo "Proof of work failed!"
            exit 2
        fi
    fi
fi

unset ENABLE_POW

# override flag from env
# if environmental variable FLAG is not empty string
if [ ! -z $FLAG ]
then
    if [ "$(cat /home/ctf/flag)" != "$FLAG" ]
    then
        echo $FLAG > /home/ctf/flag
        chmod 644 /home/ctf/flag
    fi
fi

# the env will not pass to ctf
unset FLAG

cd /home/ctf

# run pwn challenge
exec timeout 300 $(which chroot) /home/ctf ./pwn