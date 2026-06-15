# 待完成的需求：

```
1. 监控指标的范围报警设置，通过数据库获取，并使用数据库配置。并验证测试。
2. 后端管理系统，权限用户页面逻辑。
3. 后端页面 tree.js 3D 动态图展示。
4. 后端页面 UI 重新设计一版。
5. QT 页面数据使用第一个页面数据，其他的给删除掉。
6. 发现了问题，发送开启指令不生效了。需要修复。
7. 后端需要设置，管理开启软件的功能，能够选择，需要从数据库中获取，然后再发送开启或者停止指令。
8. 后端 echarts 图，可以选查看据图的那个指标。显示和隐藏。

流程：
开启客户端软件 -> 填写 服务器 IP 地址 -> 链接 推送到 python_api 数据 -> 保存数据。
采集数据 -> 发布到 MQTT 服务  python_api 订阅 数据  保存到 influxdb 中 
客户端 请求 -> python_api 获取实时数据，和历史数据。反馈到 qt_qml 中显示。
python_api 开启软件指令 到执行指令 到执行动作，到反馈结果的流程图。
vue_admin 开启软件指令 到开启软件的流程。
有关所有 mqtt 订阅和发布的流程图。
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
# 查看帮助
.\scripts\dev.ps1 -Action help

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

