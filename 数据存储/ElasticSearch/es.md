### ElacticSearch
- index 索引， 文档 document row， 字段 field text/keyword/byte 列，映射 Mapping scheam. 查询 DSL, SQL， 分片 sharding 和 副本 replicas 
- DLS 有打分机制，query match (模糊查询，会拆分搜索条件) term (精确匹配), "analyzer":"ik_max_wrod" 不去拆分分词，
- 中文分词 IK 分词器。
- 写入原理，hash 计算 分片 sharding , 写入到对应的主节点和副本节点。分词-》去重-》排序
- 查询，发送到任意节点，协调节点。由协调节点，广播到每一个数据节点。合并数据放入到队列，统一返回结果。
- 商城搜索，比如搜索手机，返回手机的品牌，价格区间，是否有库存，容量。等信息返回
- 配置信息。主机地址，可以使用 master 的节点，