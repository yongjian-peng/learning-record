gstreamer安装说明

# **注意：**

先安装依赖

配置 meson 工具 指定版本

下载安装包，从 https://gitlab.freedesktop.org/ 下载 编译时间比较长。需要安装很多的库文件。

设置 字符编码。

编译安装。

# **安装依赖**

apt-get install libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libgstreamer-plugins-bad1.0-dev gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-libav gstreamer1.0-tools gstreamer1.0-x gstreamer1.0-alsa gstreamer1.0-gl gstreamer1.0-gtk3 gstreamer1.0-qt5 gstreamer1.0-pulseaudio

 

sudo apt install -y build-essential meson ninja-build python3-pip bison flex \

sudo apt install -y libasound2-dev libpulse-dev libopus-dev libvpx-dev \

sudo apt install -y libglib2.0-dev libdw-dev libunwind-dev libcap2-bin

sudo apt install -y libx264-dev libjpeg-dev libmp3lame-dev libfdk-aac-dev

sudo apt install -y libgtk-3-dev libsoup2.4-dev libssl-dev

 

 

# **下载安装包**

从官网下载 安装包

https://gitlab.freedesktop.org/gstreamer/gstreamer/-/tags?page=4 

 

# **更新** **meson**

删除所有 meson 安装

sudo apt remove --purge meson

rm -rf ~/.local/lib/python3.*/site-packages/meson*

rm -f ~/.local/bin/meson

pip3 uninstall meson

 

从源码安装最新版 meson

https://github.com/mesonbuild/meson/releases?page=7

![img](E:\Git\learning-record\MCU\Project\Document\assets\wps1.jpg) 

tar xzf meson-0.64.0.tar.gz 

cd meson-0.64.0

\# 安装

sudo python3 setup.py install

\# 验证

meson --version

 

![img](E:\Git\learning-record\MCU\Project\Document\assets\wps2.jpg) 

# **安装 gstreamer**

解压 gstreamer-1.20.0.zip

unzip gstreamer-1.20.0.zip

进入 目录

cd gstreamer-1.20.0/

# **设置编码**

export LANG=en_US.UTF-8

export LC_ALL=en_US.UTF-8

 

配置

meson setup build --prefix=/usr/local  -Dintrospection=disabled

编译， 时间比较长，下载很多的库文件

ninja -C build

安装 安装时间比较长。

sudo ninja -C build install

更新配置

sudo ldconfig

 

查看版本

gst-inspect-1.0 --version

 

 