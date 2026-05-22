## 问题记录

```
不使用多线程会发生：多个事件发生时候，会阻塞后面的事件。需要等待当前事件处理完成后，才能执行下一个事件。使用多线程，等于每个事件隔离，相互不干扰，就要考虑数据共享问题了。多线程，看起来是多个线程分别处理单独的事情，实际上还是多个CPU 来处理。用时间片切分的很细。每个线程执行一点，然后执行下个线程。看起来所有线程都在同时执行任务。也可以借助于 CPU 高速轮询的方式+状态机的方法。改变状态标志位来执行逻辑哦。

首先在 Device::Device 链接上设备后 m_server 为 真
然后 将信号和槽绑定connect(m_server, &Server::serverStarted, this, [this](bool success, const QString &deviceName, const QSize &size) 
拆包：Demuxer::run() -> 接收 Demuxer::recvPacket -> 推送 pushPacket(packet)  -> bool ok = parse(packet); -> processFrame(packet);
 emit getFrame(packet); 
 渲染 这一块缕的不是很清楚。
 Dialog::Dialog -> auto videoForm = new VideoForm -> VideoForm::updateRender -> m_videoWidget->updateTextures( -> update() 
```



## 答案记录

```
这个项目可以理解为：Qt 桌面 UI 壳 + QtScrcpyCore 静态库 + adb/scrcpy-server/ffmpeg 三方运行时。整体是事件驱动、信号槽串联的异步架构。
总体架构
```

![image-20260522175244883](E:\Git\learning-record\Prompt\Project\assets\image-20260522175244883.png)、

```

构建与模块
顶层可执行程序在 QtScrcpy/CMakeLists.txt (line 277) 创建，链接 Qt Widgets/Network/Multimedia/OpenGL 以及 QtScrcpyCore。
核心库在 QtScrcpyCore/CMakeLists.txt (line 132) 作为静态库构建。
core 内部按职责分成 adb、devicemanage、device/server、demuxer、decoder、controller、recorder、filehandler。
三方依赖随项目带上：adb、scrcpy-server、FFmpeg。构建后复制到部署目录，Windows 逻辑可见于 QtScrcpyCore/CMakeLists.txt (line 187)。
启动流程
main() 设置 adb、server、keymap、config 等环境路径，设置 OpenGL、日志、翻译和主题，见 main.cpp (line 26)。
通过 qsc::AdbProcess::setAdbPath() 设置 adb 路径，见 main.cpp (line 132)。
创建主窗口 Dialog，见 main.cpp (line 134)。
Dialog 监听 IDeviceManage::deviceConnected，连接成功后创建投屏窗口，见 dialog.cpp (line 126)。
连接设备主流程
用户点击“启动服务”，Dialog::on_startServerBtn_clicked() 从 UI 和配置组装 DeviceParams：serial、码率、分辨率、录制、reverse、server 路径、按键脚本等，见 dialog.cpp (line 331)。
调用 IDeviceManage::connectDevice(params)。
DeviceManage 检查 serial、重复连接、数量上限，然后创建 Device，见 devicemanage.cpp (line 35)。
Device 构造时创建这些子组件：
Server：负责 adb 和 socket 建链。
Demuxer：读取视频流 packet。
Decoder：H.264 解码到 YUV。
Controller：把键鼠转换成 Android 控制消息。
FileHandler：拖文件、装 APK。
Recorder：可选录屏。

Device::connectDevice() 把 DeviceParams 转成 ServerParams 并启动 server，见 device.cpp (line 263)。
Server 建链流程
Server 是整个项目最关键的状态机，流程在 server.cpp (line 476)：
adb push：把本地 scrcpy-server 推到 /data/local/tmp/scrcpy-server.jar。
建 tunnel：优先 adb reverse localabstract:scrcpy_xxx tcp:27183。
reverse 失败则退到 adb forward tcp:27183 localabstract:scrcpy_xxx。

adb shell CLASSPATH=... app_process / com.genymobile.scrcpy.Server ... 启动 Android 端 server，参数包括码率、max_size、max_fps、方向锁定、codec option 等。
建两个 socket：video socket：接收设备名、尺寸、H.264 视频流。
control socket：发送键鼠/剪贴板/电源等控制消息。

成功后发 serverStarted(true, deviceName, size)，Device 再初始化解码、录制和视频流。
视频流流程
Android 端 server 输出 H.264 packet。
Server 拿到 video socket 后交给 Demuxer。
Demuxer::run() 在线程中循环读 12 字节 packet header，再读 packet body，处理 config packet/key frame/PTS，见 demuxer.cpp (line 117)。
Demuxer 发 getFrame(AVPacket*)。
Device::initSignals() 把 packet 同时送到：Decoder::push() 实时解码，见 decoder.cpp (line 60)。
Recorder::push() 可选写录屏，见 recorder.cpp (line 243)。

Decoder 用 VideoBuffer 做解码帧和渲染帧交换，尽量丢过期帧降低延迟。
VideoForm::onFrame() 接收 Y/U/V 三平面数据，见 videoform.cpp (line 539)。
QYUVOpenGLWidget::updateTextures() 更新三张 YUV 纹理，fragment shader 转 RGB 后绘制，见 qyuvopenglwidget.cpp (line 132)。
控制流流程
VideoForm 捕获鼠标、滚轮、键盘事件，例如 videoform.cpp (line 559)。
事件转给对应 Device。
Device 调 Controller。
InputConvertNormal 把 Qt 坐标按显示尺寸映射到真实 Android 帧尺寸，并生成 ControlMsg，见 inputconvertnormal.cpp (line 11)。
Controller::event() 收到自定义 ControlMsg 后序列化，通过 control socket 写给 Android server，见 controller.cpp (line 224)。
游戏映射模式走 InputConvertGame，读取 json keymap，把键盘映射为多点触控、拖拽、视角移动等。
UI 与多设备
Dialog::onDeviceConnected() 成功后创建一个 VideoForm，注册为 DeviceObserver，见 dialog.cpp (line 499)。
每个设备对应一个 Device 和一个投屏窗口。
ToolForm 是悬浮工具条：主页、返回、菜单、音量、截图、开关屏、剪贴板、群控等。
GroupController 本身也是 DeviceObserver。主控窗口的输入会广播到非 host 设备，见 groupcontroller.cpp (line 82)。
一句话总结
QtScrcpy 的核心链路是：Dialog 收集参数 → DeviceManage 创建 Device → Server 用 adb 启 Android scrcpy-server 并建双 socket → Demuxer/Decoder/OpenGL 显示画面 → VideoForm/Controller 把本机输入序列化回 Android。架构边界挺清楚，Device 是中枢，Server 负责建链，Demuxer/Decoder 负责视频，Controller/InputConvert 负责控制。
```

