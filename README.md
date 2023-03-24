# pwn题部署模板
编写了常用的`11`种`pwn`题部署模板文件，支持`sha256 proof of work`验证，支持从环境变量中设置`flag`。
详情可参考每个模板下的`readme.md`文件。

# 注意事项
如果是`ubuntu:18.04`以及以下版本，使用`chroot`构造沙箱环境的时候，`dockerfile`对应语句需要更新为：

```dockerfile
FROM ubuntu:18.04
# ......
RUN cp -R /usr/lib* /home/ctf && \
    cp -R /lib* /home/ctf && \
	mkdir /home/ctf/dev && \
    mknod /home/ctf/dev/null c 1 3 && \
    mknod /home/ctf/dev/zero c 1 5 && \
    mknod /home/ctf/dev/random c 1 8 && \
    mknod /home/ctf/dev/urandom c 1 9 && \
    chmod 666 /home/ctf/dev/*
```

如果是`ubuntu:16.04`，前面的语句需要更新为：

```dockerfile
FROM ubuntu:16.04 as runner
# ......
RUN apt clean && apt autoclean && apt update && apt install -y xinetd && \
	useradd ctf -u 1000 -s /bin/sh
```

如果是`ubuntu:23.04`及更高版本，需要将用户的`uid`修改为`1001`：

```dockerfile
FROM ubuntu:23.04 as runner

RUN apt update && apt install -y xinetd

RUN	useradd ctf -u 1001 -s /bin/sh
```

# deploy_pwn_template
Templates for deploying pwn challenge in ctf.

There're four general templates for deploying pwn challenge, `sha256 proof of work` is supported, and setting flag from `env` is supported.

please read `readme.md` in each template to get more infomation.