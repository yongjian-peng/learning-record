###
文档地址：[时序图的语法和功能](https://plantuml.com/zh/sequence-diagram)

```
@startuml


actor user as user
participant "商户" as merchant
participant "渠道" as channel
participant "机构" as inst

autonumber
activate user
user -> merchant: 进件
activate merchant
merchant -> channel: 上传图片
note right
<b><font color="red">商户上传图片接口xxxx</font></b>
end note
activate channel
channel -> inst: 进件
activate inst
inst --> channel
deactivate inst
channel --> merchant
deactivate channel
merchant --> user

@enduml
```

### 实例二
```
@startuml
title 消金直连银行-后端静默签约

actor "数据开发Dorado" as dorado
database "hive" as hive #lightblue
queue "Kafka" as mq #lightblue
participant "二次确认平台" as recon #yellow
participant "KMS" as kms
participant "聚合会员" as member
participant "鉴权交换" as auth #yellow
database "DB" as db #lightblue
participant "统一网关" as gateway
participant "上海建行" as ccb

activate recon
recon --> recon: <p style="color: red;">系统启动</p>
recon -> kms: <p style="color: red;">获取会员AES密钥</p>
activate kms
kms -> recon: <p style="color: red;">会员aes密钥</p>
note right
会员的密钥是：<p style="color: red;">member_secret/memberaessecret/memberAesKey</p>
end note
deactivate kms
recon -> recon: <p style="color: red;">将密钥保存在内存里</p>
|||
deactivate recon


activate dorado
dorado -> dorado: <p style="color: red;">定时任务触发</p>
dorado -> hive: <p style="color: red;">拉取数据</p>
activate hive
hive -> dorado
deactivate hive
dorado -> dorado: <p style="color: red;">清洗数据</p>
loop 数据
  dorado --> mq: <p style="color: red;">发送mq消息\n（控制发送频率）</p>
  activate mq
  mq --> dorado
  deactivate mq
end
deactivate dorado

mq --> recon: <p style="color: red;">mq消息触发</p>
activate mq
activate recon
deactivate mq
recon -> member: <p style="color: red;">查询四要素（加密）</p>
activate member
member -> recon
deactivate member
recon -> recon: <p style="color: red;">使用会员aes_key对四要素进行解密</p>
recon -> member: <p style="color: red;">下签约单</p>
activate member
member -> recon
deactivate member
recon -> auth: <p style="color: red;">签约发短信（携带解密后的四要素）</p>
activate auth
auth -> auth: 落db（非终态）
auth -> gateway: 签约发短信
activate gateway
gateway -> ccb: 签约
activate ccb
ccb -> gateway: 签约结果
deactivate ccb
gateway -> auth: 签约结果
deactivate gateway
auth -> recon: <p style="color: red;">签约结果(携带签约号)</p>
deactivate auth
alt 签约成功
  recon -> member: <p style="color: red;">签约结果（成功）</p>
  activate member
  member -> recon
  deactivate member
else 签约失败
  recon -> recon: <p style="color: red;">打印失败日志</p>
end



@enduml
```