# CRM 硬件监控系统通信流程与代码调用链 Mermaid 总结

> 适用项目：
>
> - `01_qt_qml_client`：Qt/QML 本地客户端
> - `02_agent_core_cpp`：C++ Agent Core 采集与执行端
> - `03_backend_python_api`：Python FastAPI / MQTT / WebSocket / InfluxDB 服务
> - `04_web_admin_vue`：Vue 后台管理系统
>
> 说明：本文按当前 CRM 项目架构整理“执行流程 + 代码函数调用链 + Mermaid 图”。  
> 如果项目中实际函数名不同，Codex 实现时应以现有代码为准，按本文调用链补齐或对齐。

---

## 1. 总体通信链路

### 1.1 总体架构 Mermaid

```mermaid
flowchart LR
    subgraph QT["01_qt_qml_client Qt/QML 客户端"]
        QT_APP["main.cpp / QML App"]
        QT_CFG["服务器 IP 配置页"]
        QT_API["ApiClient HTTP"]
        QT_WS["WebSocketClient"]
        QT_UI["Dashboard / History Chart / Stress Page"]
    end

    subgraph VUE["04_web_admin_vue Vue 后台"]
        VUE_PAGE["设备监控 / 压力测试页面"]
        VUE_API["axios api modules"]
        VUE_WS["WebSocket service"]
    end

    subgraph API["03_backend_python_api Python API"]
        FASTAPI["FastAPI REST Routes"]
        WS_MANAGER["WebSocket ConnectionManager"]
        MQTT_SERVICE["MQTT Service"]
        INGEST["Telemetry Ingest Service"]
        COMMAND["Command Service"]
        INFLUX_REPO["InfluxDB Repository"]
        MYSQL_REPO["MySQL Repository"]
        CACHE["Latest Telemetry Cache"]
    end

    subgraph MQTT["MQTT Broker EMQX"]
        BROKER["EMQX :1883 / :8083"]
    end

    subgraph AGENT["02_agent_core_cpp Agent Core"]
        AGENT_APP["AgentApp"]
        COLLECTOR["Hardware Collector"]
        MQTT_CLIENT["MqttClient"]
        CMD_DISPATCH["CommandDispatcher"]
        EXECUTOR["Stress Executor / Process Launcher"]
    end

    QT_CFG --> QT_API
    QT_API --> FASTAPI
    QT_WS --> WS_MANAGER
    QT_UI --> QT_API

    VUE_PAGE --> VUE_API
    VUE_API --> FASTAPI
    VUE_WS --> WS_MANAGER

    FASTAPI --> MYSQL_REPO
    FASTAPI --> INFLUX_REPO
    FASTAPI --> COMMAND
    COMMAND --> MQTT_SERVICE
    MQTT_SERVICE --> BROKER

    AGENT_APP --> COLLECTOR
    COLLECTOR --> MQTT_CLIENT
    MQTT_CLIENT --> BROKER
    BROKER --> MQTT_SERVICE

    MQTT_SERVICE --> INGEST
    INGEST --> INFLUX_REPO
    INGEST --> CACHE
    INGEST --> WS_MANAGER

    BROKER --> MQTT_CLIENT
    MQTT_CLIENT --> CMD_DISPATCH
    CMD_DISPATCH --> EXECUTOR
    EXECUTOR --> MQTT_CLIENT

    WS_MANAGER --> QT_WS
    WS_MANAGER --> VUE_WS
```

---

## 2. 流程一：开启客户端软件 -> 填写服务器 IP -> 连接 Python API -> 推送客户端信息 -> 保存配置

### 2.1 执行目标

客户端首次启动或服务器地址变化时，需要：

1. Qt/QML 打开客户端软件。
2. 用户填写 Python API 服务器 IP 地址。
3. Qt 保存本地配置。
4. Qt 重新设置 HTTP Base URL 和 WebSocket URL。
5. Qt 连接 Python API。
6. Qt 将客户端信息或设备绑定信息推送到 Python API。
7. Python API 保存客户端/设备连接信息。
8. Python API 通过 WebSocket 返回连接成功状态。

### 2.2 Mermaid 流程图

```mermaid
sequenceDiagram
    autonumber
    participant User as 用户
    participant QML as Qt/QML ServerConfigPage.qml
    participant VM as ServerConfigViewModel
    participant Config as AppConfig / SettingsStore
    participant APIClient as ApiClient
    participant WS as WebSocketClient
    participant PyAPI as Python FastAPI
    participant MySQL as MySQL
    participant WSManager as WebSocketManager

    User->>QML: 打开客户端软件
    QML->>VM: Component.onCompleted()
    VM->>Config: loadServerConfig()
    Config-->>VM: serverIp / apiBaseUrl / wsUrl

    User->>QML: 输入服务器 IP 和端口
    QML->>VM: saveServerConfig(serverIp, apiPort, wsPath)
    VM->>Config: saveServerConfig(...)
    Config-->>VM: 保存成功

    VM->>APIClient: setBaseUrl(http://serverIp:8000)
    VM->>WS: setUrl(ws://serverIp:8000/ws/realtime)

    VM->>APIClient: ping()
    APIClient->>PyAPI: GET /api/health
    PyAPI-->>APIClient: 200 OK

    VM->>APIClient: registerClient(clientInfo)
    APIClient->>PyAPI: POST /api/clients/register
    PyAPI->>MySQL: ClientRepository.upsert_client()
    MySQL-->>PyAPI: saved

    VM->>WS: connectToServer()
    WS->>WSManager: WebSocket /ws/realtime?clientId=xxx&deviceId=xxx
    WSManager-->>WS: connected

    WSManager-->>WS: client_connected event
    WS-->>QML: 更新连接状态
```

### 2.3 代码函数调用链

#### 01_qt_qml_client

```text
main.cpp
  -> AppBootstrap::init()
  -> AppConfig::load()
  -> QQmlApplicationEngine::load(Main.qml)

Main.qml
  -> ServerConfigPage.qml
  -> ServerConfigViewModel::loadConfig()
  -> SettingsStore::read("server.ip", "server.port", "ws.path")

ServerConfigPage.qml 点击保存
  -> ServerConfigViewModel::saveServerConfig(serverIp, apiPort)
  -> SettingsStore::write(...)
  -> ApiClient::setBaseUrl("http://{serverIp}:{apiPort}")
  -> WebSocketClient::setUrl("ws://{serverIp}:{apiPort}/ws/realtime")
  -> ApiClient::get("/api/health")
  -> ApiClient::post("/api/clients/register", clientInfo)
  -> WebSocketClient::connect()
  -> DashboardViewModel::setConnected(true)
```

#### 03_backend_python_api

```text
main.py
  -> create_app()
  -> include_router(client_router)
  -> include_router(health_router)
  -> websocket_router

GET /api/health
  -> health_router.health_check()
  -> return {"status": "ok"}

POST /api/clients/register
  -> client_router.register_client()
  -> ClientRegisterSchema 校验参数
  -> ClientService.register_or_update_client()
  -> ClientRepository.upsert_client()
  -> MySQL 保存 client_id / device_id / ip / version / last_seen
  -> return client info

WebSocket /ws/realtime
  -> websocket_endpoint()
  -> ConnectionManager.connect(websocket, client_id, device_id)
  -> ConnectionManager.send_json("client_connected")
```

### 2.4 推荐接口

#### 健康检查

```http
GET /api/health
```

响应：

```json
{
  "status": "ok",
  "service": "crm-python-api",
  "mqtt": "connected",
  "influxdb": "ready"
}
```

#### 客户端注册

```http
POST /api/clients/register
Content-Type: application/json
```

请求参数：

```json
{
  "clientId": "qt-client-001",
  "deviceId": "device-crm",
  "clientName": "工控机本地客户端",
  "serverIp": "192.168.31.206",
  "clientIp": "192.168.31.100",
  "version": "1.0.0",
  "platform": "windows",
  "wsEvents": ["telemetry_update", "basic_info_updated", "stress_state_changed"]
}
```

---

## 3. 流程二：Agent Core 采集数据 -> 发布到 MQTT -> Python API 订阅 -> 保存到 InfluxDB

### 3.1 执行目标

1. Agent Core 定时采集硬件实时指标。
2. Agent Core 将实时指标封装成 JSON。
3. Agent Core 发布到 MQTT Broker。
4. Python API 订阅 MQTT Topic。
5. Python API 解析消息并写入 InfluxDB。
6. Python API 更新最新缓存。
7. Python API 通过 WebSocket 推送到 Qt/QML 和 Vue。

### 3.2 Mermaid 流程图

```mermaid
sequenceDiagram
    autonumber
    participant AgentApp as AgentApp
    participant Scheduler as TelemetryScheduler
    participant Collector as HardwareCollector
    participant Builder as MetricPayloadBuilder
    participant MqttClient as Agent MqttClient
    participant EMQX as EMQX MQTT Broker
    participant PyMqtt as Python MQTTService
    participant Router as TelemetryMessageRouter
    participant Ingest as TelemetryIngestService
    participant Influx as InfluxTelemetryRepository
    participant Cache as LatestTelemetryCache
    participant WS as WebSocketManager
    participant Qt as Qt/QML Client
    participant Vue as Vue Admin

    AgentApp->>Scheduler: start(interval=1s/2s)
    loop 每 1-2 秒
        Scheduler->>Collector: collectRealtimeMetrics()
        Collector-->>Scheduler: RealtimeMetric
        Scheduler->>Builder: buildRealtimeMetricV2(deviceId, metrics)
        Builder-->>Scheduler: JSON payload
        Scheduler->>MqttClient: publish("agent/{deviceId}/telemetry", payload)
        MqttClient->>EMQX: MQTT PUBLISH
        EMQX->>PyMqtt: MQTT MESSAGE
        PyMqtt->>Router: route(topic, payload)
        Router->>Ingest: handleRealtimeMetricV2(payload)
        Ingest->>Influx: writeRealtimeMetrics(deviceId, metrics, timestamp)
        Ingest->>Cache: updateLatest(deviceId, payload)
        Ingest->>WS: broadcast("telemetry_update", payload)
        WS-->>Qt: telemetry_update
        WS-->>Vue: telemetry_update
    end
```

### 3.3 代码函数调用链

#### 02_agent_core_cpp

```text
main.cpp
  -> AgentApp::run()
  -> ConfigManager::load("agent_config.json")
  -> MqttClient::connect(host, port, username, password)
  -> MqttClient::subscribe("agent/{deviceId}/cmd")
  -> TelemetryScheduler::start(intervalMs)

TelemetryScheduler::tick()
  -> HardwareCollector::collectRealtime()
      -> CpuCollector::collect()
      -> MemoryCollector::collect()
      -> DiskCollector::collect()
      -> NetworkCollector::collect()
      -> GpuCollector::collect()
      -> SensorCollector::collect()
  -> MetricPayloadBuilder::buildRealtimeMetricV2()
  -> MqttClient::publish("agent/{deviceId}/telemetry", json)
```

#### 03_backend_python_api

```text
main.py startup
  -> MqttService.start()
  -> mqtt_client.connect(MQTT_HOST, MQTT_PORT)
  -> mqtt_client.subscribe("agent/+/telemetry")
  -> mqtt_client.subscribe("agent/+/basic-info")
  -> mqtt_client.subscribe("agent/+/event")
  -> mqtt_client.loop_start()

MQTT on_message
  -> MqttService.on_message(client, userdata, msg)
  -> MqttTopicParser.parse(msg.topic)
  -> TelemetryMessageRouter.route(topic, payload)
  -> TelemetryIngestService.handle_realtime_metric_v2(payload)
  -> InfluxTelemetryRepository.write_realtime_metrics(payload)
  -> LatestTelemetryCache.update(device_id, payload)
  -> WebSocketManager.broadcast_to_device(device_id, "telemetry_update", payload)
```

### 3.4 MQTT Topic 与 Payload

#### Agent 发布实时指标

```text
Topic: agent/{deviceId}/telemetry
Direction: Agent Core -> EMQX -> Python API
QoS: 0 或 1
```

Payload：

```json
{
  "dataType": "realtime_metric_v2",
  "messageId": "metric-1781333677601",
  "deviceId": "device-crm",
  "deviceName": "device-crm",
  "timestamp": 1781333677601,
  "metrics": {
    "cpu": {
      "name": "AMD Ryzen 5 5600GT",
      "usage_percent": 35.6,
      "temperature_celsius": 62.0,
      "power_watt": null
    },
    "memory": {
      "total_gb": 31.8,
      "used_gb": 13.2,
      "usage_percent": 41.5
    },
    "disks": {
      "count": 2,
      "physical": []
    },
    "gpus": {
      "gpu0": {
        "name": "AMD Radeon Graphics",
        "usage_percent": null,
        "temperature_celsius": null,
        "power_watt": null
      }
    }
  },
  "issues": []
}
```

---

## 4. 流程三：客户端请求实时数据和历史数据 -> Python API 查询 -> Qt/QML 显示

### 4.1 执行目标

Qt/QML 客户端显示数据有两种来源：

1. WebSocket 实时推送：适合仪表盘实时刷新。
2. REST 主动查询：适合页面进入时拉取最新数据、历史曲线、补数据。

### 4.2 Mermaid 流程图

```mermaid
sequenceDiagram
    autonumber
    participant QML as DashboardPage.qml / HistoryPage.qml
    participant VM as TelemetryViewModel / HistoryViewModel
    participant API as Qt ApiClient
    participant PyRoute as Python telemetry_router
    participant Service as TelemetryQueryService
    participant Cache as LatestTelemetryCache
    participant Influx as InfluxTelemetryRepository
    participant Chart as QML ChartView

    QML->>VM: 页面加载 / 手动刷新
    VM->>API: getLatest(deviceId)
    API->>PyRoute: GET /api/telemetry/latest/{deviceId}
    PyRoute->>Service: get_latest(deviceId)
    Service->>Cache: get(deviceId)
    alt 缓存存在
        Cache-->>Service: latest payload
    else 缓存不存在
        Service->>Influx: query_latest(deviceId)
        Influx-->>Service: latest payload
    end
    Service-->>PyRoute: latest telemetry
    PyRoute-->>API: JSON
    API-->>VM: telemetry model
    VM-->>QML: update properties

    QML->>VM: 选择时间范围 / 指标
    VM->>API: getHistory(deviceId, metric, start, end, window)
    API->>PyRoute: GET /api/telemetry/history
    PyRoute->>Service: query_history(...)
    Service->>Influx: query_range(...)
    Influx-->>Service: time series points
    Service-->>PyRoute: history data
    PyRoute-->>API: JSON
    API-->>VM: chart series
    VM-->>Chart: setSeries(points)
```

### 4.3 Qt/QML 代码调用链

```text
DashboardPage.qml
  -> Component.onCompleted()
  -> TelemetryViewModel::loadLatest(deviceId)
  -> ApiClient::get("/api/telemetry/latest/{deviceId}")
  -> TelemetryMapper::fromJson(response)
  -> TelemetryViewModel::setCpuUsage(...)
  -> QML Binding 自动刷新仪表盘

WebSocketClient::onTextMessageReceived(message)
  -> WsMessageRouter::route(message)
  -> TelemetryViewModel::onTelemetryUpdate(payload)
  -> DashboardPage.qml 刷新实时 UI

HistoryPage.qml
  -> onQueryClicked()
  -> HistoryTelemetryViewModel::loadHistory(deviceId, metric, startTime, endTime)
  -> ApiClient::get("/api/telemetry/history?deviceId=...&metric=...&start=...&end=...")
  -> HistoryTelemetryMapper::toSeries(response)
  -> ChartSeriesModel::replace(points)
  -> QML ChartView / Canvas / QtCharts 渲染曲线
```

### 4.4 Python API 代码调用链

```text
GET /api/telemetry/latest/{device_id}
  -> telemetry_router.get_latest_telemetry(device_id)
  -> TelemetryQueryService.get_latest(device_id)
  -> LatestTelemetryCache.get(device_id)
  -> 如果缓存为空：InfluxTelemetryRepository.query_latest(device_id)
  -> TelemetryResponseMapper.to_latest_response()
  -> return JSON

GET /api/telemetry/history
  -> telemetry_router.get_history(device_id, metric, start, end, window)
  -> TelemetryQueryService.query_history(...)
  -> InfluxTelemetryRepository.query_range(...)
  -> FluxQueryBuilder.build_history_query(...)
  -> InfluxDBClient.query_api().query()
  -> TelemetryResponseMapper.to_history_response()
  -> return JSON
```

### 4.5 推荐接口

#### 获取实时最新数据

```http
GET /api/telemetry/latest/{deviceId}
```

示例：

```http
GET /api/telemetry/latest/device-crm
```

#### 获取历史曲线数据

```http
GET /api/telemetry/history?deviceId=device-crm&metric=cpu.usage_percent&start=2026-06-13T00:00:00Z&end=2026-06-13T23:59:59Z&window=10s
```

响应：

```json
{
  "deviceId": "device-crm",
  "metric": "cpu.usage_percent",
  "start": "2026-06-13T00:00:00Z",
  "end": "2026-06-13T23:59:59Z",
  "points": [
    {
      "time": "2026-06-13T10:00:00Z",
      "value": 36.5
    }
  ]
}
```

---

## 5. 流程四：Python API 开启软件指令 -> 执行指令 -> 执行动作 -> 反馈结果

### 5.1 执行目标

1. Qt/QML 或 Vue 调用 Python API 的开启压力软件接口。
2. Python API 生成 commandId。
3. Python API 保存命令记录到 MySQL。
4. Python API 发布 MQTT 指令到 `agent/{deviceId}/cmd`。
5. Agent Core 订阅指令。
6. Agent Core 分发到 CommandDispatcher。
7. 执行器启动软件或自研压力引擎。
8. Agent Core 发布执行状态事件。
9. Python API 订阅事件，更新命令状态。
10. Python API 通过 WebSocket 推送结果给 Qt/QML 和 Vue。

### 5.2 Mermaid 流程图

```mermaid
sequenceDiagram
    autonumber
    participant Client as Qt/QML 或 Vue
    participant PyRoute as Python stress_router
    participant CmdService as CommandService
    participant MySQL as MySQL CommandRepository
    participant PyMqtt as Python MQTTService
    participant EMQX as EMQX
    participant AgentMqtt as Agent MqttClient
    participant Dispatcher as CommandDispatcher
    participant Handler as StressCommandHandler
    participant Executor as StressExecutor / ProcessLauncher
    participant EventHandler as Python AgentEventHandler
    participant WS as WebSocketManager

    Client->>PyRoute: POST /api/stress/start
    PyRoute->>CmdService: start_stress(deviceId, software, modules, duration)
    CmdService->>CmdService: generate commandId
    CmdService->>MySQL: save_command(status=PENDING)
    CmdService->>PyMqtt: publish_command("agent/{deviceId}/cmd", payload)
    PyMqtt->>EMQX: MQTT PUBLISH

    EMQX->>AgentMqtt: MESSAGE agent/{deviceId}/cmd
    AgentMqtt->>Dispatcher: dispatch(command)
    Dispatcher->>Handler: handleStartStress(command)
    Handler->>Executor: start(software, modules, args)
    Executor-->>Handler: process started / engine started
    Handler->>AgentMqtt: publish event RUNNING
    AgentMqtt->>EMQX: PUBLISH agent/{deviceId}/event

    EMQX->>EventHandler: MESSAGE agent/{deviceId}/event
    EventHandler->>MySQL: update_command(status=RUNNING)
    EventHandler->>WS: broadcast("stress_state_changed")
    WS-->>Client: stress_state_changed
```

### 5.3 Python API 函数调用链

```text
POST /api/stress/start
  -> stress_router.start_stress(request)
  -> StressStartRequest 校验参数
  -> CommandService.create_start_stress_command()
      -> command_id = CommandIdGenerator.generate()
      -> CommandRepository.insert(command_id, device_id, command_type, status="PENDING")
      -> MqttCommandPayloadBuilder.build_start_stress_payload()
      -> MqttService.publish("agent/{deviceId}/cmd", payload)
  -> return {"commandId": "...", "status": "PENDING"}

MQTT 收到 Agent 事件
  -> MqttService.on_message()
  -> AgentEventRouter.route()
  -> AgentEventHandler.handle_stress_state_changed()
  -> CommandRepository.update_status(command_id, status)
  -> StressStateRepository.upsert(device_id, state)
  -> WebSocketManager.broadcast_to_device(device_id, "stress_state_changed", event)
```

### 5.4 Agent Core 函数调用链

```text
MqttClient::onMessage("agent/{deviceId}/cmd", payload)
  -> CommandParser::parse(payload)
  -> CommandDispatcher::dispatch(command)

CommandDispatcher::dispatch()
  -> if command.type == "stress.start"
       StressCommandHandler::handleStart(command)
  -> if command.type == "stress.stop"
       StressCommandHandler::handleStop(command)

StressCommandHandler::handleStart(command)
  -> CommandStateMachine::transition(PENDING -> RECEIVED)
  -> AgentEventPublisher::publishCommandAck(commandId, RECEIVED)
  -> StressExecutorFactory::create(command.software)
  -> StressExecutor::start(command.modules, command.durationSec, command.args)
      -> BuiltinCpuStressEngine::start()
      -> BuiltinGpuStressEngine::start()
      -> BuiltinMemoryStressEngine::start()
      -> BuiltinDiskStressEngine::start()
      或
      -> ExternalProcessLauncher::start("AIDA64.exe" / "BurnInTest.exe")
  -> CommandStateMachine::transition(RECEIVED -> RUNNING)
  -> AgentEventPublisher::publishStressState(commandId, RUNNING)

StressCommandHandler::handleStop(command)
  -> StressExecutor::stop()
  -> ProcessLauncher::terminate()
  -> CommandStateMachine::transition(RUNNING -> STOPPED)
  -> AgentEventPublisher::publishStressState(commandId, STOPPED)
```

### 5.5 推荐开启压力测试接口

```http
POST /api/stress/start
Content-Type: application/json
```

请求：

```json
{
  "deviceId": "device-crm",
  "software": "builtin",
  "modules": ["cpu", "memory", "disk"],
  "durationSec": 3600,
  "args": {
    "cpuLoadPercent": 90,
    "memoryPercent": 80,
    "diskPath": "D:/stress_test",
    "diskMode": "sequential_write"
  }
}
```

Python API 发布 MQTT：

```text
Topic: agent/{deviceId}/cmd
```

Payload：

```json
{
  "dataType": "command",
  "commandId": "cmd-20260613-000001",
  "deviceId": "device-crm",
  "commandType": "stress.start",
  "software": "builtin",
  "modules": ["cpu", "memory", "disk"],
  "durationSec": 3600,
  "args": {
    "cpuLoadPercent": 90,
    "memoryPercent": 80,
    "diskPath": "D:/stress_test"
  },
  "timestamp": 1781333677601
}
```

Agent 执行结果发布：

```text
Topic: agent/{deviceId}/event
```

Payload：

```json
{
  "dataType": "stress_state_changed",
  "commandId": "cmd-20260613-000001",
  "deviceId": "device-crm",
  "state": "RUNNING",
  "software": "builtin",
  "modules": ["cpu", "memory", "disk"],
  "message": "stress started",
  "timestamp": 1781333678601
}
```

---

## 6. 流程五：Vue Admin 开启软件指令 -> Python API -> MQTT -> Agent Core -> 反馈 Vue

### 6.1 Mermaid 流程图

```mermaid
sequenceDiagram
    autonumber
    participant Page as Vue StressControl.vue
    participant Store as Pinia DeviceStore / StressStore
    participant API as stressApi.ts / axios
    participant PyAPI as Python FastAPI
    participant CmdService as CommandService
    participant MQTT as MQTTService
    participant EMQX as EMQX
    participant Agent as Agent Core
    participant WS as WebSocket service
    participant UI as Vue UI

    Page->>Store: startStress(deviceId, form)
    Store->>API: startStress(payload)
    API->>PyAPI: POST /api/stress/start
    PyAPI->>CmdService: create command
    CmdService->>MQTT: publish agent/{deviceId}/cmd
    MQTT->>EMQX: MQTT PUBLISH
    EMQX->>Agent: MESSAGE command
    Agent->>Agent: CommandDispatcher -> StressExecutor.start()
    Agent->>EMQX: PUBLISH agent/{deviceId}/event RUNNING
    EMQX->>PyAPI: Python MQTT subscriber receives event
    PyAPI->>WS: broadcast stress_state_changed
    WS-->>Store: onmessage
    Store->>UI: update stress state
```

### 6.2 Vue Admin 代码调用链

```text
src/views/device/StressControl.vue
  -> onStartClicked()
  -> stressStore.startStress(form)

src/stores/stress.ts
  -> startStress(payload)
  -> stressApi.startStress(payload)
  -> 保存 commandId 到 store
  -> 等待 WebSocket stress_state_changed 事件

src/api/stress.ts
  -> request.post("/api/stress/start", payload)

src/services/websocket.ts
  -> connect(wsUrl)
  -> onmessage(event)
  -> wsEventRouter.route(event)

src/services/wsEventRouter.ts
  -> if event.type == "stress_state_changed"
       stressStore.updateStressState(event.payload)
  -> if event.type == "telemetry_update"
       telemetryStore.updateLatest(event.payload)
```

---

## 7. 所有 MQTT 发布和订阅流程图

### 7.1 MQTT Topic 总览

```mermaid
flowchart TB
    subgraph AGENT["02_agent_core_cpp Agent Core"]
        A1["采集基本信息<br/>BasicInfoCollector"]
        A2["采集实时指标<br/>HardwareCollector"]
        A3["执行命令<br/>CommandDispatcher"]
        A4["执行结果/状态<br/>AgentEventPublisher"]
        A5["订阅命令<br/>MqttClient.subscribe"]
    end

    subgraph MQTT["EMQX MQTT Broker"]
        T1["agent/{deviceId}/basic-info"]
        T2["agent/{deviceId}/telemetry"]
        T3["agent/{deviceId}/event"]
        T4["agent/{deviceId}/cmd"]
    end

    subgraph API["03_backend_python_api"]
        P1["MqttService.subscribe(agent/+/basic-info)"]
        P2["MqttService.subscribe(agent/+/telemetry)"]
        P3["MqttService.subscribe(agent/+/event)"]
        P4["MqttService.publish(agent/{deviceId}/cmd)"]
        P5["TelemetryIngestService"]
        P6["CommandService"]
        P7["WebSocketManager"]
        P8["InfluxDB / MySQL"]
    end

    A1 -->|PUBLISH| T1
    A2 -->|PUBLISH| T2
    A4 -->|PUBLISH| T3

    T1 -->|SUBSCRIBE| P1
    T2 -->|SUBSCRIBE| P2
    T3 -->|SUBSCRIBE| P3

    P1 --> P5
    P2 --> P5
    P3 --> P6

    P5 --> P8
    P5 --> P7
    P6 --> P8
    P6 --> P7

    P4 -->|PUBLISH| T4
    T4 -->|SUBSCRIBE| A5
    A5 --> A3
```

### 7.2 MQTT Topic 明细表

| Topic | 发布方 | 订阅方 | 用途 | Python 处理函数 | Agent 处理函数 |
|---|---|---|---|---|---|
| `agent/{deviceId}/basic-info` | Agent Core | Python API | 启动或手动上报基本信息 | `handle_basic_info()` | `BasicInfoCollector::collect()` |
| `agent/{deviceId}/telemetry` | Agent Core | Python API | 实时硬件指标 | `handle_realtime_metric_v2()` | `HardwareCollector::collectRealtime()` |
| `agent/{deviceId}/event` | Agent Core | Python API | 命令结果、压力状态、异常事件 | `handle_agent_event()` | `AgentEventPublisher::publish()` |
| `agent/{deviceId}/cmd` | Python API | Agent Core | 开启/停止软件、采集基本信息、刷新配置 | `publish_command()` | `CommandDispatcher::dispatch()` |

---

## 8. WebSocket 推送流程

### 8.1 Mermaid 流程图

```mermaid
sequenceDiagram
    autonumber
    participant Qt as Qt/QML WebSocketClient
    participant Vue as Vue WebSocketService
    participant WS as Python WebSocketManager
    participant Ingest as TelemetryIngestService
    participant Cmd as CommandEventHandler

    Qt->>WS: connect /ws/realtime?clientId=qt-client-001&deviceId=device-crm
    Vue->>WS: connect /ws/realtime?clientId=web-admin&deviceId=device-crm

    Ingest->>WS: broadcast_to_device(deviceId, telemetry_update)
    WS-->>Qt: telemetry_update
    WS-->>Vue: telemetry_update

    Ingest->>WS: broadcast_to_device(deviceId, basic_info_updated)
    WS-->>Qt: basic_info_updated
    WS-->>Vue: basic_info_updated

    Cmd->>WS: broadcast_to_device(deviceId, stress_state_changed)
    WS-->>Qt: stress_state_changed
    WS-->>Vue: stress_state_changed
```

### 8.2 WebSocket 事件格式

```json
{
  "type": "telemetry_update",
  "deviceId": "device-crm",
  "timestamp": 1781333677601,
  "payload": {}
}
```

事件类型：

| type | 说明 | 来源 |
|---|---|---|
| `client_connected` | 客户端连接成功 | Python API |
| `basic_info_updated` | 基本信息更新 | Agent MQTT -> Python API -> WS |
| `telemetry_update` | 实时指标更新 | Agent MQTT -> Python API -> WS |
| `stress_state_changed` | 压力测试状态变化 | Agent MQTT event -> Python API -> WS |
| `command_ack` | 命令已接收或已下发 | Python API / Agent Event |
| `agent_offline` | Agent 离线 | Python API 心跳检测 |

---

## 9. Python API 内部启动流程

### 9.1 Mermaid 流程图

```mermaid
flowchart TD
    A["uvicorn main:app"] --> B["main.py create_app()"]
    B --> C["注册 REST routers"]
    B --> D["注册 WebSocket router"]
    B --> E["startup_event()"]
    E --> F["初始化 MySQL Session"]
    E --> G["初始化 InfluxDBClient"]
    E --> H["初始化 WebSocketManager"]
    E --> I["MqttService.start()"]
    I --> J["connect EMQX"]
    J --> K["subscribe agent/+/telemetry"]
    J --> L["subscribe agent/+/basic-info"]
    J --> M["subscribe agent/+/event"]
    K --> N["loop_start() 等待消息"]
    L --> N
    M --> N
```

### 9.2 Python API 启动函数调用链

```text
main.py
  -> app = FastAPI()
  -> app.include_router(health_router)
  -> app.include_router(device_router)
  -> app.include_router(telemetry_router)
  -> app.include_router(stress_router)
  -> app.include_router(client_router)
  -> app.websocket("/ws/realtime")(websocket_endpoint)

@app.on_event("startup")
  -> settings = get_settings()
  -> mysql.init_engine(settings.MYSQL_URL)
  -> influx_client = InfluxDBClient(...)
  -> websocket_manager = ConnectionManager()
  -> mqtt_service = MqttService(settings, handlers)
  -> mqtt_service.start()
```

---

## 10. Agent Core 内部启动流程

### 10.1 Mermaid 流程图

```mermaid
flowchart TD
    A["agent_core.exe / Windows Service"] --> B["main.cpp"]
    B --> C["AgentApp::run()"]
    C --> D["ConfigManager::load()"]
    C --> E["DeviceIdentity::loadOrCreate()"]
    C --> F["MqttClient::connect()"]
    F --> G["subscribe agent/{deviceId}/cmd"]
    C --> H["BasicInfoCollector::collect()"]
    H --> I["publish agent/{deviceId}/basic-info"]
    C --> J["TelemetryScheduler::start()"]
    J --> K["HardwareCollector::collectRealtime()"]
    K --> L["publish agent/{deviceId}/telemetry"]
    G --> M["MqttClient::onMessage()"]
    M --> N["CommandDispatcher::dispatch()"]
```

### 10.2 Agent Core 启动函数调用链

```text
main.cpp
  -> AgentApp app(argc, argv)
  -> app.run()

AgentApp::run()
  -> ConfigManager::load()
  -> Logger::init()
  -> DeviceIdentity::loadOrCreate()
  -> MqttClient::connect()
  -> MqttClient::subscribe("agent/{deviceId}/cmd")
  -> BasicInfoCollector::collect()
  -> MqttClient::publish("agent/{deviceId}/basic-info", payload)
  -> TelemetryScheduler::start()
  -> EventLoop::run()

TelemetryScheduler::start()
  -> Timer::every(intervalMs)
  -> HardwareCollector::collectRealtime()
  -> MetricPayloadBuilder::buildRealtimeMetricV2()
  -> MqttClient::publish("agent/{deviceId}/telemetry", payload)
```

---

## 11. Qt/QML 客户端内部启动流程

### 11.1 Mermaid 流程图

```mermaid
flowchart TD
    A["启动 aging_qt_client.exe"] --> B["main.cpp"]
    B --> C["QGuiApplication"]
    C --> D["注册 C++ ViewModel 到 QML"]
    D --> E["AppConfig::load()"]
    E --> F{"是否已有 serverIp"}
    F -- 否 --> G["显示 ServerConfigPage.qml"]
    F -- 是 --> H["ApiClient::setBaseUrl()"]
    H --> I["WebSocketClient::connect()"]
    I --> J["DashboardPage.qml"]
    J --> K["TelemetryViewModel::loadLatest()"]
    K --> L["GET /api/telemetry/latest/{deviceId}"]
    I --> M["等待 telemetry_update WS 推送"]
```

### 11.2 Qt/QML 启动函数调用链

```text
main.cpp
  -> QGuiApplication app
  -> qmlRegisterSingletonInstance("App", "ApiClient", apiClient)
  -> qmlRegisterType<TelemetryViewModel>()
  -> qmlRegisterType<ServerConfigViewModel>()
  -> QQmlApplicationEngine.load("Main.qml")

Main.qml
  -> AppConfig.hasServerConfig ? DashboardPage : ServerConfigPage

ServerConfigPage.qml
  -> ServerConfigViewModel.saveServerConfig()
  -> ApiClient.setBaseUrl()
  -> WebSocketClient.connect()

DashboardPage.qml
  -> TelemetryViewModel.loadLatest()
  -> TelemetryViewModel.bindWebSocket(WebSocketClient)
```

---

## 12. Vue Admin 内部启动流程

### 12.1 Mermaid 流程图

```mermaid
flowchart TD
    A["pnpm dev / npm run build"] --> B["main.ts"]
    B --> C["createApp(App)"]
    C --> D["install Pinia / Router / UI"]
    D --> E["读取 VITE_API_BASE_URL / VITE_WS_URL"]
    E --> F["axios request.ts 初始化"]
    E --> G["websocket.ts 初始化"]
    G --> H["connect /ws/realtime"]
    H --> I["订阅 telemetry_update / stress_state_changed"]
    D --> J["DeviceMonitor.vue"]
    J --> K["deviceApi.getDevices()"]
    J --> L["telemetryApi.getLatest()"]
```

### 12.2 Vue Admin 函数调用链

```text
src/main.ts
  -> createApp(App)
  -> app.use(router)
  -> app.use(pinia)
  -> app.mount("#app")

src/utils/request.ts
  -> axios.create({ baseURL: import.meta.env.VITE_API_BASE_URL })

src/services/websocket.ts
  -> createWebSocket(import.meta.env.VITE_WS_URL)
  -> onmessage()
  -> wsEventRouter.route()

src/views/device/DeviceMonitor.vue
  -> onMounted()
  -> deviceStore.fetchDevices()
  -> telemetryStore.fetchLatest(deviceId)
  -> websocketService.subscribe(deviceId)
```

---

## 13. 关键配置项

### 13.1 Qt/QML 客户端配置

```json
{
  "apiBaseUrl": "http://192.168.31.206:8000",
  "webSocketUrl": "ws://192.168.31.206:8000/ws/realtime",
  "deviceId": "device-crm",
  "clientId": "qt-client-001"
}
```

### 13.2 Agent Core 配置

```json
{
  "deviceId": "device-crm",
  "deviceName": "device-crm",
  "mqtt": {
    "host": "192.168.31.206",
    "port": 1883,
    "username": "crm_agent",
    "password": "******",
    "clientId": "agent-device-crm"
  },
  "telemetry": {
    "intervalMs": 1000,
    "publishTopic": "agent/{deviceId}/telemetry"
  },
  "command": {
    "subscribeTopic": "agent/{deviceId}/cmd"
  }
}
```

### 13.3 Python API `.env`

```env
API_HOST=0.0.0.0
API_PORT=8000

MQTT_ENABLED=true
MQTT_HOST=192.168.31.206
MQTT_PORT=1883
MQTT_USERNAME=crm_api
MQTT_PASSWORD=******

INFLUX_URL=http://192.168.31.206:8086
INFLUX_ORG=aging
INFLUX_BUCKET=metrics
INFLUX_TOKEN=******

MYSQL_URL=mysql+pymysql://user:password@127.0.0.1:3306/crm
```

### 13.4 Vue Admin `.env.development`

```env
VITE_API_BASE_URL=http://192.168.31.206:8000
VITE_WS_URL=ws://192.168.31.206:8000/ws/realtime
```

---

## 14. Codex 节省 Token 实现提示语

```text
你在 CRM 项目中实现通信链路与调用链对齐。不要大范围重构，只做最小修改。

项目：
01_qt_qml_client：Qt/QML 客户端
02_agent_core_cpp：C++ Agent Core
03_backend_python_api：FastAPI + MQTT + WebSocket + InfluxDB
04_web_admin_vue：Vue Admin

目标：
1. Qt/QML 启动后可填写 serverIp，保存本地配置，更新 apiBaseUrl/wsUrl。
2. Qt/QML 调用 GET /api/health 验证连接，POST /api/clients/register 注册客户端，然后连接 /ws/realtime。
3. Agent Core 启动后连接 MQTT，订阅 agent/{deviceId}/cmd，发布：
   - agent/{deviceId}/basic-info
   - agent/{deviceId}/telemetry
   - agent/{deviceId}/event
4. Python API 启动后订阅：
   - agent/+/basic-info
   - agent/+/telemetry
   - agent/+/event
5. Python API 收到 telemetry 后写入 InfluxDB，更新 latest cache，并通过 WebSocket 推送 telemetry_update。
6. Python API 收到 basic-info 后保存 MySQL，并通过 WebSocket 推送 basic_info_updated。
7. Qt/QML 和 Vue 可请求：
   - GET /api/telemetry/latest/{deviceId}
   - GET /api/telemetry/history?deviceId=&metric=&start=&end=&window=
8. Python API 提供：
   - POST /api/stress/start
   - POST /api/stress/stop
   下发 MQTT 到 agent/{deviceId}/cmd。
9. Agent Core 收到 stress.start/stress.stop 后通过 CommandDispatcher -> StressCommandHandler -> StressExecutor/ProcessLauncher 执行，并发布 agent/{deviceId}/event。
10. Python API 收到 event 后更新命令状态，并通过 WebSocket 推送 stress_state_changed。
11. Vue Admin 的开启软件流程为 StressControl.vue -> stressStore -> stressApi.ts -> POST /api/stress/start -> WS 回写状态。

要求：
- 以现有类/函数名为准；没有则新增最小类。
- 所有接口、MQTT Topic、WebSocket 事件必须打印开发日志。
- 禁止继续使用 mock 数据作为正式链路。
- 历史曲线必须从 InfluxDB 查询。
- 输出修改文件清单和每个文件修改原因。
```

---

## 15. 排查顺序建议

### 15.1 客户端连不上 Python API

```text
1. 检查 Qt 保存的 apiBaseUrl 是否正确。
2. 浏览器访问 http://服务器IP:8000/api/health。
3. 检查 Python API 是否监听 0.0.0.0:8000。
4. 检查 Windows 防火墙是否放行 8000。
5. 检查 Qt ApiClient 是否仍使用旧 IP 或 mock URL。
```

### 15.2 WebSocket 无实时推送

```text
1. 检查 Qt/Vue 的 wsUrl 是否为 ws://服务器IP:8000/ws/realtime。
2. 检查 Python WebSocketManager 是否有连接日志。
3. 检查 Python 是否收到 MQTT telemetry。
4. 检查 TelemetryIngestService 是否调用 broadcast。
5. 检查前端 wsEventRouter 是否识别 telemetry_update。
```

### 15.3 InfluxDB 没有数据

```text
1. 检查 Agent 是否发布 agent/{deviceId}/telemetry。
2. 检查 EMQX Dashboard 是否看到消息。
3. 检查 Python API 是否订阅 agent/+/telemetry。
4. 检查 InfluxDB token/org/bucket 是否正确。
5. 检查 InfluxTelemetryRepository.write_realtime_metrics 是否执行。
```

### 15.4 开启软件无反应

```text
1. 检查 POST /api/stress/start 是否返回 commandId。
2. 检查 Python 是否发布 agent/{deviceId}/cmd。
3. 检查 Agent 是否订阅了相同 deviceId 的 cmd topic。
4. 检查 CommandDispatcher 是否识别 commandType。
5. 检查 StressExecutor 或 ProcessLauncher 的启动路径。
6. 检查 Agent 是否发布 agent/{deviceId}/event。
7. 检查 Python 是否推送 stress_state_changed。
```

---

## 16. 最终代码链路检查清单

### 16.1 01_qt_qml_client

- [ ] `ServerConfigPage.qml` 能填写服务器 IP。
- [ ] `ServerConfigViewModel` 能保存配置。
- [ ] `ApiClient` 使用动态 `apiBaseUrl`。
- [ ] `WebSocketClient` 使用动态 `webSocketUrl`。
- [ ] `TelemetryViewModel` 支持 REST 最新数据。
- [ ] `HistoryTelemetryViewModel` 支持 REST 历史曲线。
- [ ] `StressViewModel` 支持 POST `/api/stress/start|stop`。
- [ ] WebSocket 支持 `telemetry_update`、`basic_info_updated`、`stress_state_changed`。

### 16.2 02_agent_core_cpp

- [ ] `MqttClient` 连接 EMQX。
- [ ] `MqttClient` 订阅 `agent/{deviceId}/cmd`。
- [ ] `BasicInfoCollector` 发布 `agent/{deviceId}/basic-info`。
- [ ] `HardwareCollector` 发布 `agent/{deviceId}/telemetry`。
- [ ] `CommandDispatcher` 分发 `stress.start`、`stress.stop`。
- [ ] `StressExecutor` 执行 CPU/GPU/内存/硬盘压力。
- [ ] `AgentEventPublisher` 发布 `agent/{deviceId}/event`。

### 16.3 03_backend_python_api

- [ ] `MqttService` 订阅 `agent/+/basic-info`。
- [ ] `MqttService` 订阅 `agent/+/telemetry`。
- [ ] `MqttService` 订阅 `agent/+/event`。
- [ ] `TelemetryIngestService` 写 InfluxDB。
- [ ] `LatestTelemetryCache` 保存实时最新数据。
- [ ] `WebSocketManager` 推送实时事件。
- [ ] `TelemetryQueryService` 查询实时和历史数据。
- [ ] `CommandService` 发布 `agent/{deviceId}/cmd`。
- [ ] `AgentEventHandler` 更新命令结果。

### 16.4 04_web_admin_vue

- [ ] `request.ts` 使用 `VITE_API_BASE_URL`。
- [ ] `websocket.ts` 使用 `VITE_WS_URL`。
- [ ] `DeviceMonitor.vue` 请求最新数据。
- [ ] `HistoryChart.vue` 请求历史曲线。
- [ ] `StressControl.vue` 调用开启/停止软件接口。
- [ ] `wsEventRouter.ts` 处理 `telemetry_update`、`stress_state_changed`。
