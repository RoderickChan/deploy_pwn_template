#!/bin/bash
set -e

# uncomment to set pow using sha256
:<<!
rand_str=$(head -c 27 /dev/urandom | base64)
hash_value=$(echo "$rand_str" | sha256sum - | cut -c 1-64)

backend=$(echo "$rand_str" | cut -c 4- )
prompt="sha256(XXXX + \"${backend}\") == ${hash_value}"

echo $prompt

echo -n "Gime me XXXX: "

read -t 300 -r input_hash

if [ "$input_hash" != "$hash_value" ]
then
    echo "Proof of work failed!"
    exit 2
fi
!

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
exec runuser -u ctf --pty -- timeout 300 ./pwn