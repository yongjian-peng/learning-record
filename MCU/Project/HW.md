# 验证RK3588 stress 

![image-20260707150451435](E:\Git\learning-record\MCU\Project\assets\image-20260707150451435.png)

```
watch -n 2 sensors
sudo stress --cpu 8 --vm 13 --vm-bytes 1G
htop
```



更新记录和镜像映射关系

```
QY3568_ubuntu_20250210.img
user@user:~/桌面$ cat /etc/buildinfo
# 系统构建信息
BUILD_TIMESTAMP="2025-06-28T09:30:06+00:00"
COMMIT_MESSAGE="20250628 优化ec200u 模组拨号模式ECM"
KERNEL_VERSION="5.10.198-ge7e660a3cbf3-dirty"
BUILD_MACHINE="user"


QY3568_ubuntu_msata_20250715.img
user@user:~$ cat /etc/buildinfo
# 系统构建信息
BUILD_TIMESTAMP="2025-07-05T06:45:27+00:00"
COMMIT_MESSAGE="1. 加入lcdparamservice 2. 修改网络优先级Metric(eth0>eth1>wlan0>4G)"
KERNEL_VERSION="5.10.198-gff88d0e8403a"
BUILD_MACHINE="user"
# 系统构建信息 - 2025-07-15 13:56:13
BUILD_TIMESTAMP="2025-07-15T13:56:13+08:00"
COMMIT_MESSAGE="1. 在/opt下配置Qt5.15.2"
KERNEL_VERSION="5.10.198-g90721e7741d7"
BUILD_MACHINE="user"
----------------------------------------



 

```

