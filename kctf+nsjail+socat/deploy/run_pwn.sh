#!/bin/bash
set -e

# uncomment to set pow using sha256
:<<!
# set pow using sha256 or timeout here
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
!


cd /home/ctf

# run pwn challenge
exec timeout 300 ./pwn
