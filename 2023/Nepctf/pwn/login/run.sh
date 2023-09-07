#!/bin/bash

while true; do
    # 复制 flag.txt
    echo $GZCTF_FLAG>/flag.txt
    export GZCTF_FLAG=flag{test}
    cp /flag.txt /home/ctf/
    # 在 chroot 环境中执行 login 程序
    chroot /home/ctf /bin/bash -c "/bin/bash -c "./login" "
    
    # 休眠 60 分钟
    sleep 3600
done
