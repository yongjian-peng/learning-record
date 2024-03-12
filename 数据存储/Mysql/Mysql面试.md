### 记录
- windows 将 mysql-bin.000001 转成 sql  mysqlbinlog --no-defaults mysql/data/mysql-bin.000001 > output.sql
- 直接查看binlog 内容 show binlog events 对应的sql 文件
- SELECT * FROM performance_schema.data_locks; 查看有哪些锁记录。
- 导入到数据库命令，mysqlbinlog master.000001  --start-position=2738 --stop-position=2973 | mysql -h127.0.0.1 -P13000 -u$user -p$pwd; 