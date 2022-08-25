#!/bin/bash
set -e

# build image
docker-compose up -d

sleep 5

# copy file and generate attachment
echo -n "copy file and generate attachment? [y/n] "
read -t 15 ANS
if [ $ANS == "y" ] || [ $ANS == "Y" ]
then
    imageid=$(docker ps -n 1 -q)
    mkdir ../attachment -p
    cd ../attachment
    docker cp ${imageid}:/chroot/home/ctf/pwn .
    docker cp -L ${imageid}:/chroot/lib/x86_64-linux-gnu/libc.so.6 .
    docker cp -L ${imageid}:/chroot/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2 .
    echo "copy file and generate attachment success!"
fi