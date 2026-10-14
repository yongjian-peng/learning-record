# BusyBox-rootfs

```
CROSS_COMPILE ?= /usr/local/arm/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-
ARCH ?= arm

make defconfig
make menuconfig

make 
 make install CONFIG_PREFIX=/home/boot/linux/nfs/rootfs
 
setenv bootargs 'console=ttymxc0,115200 root=/dev/nfs nfsroot=192.168.10.246:/home/boot/linux/nfs/rootfs,proto=tcp rw ip=192.168.10.100:192.168.10.246:192.168.10.1:255.255.255.0::eth0:off' //设置 bootargs
saveenv //保存环境变量
```





# Linux 移植

```
make imx_alientek_emmc_defconfig

添加开发板默认配置文件
cd arch/arm/configs
cp imx_v7_mfg_defconfig imx_alientek_emmc_defconfig
找到“CONFIG_ARCH_MULTI_V6=y”这一行，将其屏蔽掉

添加开发板对应的设备树文件
cd arch/arm/boot/dts
cp imx6ull-14x14-evk.dts imx6ull-alientek-emmc.dts

chmod 777 imx6ull_alientek_emmc.sh //给予可执行权限
./imx6ull_alientek_emmc.sh //执行 shell 脚本编译内核

#!/bin/sh
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- distclean
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- imx_alientek_emmc_defconfig
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- menuconfig
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- all -j16

cp arch/arm/boot/zImage /home/boot/linux/tftpboot/ -f
cp arch/arm/boot/dts/imx6ull-alientek-emmc.dtb /home/boot/linux/tftpboot/ -f

tftp 80800000 zImage
tftp 83000000 imx6ull-alientek-emmc.dtb
bootz 80800000 – 83000000

cat /proc/cpuinfo
有 BogoMIPS 这一条，此时 BogoMIPS 为 3.00， BogoMIPS 是 Linux 系统中衡量处理器运行速度的一个“尺子”，处理器性能越强，主频越高
进入到目录/sys/bus/cpu/devices/cpu0/cpufreq 中，此目录下会有很多文件
查看当前 CPU 频率：
cat /sys/bus/cpu/devices/cpu0/cpufreq/cpuinfo_cur_freq
CPU 在各频率下的工作时间，命令如下：
cat /sys/bus/cpu/devices/cpu0/cpufreq/stats/time_in_state

make dtbs
命令“make dtbs”只编译设备树文件，也就是将.dts 编译为.dtb，

修改 EMMC 驱动 和 关闭 EMMC 1.8V 供电选项

```





# Uboot 移植

```
make mx6ull_alientek_emmc_defconfig
make menuconfig

从 EMMC 启动 Linux 系统
setenv bootargs 'console=ttymxc0,115200 root=/dev/mmcblk1p2 rootwait rw'
setenv bootcmd 'mmc dev 1; fatload mmc 1:1 80800000 zImage; fatload mmc 1:1 83000000 imx6ull-alientek-emmc.dtb; bootz 80800000 - 83000000;'

从网络启动 linux 系统
setenv bootargs 'console=ttymxc0,115200 root=/dev/mmcblk1p2 rootwait rw'
setenv bootcmd 'tftp 80800000 zImage; tftp 83000000 imx6ull-alientek-emmc.dtb; bootz 80800000 - 83000000'
saveenv
```



# 安装工具

```
NFS 服务开启
sudo apt-get install nfs-kernel-server rpcbind
使用如下命令打开 nfs 配置文件/etc/exports：
sudo vi /etc/exports

打开/etc/exports 以后在后面添加如下所示内容：
/home/zuozhongkai/linux/nfs *(rw,sync,no_root_squash)
重启 NFS 服务，
sudo /etc/init.d/nfs-kernel-server restart

SSH 服务开启
 sudo apt-get install openssh-server

/******************************************************************************************************/
tftp 服务
sudo apt-get install tftp-hpa tftpd-hpa
sudo apt-get install xinetd
mkdir /home/zuozhongkai/linux/tftpboot
chmod 777 /home/zuozhongkai/linux/tftpboot
安装完成以后新建文件/etc/xinetd.d/tftp
server tftp
{
	socket_type = dgram
	protocol = udp
	wait = yes
	user = root
	server = /usr/sbin/in.tftpd
	server_args = -s /home/zuozhongkai/linux/tftpboot/
	disable = no
	per_source = 11
	cps = 100 2
	flags = IPv4
}

完了以后启动 tftp 服务， sudo service tftpd-hpa start
打开/etc/default/tftpd-hpa 文件，将其修改为如下所示内容：
# /etc/default/tftpd-hpa

TP_USERNAME="tftp"
TFTP_DIRECTORY="/home/zuozhongkai/linux/tftpboot"
TFTP_ADDRESS=":69"
TFTP_OPTIONS="-l -c -s"

最后重启服务
sudo service tftpd-hpa restart


tar -vxjf uboot-imx-rel_imx_4.1.15_2.1.0_ga.tar.bz2
```



# 空间分布

常用地址分布

![image-20260903203847086](E:\Git\learning-record\MCU\IMX6ULL\assets\image-20260903203847086.png)

# 启动流程

```
CPU上电复位
    ↓
执行芯片内部Boot ROM
    ↓
检测BOOT_MODE和启动设备
    ↓
确定从SD卡启动
    ↓
Boot ROM初始化USDHC
    ↓
读取SD卡最初4 KiB到片内OCRAM
    ↓
找到IVT
    ↓
根据IVT找到DCD
    ↓
执行DCD，初始化DDR
    ↓
读取Boot Data
    ↓
把完整镜像从SD卡搬到DDR
    ↓
跳转到IVT.entry
    ↓
开始执行用户程序
```



# 逻辑开发

```
开启 LED 灯：
	使用 .S  文件。里面直接编写汇编。来初始化时钟，初始化GPIO, 初始化复用关系。led.elf 中间文件
	设置使用 SD 卡 来做为存储
	烧录到 SD 卡中，设置拨码开关
	
设置 SVC模式启动：
	文件 boot 内容 IVT 
    设置内核启动方式。设置内核寄存器。 编写 Makefile 生成中间文件。
    设置 .S 文件引入 lds 后缀文件。设置 SVC模式 其中 9 种模式中的其中一种。
    
.S 文件 清除 BSS 段:
引入 STM32 风格中的 寄存器宏定义使用。
项目架构按照 BSP 结构目录拆分。并编写 工程文件 Makefile 文件。 （蜂鸣器，按键控制蜂鸣器）
设置芯片 PLL 各路时钟的初始化。（时钟树的路径讲解。拆分）

设置 复位中断 和 IRQ 中断
	中断向量偏移设置
	GIC寄存器，访问CPU接口端的基地址
```

![image-20260728202631118](E:\Git\learning-record\MCU\IMX6ULL\assets\image-20260728202631118.png)

![image-20260728202831686](E:\Git\learning-record\MCU\IMX6ULL\assets\image-20260728202831686.png)

