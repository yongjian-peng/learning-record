# maple 监控实现

https://github.com/Makisuo/maple 这个监控的项目，能不能监控 CRM 项目，并且免费使用

```
结论

可以用，适合免费自用/内部使用。
Maple 项目定位是基于 OpenTelemetry + ClickHouse 的可观测性平台，用来采集和分析 traces、logs、metrics，仓库 README 明确写的是 “Open-source observability for traces, logs & metrics — built on OpenTelemetry + ClickHouse”。

但是它的许可证不是传统 MIT / Apache 这种完全宽松开源，而是 FSL-1.1-ALv2。许可证允许内部使用、非商业教育、非商业研究等用途；但禁止把它做成和 Maple 本身竞争的商业产品或服务。并且每个版本在发布两年后会转为 Apache License 2.0。

所以对于你的 CRM 项目：

内部研发使用：可以
公司内部部署使用：可以
用它监控 CRM 后端、Agent、Web、接口：可以
把 Maple 包进你的商业监控平台对外卖：不建议，可能触发 Competing Use 限制
替代 InfluxDB 做设备硬件实时曲线：不推荐
Maple 能监控 CRM 项目的哪些部分

你的 CRM / 工业老化测试平台现在大概有：

01_qt_qml_client
02_agent_core_cpp
03_backend_python_api
04_web_admin_vue
InfluxDB
MySQL
EMQX / MQTT
WebSocket
AIDA64 / BurnInTest 控制链路

Maple 更适合监控这些内容：

CRM 模块	Maple 是否适合	监控内容
03_backend_python_api	很适合	FastAPI 请求耗时、接口错误、异常堆栈、数据库调用、WebSocket 连接
02_agent_core_cpp	适合	Agent 启动、采集异常、MQTT 上报失败、BurnInTest 控制失败
04_web_admin_vue	部分适合	前端接口错误、页面异常、接口耗时
01_qt_qml_client	部分适合	客户端崩溃日志、接口调用失败、WebSocket 断开
InfluxDB / MySQL	间接适合	通过后端埋点监控 SQL 耗时、写入失败、查询慢
硬件实时数据	不建议替代	CPU/GPU/温度/曲线仍建议走 InfluxDB

Maple 本地模式支持 OTLP HTTP 接收端点，文档里写的是 POST /v1/{traces,logs,metrics}，默认 maple start 会启动 OTLP ingest 和嵌入式 ClickHouse，端口是 4318。

它不能完全替代你现在的 InfluxDB

你的 CRM 项目现在核心是：

Agent Core C++
    ↓
实时采集 CPU / GPU / 内存 / 磁盘 / 温度
    ↓
MQTT / 后端
    ↓
InfluxDB
    ↓
历史曲线 / 实时曲线

这类数据是设备级时序指标，尤其你要画历史曲线、秒级采集、长周期存储，InfluxDB 更合适。

Maple 更偏向：

接口有没有慢？
哪个服务报错？
一次请求经过了哪些模块？
哪个 API 调用了 MySQL / InfluxDB？
BurnInTest 控制失败发生在哪一步？
Agent 有没有掉线？
MQTT 发布失败了吗？

所以推荐架构是：InfluxDB 继续做设备指标，Maple 做系统可观测性。

推荐接入架构
CRM 系统
│
├─ 01_qt_qml_client
│  └─ 上报客户端错误日志 / API 调用耗时
│
├─ 02_agent_core_cpp
│  └─ 上报 Agent 状态、MQTT 状态、压力测试控制日志
│
├─ 03_backend_python_api
│  └─ OpenTelemetry 自动埋点 FastAPI / HTTP / MySQL / InfluxDB
│
├─ 04_web_admin_vue
│  └─ 上报前端异常 / API 请求耗时
│
└─ Maple
   ├─ OTLP HTTP :4318
   ├─ traces
   ├─ logs
   ├─ metrics
   └─ dashboard

部署后你可以重点看这些问题：

/api/stress/start 为什么失败？
performance.start 从 Web 到 Backend 到 Agent 到 Runner 哪一步失败？
BurnInTest Start Test Run 调用了没有？
确认框有没有识别到？
InfluxDB 写入耗时是多少？
MQTT 连接多久断一次？
WebSocket 推送 basic_info_updated 是否正常？
免费使用判断

从许可证看，内部使用是允许的。许可证里写明 Permitted Purposes 包括 “internal use and access”，也就是内部使用和访问。

但是要注意：

可以：
- 公司内部部署 Maple
- 用 Maple 监控 CRM 项目
- 修改 Maple 源码自用
- 内部研发环境、测试环境、生产环境自用

不建议：
- 把 Maple 改名后作为你的商业监控平台卖
- 把 Maple 功能包装成 SaaS 对外提供
- 让 Maple 成为你产品里的核心对外销售功能

如果你的 CRM 是公司内部项目或设备配套系统，Maple 作为内部监控工具使用，一般问题不大。
如果你准备把“监控平台”作为 CRM 的商业功能对外卖，建议不要直接内置 Maple，或者等对应版本两年后转 Apache 2.0，再评估合规性。

Windows 环境要注意

你的环境主要是 Windows。Maple Release 页面写的安装方式主要是 macOS / Linux，例如 curl -fsSL https://maple.dev/cli/install | sh，并且 bundle 里包含 maple + libchdb.so。

所以在你的 Windows 机器上，建议不要直接裸跑 Maple。更推荐：

方案 1：Docker Desktop 部署 Maple
方案 2：WSL2 Ubuntu 里部署 Maple
方案 3：单独找一台 Linux 服务器部署 Maple

仓库里也提供 Docker 本地部署方式，README 里写的是 docker compose -f docker-compose.yml up --build，本地服务包含 API、Web、Ingest、OTEL Collector；端口包括 Web 3471、API 3472、Ingest 3474、OTEL Collector 4317/4318。

对 CRM 项目的推荐方案

我建议这样设计：

第一阶段：只接入 03_backend_python_api

先监控 FastAPI：

HTTP 请求耗时
接口 500 错误
/api/devices
/api/telemetry/latest
/api/stress/start
/api/stress/stop
/api/devices/{id}/basic-info
InfluxDB 查询耗时
MySQL 查询耗时

第二阶段再接入：

02_agent_core_cpp
stress_tool_runner.exe
BurnInTestController
Aida64Controller

重点记录：

performance.start
performance.stop
performance.reset
performance.close
commandId
taskId
deviceId
tool=aida64/burnintest
runnerStatus
errorMessage
phase
timeoutMs

这样以后你看到数据库里：

"BurnInTest Start Test Run invoked but running state was not detected"

就可以在 Maple 里追踪完整链路：

Web 点击按钮
  → Backend 收到 performance.start
  → 下发 MQTT / IPC 命令
  → Agent 收到命令
  → Runner 调用 BurnInTest 菜单
  → 等待确认框
  → 点击确定
  → 等待 Running
  → 返回成功 / 失败
最终建议

可以使用 Maple 监控 CRM 项目，并且内部使用基本可以免费。

但推荐定位是：

Maple = CRM 软件系统可观测性平台
InfluxDB = 设备硬件实时数据/历史曲线数据库
MySQL = 业务数据数据库
EMQX = 设备通信


```

