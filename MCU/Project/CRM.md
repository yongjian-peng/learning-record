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
```

