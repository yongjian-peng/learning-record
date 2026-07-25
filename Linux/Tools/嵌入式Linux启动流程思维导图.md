# 嵌入式 Linux 启动流程思维导图

> 适用范围：ARM / i.MX6ULL 等嵌入式 Linux 平台  
> 核心主线：**上电 → Boot ROM → DDR 初始化 → U-Boot → Linux Kernel → RootFS → init/systemd → 用户应用**

---

## 一、总思维导图

```mermaid
mindmap
  root((嵌入式 Linux 启动流程))
    1. 程序构建
      源文件
        汇编 start.S
        C 源文件 main.c
        设备树 dts/dtsi
      编译
        gcc 将 C 编译为 .o
        as/gcc 将汇编编译为 .o
      链接
        链接脚本 .lds
        合并多个 .o
        确定代码和数据地址
        生成 ELF
      格式转换
        ELF 保留段和符号
        BIN 是纯二进制
        objcopy 生成 bin
        objdump/readelf 用于分析
    2. 上电与 Boot ROM
      上电复位
      CPU 从片内 ROM 执行
      判断启动介质
        SD
        eMMC
        NAND
        SPI NOR
        USB 下载
      读取启动镜像头
        IVT
        Boot Data
        DCD
      加载第一阶段程序
    3. DDR 初始化
      为什么必须初始化
        DDR 上电后不能直接使用
        需要时钟
        需要时序
        需要校准
        需要引脚复用
      两种模式
        DCD 模式
          Boot ROM 执行寄存器配置表
          初始化 IOMUX CCM MMDC DDR
        SPL 模式
          SPL 运行在 OCRAM/SRAM
          SPL 初始化 DDR
          SPL 加载完整 U-Boot
      片内内存
        Boot ROM
        OCRAM/SRAM
        容量小但上电可用
      外部 DDR
        例如 0x80000000 起始
        256MB 到 0x8FFFFFFF
        512MB 到 0x9FFFFFFF
    4. Bootloader 与 U-Boot
      Boot
        整个启动过程
      Bootloader
        引导加载程序的统称
      U-Boot
        Bootloader 的具体实现
      SPL
        小型一级加载程序
        初始化 DDR
        加载 U-Boot proper
      U-Boot proper
        完整命令行
        初始化 MMC 网络 USB
        读取环境变量
        加载 Linux 文件
        更新和烧录
      U-Boot 环境变量
        bootcmd
        bootargs
        bootdelay
        kernel_addr_r
        fdt_addr_r
        ramdisk_addr_r
    5. U-Boot 加载 Linux
      从存储介质加载到 DDR
        Kernel
          ARM32 zImage
          ARM64 Image
          uImage
          FIT/itb
        DTB
          描述开发板硬件
        initramfs
          可选临时根文件系统
      设置 bootargs
        console 串口
        root 根分区
        rootwait 等待设备
        rw 读写挂载
      启动命令
        bootz 启动 zImage
        booti 启动 Image
        bootm 启动 uImage/FIT
      控制权交接
        Starting kernel
        U-Boot 使命结束
    6. Linux Kernel
      内核解压
      初始化 CPU 和异常
      初始化 MMU 和虚拟内存
      初始化中断和调度器
      解析 DTB
      初始化驱动
        MMC
        网络
        串口
        GPIO
        I2C
        SPI
        显示
      挂载根文件系统
      启动 PID 1
    7. DTB 设备树
      源文件
        dts
        dtsi
      编译结果
        dtb
      描述内容
        CPU
        内存
        总线
        GPIO
        串口
        I2C/SPI
        网卡 PHY
        LCD
      不是程序
        CPU 不直接执行
        供内核读取
    8. 文件系统
      initramfs
        位于 RAM
        包含 /init
        用于早期启动
        加载驱动
        解密磁盘
        A/B 选择
        恢复升级
        切换真正 RootFS
      RootFS
        最终根文件系统
        bin sbin etc lib usr
        BusyBox/systemd
        动态库
        配置文件
        驱动模块
        用户应用
      init/systemd
        PID 1
        挂载 proc sys dev
        配置网络
        启动 SSH
        启动服务
        启动业务应用
    9. 烧录镜像组成
      原始启动区域
        SPL
        U-Boot
        U-Boot Environment
      Boot 分区
        zImage/Image
        dtb
        initramfs
        boot.scr
        extlinux.conf
        uEnv.txt
      RootFS 分区
        ext4
        squashfs
        ubifs/ubi
      Data 分区
        日志
        数据库
        用户数据
        升级包
      完整镜像
        sdcard.img
        wic
        system.img
    10. 地址与搬运
      链接地址
        链接器假定的运行地址
      加载地址
        Boot ROM/U-Boot 实际复制地址
      入口地址
        PC 最终跳转地址
      原则
        非位置无关代码要求地址匹配
        内存数据先装入寄存器再写回
        LDR 从内存读取
        STR 向内存写入
    11. 故障定位
      无串口输出
        供电复位
        启动拨码
        镜像偏移
        IVT/DCD
        DDR
      卡在 DRAM
        DDR 参数
        时钟
        位宽
        电源和走线
      U-Boot 找不到 Kernel
        MMC 设备号
        分区号
        文件名
        文件系统
      Starting kernel 后无输出
        console 参数
        DTB
        内核地址冲突
        启动命令和镜像格式
      无法挂载 RootFS
        root 参数
        分区号
        MMC/ext4 驱动
        rootwait
      找不到 init
        /sbin/init
        动态链接器
        权限
        架构不匹配
```

---

## 二、最核心的启动主线

```mermaid
flowchart TD
    A[设备上电/复位] --> B[SoC 片内 Boot ROM]
    B --> C{启动介质}
    C -->|SD/eMMC/NAND/SPI NOR| D[读取启动镜像头]
    D --> E{DDR 初始化方式}
    E -->|DCD| F[Boot ROM 按 DCD 配置 DDR]
    E -->|SPL| G[加载 SPL 到 OCRAM/SRAM]
    G --> H[SPL 初始化 DDR]
    F --> I[加载完整 U-Boot 到 DDR]
    H --> I
    I --> J[U-Boot proper]
    J --> K[加载 Kernel 到 DDR]
    J --> L[加载 DTB 到 DDR]
    J --> M[加载 initramfs 可选]
    J --> N[设置 bootargs]
    K --> O[bootz / booti / bootm]
    L --> O
    M --> O
    N --> O
    O --> P[Linux Kernel 接管 CPU]
    P --> Q[解析 DTB并初始化驱动]
    Q --> R{根文件系统方式}
    R -->|直接挂载| S[挂载 eMMC/SD/NAND RootFS]
    R -->|使用 initramfs| T[运行 /init]
    T --> S
    S --> U[启动 PID 1]
    U --> V[systemd / BusyBox init]
    V --> W[系统服务]
    W --> X[用户业务应用]
```

---

## 三、各阶段职责对照

| 阶段 | 运行位置 | 主要作用 | 下一阶段 |
|---|---|---|---|
| Boot ROM | SoC 片内 ROM | 判断启动介质、读取镜像头、加载第一阶段程序 | SPL 或 U-Boot |
| DCD | 镜像中的配置数据 | 供 Boot ROM 初始化 DDR 和相关寄存器 | U-Boot |
| SPL | OCRAM/SRAM | 初始化 DDR、加载完整 U-Boot | U-Boot proper |
| U-Boot proper | DDR | 加载 Kernel/DTB/initramfs，设置 bootargs | Linux Kernel |
| Linux Kernel | DDR | 管理 CPU、内存、驱动、进程和文件系统 | PID 1 |
| DTB | DDR 中的数据 | 描述开发板硬件，不是可执行程序 | 供 Kernel 解析 |
| initramfs | RAM | 早期用户空间、寻找真正 RootFS | RootFS |
| RootFS | SD/eMMC/NAND/NFS | 提供命令、库、配置、服务和应用 | init/systemd |
| init/systemd | 用户空间 | 启动服务和业务程序 | 用户应用 |

---

## 四、烧录镜像常见结构

```text
SD / eMMC 整盘
│
├── 原始启动区域
│   ├── SPL（可选）
│   ├── U-Boot / u-boot.imx
│   └── U-Boot Environment
│
├── Boot 分区
│   ├── zImage / Image / uImage / fitImage
│   ├── board.dtb
│   ├── initramfs（可选）
│   ├── boot.scr
│   ├── uEnv.txt
│   └── extlinux/extlinux.conf
│
├── RootFS 分区
│   ├── /bin
│   ├── /sbin
│   ├── /etc
│   ├── /lib
│   ├── /usr
│   └── 用户程序
│
└── Data 分区（可选）
    ├── 日志
    ├── 数据库
    ├── 用户数据
    └── 升级包
```

---

## 五、构建产物归类

### 1. 启动程序

```text
SPL
u-boot-spl.bin
u-boot.bin
u-boot.img
u-boot.imx
u-boot-dtb.imx
u-boot.itb
```

### 2. Linux 内核

```text
zImage      ARM 32 位常见
Image       ARM 64 位常见
uImage      传统 U-Boot 格式
fitImage    FIT 打包格式
```

### 3. 硬件描述

```text
*.dts
*.dtsi
*.dtb
```

### 4. 文件系统

```text
initramfs.cpio.gz
rootfs.ext4
rootfs.squashfs
rootfs.ubifs
ubi.img
rootfs.tar.gz
```

### 5. 完整烧录镜像

```text
sdcard.img
system.img
*.wic
```

---

## 六、链接地址、加载地址和入口地址

```text
链接地址：
链接器认为代码将要运行的位置。

加载地址：
Boot ROM、SPL 或 U-Boot 实际将镜像复制到的内存地址。

入口地址：
CPU 最终写入 PC，并从该地址开始执行。
```

对普通非位置无关代码：

```text
链接地址 ≈ 实际执行地址 ≈ 入口地址
```

如果三者不匹配，可能导致：

```text
函数跳转错误
全局变量地址错误
常量表读取错误
程序跑飞或死机
```

---

## 七、汇编赋值与启动代码中的数据搬运

ARM 是典型 Load/Store 架构：

```text
内存 → LDR → 寄存器 → STR → 内存
```

示例：

```asm
LDR R0, =0x30    /* R0 保存地址 */
LDR R1, [R0]     /* 从地址 0x30 读取值 */

LDR R0, =0x20    /* R0 改为目标地址 */
STR R1, [R0]     /* 将值写入地址 0x20 */
```

记忆：

```text
R0   ：寄存器中的内容
[R0] ：R0 指向的内存内容
LDR  ：从内存装载到寄存器
STR  ：从寄存器保存到内存
```

启动程序中的 DDR 搬运同样遵循这个原则，只是数据量更大，通常由 Boot ROM、DMA、SPL 或 U-Boot 驱动完成。

---

## 八、故障定位思维导图

```mermaid
flowchart TD
    A[启动失败] --> B{是否有串口输出}
    B -->|完全没有| C[检查供电/复位/启动拨码/镜像偏移/Boot ROM/DDR]
    B -->|有 U-Boot 输出| D{停在哪一步}
    D -->|DRAM| E[检查 DDR 型号/容量/位宽/时序/时钟/DCD/SPL]
    D -->|找不到 Kernel| F[检查 MMC 设备号/分区/文件名/文件系统/bootcmd]
    D -->|Starting kernel 后无输出| G[检查 console/DTB/加载地址/内核格式/启动命令]
    D -->|Kernel panic 无法挂载 RootFS| H[检查 root=/dev/.../rootwait/rootfstype/驱动]
    D -->|找不到 init| I[检查 /sbin/init/动态库/权限/CPU 架构]
```

---

## 九、一句话总记忆

> **Boot ROM 找到第一阶段程序；DCD 或 SPL 让 DDR 工作；U-Boot 加载 Kernel、DTB 和可选 initramfs；Kernel 根据 DTB 初始化硬件并挂载 RootFS；init/systemd 最后启动系统服务和用户应用。**

---

## 十、极简记忆口诀

```text
ROM 找入口
DCD/SPL 开 DDR
U-Boot 搬内核
DTB 讲硬件
Kernel 管系统
RootFS 提供用户空间
init 启动服务和应用
```
