#!/bin/bash
set -e

# build image
docker-compose up -d || docker compose up -d

sleep 3