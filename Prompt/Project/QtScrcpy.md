## 问题记录

```
不使用多线程会发生：多个事件发生时候，会阻塞后面的事件。需要等待当前事件处理完成后，才能执行下一个事件。使用多线程，等于每个事件隔离，相互不干扰，就要考虑数据共享问题了。多线程，看起来是多个线程分别处理单独的事情，实际上还是多个CPU 来处理。用时间片切分的很细。每个线程执行一点，然后执行下个线程。看起来所有线程都在同时执行任务。也可以借助于 CPU 高速轮询的方式+状态机的方法。改变状态标志位来执行逻辑哦。

首先在 Device::Device 链接上设备后 m_server 为 真
然后 将信号和槽绑定connect(m_server, &Server::serverStarted, this, [this](bool success, const QString &deviceName, const QSize &size) 
拆包：Demuxer::run() -> 接收 Demuxer::recvPacket -> 推送 pushPacket(packet)  -> bool ok = parse(packet); -> processFrame(packet);
 emit getFrame(packet); 
 渲染 VideoForm::updateRender -> m_videoWidget->updateTextures( -> update() 这一块缕的不是很清楚。
 Dialog::Dialog -> auto videoForm = new VideoForm
```



## 答案记录

```
```

