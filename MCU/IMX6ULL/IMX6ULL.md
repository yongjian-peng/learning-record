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

