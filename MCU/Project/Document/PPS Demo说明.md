PPS Demo说明

# **编译驱动 pps_gpio_custo**

说明：使用的是 SuperIO GPIO 接入 方式。配置引脚为 IN GP11。

这个示例是轮询的方式。如果要生产环境的话，优化为 外部中断检测的方式。需要优化驱动代码。

 

将提供到驱动代码，pps_gpio_custom 拷贝到机器的家目录下，使用 root 来编译。

![img](E:\Git\learning-record\MCU\Project\Document\assets\wps3.jpg) 

![img](E:\Git\learning-record\MCU\Project\Document\assets\wps4.jpg) 

 

修改 驱动源码 引脚编号位置 0xA00 bit 1 代表的是 IN GP11  

![img](E:\Git\learning-record\MCU\Project\Document\assets\wps5.jpg) 

修改 驱动源码，单位 微秒 usleep_range(1000000, 1500000); 延时被设置在1秒到1.5秒之间 根据需求来改轮询频率。

![img](E:\Git\learning-record\MCU\Project\Document\assets\wps6.jpg) 

 

make

![img](E:\Git\learning-record\MCU\Project\Document\assets\wps7.jpg) 

 

insmod pps_gpio_custom.ko

lsmod | grep pps 可以查看到 驱动加载

![img](E:\Git\learning-record\MCU\Project\Document\assets\wps8.jpg) 

 

dmesg | grep pps

![img](E:\Git\learning-record\MCU\Project\Document\assets\wps9.jpg) 

 

ls -al /dev/pps

![img](E:\Git\learning-record\MCU\Project\Document\assets\wps10.jpg) 

 

卸载 pps  rmmod pps_gpio_custo 

查看 pps  lsmod | grep pp

# ![img](E:\Git\learning-record\MCU\Project\Document\assets\wps11.jpg) 

# **安装 测试工具 pps-tools** 

sudo apt install pps-tools

 

测试 pps 命令：ppstest /dev/pps2

![img](E:\Git\learning-record\MCU\Project\Document\assets\wps12.jpg) 

 

# **引脚说明**

![img](E:\Git\learning-record\MCU\Project\Document\assets\wps13.jpg) 