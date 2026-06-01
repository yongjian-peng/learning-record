## 掌握技能点

```
网络编程

HTTP通信：使用QNetworkAccessManager发送请求，解析JSON/XML数据。
TCP/UDP通信：掌握QTcpSocket、QUdpSocket的用法，实现客户端与服务器通信。
3. 多线程与并发编程

QThread与QRunnable：创建线程，实现任务分发。
线程同步：使用QMutex、QWaitCondition等工具避免资源竞争。
```





## 检测工具推荐

```
当前开发阶段首选：GammaRay + Heob + Qt Creator QML Profiler。

后期 UI 稳定后，再引入 Squish 做自动化回归测试。

原因很简单：

Squish 适合“功能流程/UI 自动化测试”，例如登录、配方编辑、开始测试、曲线显示、报警、打印、数据保存等完整操作流程。它是专业 GUI 自动化测试工具，支持 Qt Widgets、QML、Qt Quick、Qt WebKit、Qt WebEngine 等应用。
GammaRay 适合“开发阶段排查 QML、QObject、信号槽、Model/View、Qt Quick 场景图、属性绑定”等问题，相当于 Qt 程序的运行时透视工具。
Heob 适合 Windows 下排查内存泄漏、缓冲区越界、崩溃相关问题，并且 Qt Creator 已经提供 Analyze > Heob 集成入口。
QML Profiler 适合排查 QML 卡顿、界面掉帧、JavaScript 执行过多、界面响应慢等性能问题。
```



## 开源项目推荐

```
项目	推荐指数	适合
CuteHMI	⭐⭐⭐⭐⭐	真工业 SCADA
QSkinny	⭐⭐⭐⭐⭐	科技大屏
QSimpleScada	⭐⭐⭐⭐	传统工业监控
Qt-HMI-Display-UI	⭐⭐⭐⭐	仪表盘 UI 原型
Qt Dashboard Demo	⭐⭐⭐	学习 QML

https://www.msys2.org/ windows QT 工具下载
pacman -S mingw-w64-x86_64-pkg-config

Shotcut 开源项目
QT 6.10 配置
QT_QPA_PLATFORM_PLUGIN_PATH=D:\WorkSpace\QT-Github\shotcut-win64-sdk-26.4.30\Shotcut\plugins\platforms
QT_PLUGIN_PATH=D:\WorkSpace\QT-Github\shotcut-win64-sdk-26.4.30\Shotcut\plugins
Path=D:\Qt\6.10.0\mingw_64\bin;D:\Qt\Tools\mingw1310_64\bin;D:\WorkSpace\QT-Github\shotcut-win64-sdk-26.4.30\Shotcut\bin;C:\Program Files\jdk-17.0.18\bin;C:\Program Files\jdk-17.0.18\jre\bin;C:\Program Files\Common Files\Oracle\Java\javapath;D:\SoftWare\VMware Workstation\bin\;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Windows\System32\OpenSSH\;C:\Program Files\dotnet\;C:\Program Files\Git\cmd;C:\Program Files\MATLAB\R2024b\bin;C:\Program Files\apache-maven-3.9.15\bin;D:\Users\pyj50\AppData\Local\nvm;D:\nvm4w\nodejs;D:\msys64\mingw64\bin;C:\Users\pyj50\AppData\Local\Microsoft\WindowsApps;D:\SoftWare\Microsoft VS Code\bin;C:\MentorGraphics\PADSVX.2.4\SDD_HOME\CAMCAD;D:\Users\pyj50\AppData\Local\nvm;D:\nvm4w\nodejs

运行设置：
QT_QPA_PLATFORM_PLUGIN_PATH=D:\WorkSpace\QT-Github\shotcut-win64-sdk-26.4.30\Shotcut\plugins\platforms
QT_PLUGIN_PATH=D:\WorkSpace\QT-Github\shotcut-win64-sdk-26.4.30\Shotcut\plugins
Path=D:\Qt\6.10.0\mingw_64\bin;D:\Qt\Tools\mingw1310_64\bin;D:\WorkSpace\QT-Github\shotcut-win64-sdk-26.4.30\Shotcut\bin;C:\Program Files\jdk-17.0.18\bin;C:\Program Files\jdk-17.0.18\jre\bin;C:\Program Files\Common Files\Oracle\Java\javapath;D:\SoftWare\VMware Workstation\bin\;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Windows\System32\OpenSSH\;C:\Program Files\dotnet\;C:\Program Files\Git\cmd;C:\Program Files\MATLAB\R2024b\bin;C:\Program Files\apache-maven-3.9.15\bin;D:\Users\pyj50\AppData\Local\nvm;D:\nvm4w\nodejs;D:\msys64\mingw64\bin;C:\Users\pyj50\AppData\Local\Microsoft\WindowsApps;D:\SoftWare\Microsoft VS Code\bin;C:\MentorGraphics\PADSVX.2.4\SDD_HOME\CAMCAD;D:\Users\pyj50\AppData\Local\nvm;D:\nvm4w\nodejs

QT 6.10 插件
MinGw 13.1.0 64-bit Qt Chars QT Multimedia Qt Shader Tools Qt Websockets
Qt 6.6.2
MinGw 11.2.0 64-bit Qt Charts Qt Positioning Qt WebChannel Qt WebEngine
CMake 3.30.5 

信号（signal）：当一个对象的状态发生变化时，它可以“发射”一个信号，告知外界“我发生了某件事”。信号只负责广播，不负责执行具体动作。

    槽（slot）：是一个普通的函数（或 Lambda），当连接到某个信号后，信号发射时，槽函数会被自动调用。

    连接（connect）：使用 QObject::connect() 将信号和槽绑定在一起。

比喻：

    信号 = 门铃按钮；槽 = 开门动作。

    你把门铃按钮连接到开门动作（connect），那么你按下按钮（emit 信号）时，门就会自动打开（槽被调用）

缺少 QtScrcpy/CMakeLists.txt 文件解决办法。    
 git clone --recursive https://github.com/barry-ran/QtScrcpy.git
 git submodule update --init --recursive
 
 libgcc_s_seh-1.dll windows 依赖库
 
```





## 执行命令

```
/home/yxw/myproject/rk356x_linux5.1/buildroot/output/rockchip_rk3568/host/bin/qmake test.pro 
make

在屏幕上创建应用
	图标显示的方式：配置文件加入图标以及执行程序
	
cat /etc/xdg/weston/weston.ini.d/06-mpv-player.ini

```





## 编译配置

```
BR2_PACKAGE_QT5IMAGEFORMATS=y

BR2_PACKAGE_QT5BASE_SQL=y
#BR2_PACKAGE_QT5BASE_SQLITE_NONE is not set
BR2_PACKAGE_QT5BASE_SQLITE_SYSTEM=y

BR2_PACKAGE_QT5BASE_PNG=y
BR2_PACKAGE_QT5BASE_EGLFS=y

BR2_PACKAGE_OPENJPEG=y
BR2_PACKAGE_QT5BASE_JPEG=y
BR2_PACKAGE_LIBJPEG=y
BR2_PACKAGE_QT5BASE_MYSQL=y
BR2_PACKAGE_QT5BASE_SQLITE_QT=y
BR2_PACKAGE_QT5BASE_GIF=y
BR2_PACKAGE_QT5BASE_SYSLOG=y

```



## 编译报错

```
2025-11-10T19:46:07 ERROR: Feature 'system-pcre2' was enabled, but the pre-condition 'libs.pcre2' failed.
2025-11-10T19:46:07 make[1]: *** [package/pkg-generic.mk:306: /home/yxw/myproject/rk356x_linux5.1/buildroot/output/rockchip_rk3568/build/qt5base-e44097b63d17ba3178a637df7fac51ddc51cb48b/.stamp_configured] Error 3
2025-11-10T19:46:07 make: *** [Makefile:114: _all] Error 2
2025-11-10T19:46:07 make: Leaving directory '/home/yxw/myproject/rk356x_linux5.1/buildroot'
Please check details in /home/yxw/myproject/rk356x_linux5.1/output/sessions/2025-11-10_19-38-49/br-rockchip_rk3568_2025-11-10_19-38-50.log
ERROR: Running /home/yxw/myproject/rk356x_linux5.1/device/rockchip/common/build-hooks/30-rootfs.sh - build_buildroot failed!
ERROR: exit code 1 from line 16:
    "$RK_SCRIPTS_DIR/mk-buildroot.sh" $RK_BUILDROOT_CFG "$IMAGE_DIR"
ERROR: call stack:
    30-rootfs.sh: build_buildroot(16)
    30-rootfs.sh: build_hook(262)
    build-helper: try_func(63)
    build-helper: try_hook(95)
    build-helper: source(175)
    30-rootfs.sh: main(286)
ERROR: Running /home/yxw/myproject/rk356x_linux5.1/device/rockchip/common/build-hooks/30-rootfs.sh - try_func build_hook buildroot failed!
ERROR: exit code 1 from line 67:
    build_hook
ERROR: call stack:
    build-helper: try_func(67)
    build-helper: try_hook(95)
    build-helper: source(175)
    30-rootfs.sh: main(286)
ERROR: Running ./build.sh - run_hooks build buildroot failed!
ERROR: exit code 1 from line 256:
    /home/yxw/myproject/rk356x_linux5.1/device/rockchip/common/build-hooks/30-rootfs.sh build buildroot
ERROR: call stack:
    build.sh: run_hooks(256)
    build.sh: run_build_hooks(275)
    build.sh: main(644)
    build.sh: main(655)
ERROR: Running ./build.sh - run_build_hooks build buildroot failed!
ERROR: exit code 1 from line 278:
    build
ERROR: call stack:
    build.sh: run_build_hooks(278)
    build.sh: main(644)
    build.sh: main(655)
```

