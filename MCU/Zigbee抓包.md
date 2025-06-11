#### 使用工具

##### CC2531 USB 

直接插入到电脑的USB 接口中，有内置的天线的

![image-20250611200855708](D:\WorkSpace\Git\learning-record\MCU\assets\image-20250611200855708.png)

##### 下载 https://www.ti.com/tool/PACKET-SNIFFER 工具 使用老版本的 SmartRF™ Packet Sniffer 2.18.1 (zip) — 23001 K

会提示安装驱动的。一定要安装驱动。是 CC2531 USB dongle 的驱动 没有叹号的使用

![image-20250611201348872](D:\WorkSpace\Git\learning-record\MCU\assets\image-20250611201348872.png)

##### ![image-20250611201207255](D:\WorkSpace\Git\learning-record\MCU\assets\image-20250611201207255.png)

##### Ubiqua 分析工具

![image-20250611200823169](D:\WorkSpace\Git\learning-record\MCU\assets\image-20250611200823169.png)

![image-20250611201454670](D:\WorkSpace\Git\learning-record\MCU\assets\image-20250611201454670.png)

设置通信秘钥

![image-20250611201556595](D:\WorkSpace\Git\learning-record\MCU\assets\image-20250611201556595.png)



保存后的秘钥

![image-20250611201630105](D:\WorkSpace\Git\learning-record\MCU\assets\image-20250611201630105.png)



分析包内容：

![image-20250611204126008](D:\WorkSpace\Git\learning-record\MCU\assets\image-20250611204126008.png)



包类型

​	Route: Beacon Request 

​	关联请求：Data Request 

​	同步秘钥: Transport Key 

​	广播包：Device Announce 应用层