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

1  PHY 物理层 实现频率选择、调制、发送/接收信号（基于 IEEE 802.15.4）

2  MAC 层  介质访问控制：信道访问、帧传输、重试等

3  NWK 网络层 路由选择、网络地址分配、拓扑控制

4  APS 支持层 数据封装、会话管理、安全处理

5  ZDO/ZDP 管理层 设备发现、绑定、网络加入、设备角色定义

6  应用对象（Application）  用户逻辑、Cluster 命令、设备行为定义（如灯泡开关）

ZDO: 它运行在 APS 层之上，辅助设备发现、加入、绑定等控制行为.在实际调试 Zigbee 网络时，ZDO 报文是诊断设备间连接逻辑的重要线索

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



#### zigbee Packet Information 命令

##### Packet Information Identify: Identify Query 是什么意思 命令： 0x01

**Identify** = “让我亮起来” → 设备开始闪灯或发出声响，用于人找设备。

**Identify Query** = “谁在亮？” → 控制器/中控发送这个命令，查询哪些设备正在执行 Identify 动作。

| 命令 | 命令 ID | 方向 | 描述 |
| ---- | ------- | ---- | ---- |
|      |         |      |      |

| **Identify** | `0x00` | C → S | 要求设备执行 Identify（闪灯、响铃） |
| ------------ | ------ | ----- | ----------------------------------- |
|              |        |       |                                     |

| **Identify Query** | `0x01` | C → S | 查询当前是否有设备在执行 Identify |
| ------------------ | ------ | ----- | --------------------------------- |
|                    |        |       |                                   |

| **Identify Query Response** | `0x00` | S → C | 回复正在 Identify 的剩余时间（单位：秒） |
| --------------------------- | ------ | ----- | ---------------------------------------- |
|                             |        |       |                                          |

##### Packet Information：Simple Descriptor Resquest Layer: ZDP 什么意思

​	Zigbee 报文中一种典型的 **ZDP（Zigbee Device Profile）层命令**，用于设备发现阶段

​	响应：Simple Descriptor Response  

​		输入 Cluster 列表（Input Clusters）输出 Cluster 列表（Output Clusters）

##### Packet Information：Beacon Request Layer: MAC 什么意思

​	这表示 Zigbee 协议栈中，在 **MAC 层（物理通信层）** 发送的一种特殊广播帧，用于发起 **网络扫描（network discovery）**，目的是请求周围 Zigbee 网络广播它们的 **Beacon（信标）信息**

​	Command ID `0x07`（Beacon Request）

​        Destination 广播地址（通常是 0xFFFF）

##### Packet Information：Data Request Layer: MAC 什么意思

​	这是在 Zigbee 抓包或协议分析中常见的一条 **MAC 层帧**，表示一个设备在 **请求父设备传输缓存的数据**

##### Packet Information：Link Status: 0x0 Layer: NWK 什么意思

​	**Link Status** 是 Zigbee 网络层的一种“路由控制帧”，**仅由具有路由能力的设备（Coordinator / Router）定期广播** 告诉邻居：“我还在线，而且这是我邻居列表的状态”



#### 通信中的key

Network Key:       网络统一密钥，用于所有节点 NWK 加密

Link Key:        特定设备对之间的点对点密钥，用于 APS 加密

Trust Center Link Key:  设备与 Trust Center 的专属密钥



#### 入网流程：

​	下面是一个 Zigbee 终端设备典型的入网顺序：

​	终端广播 Beacon Request；

​	协调器回复 Beacon（可能带有 NwkOpen）；

​	终端发送 Association Request；

​	协调器发送 Association Response（加密）；

​	若密钥正确，终端加入网络；

​	否则就会看到 Unable to decrypt Data。