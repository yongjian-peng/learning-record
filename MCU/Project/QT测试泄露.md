# 项目有：

```
-DWITH_WEBENGINE=OFF -DFETCHCONTENT_SOURCE_DIR_ZLIB=E:/WorkSpace/QT-Github/thirdparty/zlib-src -DFETCHCONTENT_SOURCE_DIR_EXPAT=E:/WorkSpace/QT-Github/thirdparty/expat-src

曲线绘制图表。
压力信号同步显示在触摸屏及工控电脑上，绘制曲线。

工控机保存测试数据，同时打印机打印序列号标签

试验数据可以转存Excel, pdf格式，可查询历史数据


1. 测试软件安装到工控机上。
2. 图标曲线图。

git@192.168.31.224:8008:qt/leak.git



阅读这个项目，参考我给你发的这个项目，
1. 这个项目，使用QT 技术实现，分析使用哪些技术实现,结合使用QMI 技术方案，给出架构设计.技术架构分析。以及使用哪个框架合适。给出架构设计。解耦，拆解模块。
2. 结合文档需求。编写自测代码和业务逻辑代码。给出可以运行的代码。
按照这个项目风格。按照中型和大型项目思想编写。
3. 环境：Qt 5.15.2 MinGW，安装路径 C:\Qt（实际映射到 F:\Qt）。使用的构建设置 F:\githup-projection\1\qskinny-build\build\Desktop_Qt_6_10_2_MinGW_64_bit-Debug 构建 Kit 用的是 Desktop Qt6.10.2 MinGW 64-bit, 需要的 cmake 也在 E 盘目录中F:\Qt\Tools\CMake_64\bin(实际映射的目录)不用 C 盘找编译工具。
4. 目前已经成功运行 demo 项目 qskinny 
5. 在目录 F:\githup-projection\qskinny-master\examples 中创建一个新的目录，直接编写需求。不用其他的地方新建一个文件夹编写，不用担心污染，修改错误，大胆的修改代码。效果是，能在基础上运行这个代码,要符合项目的需求
6. 使用样式，修改页面。美化页面,记得要加需求里的曲线图
整理好的整理一个可编译运行的完整工程目录结构、CMake 配置和需要新增的全部源码文件清单：
标准的 Qt6 + QSkinny + PLC/CANOpen + SQLite 工业软件架构。

UI扩展方便
PLC接入方便
CANOpen接入方便
数据追溯方便
后期MES对接方便
一、最终工程结构
LeakTestSystem
│
├── CMakeLists.txt
├── README.md
│
├── 3rdparty
│   ├── qskinny
│   ├── sqlite
│   └── canopen
│
├── resources
│   ├── icons
│   ├── images
│   ├── skins
│   └── qml
│
├── database
│   └── leaktest.db
│
├── src
│
│   ├── app
│   │   ├── Application.cpp
│   │   ├── Application.h
│   │   ├── Bootstrap.cpp
│   │   └── Bootstrap.h
│
│   ├── core
│   │
│   │   ├── domain
│   │   │
│   │   │   ├── Recipe.h
│   │   │   ├── Recipe.cpp
│   │   │
│   │   │   ├── WorkOrder.h
│   │   │   ├── WorkOrder.cpp
│   │   │
│   │   │   ├── TestResult.h
│   │   │   ├── TestResult.cpp
│   │   │
│   │   │   ├── SensorValue.h
│   │   │   └── SensorValue.cpp
│   │
│   │   ├── service
│   │   │
│   │   │   ├── TestService.h
│   │   │   ├── TestService.cpp
│   │   │
│   │   │   ├── RecipeService.h
│   │   │   ├── RecipeService.cpp
│   │   │
│   │   │   ├── ReportService.h
│   │   │   └── ReportService.cpp
│   │
│   │   └── stateMachine
│   │
│   │       ├── TestStateMachine.h
│   │       ├── TestStateMachine.cpp
│   │       │
│   │       ├── IdleState.cpp
│   │       ├── FillingState.cpp
│   │       ├── HoldState.cpp
│   │       ├── LeakCheckState.cpp
│   │       ├── PassState.cpp
│   │       └── FailState.cpp
│
│   ├── device
│   │
│   │   ├── plc
│   │   │
│   │   ├── PlcClient.h
│   │   ├── PlcClient.cpp
│   │   │
│   │   ├── SiemensS7Client.cpp
│   │   ├── SiemensS7Client.h
│   │
│   │   ├── canopen
│   │   │
│   │   ├── CanOpenMaster.h
│   │   ├── CanOpenMaster.cpp
│   │   │
│   │   ├── PressureSensorNode.h
│   │   └── PressureSensorNode.cpp
│   │
│   │   ├── sensor
│   │   │
│   │   ├── TemperatureSensor.h
│   │   ├── TemperatureSensor.cpp
│   │
│   │   └── valve
│   │
│   │       ├── DiaphragmValve.h
│   │       └── DiaphragmValve.cpp
│
│   ├── repository
│   │
│   │   ├── DbManager.h
│   │   ├── DbManager.cpp
│   │
│   │   ├── RecipeRepository.h
│   │   ├── RecipeRepository.cpp
│   │
│   │   ├── ResultRepository.h
│   │   └── ResultRepository.cpp
│
│   ├── ui
│   │
│   │   ├── mainwindow
│   │   │
│   │   ├── MainWindow.h
│   │   └── MainWindow.cpp
│   │
│   │   ├── pages
│   │   │
│   │   ├── HomePage.h
│   │   ├── HomePage.cpp
│   │
│   │   ├── RecipePage.h
│   │   ├── RecipePage.cpp
│   │
│   │   ├── RecipeEditPage.h
│   │   ├── RecipeEditPage.cpp
│   │
│   │   ├── TestPage.h
│   │   ├── TestPage.cpp
│   │
│   │   ├── HistoryPage.h
│   │   ├── HistoryPage.cpp
│   │
│   │   ├── AlarmPage.h
│   │   └── AlarmPage.cpp
│   │
│   │   ├── widgets
│   │   │
│   │   ├── CurveChartWidget.h
│   │   ├── CurveChartWidget.cpp
│   │
│   │   ├── PressureGauge.h
│   │   ├── PressureGauge.cpp
│   │
│   │   ├── StatusIndicator.h
│   │   └── StatusIndicator.cpp
│   │
│   │   └── dialogs
│   │
│   │       ├── LoginDialog.h
│   │       ├── LoginDialog.cpp
│   │
│   │       ├── StopReasonDialog.h
│   │       └── StopReasonDialog.cpp
│
│   ├── report
│   │
│   │   ├── PdfExporter.h
│   │   ├── PdfExporter.cpp
│   │
│   │   ├── ExcelExporter.h
│   │   └── ExcelExporter.cpp
│
│   └── main.cpp
│
└── tests
    ├── unit
    └── integration
二、主界面模块
对应PPT要求
MainWindow
MainWindow
 ├─ HomePage
 ├─ RecipePage
 ├─ TestPage
 ├─ HistoryPage
 └─ AlarmPage
采用：QskStackBox
页面切换。
三、主页(HomePage)
对应PPT首页
包含：设备名称，系统时间，工单号，物料号，测试配方，开始测试，参数设置，历史记录，报警信息
PPT要求：
四、配方管理
RecipePage
显示：QskListView
内容：配方名称，压力表型号，输入压力
泄漏限值
PRV开启
PRV关闭
测试时间

支持：

新增
修改
删除
查询

对应PPT：

五、测试页面
TestPage

工业大屏风格：

左侧：

环境温度
输入压力
压差
实际测试时间

中间：

实时曲线

右侧：

合格
不合格
继续测试
停止测试
打印标签

对应PPT：

六、实时曲线组件

新增：

CurveChartWidget

内部使用：

QtCharts

三条曲线：

temperatureSeries
pressureSeries
deltaPressureSeries

采样：

2s

PPT明确要求：

七、测试状态机

现有项目保留并扩展

Idle
 ↓
FillPressure
 ↓
PRVCheck
 ↓
Holding
 ↓
LeakTesting
 ↓
Pass
 ↓
Finish

失败：

Fail

随时：

EmergencyStop
八、PLC控制

抽象接口

class PlcClient
{
public:

virtual void openValve() = 0;

virtual void closeValve() = 0;

virtual bool pressureReady() = 0;
};

实现：

SiemensS7Client

后期可扩展：

ModbusTcpClient
OmronFinsClient
九、CANOpen压力传感器

根据PPT

初级表：

100bar × 6

次级表：

10bar × 2

设计：

PressureSensorNode

功能：

SDO读取

PDO订阅

在线检测

故障检测
十、数据库设计
recipe
CREATE TABLE recipe
(
id INTEGER PRIMARY KEY,

name TEXT,

gaugeType TEXT,

inputPressure REAL,

leakLimit REAL,

prvOpen REAL,

prvClose REAL,

testTime INTEGER
);
test_result
CREATE TABLE test_result
(
id INTEGER PRIMARY KEY,

serialNo TEXT,

workOrder TEXT,

materialNo TEXT,

recipeName TEXT,

temperature REAL,

pressure REAL,

deltaPressure REAL,

result INTEGER,

startTime TEXT,

endTime TEXT
);
sample_data

保存曲线

CREATE TABLE sample_data
(
id INTEGER PRIMARY KEY,

resultId INTEGER,

sampleTime INTEGER,

temperature REAL,

pressure REAL,

deltaPressure REAL
);
十一、导出功能

对应PPT要求：

PDF
PdfExporter

生成：

测试报告
曲线图
结果
签名
Excel
ExcelExporter

导出：

历史测试记录

推荐：

QXlsx
十二、标签打印

新增：

LabelPrinter

支持：

Zebra
TSC
Godex

输出：

SN
工单号
测试结果
日期
十三、推荐CMake配置

根目录：

cmake_minimum_required(VERSION 3.22)

project(LeakTestSystem)

set(CMAKE_CXX_STANDARD 20)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Sql
    Charts
    PrintSupport
)

find_package(QSkinny REQUIRED)

add_subdirectory(src)

src/CMakeLists.txt

qt_add_executable(LeakTestSystem
    main.cpp

    app/Application.cpp

    core/service/TestService.cpp

    device/plc/SiemensS7Client.cpp

    repository/DbManager.cpp

    ui/mainwindow/MainWindow.cpp

    ui/pages/HomePage.cpp
    ui/pages/TestPage.cpp
    ui/pages/RecipePage.cpp
    ui/pages/HistoryPage.cpp
)

target_link_libraries(LeakTestSystem
PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Sql
    Qt6::Charts
    Qt6::PrintSupport
    QSkinny
)
第一阶段建议实现顺序

优先实现以下 12 个核心类即可运行：

Application
MainWindow

HomePage
RecipePage
TestPage
HistoryPage

CurveChartWidget

RecipeRepository
ResultRepository

TestService

PressureSensorNode

SiemensS7Client

完成后即可形成：

主界面
配方管理
实时曲线
历史记录
SQLite存储
PLC通信接口
CANOpen采集接口


```

