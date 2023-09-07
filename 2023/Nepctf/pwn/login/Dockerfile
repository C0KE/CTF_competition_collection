FROM ubuntu:22.04

RUN sed -i "s/http:\/\/archive.ubuntu.com/http:\/\/mirrors.tuna.tsinghua.edu.cn/g" /etc/apt/sources.list && \
    apt-get update && apt-get -y dist-upgrade && \
    apt-get update && apt-get install -y lib32z1 build-essential && \
    apt-get install -y openssl && \
    useradd -m ctf

WORKDIR /home/ctf



RUN mkdir /home/ctf/www && \
    mkdir /home/ctf/dev && \
    mkdir /home/ctf/bin && \
    cp -r /usr/lib /home/ctf/ && \
    cp -r /usr/lib64 /home/ctf/ && \
    cp /bin/sh /home/ctf/bin && \
    cp /bin/bash /home/ctf/bin && \
    cp /usr/bin/timeout /home/ctf/bin && \
    cp /bin/ls /home/ctf/bin && \
    cp /bin/cat /home/ctf/bin && \
    cp /usr/bin/openssl /home/ctf/bin && \
    mknod /home/ctf/dev/null c 1 3 && \
    mknod /home/ctf/dev/zero c 1 5 && \
    mknod /home/ctf/dev/random c 1 8 && \
    mknod /home/ctf/dev/urandom c 1 9 && \
    rm -rf /home/ctf/lib/apt /home/ctf/lib/cpp /home/ctf/lib/gnupg /home/ctf/lib/init /home/ctf/lib/lsb /home/ctf/lib/os-release /home/ctf/lib/rsyslog /home/ctf/lib/tc /home/ctf/lib/udev /home/ctf/lib/binfmt.d /home/ctf/lib/dpkg /home/ctf/lib/gold-ld /home/ctf/lib/initramfs-tools /home/ctf/lib/ldscripts /home/ctf/lib/mime /home/ctf/lib/python2.7 /home/ctf/lib/systemd /home/ctf/lib/terminfo /home/ctf/lib/compat-ld /home/ctf/lib/gcc /home/ctf/lib/ifupdown /home/ctf/lib/insserv /home/ctf/lib/locale /home/ctf/lib/modules-load.d /home/ctf/lib/python3 /home/ctf/lib/tar /home/ctf/lib/tmpfiles.d


COPY ./libmicrohttpd.so.12 /home/ctf/lib/x86_64-linux-gnu/
COPY ./ld-linux-x86-64.so.2 /home/ctf/lib64
COPY ./login /home/ctf/
COPY ./index.html /home/ctf/
COPY ./file.html /home/ctf/
COPY ./flag.txt /
COPY ./run.sh /home/ctf
COPY ./readme.txt /home/ctf/www
COPY ./Dockerfile /home/ctf/
COPY ./docker-compose.yml /home/ctf/
USER root

RUN chmod +x /home/ctf/run.sh && \
    cp /home/ctf/lib/x86_64-linux-gnu/libmicrohttpd.so.12 /lib/x86_64-linux-gnu/ && \
    chmod +x /home/ctf/login && \
    chown -R ctf /home/ctf && \
    chmod -R 750 /home/ctf

CMD exec /bin/bash -c "/home/ctf/run.sh"

EXPOSE 8080
