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



![image-20250611205401042](D:\WorkSpace\Git\learning-record\MCU\assets\image-20250611205401042.png)

#### 1.协调器广播包：允许设备入网

![image-20250612110158655](D:\WorkSpace\Git\learning-record\MCU\assets\image-20250612110158655.png)

#### 2.Beacon Request

触发设备进入配网状态，此时设备会在指定信道依次发送`Beacon Request`。考虑效率和与 WiFi 信道干扰问题，一般会先选择优先信道（如 11,15,20,25,26）进行扫网

![image-20250612110743243](D:\WorkSpace\Git\learning-record\MCU\assets\image-20250612110743243.png)

#### 3.Beacon

在收到`Beacon Request`后，同一信道的不同 PAN 的协调器会发出`Beacon`回应，

报文中包含 PAN ID、是否允许入网、节点深度、是否接入当前设备类型等参数。

![image-20250612111401312](D:\WorkSpace\Git\learning-record\MCU\assets\image-20250612111401312.png)

#### 4.Association Request

待入网设备收到后会筛选出合适的网络，并对其发出`Association Request`请求入网。

报文中携带自身设备类型，长地址，设备能力等信息。

#### 6.Transport key: NWK key

![image-20250612112953984](D:\WorkSpace\Git\learning-record\MCU\assets\image-20250612112953984.png)



#### 7.Device Announce

子设备入网成功后会发出`Device Announce`广播，声明其已成为该网络新设备。

报文包含器长地址、短地址、设备能力等信息。

![image-20250612113231244](D:\WorkSpace\Git\learning-record\MCU\assets\image-20250612113231244.png)

#### 10.Node Descriptor Request

如果入网设备是一个符合`zigbee3.0`协议的设备，需要向信任中心发起`Node Descriptor Request`，查询信任中心是否支持`zigbee3.0`。

![image-20250612114117265](D:\WorkSpace\Git\learning-record\MCU\assets\image-20250612114117265.png)

#### 11.Node Descriptor Response

信任中心回复`Node Descriptor Response`报文包含：状态码、节点类型、设备能力等信息。协调器是否支持`zigbee3.0`可以通过`Server Mask Capabilities`字段的`Bit[9-15]:Stack Compliance Revision`获取。具体可以参考联盟最新的`Zigbee Specification`。

![image-20250612115351725](D:\WorkSpace\Git\learning-record\MCU\assets\image-20250612115351725.png)

#### 12.ZCL标识

![image-20250612115336121](D:\WorkSpace\Git\learning-record\MCU\assets\image-20250612115336121.png)



ZCL 是 zigbee 3.0 的核心

包类型

​	Route: Beacon Request 

​	关联请求：Data Request 

​	同步秘钥: Transport Key 

​	广播包：Device Announce 应用层

如果学过面向对象编程的思想，可以理解为：

- 应用层协议（Profile）是面向对象编程中的类；
- 簇（Cluster）是面向对象编程中的对象；
- 命令（Command）是每个对象中的方法；
- 状态/属性（Attribute）是每个对象的属性；

#### zigbee 加解密

1. 相关涉及到的key NWK Key  
2. Link Key 