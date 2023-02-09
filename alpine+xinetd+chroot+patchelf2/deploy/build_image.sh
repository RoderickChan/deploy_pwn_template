#!/bin/bash
set -e

# build image
docker-compose up -d || docker compose up -d

# wait for starting container
sleep 5

# copy file and generate attachment
echo -n "copy file and generate attachment? [y/n] "
read -t 15 ANS 
if [ $ANS == "y" ] || [ $ANS == "Y" ]
then
    imageid=$(docker ps -n 1 -q)
    mkdir ../attachment/glibc -p
    cd ../attachment
    docker cp ${imageid}:/home/ctf/pwn .
    docker cp ${imageid}:/home/ctf/glibc .
    echo "copy file and generate attachment success!"
fi