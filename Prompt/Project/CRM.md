# BurnInTest 运行时配置是否采集

```
改动集中在这几处：
[ConfigManager.h (line 19)](E:/WorkSpace/CRM/02_agent_core_cpp/src/core/ConfigManager.h:19)：新增 collector.resourcePressureGuard 配置结构。
[ConfigManager.cpp (line 236)](E:/WorkSpace/CRM/02_agent_core_cpp/src/core/ConfigManager.cpp:236)：读取/保存 skipOnStressRunning、CPU、内存、硬盘开关和阈值。
[MetricCollectorManager.cpp (line 482)](E:/WorkSpace/CRM/02_agent_core_cpp/src/collectors/MetricCollectorManager.cpp:482)：采集跳过逻辑改为配置驱动；默认 BurnInTest running 不再跳过 CPU 温度/功耗采集。
[ResourceSafetyGuard.cpp (line 41)](E:/WorkSpace/CRM/02_agent_core_cpp/src/core/ResourceSafetyGuard.cpp:41)：内存/硬盘停止压力测试的保护逻辑也读同一份配置。
[AgentApp.cpp (line 121)](E:/WorkSpace/CRM/02_agent_core_cpp/src/core/AgentApp.cpp:121)：启动和配置热更新时同步保护配置。
三个配置文件已加默认项：skipOnStressRunning=false，CPU 跳过保护默认关闭，内存/硬盘保护默认开启。
新增测试覆盖：
BurnInTest running 默认继续采集 CPU 温度。
skipOnStressRunning=true 可退回旧跳过行为。
内存/硬盘开关能关闭采集跳过。
ResourceSafetyGuard 也遵守内存/硬盘开关。
配置读写默认值和自定义阈值。
```



# 保存日志，帮助定位问题

```
我会按 ponytail 走最小闭环：先读现有日志/MQTT/命令路径，找一个能贯穿后端发布、Agent 接收、动作执行的最小 trace_id 注入点；先给方案，不动代码。
我会用 ponytail 控制改动规模，同时用“先计划”的方式把链路画清楚；重点不是造一套 tracing 系统，而是在已有 payload/log 上补一个稳定的 trace_id。
已运行 3 条命令已运行 Get-Content -LiteralPath 'C:\Users\admin\.codex\plugins\cache\ponytail\ponytail\4.7.0\skills\ponytail\SKILL.md'，已持续 1s
已运行 Get-Content -LiteralPath 'C:\Users\admin\.codex\plugins\cache\openai-curated\superpowers\43313cc9\skills\writing-plans\SKILL.md'，已持续 1s已运行 Get-Content -LiteralPath 'C:\Users\admin\.codex\plugins\cache\openai-curated\superpowers\43313cc9\skills\brainstorming\SKILL.md'，已持续 1s

我先把现有链路挖出来：规则发布、MQTT 命令、Agent 规则应用、BurnInTest 启停这些节点在哪里打日志、payload 里现在有什么 ID。这样计划不会飘。
已运行 3 条命令已运行 rg -n "alarm\.rules\.update|publishAlarmRules|publish_alarm|MQTT_CMD_PUBLISH|rules.update|alarm rules|AlarmRule" 03_backend_python_api 02_agent_core_cpp 04_web_admin_vue 01_qt_qml_client
已运行 rg -n "BurnInTest|burnintest|start_software|stop_software|stop_memory_stress|stop_disk_stress|CMD_RECEIVED|commandType|agent/.*/cmd|cmd" 03_backend_python_api 02_agent_core_cpp 04_web_admin_vue 01_qt_qml_client
已运行 rg -n "trace_id|traceId|request_id|requestId|messageId|correlation|MQTT publish|WebSocket" 03_backend_python_api 02_agent_core_cpp 04_web_admin_vue 01_qt_qml_client

我已经看到两个事实：报警规则走后端 /api/alarm/rules/publish 到 Agent 的 alarm WebSocket；性能测试控制走后端命令服务到 MQTT，再由 Agent 执行 BurnInTest runner。下一步只读关键函数，确定 trace_id 放哪里最省。

现在能确认：规则发布本来就有 ack，只是缺 traceId 和清晰阶段日志；性能控制已有 commandId 和 ACK 表，最省做法是把 traceId 直接等于 commandId 或 rule-publish version/request，不新建表。继续读 Agent 命令分发与 ACK。

这里还有一个容易省事的点：命令链路已经有 commandId、command_ack、command_record，所以性能启停不需要额外“事务表”。规则发布没有命令表，才需要 traceId 写进 publish message 和 ack。

还有个关键缺口：Agent 的 ACK 目前只回 commandId，没有 traceId；但因为 traceId=commandId，兼容成本最低。规则发布这条链没有 commandId，所以用 rulepub-<versionNo> 或短 UUID。
已运行 4 条命令已运行 Get-Content -LiteralPath '02_agent_core_cpp\src\command\CommandModels.h'，已持续 1s
已运行 Get-Content -LiteralPath '02_agent_core_cpp\tests\test_agent_core.cpp' -TotalCount 3300 | Select-Object -Skip 2580，已持续 1s
已运行 Get-Content -LiteralPath '03_backend_python_api\tests\test_alarm_rules.py'，已持续 1s
已运行 Get-Content -LiteralPath '03_backend_python_api\tests\test_api.py' -TotalCount 1300 | Select-Object -Skip 1210，已持续 1s
我还会把“如何排查”设计成命令级 grep：同一个 traceId 在 Python 日志、Agent 日志、MQTT ACK、数据库 command_record/command_ack/metric_alarm_events 都能搜到。这样不用引入 OpenTelemetry。
已运行 4 条命令已运行 rg -n "publish_performance_start|publish_performance_stop|performance/start|performance.stop|basic_info.refresh|BASIC_INFO_REFRESH_REQUEST" 03_backend_python_api\app 04_web_admin_vue\src 01_qt_qml_client\src
已运行 Get-Content -LiteralPath '03_backend_python_api\app\api\devices.py'，已持续 1s
已运行 Get-Content -LiteralPath '03_backend_python_api\app\api\performance.py'，已持续 1s
已运行 Get-Content -LiteralPath '03_backend_python_api\app\main.py' -TotalCount 190 | Select-Object -Skip 130，已持续 1s
规则 ACK 的后端入口也找到了：/ws/agent/{device_id} 收到 alarm.rules.update.ack 后只更新版本状态，没有打阶段日志。最小修复就是这里补 traceId/versionNo/success。


可选方案
推荐：traceId = 已有唯一 ID
规则发布：traceId = versionNo
命令链路：traceId = commandId
报警事件：traceId = eventId
改动少、兼容好、数据库已有落点
实现计划
后端规则发布 /api/alarm/rules/publish
生成规则版本后：traceId = versionNo
下发 WebSocket payload 增加 traceId
日志增加：ALARM_RULES_PUBLISH traceId=... versionNo=... ruleCount=... deliveredAgents=...
ALARM_RULES_ACK traceId=... deviceId=... success=... message=...

成功标准：metric_alarm_rule_versions.publish_status = applied

Agent 规则接收
alarm.rules.update 收到后读取 traceId
ACK 回传 traceId
日志增加：ALARM_RULES_UPDATE_RECEIVED traceId=... versionNo=...
ALARM_RULES_UPDATE_APPLIED traceId=... success=... count=...


后端性能测试命令
performance.start / stop / reset / close / basic_info.refresh
traceId = commandId
MQTT payload 增加 traceId
API 返回增加 traceId
日志增加：COMMAND_PUBLISH traceId=... commandId=... commandType=... topic=... published=...
COMMAND_ACK_RECEIVED traceId=... commandId=... ackStatus=...


Agent 命令执行
收到命令时打印：CMD_RECEIVED traceId=... commandId=... commandType=... taskId=... runner=...

执行结束打印：CMD_RESULT traceId=... success=... runnerStatus=... message=...

ACK payload 增加 traceId
BurnInTest 内部不重写全量日志，先在命令入口/出口串起来，够定位 90% 问题。

报警事件链路
alarm.event 增加或复用 traceId = eventId
后端保存时日志补：METRIC_ALARM_EVENT_SAVED traceId=... deviceId=... ruleCode=... actionStatus=...


最小测试
后端：规则发布 payload/返回值包含 traceId
后端：命令 payload/返回值包含 traceId=commandId
Agent：规则 ACK、命令 ACK 包含 traceId


开启软件命令：
COMMAND_REQUEST
COMMAND_TASK_RESOLVED
COMMAND_DB_SAVED
COMMAND_MQTT_PUBLISH_OK / COMMAND_MQTT_PUBLISH_FAILED
AGENT_COMMAND_ACK_SEND
MQTT_ACK_RECEIVED
COMMAND_RECORD_UPDATED
COMMAND_WS_PUSH
PERFORMANCE_STATUS_PUBLISH
PERFORMANCE_STATUS_RECEIVED

现在同一个 traceId/commandId 可以串起这些状态：
COMMAND_REQUEST -> COMMAND_TASK_RESOLVED -> COMMAND_MQTT_PUBLISH_OK/FAILED -> COMMAND_DB_SAVED -> CMD_RECEIVED -> AGENT_COMMAND_ACK_SEND -> PERFORMANCE_STATUS_PUBLISH -> PERFORMANCE_STATUS_RECEIVED -> MQTT_ACK_RECEIVED -> COMMAND_RECORD_UPDATED -> COMMAND_WS_PUSH


排查方法
$tid = "cmd-xxxx"   # 或 alarm-rules-xxxx / alarm-event-id
Select-String -Path `
  "03_backend_python_api\logs\*.log", `
  "02_agent_core_cpp\logs\*.log", `
  "C:\ProgramData\CRMStressPlatform\logs\*.log" `
  -Pattern $tid
数据库侧：
select command_id, status, payload, agent_response
from command_record
where command_id = 'cmd-xxxx';

select *
from command_ack
where command_id = 'cmd-xxxx';

select version_no, publish_status, applied_at, error_message
from metric_alarm_rule_versions
where version_no = 'alarm-rules-xxxx';

select event_id, rule_code, action_type, action_status, raw_json
from metric_alarm_events
where event_id = 'alarm-xxxx';



报警发布规则，新增/增强日志：
ALARM_RULES_PUBLISH_REQUEST
ALARM_RULES_PUBLISH
ALARM_RULES_WS_DELIVERED
ALARM_RULES_UPDATE_RECEIVED
ALARM_RULES_UPDATE_APPLIED
ALARM_RULES_ACK_SEND
ALARM_RULES_ACK
ALARM_RULES_VERSION_APPLIED


Select-String -Path "03_backend_python_api\logs\*.log","02_agent_core_cpp\logs\*.log","C:\ProgramData\CRMStressPlatform\logs\*.log" -Pattern "cmd-xxxx"
```



# AlarmActionExecutor.Execute链路

```
验证指令：
$env:PATH='E:\Qt\Qt5.12\Tools\mingw1310_64\bin;' + $env:PATH
cmake --build build --target agent_core_tests
.\build\agent_core_tests.exe
cmake --build build --target aging-agent
ctest --test-dir build --output-on-failure


AlarmActionExecutor.Execute("stop_memory_stress", taskId) 和 Execute("stop_disk_stress", taskId) 当前内部走的是“停止压力项/测试运行”链路。对于 BurnInTestPerformanceRunner 来说，最终会调用 BurnInTest 的菜单 Test -> Stop Test Run，不会走 Close()，不会发 WM_CLOSE，也不会主动关闭 BurnInTest 软件窗口。
调用链
入口在 [AlarmActionExecutor.cpp (line 12)](E:/WorkSpace/CRM/02_agent_core_cpp/src/alarm/AlarmActionExecutor.cpp:12)：
stop_memory_stress -> item = "memory"
stop_disk_stress   -> item = "disk"
然后调用：
performance_.StopStressItem(taskId, item)
继续进入 [PerformanceToolManager.cpp (line 154)](E:/WorkSpace/CRM/02_agent_core_cpp/src/performance/PerformanceToolManager.cpp:154)：
runner->StopStressItem(taskId, item)
关键点是 BurnInTestPerformanceRunner 没有单独实现 StopStressItem，所以使用基类默认逻辑，在 [IPerformanceRunner.h (line 51)](E:/WorkSpace/CRM/02_agent_core_cpp/src/performance/IPerformanceRunner.h:51)：
StopStressItem(...) -> StopTest(taskId)
StopTest(taskId) -> Stop(taskId)
而 BurnInTest 覆盖了 Stop()，在 [BurnInTestPerformanceRunner.cpp (line 236)](E:/WorkSpace/CRM/02_agent_core_cpp/src/performance/BurnInTestPerformanceRunner.cpp:236)：
Stop(taskId) -> StopLocked(taskId)
StopLocked() 的实际行为是在 [BurnInTestPerformanceRunner.cpp (line 248)](E:/WorkSpace/CRM/02_agent_core_cpp/src/performance/BurnInTestPerformanceRunner.cpp:248)：
检测 BurnInTest 当前状态。
如果已经停止，直接返回成功。
如果正在运行，调用 InvokeControl(...)。
InvokeControl 目标是：菜单：Test
菜单项：Stop Test Run
兜底按钮：STOP

成功后等待 BurnInTest 状态从 running/stopping 变成非运行状态。
也就是说，对 BurnInTest：
stop_memory_stress
  -> StopStressItem(taskId, "memory")
  -> StopTest(taskId)
  -> Stop(taskId)
  -> StopLocked(taskId)
  -> BurnInTest: Test -> Stop Test Run
stop_disk_stress 同理，只是传入的 item 是 "disk"。
不会关闭窗口的依据
BurnInTest 真正关闭软件窗口的逻辑在 [BurnInTestPerformanceRunner.cpp (line 414)](E:/WorkSpace/CRM/02_agent_core_cpp/src/performance/BurnInTestPerformanceRunner.cpp:414) 的 Close()：
PostMessageW(record.window, WM_CLOSE, 0, 0)
并且超时后还有 TerminateProcess 兜底。
但 stop_memory_stress / stop_disk_stress 当前链路不会调用 Close()，也不会调用 Kill()。
所以：它是停止 BurnInTest 测试运行，不关闭 BurnInTest 软件窗口。
```





# 需求记录

```
CPU 现有逻辑
当前 CPU 保护不是在 BurnInTest 里做的，而是在 Agent 实时采集链路里做的：
MetricCollectorManager 先采集 CpuCollector，拿到 cpu.usage_percent。
如果满足任一条件：performance.status=running
CPU 使用率不可用
cpu.usage_percent >= 95

就跳过高风险硬件传感器采集：GpuCollector
TemperatureCollector

被跳过的字段写成 null，同时在 issues 中记录原因，例如 stress_guard cpu.usage_percent=100。
这样 Agent 在 CPU 100% 或 BurnInTest 压力运行时，不继续调用可能卡死的 LHM / GPU / 温度传感器逻辑，保护 Agent 采集线程和 MQTT 心跳。
规则停止是另一条链路：AgentApp::EvaluateAlarms() 读取缓存规则后，通过 AlarmActionExecutor -> PerformanceToolManager -> BurnInTestPerformanceRunner 停止 BurnInTest 测试，不关闭软件窗口。
手动打开 BurnInTest 也能被纳入管理：PerformanceToolManager::GetStatus() 会尝试从默认 runner 附着到正在运行的 BurnInTest，并生成 stress-tool-burnintest 任务上下文。
问题判断
内存和磁盘现在有数据库报警规则支持：
memory.usage_percent >= 90 可触发 stop_memory_stress
disk.free_percent < 10 可触发 stop_disk_stress
但这是“规则链路”，依赖规则已经加载、Agent 与后端 WebSocket 同步正常。
当内存或磁盘真的到 100% 时，Python API、MQTT、写日志、Influx 写入都有可能已经被系统资源拖垮，所以还需要 Agent 本地硬保护，和 CPU 高压隔离同一个思想。
实现计划
只改 02_agent_core_cpp，不动数据库，不动 Python API，不动 Vue。
在 MetricCollectorManager.cpp 增加统一资源压力判断：
CPU：沿用现有 cpu.usage_percent >= 95
内存：新增 memory.usage_percent >= 99.5，按“100% 满载”处理
磁盘：新增任一 disk.volume.N.usage_percent >= 99.5 或 free_gb <= 0.01，按“磁盘 100%”处理

把现有 CPU 的硬件传感器跳过逻辑扩展成通用 ResourcePressureGuard：
CPU 高压、内存满载、磁盘满载、压力测试运行中，都会跳过 GpuCollector / TemperatureCollector
日志从 stress_guard 扩展为更清晰的 resource_guard
issues.reason 写明具体原因，比如：ResourcePressureGuard: memory.usage_percent=100
ResourcePressureGuard: disk.volume.2 usage_percent=100 path=E:\


在 AgentApp 增加本地安全停止：
采集到 realtime_metric_v2 后，先检查内存/磁盘是否达到 100%
内存满载时调用现有 AlarmActionExecutor.Execute("stop_memory_stress", taskId)
磁盘满载时调用 AlarmActionExecutor.Execute("stop_disk_stress", taskId)
复用 BurnInTest 当前停止语义，只停止测试项/测试运行，不关闭 BurnInTest 软件窗口
不依赖 Python API、WebSocket 规则下发、MySQL 规则是否正常

做去抖/防重复：
同一个资源满载后短时间内只执行一次停止，避免每秒重复点 Stop
记录日志：RESOURCE_GUARD_TRIGGER
RESOURCE_GUARD_ACTION
RESOURCE_GUARD_ACTION_RESULT


补测试：
CPU 原有保护测试不破坏
新增内存 100% 时跳过硬件传感器的测试
新增磁盘 100% 时跳过硬件传感器的测试
新增本地资源保护触发 stop_memory_stress / stop_disk_stress 的单元测试，尽量复用 mock runner

验证：
编译 agent_core
跑 agent_core_tests
确认现有 BurnInTest 测试仍通过
检查日志中能看到资源保护触发原因
```

