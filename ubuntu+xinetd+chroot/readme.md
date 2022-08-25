# 说明

使用`ubuntu + xinetd + chroot` 部署`pwn`题

- 参照模板，将源码和`makefile`放置在`./deploy/src`目录下
- 确保生成目标文件名称为`pwn`，或者根据需要修改`dockerfile`中的`make pwn`语句
- `run_pwn.sh`中可以放置`proof of work`代码或者设置`timeout`时间
- 运行`./build_image.sh`可以生成镜像以及提供给选手的`attachment`文件夹
- 在`./deploy/docker-compose.yaml`文件中修改映射的宿主机端口、`flag`的内容
- 其他需求可自行根据模板文件定制

# Introduction

use `ubuntu + xinetd + chroot` to deploy pwn challenge

- put source file in `./deploy/src`
- make sure the name of target binary is `pwn`, or you can change the `dockerfile`
- set `proof of work` or `timeout` in `run_pwn.sh`
- execute `./build_image.sh` to generate image and then run a container. Also, you can decide to get attachment file, which is offered to player 
- change the port mapping in host and the content of flag in `./deploy/docker-compose.yaml`
