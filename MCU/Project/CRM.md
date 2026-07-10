# 配置说明

```
ENABLE_INFLUX=true、INFLUX_BUCKET=metrics 没问题。
Agent payloadVersion=v2、emitLegacyFlatMetrics=false 没问题，后端负责展平写入。

sensors.gpu.amd.collector  adlx
amd.collector=libreHardwareMonitor
AMD=LHM
QT_QPA_PLATFORM=offscreen

推荐 方案 A：Python API 内置调度器 + MySQL 任务表 + 复用现有命令下发。
不用 Redis/Celery，不新增服务；在 FastAPI 启动时跑一个后台 loop，扫描到期任务，复用现有 performance.stop 和 performance.close。
方案 B：外部队列/定时器服务。可靠但太重，当前 200 台规模没必要。
方案 C：让 Agent 自己按时关闭。后端不可统一审计、重试和失败落库，不适合这次需求。

# /***************************************
# 函数名 ：xxx
# 函数功能 ：一句话说明业务目的
# 函数参数 ：
# 参数名：说明该参数的业务含义、用途、约束及是否可为空。不要业务入参文字，给到具体的业务说明。
# 函数返回值 ：说明成功、未找到、异常或无效输入时的返回语义。
# 函数描述 ：
# 说明函数从哪里获取或检查数据；
# 说明采用哪些条件、顺序或规则进行处理；
# 说明命中、未命中及异常情况下的行为；
# 说明是否产生状态修改或其他副作用。
# 注意事项 ：说明调用方需要关注的边界条件；无法确认的内容标记为“需确认”。
# ***************************************/

如果修改影响到了编译发布的逻辑，修改时注意不要漏掉之前的逻辑。
- 新增函数：100% 必须有完整注释
- 修改函数：若原注释不完整，必须同步完善
- 新增类：100% 必须有类说明
- 新增文件：100% 必须有文件头说明
- 新增 API：100% 必须有接口文档
- 新增数据库表：100% 必须说明字段含义
- 新增配置项：100% 必须说明用途、默认值、取值范围
- 文件建议<=500行；函数建议<=80行。
- 超 600 行业务文件拆分后的模块职责。
- 全项目统一命名、日志、异常、类型注解、Docstring、Import 顺序。
- 中文注释，使用utf-8 中文编码格式。无乱码。
将这次修改同步到 docs CRM 整体梳理 .md 文件中。
def xxx(...):

业务入参

每次编写函数，都需要加上对应的注释。核心代码加上注释。


P0：采集 pipeline 多窗口合并写 InfluxDB。
P1：dispatch 阶段批量插入 CommandRecord、批量更新 TestTaskDevice，但保持 MQTT 逐台节流。
P2：批次广播降频。
P3：_persist_start_batch() 的 add_all() 小优化。
```

# 待完成的需求：

```

3. 后端页面 tree.js 3D 动态图展示。
16. 将日志文件，每天固定时间给打包成一个文件。并压缩文件。以 tar.gz 格式压缩。
17. 删除安装包时候，将配置文件和log 也一起删除掉。
7. 后端需要设置，管理开启软件的功能，能够选择，需要从数据库中获取，然后再发送开启或者停止指令。
23. 将监控指标，做成可视化。
26. 使用 LibreHardwareMonitor Bridge 将硬盘的温度采集到。读取 HDD / SSD / NVMe 温度， 要重新做一套硬盘温度采集，直接扩展现有的. LibreHardwareMonitor Bridge。 就是在 crm_lhm_bridge.dll 里面增加 Storage / HDD / SSD / NVMe 的枚举，把硬盘温度返回给 Agent Core。
新增硬盘温度的指标和数据结构，兼容之前 CPU 温度格式。保存到 influxdb 数据库中。并在 qt_qml_client 实时信息 和 web_admin_vue 列表详情，实时信息 显示出来。在历史数据中，温度显示这一项，也新增硬盘温度 Echarts。采集频率参考 CPU 温度频率即可。保证系统稳定运行。
27. 当python_api 服务不可用，或者 mqtt 不可用，或者 influxdb 不可用的时候，则做出止损的动作，应该怎么设计比较合理。如果python_api 不可用的话，则当前客户端在执行的任务。怎么处理。 api 已不可用的话。api 是关键，很多功能都需要提供。怎么解决比较好。01_qt_qml_client 02_agent_core_cpp 部署客户端 03_backend_python_api 04_web_admin_vue 部署在服务器
31. 任务设置优先级 客户端心跳包，保存配置，保存采集数据。
40. 后端单个测试，和批量测试的话，记录历史记录表。记录状态，老化时间。就和主机信息中的状态更新的复制版。就是历史数据。将 task 任务显示出来后端显示。
43. 再后端扫描设备ID时候，获取到了设备ID后，不自动关闭，需要点击确认后，然后发送同步请求后，然后再关闭。防止扫描错误，可以停在页面修改设备ID.
44. qt_qml_client 设置页面修改 保存的 brunintest 软件时候，兼容 反斜杠，和 /  要不程序识别不到软件的路径。



11. 将项目打包成服务，安装到 windows 注册表中，简单部署。并在测试机上运行。 OK
10. cpu测试 内存测试 网络测试 磁盘测试 当压测时候，能保证采集到数据。配置压测到 90% 如果有 100% 的情况，则停止采集数据。 OK 待验证
9. 将执行命令的操作，统一使用 trace_id 当做主键，记录生命周期的日志，后期排查问题使用。 OK 发布规则，和启动关闭软件
1. 监控指标的范围报警设置，通过数据库获取，并使用数据库配置。并验证测试。OK 测试内存到达后，关闭软件。
4. 后端页面 UI 重新设计一版。 OK 
5. QT 页面数据使用第一个页面数据，其他的给删除掉。 OK 
6. 发现了问题，发送开启指令不生效了。需要修复。 OK
8. 后端 echarts 图，可以选查看据图的那个指标。显示和隐藏。 OK
19. qml_client 页面中， 头部的图标，打印，剪切板，帮助，去掉，刷新按钮，点击实现这个功能。刷新基本数据，设置页面中的，如果已经填写过了 配置信息的话，不用每次打开都填写这些任务。底部的 CPU 使用率 和 内存使用， 性能状态，可以删除掉。左侧的 Task Control 隐藏。 OK
13. 历史曲线中的 报表曲线图数据不完整，修复。主机信息页，详情中，删除多余的样式，包含左侧的概览，系统信息等。底部的提示运行时间，CPU温度。修复 Agent 状态。OK
18. 主机信息中 设备在线状态，和老化状态，这两个不是实时的。设计一个方案，当在线时候，或者离线时候，老化状态变更了。即使更新状态。OK
14. qt 链接 backend_python_api 服务失败的时候，应该保持一定频率的请求连接。确认服务是否可用。并更新状态。OK
2. 后端管理系统，权限用户页面逻辑。OK
12. influxdb 数据怎么优化，保证服务正常使用。OK 
21. 优化，显示的采集到的数据，使用保留两位小数，计算的百分比也是保留两位。扫描整个项目。从数据采集到保存到 influxdb 数据库。我理解采集处理好了，就不需要处理显示了 OK
22. 主机信息中的详情，不用弹框方式，改为打开新的页面。 OK
20. 历史曲线页面的宽度没有自适应，目前比较宽，搜索条件，和列表都修复正常显示。 OK
28. 设计一个方案，扫码枪方案。 OK 
34. 设计一个方案。将 qt_qml_client 程序。启动时候，新增一个逻辑。判断当前日志和文件写入的日期的差值，当大于半年后。则开启后，提示请激活工具。然后关闭程序。保存日期的文件。是在安装的本地目录中。这个文件需要手动更新。不用自动更新。文件日期需要使用 RSA 非对称加密。使用私钥加密，程序使用公钥解密后，才能获取到日期，使用 OPENSSL_PKCS1_PADDING 填充个标志 然后对比。这个方法放在入口函数 main 中。创建一个单独的文件。执行这个业务需求。公钥和私钥E:\WorkSpace\CRM\generate_rsa_key_pair 文件中。将公钥写在配置文件中。将公钥使用 base64 转成字符串。私钥待加密的格式是 json {"timestamp":1796267012000} 是13位时间戳。获取本机的时间。对比如果小于时间则弹出提示，并关闭软件，当有网络的时候，获取网络的时间，不获取本机的时间戳。如果没有网络的时候则使用本机的时间。 OK 
36. Qt 配置注册设备 ID，条件是 device_node 存在，并且 device_config 中存在非空 clientType 配置痕迹，则不请求自动上报采集数据。 OK
35. 新增部署脚本，将 01_qt_qml_client\generate_rsa_key_pair\activation.lic 文件，编译时候，放入到 build\CRMStressStation\client 文件夹中，保证不提示 请激活工具。请激活工具页面设计一款。 OK 
32. 后端写入采集数据时候，批量整合后，聚合再一次写入多次采集数据。根据实际情况，来优化。先梳理整个同步异步的流程。确认后，再分析哪里可以优化成批量写入数据。OK 
29. 设计一个客户端应用程序，需要过期机制，每半年执行一次日期验证升级到最新的日期。如果到期后，则提示授权。 OK 
34. 后端环境部署，使用 windows 还是 linux 部署 docker OK
33. 批量操作设备时候，更新数据库，也是将这个批次的数据。一次写入多条数据。先梳理整个同步异步的流程。确认后，再分析哪里可以优化成批量写入数据。OK 
30. 设计方案，编写流程架构图。OK 
25. 使用 windows 环境 部署 python 和 vue 项目。不需要
24. 可以控制压力测试的时间。兼容批量设置时间。保证服务正常运行。运行一个脚本文件，监听任务是否到期，到期的话，则执行关闭压力测试的动作。OK
15. agent_core 记录的日志，转成时间格式。格式化一下。OK
42. 目前文件日志文件分散到 C 盘目录中，怎么修改将日志文件和配置文件，保存到拷贝文件的同级目录中。怎么修改。 
目前生成的最新的 CRMStressPlatform.cmd 文件，双击，打不开软件。
再 qt_qml_client 设置 点击了确定，最后，写入配置文件到本地目录的时候，写入的 emqx 修复这个问题，应该和 linux 环境有关系。 "mqtt": {
        "host": "emqx",
        "password": "public123456",
        "port": 1883,
        "username": "admin"
    }
修复这个三个问题，先编写计划，再修复问题。 OK 
37. 读取 scripts 目录的 安装 01 和 02 环境的脚本。计划将分三步进行。注意配置文件的位置，修改后，验证是否正确。
	1. 将 01 和 02 项目 编译到一个文件夹中，并且将 BurnInTest （E:\Program Files\BurnInTest）也放入到这个目录中。相关配置文件，相关验证时间的文件。
	2. 开启软件的话，直接点击软件图标。就开启 qt 和 agent 两个软件，退出的话，则是退出  qt 和 agent 两个软件。
	3. 生成绿色版本的配置，直接就是将绿色版的文件夹，压缩后，直接复制到另外的设备上，就傻瓜式直接运行即可。OK
41. 设计一个方案。后端新增设备组，就是批次号表，设备表新增批次号字段。还是记录到device_config 表中。在 qt_client 界面 将 位置改成批次号。进入设置页面先获取批次号的列表。提供可以选择的下拉框，下拉框可以搜索。将批次号作为一个统计的单位。按照批次号。统计设备的数量，在 admin/home 区域设备分布图形展示。
修改的地方，牵扯到整个项目。先编写计划，确认后，再编写代码实现。OK 
38. qt_qml_client logo 图标重新修改。OK 
39. 后端首页优化布局。设计一个首页。OK 

流程：
开启客户端软件 -> 填写 服务器 IP 地址 -> 链接 推送到 python_api 数据 -> 保存数据。
采集数据 -> 发布到 MQTT 服务  python_api 订阅 数据  保存到 influxdb 中 
客户端 请求 -> python_api 获取实时数据，和历史数据。反馈到 qt_qml 中显示。
python_api 开启软件指令 到执行指令 到执行动作，到反馈结果的流程图。
vue_admin 开启软件指令 到开启软件的流程。
有关所有 mqtt 订阅和发布的流程图。

但现有仓库可能已经有一部分报警执行骨架。我先确认哪些能力已存在、哪些只是占位，避免重复建表或破坏当前压力控制链路。而不是重写一套重复实现


修复多台设备数据采集和显示数据换乱问题。采集的数据覆盖，显示错误的采集数据。

如果修改影响到了启动性能测试软件，报警规则发布规则，01_qt_qml_client 的日志记录，不能修改原有的逻辑。
如果修改影响到了编译发布的逻辑，不能修改原有的逻辑。
如果修改影响到了编译发布的逻辑，不能修改原有的逻辑。

目前的客户端量是 200台设备同时测试并在服务器上监控 部署 01_qt_qml_client 和02_agent_core_cpp  服务器是单独的机器 部署 03_backend_python_api和04_web_admin_vue 是这么计划的。评估 websocker mqtt  client 和 数据 influxdb 保存数据 7天的风险。有哪些？先评估，做计划，然后再编写代码

28. 目前已经有了 扫码枪，扫码枪扫描每台设备出货时候的唯一条形码。来作为设备的 设备的ID, 去掉 01_qt_qml_client 设置功能的 设备ID 能够通过扫码枪来获取。
获取的基本信息按钮功能，新增当开启软件时候，弹出设置功能时候。执行一次。并将获取的基本信息填写到输入框中，触发条件为 弹出设置框就触发。
链路是 QML 设置弹窗 -> Qt ViewModel -> HTTP 注册到 python_api -> MySQL 保存设备 -> python_api 返回 MQTT 配置 -> Qt 通过本地 WebSocket 同步给 agent -> Qt/agent 写本地共享配置文件 
设置当点击了确认后，在 Mysql 保存设备后。则新增一个标识。后端04_web_admin_vue 当获取到这个标识后, 则弹出详情页面。弹出这个页面的话，可以做成同步的。效果是qt 发送请求后，后端页面就打开。数据是从数据库中获取的。
这个弹出详情框，走异步的功能吧。要不流程太长了。和保存配置交集在一起。可以设计一个方案。来监听这个标识，有标识的话，则弹出详情信息页面。需要设计数据库的话，则新增数据库，记录这次的任务。设备是唯一的。唯一标识即扫描到的设备ID.

功能是获取到的基本信息详情页面，对应的详情信息，有 API 主机，API 端口，设备ID, 设备名称， 位置。 BurnInTest 输入框，其中这些信息都显示。鼠标自动进入到 设备ID 这一项。
这个时候，扫码枪会自动扫码，将设备ID 同步到输入框中。
当获取到了 设备ID 输入框的时候，则进入新的流程。python_api websocket 推送01_qt_qml_client，推送的iP是 设备ip. 设备ID 配置 -> Qt 通过本地 WebSocket 同步给 agent -> Qt/agent 更新本地共享配置文件，注意：python_api websocket 推送01_qt_qml_client 设备ID 配置 后，01_qt_qml_client判断是否打开设置页面，如果打开的话，则更新设备ID，到输入框中。如果没有打开设置页面，则继续走下面流程即可。

 QML 设置弹窗 -> Qt ViewModel -> HTTP 注册到 python_api -> MySQL 保存设备 -> python_api 返回 MQTT 配置 -> Qt 通过本地 WebSocket 同步给 agent -> Qt/agent 写本地共享配置文件 这个链路中间新增了一个扫码获取设备ID 的动作。并同步到qt和agent 两个服务。原因是服务器连接扫码枪，代测试的设别有很多。就是将每个设备的ID 通过先获取到设备的 IP,然后将 设备 ID 不用手动输入到设备中。因为设备和扫码枪没有连接，当服务器作为代理传递设备ID。
 
 这次需求代码不要和之前的放到一个文件中，新开启一个文件，能复用之前代码则复用，单独做这个需求。这个项目都是的。

```





# 压力测试说明

```
总体实现架构:核心原则
推荐的 CPU / GPU / 内存 / 硬盘实现方法 windows 方法 
推荐 Agent Core 内部模块设计 1. StressManager 负责统一调度。2. Windows 外部进程控制。
推荐， 统一指令设计。启动 CPU 压力，停止 CPU 压力， 启动 GPU 压力，停止，启动内存压力， 停止。启动硬盘压力， 停止。
推荐，运行时长和报警值设计，运行时长， 报警配置， 初始默认阈值建议，并刚给到 SQL 表数据，从数据库中获取配置。
推荐，Python Backend API 设计。
推荐，Qt/QML 页面建议。Qt/QML 不直接运行 FurMark / AIDA64 / CPU-Z / AS SSD。它只调用 Backend API。
工具选择建议： 使用命令方式，使用DiskSpd。
最终开发顺序，按照推荐顺序

V2 升级版：
CPU：C++ 内置压力引擎
GPU：Direct3D / Vulkan / OpenCL 内置压力引擎
内存：C++ 内置内存压力引擎
硬盘：C++ 内置文件 I/O 压力引擎
报警：温度 / 使用率 / 剩余空间
控制：start / stop / duration / status
推送：stress_status_updated / stress_alarm / telemetry_updated
```

# CPU / GPU / 内存 / 硬盘实现方法

| 类型     | Windows 首版实现                                             | Linux 兼容方案                               | Android 兼容方案                    |
| -------- | ------------------------------------------------------------ | -------------------------------------------- | ----------------------------------- |
| CPU 压力 | C++ 内置线程压力，支持整数/FPU/AVX 循环；可选 AIDA64 `/SST CPU,FPU,Cache` | `stress-ng --cpu` 或 C++ 内置压力            | JNI / Native C++ 线程压力           |
| GPU 压力 | FurMark 外部进程；后续可做 Direct3D / Vulkan / OpenCL 内置压力 | Vulkan / OpenGL / glmark2 / 自研 shader loop | OpenGL ES / Vulkan offscreen render |
| 内存压力 | C++ 内置申请内存、写入、校验、释放；可选 AIDA64 RAM          | `stress-ng --vm` 或 C++ 内置                 | JNI / Native C++ 分配内存块         |
| 硬盘压力 | DiskSpd 或 fio；不要优先用 AS SSD GUI                        | fio                                          | fio 移植版或 Native 文件读写压力    |

# InfluxDB 数据操作命令

```
$InfluxUrl = "http://192.168.31.224:8086"
$Token = "P1pKah75yVRjDNvo5QxEsKu8R3FET0vgPoxjzzJdcWdFtoV_Wbe2dfIQNeHnS348abkkJro8dr3F3Am0Vgr76w=="
$Org = "crm"
$Bucket = "metrics"

$Body = @{
    start = "1970-01-01T00:00:00Z"
    stop = "2035-01-01T00:00:00Z"
    predicate = '_measurement="host_realtime_metric"'
} | ConvertTo-Json

Invoke-RestMethod `
    -Method Post `
    -Uri "$InfluxUrl/api/v2/delete?org=$Org&bucket=$Bucket" `
    -Headers @{ Authorization = "Token $Token" } `
    -ContentType "application/json" `
    -Body $Body
```



# 链路调用

```
下面按真实数据链路解释：

01_qt_qml_client  ──HTTP/WS──> 03_backend_python_api ──写入──> InfluxDB
04_web_admin_vue  ──HTTP/WS──> 03_backend_python_api ──订阅──> EMQX/MQTT
02_agent_core_cpp ──MQTT─────> EMQX/MQTT Broker ──────> 03_backend_python_api

也就是：

Agent Core 采集硬件数据
        ↓ MQTT 发布
EMQX / MQ
        ↓ Backend 订阅
FastAPI 后端
        ↓ 写入
InfluxDB
        ↓ REST / WebSocket
Qt/QML 客户端、Vue 管理端显示

将整个日志生成一个 类似于 唯一的 trace_id 并将整个链路串起来。设计一个最小改动的方法。
后端发布规则 更新到报警规则后，怎么保证执行成功，并且有迹可查日志。方便定位问题。
开启测试软件，初始化测试软件，关闭测试软件 怎么保证执行成功，并有迹可查日志。方便定位问题。

后端规则发布 /api/alarm/rules/publish
```







# 问题记录 - `01_qt_qml_client`

```
发送指令时候，开启和关闭失效，缺少 task_id
```



# 问题记录-  02_agent_core_cpp

# 问题记录 - 03_backend_python_api



# 问题记录 - 04_web_admin_vue

```
```





# 启动命令

```
更新
cd E:\WorkSpace\CRM

# 查看帮助
.\scripts\dev.ps1 -Action help

.\scripts\dev.ps1 -Action agent-stop
.\scripts\dev.ps1 -Action start -Target agent

# 编译 Agent 并测试
.\scripts\dev.ps1 -Action build -Target agent -RunTests

# 管理员权限单次采集
.\scripts\dev.ps1 -Action agent-once

# 生成完整部署包
.\scripts\dev.ps1 -Action agent-package -RunTests

# 更新 LocalSystem 服务及配置
.\scripts\dev.ps1 -Action agent-service -ForceConfig

# 查看服务和 PawnIO 状态
.\scripts\dev.ps1 -Action agent-status

# 编译并启动全部项目
.\scripts\dev.ps1 -Action dev -Target all


# 搜索日志命令 启动或者关闭命令
Select-String -Path "03_backend_python_api\logs\*.log","02_agent_core_cpp\logs\*.log","C:\ProgramData\CRMStressPlatform\logs\*.log" -Pattern "cmd-xxxx"
$tid = "cmd-b6e601ba8fc1"
Select-String -Path `
  "E:\WorkSpace\CRM\03_backend_python_api\logs\backend.log", `
  "C:\ProgramData\CRMStressPlatform\logs\aging-agent.log" `
  -Pattern "$tid|COMMAND_|PERFORMANCE_STATUS_|MQTT_ACK_RECEIVED|AGENT_COMMAND_ACK_SEND"


# 搜索日志命令 报警发布规则命令
 Select-String -Path `
  "03_backend_python_api\logs\backend.log", `
  "C:\ProgramData\CRMStressPlatform\logs\aging-agent.log" `
  -Pattern "ALARM_RULES_"
  
$tid = "alarm-rules-1781677869185"
Select-String -Path `
  "03_backend_python_api\logs\backend.log", `
  "C:\ProgramData\CRMStressPlatform\logs\aging-agent.log" `
  -Pattern $tid


- `01_qt_qml_client`
cd E:\WorkSpace\CRM\01_qt_qml_client
$env:PATH='E:\Qt\Qt5.12\Tools\mingw1310_64\bin;E:\Qt\Qt5.12\6.10.2\mingw_64\bin;C:\ninja;' + $env:PATH
& 'E:\Qt\Qt5.12\Tools\CMake_64\bin\cmake.exe' -S . -B build -G Ninja -DCMAKE_PREFIX_PATH='E:\Qt\Qt5.12\6.10.2\mingw_64'
& 'E:\Qt\Qt5.12\Tools\CMake_64\bin\cmake.exe' --build build

.\build\aging_qt_client.exe


- `02_agent_core_cpp`
cd E:\WorkSpace\CRM\02_agent_core_cpp
$env:PATH='E:\Qt\Qt5.12\Tools\mingw1310_64\bin;C:\ninja;' + $env:PATH
cmake -S . -B build -G Ninja
cmake --build build
ctest --test-dir build --output-on-failure

.\build\aging-agent.exe --console --config .\config\agent_config.json
.\build\aging-agent.exe --once --config .\config\agent_config.json


- `03_backend_python_api`
powershell
cd E:\WorkSpace\CRM\03_backend_python_api
python -m venv .venv
.\.venv\Scripts\Activate.ps1
pip install -r requirements.txt
Copy-Item .env.example .env
.\scripts\run_dev.ps1

- `04_web_admin_vue`
   
1. EMQX 常用端口区别
端口	用途	谁来访问
18083	EMQX 管理后台 Web 页面	浏览器访问
1883	MQTT TCP 连接	Agent Core、Python 后端
8083	MQTT over WebSocket	WebSocket MQTT 客户端
8084	MQTT over WebSocket SSL	加密 WebSocket
8883	MQTT SSL/TLS	加密 MQTT

 [ValidateSet("init", "build", "start", "dev", "agent-once", "clean")]
    [string]$Action = "dev",

    [ValidateSet("all", "qt", "agent", "backend")]
    [string]$Target = "all",


第一次初始化
cd E:\WorkSpace\CRM
powershell -ExecutionPolicy Bypass -File .\scripts\dev.ps1 -Action init
1. 创建 Python .venv
2. 安装 requirements.txt
3. 复制 .env.example 到 .env
4. 配置并编译 Qt/QML 客户端
5. 配置并编译 Agent Core

一条命令编译并启动三个服务
powershell -ExecutionPolicy Bypass -File .\scripts\dev.ps1 -Action dev
1. 增量编译 01_qt_qml_client
2. 增量编译 02_agent_core_cpp
3. 检查 03_backend_python_api 的 Python 环境
4. 分别打开 3 个 PowerShell 窗口启动服务


只启动，不重新编译
powershell -ExecutionPolicy Bypass -File .\scripts\dev.ps1 -Action start

只编译 Qt 客户端
powershell -ExecutionPolicy Bypass -File .\scripts\dev.ps1 -Action build -Target qt

只编译 Agent Core
powershell -ExecutionPolicy Bypass -File .\scripts\dev.ps1 -Action build -Target agent

只启动后端
powershell -ExecutionPolicy Bypass -File .\scripts\dev.ps1 -Action start -Target backend

只启动 Agent
powershell -ExecutionPolicy Bypass -File .\scripts\dev.ps1 -Action start -Target agent

清理重新编译
powershell -ExecutionPolicy Bypass -File .\scripts\dev.ps1 -Action clean
powershell -ExecutionPolicy Bypass -File .\scripts\dev.ps1 -Action init



推荐直接执行这组命令，删除

$INFLUX_URL = "http://192.168.31.224:8086"
$ORG_ID = "90ba40944f480108"
$TOKEN = "P1pKah75yVRjDNvo5QxEsKu8R3FET0vgPoxjzzJdcWdFtoV_Wbe2dfIQNeHnS348abkkJro8dr3F3Am0Vgr76w=="

$headers = @{
  Authorization = "Token $TOKEN"
}

# 删除 metrics
curl.exe -i -X DELETE "$INFLUX_URL/api/v2/buckets/e021fd02c7df156b" `
  -H "Authorization: Token $TOKEN" `
  --max-time 15

# 删除 metrics-001
curl.exe -i -X DELETE "$INFLUX_URL/api/v2/buckets/b14ac64caa776d0a" `
  -H "Authorization: Token $TOKEN" `
  --max-time 15

# 重新查看
Invoke-RestMethod `
  -Method Get `
  -Uri "$INFLUX_URL/api/v2/buckets?orgID=$ORG_ID" `
  -Headers $headers | ConvertTo-Json -Depth 10
  
  
  
  
  001 启动命令
  - `01_qt_qml_client`
cd E:\WorkSpace\CRM001\01_qt_qml_client
$env:PATH='E:\Qt\Qt5.12\Tools\mingw1310_64\bin;E:\Qt\Qt5.12\6.10.2\mingw_64\bin;C:\ninja;' + $env:PATH
& 'E:\Qt\Qt5.12\Tools\CMake_64\bin\cmake.exe' -S . -B build -G Ninja -DCMAKE_PREFIX_PATH='E:\Qt\Qt5.12\6.10.2\mingw_64'
& 'E:\Qt\Qt5.12\Tools\CMake_64\bin\cmake.exe' --build build

.\build\aging_qt_client.exe


- `02_agent_core_cpp`
cd E:\WorkSpace\CRM001\02_agent_core_cpp
$env:PATH='E:\Qt\Qt5.12\Tools\mingw1310_64\bin;C:\ninja;' + $env:PATH
cmake -S . -B build -G Ninja
cmake --build build
ctest --test-dir build --output-on-failure

.\build\aging-agent.exe --console --config .\config\agent_config.json
.\build\aging-agent.exe --once --config .\config\agent_config.json


- `03_backend_python_api`
powershell
cd E:\WorkSpace\CRM001\03_backend_python_api
python -m venv .venv
.\.venv\Scripts\Activate.ps1
pip install -r requirements.txt
Copy-Item .env.example .env
.\scripts\run_dev.ps1


TDD 红灯：先写测试，构建因缺少 services/ActivationGuard.h 失败。
ctest --test-dir 01_qt_qml_client\build --output-on-failure：1/1 通过。
cmake --build 01_qt_qml_client\build --target aging_qt_client：通过。
package_qt_client.ps1 -SkipWindeployQt：通过，package 内已复制 libcrypto-3-x64.dll




```



# 服务器启动命令

```
启动 agent 
cd E:\WorkSpace\CRM\build\CRMStressStation\agent
.\aging-agent.exe --console --config .\config\agent_config.json


sudo mkdir -p /srv/crm
sudo chown -R boot:boot /srv/crm
tar -xzf /tmp/crm-ubuntu-deploy.tar.gz -C /srv/crm

cd /srv/crm/deploy/ubuntu
cp -n .env.example .env
chmod +x scripts/crm.sh
nano .env

./scripts/crm.sh config
./scripts/crm.sh up
./scripts/crm.sh ps
./scripts/crm.sh health


cd /srv/crm/deploy/ubuntu

set -a
. ./.env
set +a

docker exec -e INFLUX_TOKEN="$INFLUX_TOKEN" crm-influxdb-1 \
  influx bucket list \
  --host http://localhost:8086 \
  --org "$INFLUX_ORG" \
  --name "$INFLUX_BUCKET"

BUCKET_ID="这里填写真实Bucket ID"
docker exec -e INFLUX_TOKEN="$INFLUX_TOKEN" crm-influxdb-1 \
  influx bucket update \
  --host http://localhost:8086 \
  --id "$BUCKET_ID" \
  --retention 7d
  
```







```
Python API:  http://192.168.31.224:8000

WebSocket:  ws://192.168.31.224:8000/ws/realtime

MQTT:     192.168.31.224:1883

MySQL:    192.168.31.224:3306

InfluxDB:   http://192.168.31.224:8086

### InfluxDB #################################################
INFLUXDB_VERSION=2.7
INFLUXDB_HTTP_PORT=8086
INFLUXDB_INIT_MODE=setup
INFLUXDB_INIT_USERNAME=admin
INFLUXDB_INIT_PASSWORD=admin123456
INFLUXDB_INIT_ORG=crm
INFLUXDB_INIT_BUCKET=aging_test
INFLUXDB_INIT_ADMIN_TOKEN=crm-influxdb-admin-token-change-me
INFLUXDB_INIT_RETENTION=30d

### EMQX #####################################################
EMQX_VERSION=5.8.4
EMQX_MQTT_PORT=1883
EMQX_MQTT_SSL_PORT=8883
EMQX_WS_PORT=8083
EMQX_WSS_PORT=8084
EMQX_DASHBOARD_PORT=18083
EMQX_DASHBOARD_USERNAME=admin
EMQX_DASHBOARD_PASSWORD=public123456
EMQX_NODE_COOKIE=crm_emqx_cookie_change_me
```



# 仓库地址：

```
http://192.168.31.224:9008/crm/web-admin-vue.git

http://192.168.31.224:9008/crm/backend-python-api.git

http://192.168.31.224:9008/crm/agent-core-cpp.git

http://192.168.31.224:9008/crm/qt-qml-client.git


curl.exe "http://127.0.0.1:8000/api/telemetry/latest/你的deviceId"
curl.exe "http://127.0.0.1:8000/api/telemetry/history/你的deviceId?window=10s&metrics=cpu.usage_percent,memory.used_gb"

curl http://127.0.0.1:8000/api/telemetry/latest/<deviceId>
curl "http://127.0.0.1:8000/api/telemetry/history/<deviceId>?window=5s&metrics=cpu.usage_percent,cpu.temperature_celsius"
curl http://127.0.0.1:8000/api/devices/<deviceId>/basic-info
```

