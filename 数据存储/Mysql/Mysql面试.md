### 记录
- windows 将 mysql-bin.000001 转成 sql  mysqlbinlog --no-defaults mysql/data/mysql-bin.000001 > output.sql
- 直接查看binlog 内容 show binlog events 对应的sql 文件