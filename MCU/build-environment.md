# 编译修复

```
编译好 kernel 配置好了 电源域后，报错修复
文件	修改	目的
build.sh	复制前增加 mkdir -p "$OUT"，并检查复制结果	避免 Kernel 输出失败
mkimage.sh	使用 get_build_var TARGET_PREBUILT_KERNEL	正确获取 Kernel 路径
mkimage.sh	用户拒绝复制时返回非零状态	避免假成功

不能直接删除 DTBO 检查或者随便复制其他产品的 dtbo.img
审计DDR 空间 
内存分区
CONFIG_DEFAULT_DEVICE_TREE="rk3568-evb" 修改成正确的
U-Boot 与 Kernel 板级配置不一致
能只移植 RK809，还要检查 TCS4525 在 U-Boot 中是否需要初始化，以及对应驱动是否存在

```



# 电源域

```
IO Domain	原理图电源网络	电压
PMUIO2	VCC3V3_PMU	3.3V
VCCIO1	VCCIO_ACODEC	3.3V
VCCIO3	VCCIO_SD	3.3V
VCCIO4	VCC_1V8	1.8V
VCCIO5	VCC_3V3	3.3V
VCCIO6	VCC_1V8	1.8V
VCCIO7	VCC_3V3	3.3V
```



# Android 编译 3568

```
rockchip_defconfig：加载 RK3568 平台基础配置。
android-11.config：合并 Android 11 所需配置。
rk3568-evb1-ddr4-v10.img：指定 RK3568 DTS，生成对应的 Kernel 镜像。

rk356x_box-userdebug

docker ps --format "table {{.Names}}\t{{.Image}}\t{{.Status}}"
sudo docker cp rk3568-android11-build:/tmp/qy-rk3568-dts-audit.txt /home/pyj/docker-log/qy-rk3568-dts-audit.txt


```

## 第一步：准备编译环境

```
cd /workspace/kernel

export ARCH=arm64

export CROSS_COMPILE=/workspace/prebuilts/gcc/linux-x86/aarch64/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-

mkdir -p /workspace/build-logs

export BUILD_LOG=/workspace/build-logs/kernel-rk3568-evb1-ddr4-v10-$(date +%Y%m%d-%H%M%S).log

set -o pipefail

echo "========== 编译环境 =========="

echo "ARCH=$ARCH"

echo "CROSS_COMPILE=$CROSS_COMPILE"

echo "GCC:"
${CROSS_COMPILE}gcc --version | head -1

echo "Kernel:"
make -s kernelversion
```

## 第二步：清理旧的编译产物

```
echo "========== 清理 Kernel 编译产物 =========="

make clean 2>&1 | tee "$BUILD_LOG"
```

## 第三步：生成 RK3568 Android 11 配置

```
echo "========== 生成 RK3568 基础配置 =========="

make ARCH=arm64 \
    CROSS_COMPILE="$CROSS_COMPILE" \
    rockchip_defconfig \
    2>&1 | tee -a "$BUILD_LOG"
 
接着合并 Android 11 配置
echo "========== 合并 Android 11 配置 =========="

make ARCH=arm64 \
    CROSS_COMPILE="$CROSS_COMPILE" \
    android-11.config \
    2>&1 | tee -a "$BUILD_LOG"
```

## 第四步：检查配置是否生成成功

```
echo "========== 1. 检查 .config =========="

ls -lh .config

echo
echo "========== 2. 检查 ARM64 =========="

grep -E '^CONFIG_ARM64=y' .config

echo
echo "========== 3. 检查 RK 平台 =========="

grep -E '^CONFIG_ARCH_ROCKCHIP=y' .config

echo
echo "========== 4. 检查 Android 配置 =========="

grep -E 'CONFIG_ANDROID|CONFIG_ANDROID_BINDER' .config | head -30

echo
echo "========== 5. 检查配置文件 =========="

grep -E 'Merging|Value of' "$BUILD_LOG" | tail -30
```

## 正式编译 RK3568 Kernel

```
cd /workspace/kernel

echo "========== 开始 RK3568 Kernel 编译 =========="

make ARCH=arm64 \
    CROSS_COMPILE="$CROSS_COMPILE" \
    rk3568-evb1-ddr4-v10.img \
    -j16 \
    2>&1 | tee -a "$BUILD_LOG"
```

## 编译完成后的产物检查

```
cd /workspace/kernel

echo "========== 1. Kernel 镜像 =========="

ls -lh arch/arm64/boot/Image

echo
echo "========== 2. 压缩镜像 =========="

ls -lh arch/arm64/boot/Image.lz4

echo
echo "========== 3. RK3568 DTB =========="

ls -lh arch/arm64/boot/dts/rockchip/rk3568-evb1-ddr4-v10.dtb

echo
echo "========== 4. Kernel IMG =========="

ls -lh arch/arm64/boot/rk3568-evb1-ddr4-v10.img 2>/dev/null || true

echo
echo "========== 5. 编译日志 =========="

tail -40 "$BUILD_LOG"
```



# docker 镜像

```
启动容器:
  

# 检查容器名称是否已经存在
if sudo docker inspect rk3568-android11-build >/dev/null 2>&1; then
    echo "[INFO] 容器已经存在，不重复创建"
else
    echo "========== 创建 Android 11 编译容器 =========="

    sudo docker run -d \
      --name rk3568-android11-build \
      --hostname rk3568-android11 \
      --restart unless-stopped \
      --cpus=20 \
      --memory=32g \
      --memory-swap=34g \
      --shm-size=4g \
      --mount type=bind,source=/srv/embedded/workspaces/rk3568/android11,target=/workspace \
      --workdir /workspace \
      ubuntu:20.04 \
      sleep infinity
fi

echo
echo "========== 容器状态 =========="
 
 检查容器是否存在：
sudo docker ps \
  --filter name=rk3568-android11-build

以后进入环境只需要 
  sudo docker exec -it \
  --user pyj \
  rk3568-android11-build \
  bash


  
检出代码
repo sync \
    --local-only \
    --no-manifest-update \
    -j4
显示用户
bash

export PS1='[\u@\h \w]\$ '

whoami
pwd
    
```



# 构建清单

```
正式搭建任务清单
按依赖顺序排列
整体完成情况
服务器
[ ] 确认 3.6TB NVMe，挂载 /srv/embedded
[ ] 创建 dev-a、dev-b 账号和权限
[ ] 建立统一目录和存储权限
本地环境
[ ] 配置 VMware Ubuntu 20.04 / 22.04，共四台
网络
[ ] 配置 VMnet0 桥接和 IP 地址
[ ] 验证四台 VM 访问服务器及四块板卡
源码
[ ] 导入 RK3568 / RK3588 原厂 SDK
[ ] 配置 Git 仓库、Manifest 和分支
Docker
[ ] 创建独立 embedded-build Compose 项目
[ ] 构建 Linux 20.04 / 22.04 镜像
[ ] 构建四套 Android SDK 专用镜像
编译
[ ] 完成 Kernel、DTB、KO 编译验证
[ ] 完成 Android 11 / 13 编译验证
调试
[ ] 配置 NFS / TFTP
[ ] 完成四块板卡独立调试
自动化
[ ] 实现统一 build.sh 和产物归档
```

# 空间规划

```
用途	规划空间
RK3568 Linux / Android SDK	800GB
RK3588 Linux / Android SDK	800GB
两人独立工作区	400GB
Docker 构建缓存	600GB
NFS / TFTP / 编译产物	500GB
预留空间	约 500GB
合计	约 3.6TB
```

# 提示语模版

```
# RK3568 / Linux 驱动开发通用提示词

## 提示词 1：让 AI 帮我制定技术排查路线

我正在进行 RK3568 Android 11 BSP 适配，当前工作目录是 `/workspace`，涉及 U-Boot、Linux Kernel、Device Tree、PMIC、Regulator、I2C 和 IO Domain。

你是一名具有丰富量产经验的资深 Linux BSP 工程师和驱动开发工程师。

我的驱动基础知识还不够扎实，不要求我自己想出应该检查哪些文件、执行哪些命令。

请根据我的实际目标，主动制定排查路线。

要求：

1. 先明确最终要实现的硬件功能。
2. 从硬件原理图、设备树、驱动源码、Kconfig、Makefile、编译配置几个层面分析。
3. 主动指出需要检查的文件、函数、结构体和关键代码。
4. 每次只检查必要的信息，禁止没有目的的大范围 grep。
5. 明确区分已经证实的事实、合理推断和未知信息。
6. 不允许根据 EVB 配置直接推断 QY 产品硬件配置。
7. 如果缺少硬件信息，明确告诉我需要什么原理图或日志。
8. 给出的命令必须完整，可以直接复制执行。
9. 不要直接进入全量编译，先完成低成本检查。

每个阶段都需要说明：
- 当前要解决什么问题。
- 为什么要检查这些文件。
- 每个命令的作用。
- 预期看到什么结果。
- 不同结果分别意味着什么。
- 下一步应该做什么。

## 提示词 2：让 AI 以资深驱动工程师身份分析日志

下面是我实际执行的命令和日志。

请不要简单总结日志，也不要直接给我一堆新的检查命令。

请完成以下工作：

1. 找出已经确认的技术事实。
2. 找出真正没有解决的问题。
3. 判断问题属于硬件、DTS、驱动、Kconfig、Makefile、编译系统中的哪一层。
4. 找出最可能的根因，并说明证据。
5. 排除已经确认正常的部分，避免重复检查。
6. 只提出能够区分不同原因的最少检查命令。
7. 如果已经有足够证据，直接给出修改方案，不要继续审计。
8. 如果没有足够证据，明确指出缺少哪一项证据。

特别要求：

不要让我反复执行耗时很长的完整 Android 编译。

优先使用：
- grep / sed 查看源码。
- make defconfig 检查配置。
- dtc 检查设备树。
- 单独编译 U-Boot。
- 单独验证 DTB。
- 最后才进行 Android 完整编译。

## 提示词 3：让 AI 帮我定位应该修改哪个文件

我的目标是：这里填写实际硬件功能或故障。

请从 Linux 驱动工程师的角度分析完整实现链路。

例如：
硬件芯片 → I2C 控制器 → Device Tree → 驱动匹配 → probe → Regulator → IO Domain → GRF 寄存器。

请回答：

1. 这个功能由谁实现？Bootloader、Kernel 还是用户空间？
2. 涉及哪些硬件模块？
3. 需要修改哪些 DTS/DTSI 文件？
4. 需要修改哪些 C 文件？
5. 需要修改哪些 Kconfig 和 Makefile？
6. 是否可以复用原厂驱动？
7. 现有驱动支持哪些 compatible？
8. DTS 中哪些属性是驱动实际读取的？
9. 驱动 probe 的调用链是什么？
10. 如何验证修改有效？

必须给出真实源码依据，包括文件路径、函数名称和关键代码。

不要凭经验猜测硬件引脚、电压、I2C 地址或 GPIO 编号。

## 提示词 4：让 AI 直接给出最小修改补丁

现在已经完成源码审计，证据如下：

这里粘贴前面收集的日志。

请根据已经确认的事实，给出最小修改方案。

要求：

1. 优先复用原厂驱动和已有板级代码。
2. 不破坏原厂 EVB 配置。
3. QY 使用独立 DTS 和独立 Defconfig。
4. 只修改实现目标必须涉及的文件。
5. 修改前自动备份。
6. 修改脚本必须具备重复执行保护，避免重复添加配置。
7. 给出完整可执行命令，不让我手动逐行编辑。
8. 修改后自动检查文件内容和关键配置。
9. 先进行 DTS 和 U-Boot 单独编译验证。
10. 验证通过后再进入 Android 完整编译。

每个修改必须解释：
- 为什么修改。
- 原来的配置有什么问题。
- 修改后的工作原理。
- 如果不修改会有什么后果。

如果缺少必要的硬件参数，不允许编造，先列出缺失项。

## 提示词 5：边解决问题边学习 Linux 驱动

这次任务是：填写当前实际任务。

请你不要只把问题解决，还要让我掌握相关驱动开发知识。

按照以下顺序教学：

第一部分：硬件原理
- 芯片是什么？
- 连接了哪些外设？
- 电源从哪里来？
- 信号通过什么总线传输？

第二部分：设备树
- 对应的 DTS 节点在哪里？
- compatible 是什么？
- reg、interrupts、pinctrl、supply 分别是什么？
- 这些属性由谁读取？

第三部分：驱动源码
- 驱动入口在哪里？
- of_match 是什么？
- probe 什么时候执行？
- 私有结构体保存什么？
- I2C 读写函数如何工作？
- Regulator 如何注册？

第四部分：实际运行流程
- U-Boot 如何识别设备？
- Kernel 如何重新识别设备？
- 驱动如何控制硬件？
- 如何从日志判断驱动成功？

第五部分：实战训练
- 给我一个真实的工程问题。
- 让我先分析。
- 不要直接给出全部答案。
- 根据我的回答纠正理解。
- 最后让我独立判断需要修改哪个文件。

教学标准：按照资深 Linux BSP / 驱动工程师的标准审核我的理解。

## 提示词 6：编译失败时避免重复浪费时间

下面是本次编译日志。

请按照专业工程师的方式排查：

1. 找到第一处真正的 fatal error，而不是只看最后的 ninja failed。
2. 区分 warning、error、ninja failed 和上层脚本退出。
3. 判断是否属于源码错误、配置错误、工具链错误或环境错误。
4. 判断失败前已经生成的产物是否有效。
5. 判断是否只需要重新编译某个模块或目标。
6. 给出最小修复方案。
7. 先进行低成本验证，再进行耗时构建。
8. 如果需要修改代码，提供完整的备份、修改、验证脚本。
9. 记录本次问题，避免下一次重复排查。

特别要求：

不要为了一个局部错误重新执行半小时甚至数小时的完整 Android 编译。

所有命令尽量合并成一个完整代码块，并保留执行日志和真实退出码。

## 工作习惯

以后我只需要提供：

- 当前任务目标。
- 实际执行记录。
- 错误日志或源码。

你负责确定下一步应该做什么，并且同时教会我背后的技术原理。
```

