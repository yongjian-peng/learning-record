# Ubuntu格式化硬盘ext4

# **插入 U 盘并识别设备**

lsblk

查看分区文件系统类型

lsblk -f

 

 卸载 U 盘的分区

sudo umount /dev/sdb1

sudo umount /media/user/xxxxx

 

确认卸载成功：

 

Lsblk 确保 MOUNTPOINT 列为空

 

# **删除现有分区表**

sudo fdisk /dev/sdb

 

进入交互式界面后，按以下步骤操作：

 

输入 p 列出分区。

输入 d 删除分区。如果有多个分区，重复输入 d，直到所有分区删除完毕。

输入 w 保存更改并退出。

# **创建新分区**

启动 fdisk 工具

sudo fdisk /dev/sdb

 

按以下步骤操作：

 

输入 n 创建新分区。

按 p 选择创建主分区。

输入分区编号（通常为 1）。

按回车键接受默认起始扇区和结束扇区。

输入 w 保存更改并退出。

 

# **格式化分区**

sudo mkfs.ext4 /dev/sdb1

 

格式化完成后，系统会输出类 似以下内容：

 

Creating filesystem with 3907584 4k blocks and 977280 inodes

Filesystem UUID: xxxx-xxxx

Superblock backups stored on blocks: ...

 

格式化为 FAT32 文件系统

运行以下命令，将分区格式化为 FAT32 文件系统：

sudo mkfs.vfat -F 32 /dev/sdb1

 

mkfs.vfat 用于格式化 FAT 文件系统。

-F 32 指定使用 FAT32 格式。

/dev/sdb1 是 U 盘的分区名称。

格式化为 exFAT 文件系统

安装 exFAT 工具

如果你的系统没有支持 exFAT 的工具，可以安装 exfat-utils：

 

sudo apt update

sudo apt install exfat-utils

一键获取完整项目代码

 

# **格式化为 exFAT**

安装完成后，运行以下命令将分区格式化为 exFAT 文件系统：

 

sudo mkfs.exfat /dev/sdb1

# **mkfs.exfat 用于格式化为 exFAT 文件系统****。**

/dev/sdb1 是 U 盘的分区名称。

格式化完成后，系统会输出类似以下内容：

 

mkexfatfs 1.3.0

Creating... done.

# **验证格式化结果**

lsblk -f

NAME FSTYPE LABEL UUID MOUNTPOINT 

sdb └─sdb1 ext4 xxxx-xxxx

 

 

 

 

 

 