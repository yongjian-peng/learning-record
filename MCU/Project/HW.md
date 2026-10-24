# 声音功能记录

```
ubuntu 22.04/update.img  声音 ok 
```



# 重启网络 WIFI

```
另外，如果客户有复现到问题，可以用这个方式试验下能否恢复网络

先ifconfig看下ip分配的情况

然后sudo nmcli device reapply wlan0 || sudo nmcli connection up 你的wifi名称

再ifconfig看下

然后ping 223.5.5.5
```



# 查看开机记录

```
如果是更换其他路由器就没有问题，那么推测是原来的随身wifi的 DHCP有点问题，租期到了没有自动续租。

journalctl --list-boots可以看到开关机的记录。找到哪一次是wifi掉的开机时间段，例如如果前面是-4，则以下面的方式抓取日志给我：

journalctl -b -4 > -4.log
```



# 更改 DPI 

```
查看当前屏幕密度  
wm density

设置屏幕密度  
wm density 640

重置屏幕密度为默认  
wm density reset
```



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

Ubuntu 22.04 update.img
user@rk3588:~$ cat /etc/buildinfo
# 系统构建信息 - 2025-12-22 09:13:14
BUILD_TIMESTAMP="2025-12-22T09:13:14+00:00"
COMMIT_MESSAGE="- 允许root用户登录ssh
- 将firefly用户更名为user用户
- 修改/home/firefly为/home/user
- 修改firefly组为user组
- 修改/home/user下文件归属为user:user
- 修改桌面自动登录的用户为user
- 修改root账户密码为123456
- 修改user账户密码为123456
- 修改主机名为rk3588
- 将GECOS字段设置为空
- 安装libxfce4ui-utils以支持xfce4-about
- 安装policykit-1 policykit-1-gnome以解决policykit缺失导致的普通用户无法安装新语言的问题
- 更换系统默认语言为中文
- 更换系统地区格式为中文
- 更换家目录下目录为中文
- 更改时间显示时区为Asia/Shanghai
- 制作并更换默认壁纸
- 删除/usr/share/xfce4/backdrops/下firefly相关壁纸
- 更换whisker图标
- 移除/home/user/.config/gtk-3.0/bookmarks对应的文件管理器中的目录书签
- 关闭面板透明、关闭终端透明、关闭whisker透明，窗口管理器微调中窗口装饰的不透明度设置为不透明
- 外观中，设置样式为Greybird
- 外观中，设置图标为elementary Xfce dark
- 调整面板背景色为深蓝，注意网络图标与背景的对比度
- 关闭plank开机自启
- 在桌面添加图标chromium、Thunar文件管理器、xfce 终端
- 预装软件：
串口软件cutecom
相机软件cheese
录音软件gnome-sound-recorder
图片查看器nomacs，设置为中文，并更改图片默认打开方式
截图软件 xfce4-screenshooter，调整保存页面过大的问题
编译相关 gcc make
- 更新系统所有软件包apt upgrade
- 增加刷屏参功能，编译并将lcdparamservice放到/usr/local/bin
- 安装软件udiskie，配置开机启动udiskie -a -n -t，解决非热插拔是，没有打开Thunar时外部存储没有自动挂载的问题
- 文件系统识别确认
ext4
fat
ntfs
- 压缩文件确认
zip: zip unzip
7z: p7zip-full
rar: unrar(rar压缩文件在apt仓库中不存在)
- 将Fcitx输入法切换由Ctrl+空格改为左shift"
KERNEL_VERSION="5.10.160+"
BUILD_MACHINE="rk3588"
----------------------------------------
# 系统构建信息 - 2025-12-24 05:44:53
BUILD_TIMESTAMP="2025-12-24T05:44:53+00:00"
COMMIT_MESSAGE="- 加入/etc/rc.local
- 调整alsamixer，修复输出和输入都无声音的问题，配置保存为/root/asound.state，开机脚本自动加载
- 修改ssh问候信息，/etc/update-motd.d/00-header中firefly改为Ubuntu，删除/etc/update-motd.d/10-help-text
- 新增`/usr/local/bin/resize-rootfs.sh`，根文件系统动态扩展
- WiFi使用C驱动，开机脚本中加载ko，该驱动会根据WiFI型号自动在/lib/firmware/cypress下寻找对应的固件
- 备份firefly系统中/lib/firmware/cypress中43455的bin和clm_blob，替换为fae提供的43455bin、clm_blob、txt，加入43430的bin和txt，都给予执行权限
- 开机脚本中不加载蓝牙，使用firefly内置的udev规则进行蓝牙加载(/usr/bin/bt-attach)
- 安装python3-pip
- 安装pip3 install pyserial
- 用户目录下test目录内置测试文件
media
png jpeg mp3 mp4
tool
gpioinfo gpiotest uarttest
- 外观中，设置样式为Greybird Dark
- /usr/local/bin中添加Broadcom无线WiFi配置工具wl64_fmac
- 安装gedit
- 安装lm-sensors"
KERNEL_VERSION="5.10.160+"
BUILD_MACHINE="rk3588"

QY3588_Ubuntu20_xfce_20260109.img 镜像 
user@user:~$ cat /etc/os-release
NAME="Ubuntu"
VERSION="20.04.6 LTS (Focal Fossa)"
ID=ubuntu
ID_LIKE=debian
PRETTY_NAME="Ubuntu 20.04.6 LTS"
VERSION_ID="20.04"
HOME_URL="https://www.ubuntu.com/"
SUPPORT_URL="https://help.ubuntu.com/"
BUG_REPORT_URL="https://bugs.launchpad.net/ubuntu/"
PRIVACY_POLICY_URL="https://www.ubuntu.com/legal/terms-and-policies/privacy-policy"
VERSION_CODENAME=focal
UBUNTU_CODENAME=focal
user@user:~$ uname -a
Linux user 5.10.160+ #136 SMP Fri Jan 9 09:21:08 CST 2026 aarch64 aarch64 aarch64 GNU/Linux
user@user:~$ uname -r
5.10.160+
user@user:~$ cat /etc/buildinfo
# 系统构建信息 - 2025-08-18 10:19:18
BUILD_TIMESTAMP="2025-08-18T10:19:18+00:00"
COMMIT_MESSAGE="重做xfce根文件系统：
1. 声音，解决静音问题，开机脚本中配置声音
2. firefly账户更名为user
3. 配置user账户自动登录桌面
4. 配置user和root密码
5. 修改为中文
6. 时区，日期时间更新
7. 更换桌面壁纸
8. 配置桌面默认壁纸
9. 开机脚本配置（gpio、蓝牙、4G、根文件系统动态扩展、重置machine id等）
10. 内置测试脚本(gpio信息脚本、gpio测试脚本、uart测试脚本)、npu demo等
11. 预装cutecom
12. 预装todesk、提供todesk、向日葵安装包
13. 面板上增加亮度条件功能
14. 修改网络优先级，有线>WiFi>4G
15. 修复DNS解析问题
16. 删除默认密钥环
17. 增加刷屏参功能
18. 增加cm372、cm256WiFi固件
19. 解决LVDS屏幕重叠在HDMI屏幕的问题
20. 蓝牙固件使用了XFCE原预置的
21. 修复Snap不支持的问题
22. 装libjpeg62-turbo:arm64，修复原生chromium无法使用的问题
23. 修改SSH连接时提示的欢迎信息
24. 允许root登录ssh
25. 修复PolicyKitError
26. 更改主机名为user
27. 通过修改用户全名GECOS字段，更改开始菜单Whisker中的用户名
28. 修改Whisker开始菜单图标
29. 安装软件python3-pip、pyserial、p7zip-full"
KERNEL_VERSION="5.10.160+"
BUILD_MACHINE="user"
----------------------------------------
# 系统构建信息 - 2025-08-19 08:43:20
BUILD_TIMESTAMP="2025-08-19T08:43:20+00:00"
COMMIT_MESSAGE="1. 卸载了todesk
2. 用于验证蓝牙"
KERNEL_VERSION="5.10.160+"
BUILD_MACHINE="user"
----------------------------------------
# 系统构建信息 - 2025-09-30 09:07:47
BUILD_TIMESTAMP="2025-09-30T09:07:47+00:00"
COMMIT_MESSAGE="- 关闭原根文件系统中/lib/udev/rules.d/50-bluetooth.rules自动触发蓝牙固件下载的逻辑，改为开机脚本中进行加载
- WiFi驱动从编译进内核改为KO模式加载，通过读PID替换固件实现cm256和nm372的兼容
- 通过读pid加载对应的蓝牙固件"
KERNEL_VERSION="5.10.160+"
BUILD_MACHINE="user"
----------------------------------------
# 系统构建信息 - 2025-10-10 03:48:11
BUILD_TIMESTAMP="2025-10-10T03:48:11+00:00"
COMMIT_MESSAGE="- 外观调整：样式 Greybird 图标 elementart Xfce darker
- 窗口管理器微调：取消勾选在常规窗口下显示阴影；将窗口装饰的不透明度改为不透明
- 调整开始菜单、面板、终端为不透明
- 修复重启后U盘、tf卡无法自动挂载的问题（udiskie自启动）
- 加入了桌面系统日志、信息展示功能（conky）（用于测试）（可在会话和启动中关闭）
- 进行了 apt upgrade
- 编写并增加了stress-gui工具
- 开启conky信息展示
- stress-gui放置在plank中"
KERNEL_VERSION="5.10.160+"
BUILD_MACHINE="user"
----------------------------------------
# 系统构建信息 - 2025-10-18 06:27:56
BUILD_TIMESTAMP="2025-10-18T06:27:56+00:00"
COMMIT_MESSAGE="- 兼容EC200M-CN
- 开机设置4G网络usb0的优先级，跃点1000
- /etc/udev/rules.d/77-dongle.rules规则更名为/etc/udev/rules.d/77-dongle.rules.disabled
- 安装相机软件cheese、录音软件gnome-sound-recorder、图片查看器nomacs
- 图片默认打开方式由chromium改为nomacs
- 预置测试用wav、mp4、png
- 解决麦克风无声音问题，alsamixer F4 Capture页中，ADC设为最大"
KERNEL_VERSION="5.10.160+"
BUILD_MACHINE="user"
----------------------------------------
# 系统构建信息 - 2026-01-09 07:31:59
BUILD_TIMESTAMP="2026-01-09T07:31:59+00:00"
COMMIT_MESSAGE="- WiFi更换为C驱动，解决WiFi相关问题，并兼容256和372
        - 开机脚本修改加载的ko为/root/brcmutil.ko、/root/brcmfmac.ko
        - /lib/firmware/cypress增加43430固件，替换43455固件，并全部给予执行权限
- 取消开机脚本中蓝牙配置，更换回udev蓝牙自动加载（取消注释/lib/udev/rules.d/50-bluetooth.rules）
- 安装软件unrar、lm-sensors、gedit
- 首次开机重启systemd-journald"
KERNEL_VERSION="5.10.160+"
BUILD_MACHINE="user"
----------------------------------------




 

```

