# **Ubuntu 安装 QT 5.12**

### **注意：**

系统属性是 5.15.0-139-generic  20.04.1-Ubuntu 

给到安装包，是从官网下载的。当前教程，依赖当前的安装包，后期有新版本发布的情况下，有安装的和编译的参数的差异，具体报错信息，具体解决，提供安装编译的思路。

新环境配置

```
sudo apt update
sudo apt install openssh-server -y
sudo systemctl status ssh
apt-get install terminator

# 如果未运行，启动SSH服务
sudo systemctl start ssh

# 设置开机自启
sudo systemctl enable ssh


# 配置Wayland禁用（如果是VMware）
sudo sed -i 's/#WaylandEnable=false/WaylandEnable=false/' /etc/gdm3/custom.conf

sudo apt install open-vm-tools-desktop

sudo apt install open-vm-tools open-vm-tools-desktop -y

# 检查服务状态
systemctl status open-vm-tools

# 重启服务
sudo systemctl restart open-vm-tools

```

# 1.**环境搭建**

编译安装 qt-online-installer-linux-x64-4.8.1.run 

下载安装包，清华源 地址不可访问，将提供下载好的安装包。

## 1.1. **执行指令，安装依赖**

sudo apt install gcc g++ make cmake build-essential libgl1-mesa-dev

![img](./assets/wps1.jpg) 

sudo apt install libxcb-cursor0 libxcb-cursor-dev

![img](./assets/wps2.jpg) 

sudo apt install gstreamer1.0-plugins-bad gstreamer1.0-libav gstreamer1.0-gl -y

![img](./assets/wps3.jpg) 

## 1.2. **给到执行权限**

ls qt-online-installer-linux-x64-4.8.1.run

chmod +x qt-online-installer-linux-x64-4.8.1.run

sudo ./qt-online-installer-linux-x64-4.8.1.run

![img](./assets/wps4.jpg) 

## 1.3. **联网安装QT**

### 1.3.1.**登录**

输入账号和密码，如果没有账号和密码需要到 QT 官网注册一个新的账号 [地址](https://login.qt.io/register) 

![img](./assets/wps5.jpg) 

### 1.3.2.**开源义务**

![img](./assets/wps6.jpg) 

### 1.3.3.**欢迎**

![img](./assets/wps7.jpg) 

### 1.3.4.**为QT 开发做贡献**

![img](./assets/wps8.jpg) 

### 1.3.5.**安装文件夹**

![img](./assets/wps9.jpg) 

### 1.3.6.**选择组件**

![img](./assets/wps10.jpg) 

筛选 QT5.15 

![img](./assets/wps11.jpg) 

### 1.3.7.**安装组件**

![img](./assets/wps12.jpg) 

 

不要勾选 Qt Creator 19.0.0 Ubuntu20.4 版本不兼容。需要更高版本的 glibc

![img](./assets/wps13.jpg) 

### 1.3.8.**协议许可**

![img](./assets/wps14.jpg) 

### 1.3.9.**准备安装**

![img](./assets/wps15.jpg) 

### 1.3.10.**已完成**

![img](./assets/wps16.jpg) 

# 2.**安装 qt-everywhere-src-5.15.2**

这个安装包的作用是 交叉编译 arm64 架构的可执行文件

## 2.1. **下载 安装包** [地址](https://download.qt.io/archive/qt/5.15/5.15.2/single/) 

![img](./assets/wps17.jpg) 

## 2.2. **解压安装包**

![img](./assets/wps18.jpg) 

![img](./assets/wps19.jpg) 

## 2.3. **安装依赖指令**

安装  sudo apt install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu

![img](./assets/wps20.jpg) 

检查版本信息  aarch64-linux-gnu-g++ -v

![img](./assets/wps21.jpg) 

## 2.4. **编译** 

./configure -release -opensource -confirm-license -xplatform linux-aarch64-gnu-g++ -prefix /opt/qt-5.15.2-arm64 -nomake examples -no-opengl -silent -qpa linuxfb  -skip qtlocation

![img](./assets/wps22.jpg) 

![img](./assets/wps23.jpg) 

make -j$(nproc) // 编译比较慢，根据机器性能。1个小时左右。

![img](./assets/wps24.jpg) 

make install

![img](./assets/wps25.jpg) 

# 3.**安装指定版本 qt-creator**

这个是 QT 编辑器的图形化界面。是QT 的组件。

文件qt-creator-opensource-linux-x86_64-18.0.2.run 复制到系统中，并给到执行权限

sudo chmod +x qt-creator-opensource-linux-x86_64-18.0.2.run

sudo ./qt-creator-opensource-linux-x86_64-18.0.2.run

![img](./assets/wps26.jpg) 

## 3.1. **欢迎**

![img](./assets/wps27.jpg) 

## 3.2. **登录**

![img](./assets/wps28.jpg) 

## 3.3. **安装文件夹**

![img](./assets/wps29.jpg) 

## 3.4. **自定义**

![img](./assets/wps30.jpg) 

## 3.5. **协议许可**

![img](./assets/wps31.jpg) 

## 3.6. **安装总结**

![img](./assets/wps32.jpg) 

## 3.7. **正在安装**

![img](./assets/wps33.jpg) 

## 3.8. **已完成**

![img](./assets/wps34.jpg) 

# 4.**搭建 DEMO**

将搭建 arm64 和 x86 两个架构的 demo 包含编译器的选择。以及运行demo, 生成可执行文件。

## 4.1. **将 demo 文件 拷贝到系统中**

cutecom-qt5-master ![img](./assets/wps35.jpg)

## 4.2. **打开 qtcreator /opt/qtcreator-18.0.2/bin 使用 root 用户**

![img](./assets/wps36.jpg) 

## 4.3. **打开项目**

将 cutecom.pro 文件 选择打开

![img](./assets/wps37.jpg) 

![img](./assets/wps38.jpg) 

弹框，点击确定

![img](./assets/wps39.jpg) 

## 4.4. **添加 QT 版本 点击项目** 

![img](./assets/wps40.jpg) 

## 4.5. **点击 管理构建套件 添加 qmake 的路径** 

编译时候的 x86 和 arm64 架构。

### 4.5.1.**添加 x86 架构 QT 版本 qmake 在 QT 安装目录**

![img](./assets/wps41.jpg) 

![img](./assets/wps42.jpg) 

点击应用

![img](./assets/wps43.jpg) 

### 4.5.2.**添加 arm64 架构 qmake QT 版本**

 

![img](./assets/wps44.jpg) 

点击应用 arm64 的 qmake

![img](./assets/wps45.jpg) 

 

## 4.6. **管理构建套件-> 添加**

![img](./assets/wps46.jpg) 

 

### 4.6.1.**添加套件 gcc_64  Kit**

![img](./assets/wps47.jpg) 

### 4.6.2.**添加套件 kit arm64**

![img](./assets/wps48.jpg) 

### 4.6.3.**添加后的效果，勾选上 Debug Release Profile 版本 点击 Configure Project** 

![img](./assets/wps49.jpg) 

### 4.6.4.**选择编译器和版本，并构建**

arm64 是 arm 架构 gcc_64 是 x86 架构的

![img](./assets/wps50.jpg) 

### 4.6.5.**编译构建效果**

![img](./assets/wps51.jpg) 

### 4.6.6.**编译 x86 架构时候。报错信息。**

如果遇到了报错的话，qt.qpa.plugin: Could not load the Qt platform plugin "xcb" in "" even though it was found.  则 需要安装依赖 

![img](./assets/wps52.jpg) 

解决问题的指令如下：

sudo apt install \

libxcb-xinerama0 \

libxcb-xinerama0-dev \

libxkbcommon-x11-0 \

libxcb-icccm4 \

libxcb-image0 \

libxcb-keysyms1 \

libxcb-render-util0 \

libxcb-xfixes0 \

libxcb-shape0 \

libxcb-randr0 \

libxcb-sync1 \

libxcb-xkb1 \

libxcb-util1 \

libxrender1 \

libxi6

生成可执行的文件，在这些目录中。依自己实际的目录。

![img](./assets/wps53.jpg) 

# 5.**安装** **linuxdeployqt 打包所有 Qt 依赖**

解决的是 编译的 x86 架构中 可执行文件，拷贝到目标机器时候，缺少依赖项问题。

将 linuxdeployqt-continuous-x86_64.AppImage 拷贝系统中。

安装包是从 [github ](https://github.com/probonopd/linuxdeployqt/releases)下载的，也可以使用 文档提供的软件

## 5.1. **查看 linuxdeployqt-continuous-x86_64.AppImage**

ls -al linuxdeployqt-continuous-x86_64.AppImage

![img](./assets/wps54.jpg) 

## 5.2. **重命名**

mv linuxdeployqt-continuous-x86_64.AppImage linuxdeployqt

chmod +x linuxdeployqt

mv linuxdeployqt /usr/local/bin

/usr/local/bin/linuxdelpoyqt --version

![img](./assets/wps55.jpg) 

显示版本 /usr/local/bin/linuxdeployqt --version

![img](./assets/wps56.jpg) 

## 5.3. **配置QT 环境变量**

终端输入 vim ~/.bashrc 在文件末尾追加以下内容，其中/opt/Qt/5.15.2/是我Qt的安装路径，大家要用自己的路径代替；

export PATH=/opt/Qt/5.15.2/gcc_64/bin:$PATH

export LD_LIBRARY_PATH=/opt/Qt/5.15.2/gcc_64/lib:$LD_LIBRARY_PATH

export QT_PLUGIN_PATH=/opt/Qt/5.15.2/gcc_64/plugins:$QT_PLUGIN_PATH

export QML2_IMPORT_PATH=/opt/Qt/5.15.2/gcc_64/qml:$QML2_IMPORT_PATH

![img](./assets/wps57.jpg) 

 

## 5.4. **然后更新环境变量**

​	source ~/.bashrc

## 5.5. **打包应用程序**

### 5.5.1.**查看文件架构**

将Qt项目用Release模式编译，进入到目录中，查看文件架构 file cutecom 确认架构是x86 架构

![img](./assets/wps58.jpg) 

### 5.5.2.**创建一个文件夹**

名称cutecomapp，然后将cutecom可执行文件复制到此文件夹下；名称自取

mkdir cutecomapp

![img](./assets/wps59.jpg) 

### 5.5.3.**使用 linuxdeployqt 进行打包**

一定要加上-appimage选项，命令如下：

/usr/local/bin/linuxdeployqt cutecom -appimage

![img](./assets/wps60.jpg) 

执行上面命令出现提示信息，不用管，只是创建了一个需要你自己编辑的desktop文件；到这一步，就把可执行文件的动态库等依赖文件都复制到该文件夹中了

可以将编译后的可执行文件，拷贝到对应的架构中的，验证 Demo 即可

# 6.**验证过程**

## 6.1. **将编译好的 build 可执行文件 拷贝到 对应架构中 arm64**

![img](./assets/wps61.jpg) 

![img](./assets/wps62.jpg) 

## 6.2. **将编译好的 build 可执行文件 拷贝到 对应架构中 x86** 

![img](./assets/wps63.jpg) 

 

![img](./assets/wps64.jpg) 

 

## 6.3. **链接 使用 485 协议 通信**

![img](./assets/wps65.jpg) 

 

## 6.4. **发送指令，接收指令。通信成功。**

![img](./assets/wps66.jpg) 

![img](./assets/wps67.jpg) 

​            