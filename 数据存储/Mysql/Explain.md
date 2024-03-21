### Explain 详解
- 引用：https://zhuanlan.zhihu.com/p/623125075?utm_id=0

列名	描述
id	在一个大的查询语句中每个SELECT关键字都对应着一个唯一的id
select_type	SELECT关键字对应查询的类型
table	表名
partitions	匹配的分区信息
type	针对单表的访问方法
possible_keys	可能使用到的索引
key	实际使用的索引
key_len	实际使用到的索引长度
ref	当使用索引列等值查询的时候，与索引列进行等值匹配的对象信息
rows	预估需要读取的记录条数
filtered	某个表经过搜索条件过滤后剩余记录条数的百分比
Extra	一些额外的信息


完整的访问方法如下：system > const > eq_ref > ref > fulltext > ref_or_null > index_merge > unique_subquery > index_subquery > range > index > all，越靠前代表效率越高

#### 创建表
```bash
CREATE TABLE s1 (
	id INT AUTO_INCREMENT,
	key1 VARCHAR(100),
	key2 INT,
	key3 VARCHAR(100),
	key_part1 VARCHAR(100),
	key_part2 VARCHAR(100),
	key_part3 VARCHAR(100),
	common_field VARCHAR(100),
	PRIMARY KEY (id),
	INDEX idx_key1 (key1),
	UNIQUE INDEX idx_key2(key2),
	INDEX idx_key3(key3),
	INDEX idx_key_part(key_part1, key_part2, key_part3)
)ENGINE=INNODB CHARSET=utf8


CREATE TABLE s2 (
	id INT AUTO_INCREMENT,
	key1 VARCHAR(100),
	key2 INT,
	key3 VARCHAR(100),
	key_part1 VARCHAR(100),
	key_part2 VARCHAR(100),
	key_part3 VARCHAR(100),
	common_field VARCHAR(100),
	PRIMARY KEY (id),
	INDEX idx_key1 (key1),
	UNIQUE INDEX idx_key2(key2),
	INDEX idx_key3(key3),
	INDEX idx_key_part(key_part1, key_part2, key_part3)
)ENGINE=INNODB CHARSET=utf8

```

### 创建存储函数
```bash
-- 函数返回随机字符串
DELIMITER //

CREATE FUNCTION `rand_string`(n INT) RETURNS varchar(255) CHARSET utf8mb4
BEGIN 
	DECLARE chars_str VARCHAR(100) DEFAULT 'abcdefghijklmnopqrstuvwxyzABCDEFJHIJKLMNOPQRSTUVWXYZ';
	DECLARE return_str VARCHAR(255) DEFAULT '';
	DECLARE i INT DEFAULT 0;
	WHILE i < n DO 
       SET return_str =CONCAT(return_str,SUBSTRING(chars_str,FLOOR(1+RAND()*52),1));
       SET i = i + 1;
    END WHILE;
    RETURN return_str;
END //
DELIMITER ;
```

- 首先要确保相信函数的变量
- log_bin_trust_function_creators为1

- SELECT @@log_bin_trust_function_creators variable;

- SET GLOBAL log_bin_trust_function_creators = 1;

### 存储过程
```bash
DELIMITER //
CREATE PROCEDURE insert_s1 (IN min_num INT (10), IN max_num INT(10))
BEGIN
	DECLARE i INT DEFAULT 0;
	SET autocommit = 0;
	REPEAT
	SET i = i + 1;
	INSERT INTO s1 VALUES(
		(min_num + i),
		rand_string(6),
		(min_num + 30* i + 5),
		rand_string(6),
		rand_string(10),
		rand_string(5),
		rand_string(10),
		rand_string(10)
	);
	UNTIL i = max_num
	END REPEAT;
	COMMIT;
END //
DELIMITER;



DELIMITER //
CREATE PROCEDURE insert_s2 (IN min_num INT (10), IN max_num INT(10))
BEGIN
	DECLARE i INT DEFAULT 0;
	SET autocommit = 0;
	REPEAT
	SET i = i + 1;
	INSERT INTO s2 VALUES(
		(min_num + i),
		rand_string(6),
		(min_num + 30* i + 5),
		rand_string(6),
		rand_string(10),
		rand_string(5),
		rand_string(10),
		rand_string(10)
	);
	UNTIL i = max_num
	END REPEAT;
	COMMIT;
END //
DELIMITER;
```

### 执行存储过程添加数据
```bash
CALL insert_s1(10001, 10000);
CALL insert_s2(10001, 10000);
```


EXPLAIN SELECT * FROM s1

EXPLAIN SELECT * FROM s1 INNER JOIN s2

EXPLAIN SELECT * FROM s1 WHERE key1 IN (SELECT key1 FROM s2) OR key3 = 'a'

EXPLAIN SELECT * FROM s1 WHERE key1 IN (SELECT key2 FROM s2 WHERE common_field = 'a')

EXPLAIN SELECT * FROM s1 WHERE key1 IN (SELECT key1 FROM s2 WHERE common_field = 'a')

EXPLAIN SELECT * FROM s1 UNION SELECT * FROM s2;

EXPLAIN SELECT * FROM s1 UNION ALL SELECT * FROM s2;

explain select * from s1 where key1 in (select key1 from s2) or key3 = 'a'; # SUBQUERY

explain select * from s1 INNER JOIN s2 on s1.key1 = s2.key1;

############## type ###############

EXPLAIN SELECT * FROM s1 WHERE id = 10005; EXPLAIN SELECT * FROM s1 WHERE key2 = 10066; # const

EXPLAIN SELECT * FROM s1 WHERE key3 = 1006; # ALL

EXPLAIN SELECT * from s1 INNER JOIN s2 WHERE s1.key2 = s2.key2; # eq_ref

EXPLAIN SELECT * FROM s1 WHERE key3 = 'CUTLVwqweqweq'; # ref

EXPLAIN SELECT * FROM s1 WHERE key3 = 'CUTLVwqweqweq' OR key3 IS NULL; # ref_or_null

EXPLAIN SELECT * FROM s1 WHERE key1 = 'a' OR key2 = 123131; # index_merge

EXPLAIN SELECT * FROM s1 WHERE key1 = 'rCLXEg' AND key2 = 10036; # ??

EXPLAIN SELECT * FROM s1 WHERE key2 IN (SELECT id FROM s2 WHERE s1.key1 = s2.key1) OR key3 = 'a'; # unique_subquery

EXPLAIN SELECT * FROM s1 WHERE key1 IN ('a', 'b', 'c'); # range

EXPLAIN SELECT key_part2 FROM s1 WHERE key_part3 = 'a'; # index

############## select_type ###############

explain select * from s1 where key1 in (select key1 from s2) or key3 = 'a'; # SUBQUERY

explain select * from s1 INNER JOIN s2 on s1.key1 = s2.key1; 

EXPLAIN SELECT * FROM s1 WHERE key1 IN (SELECT key1 from s2 WHERE s1.key2 = s2.key2) OR key3 = 'a'; # DEPENDENT SUBQUERY

EXPLAIN SELECT * FROM s1 WHERE key1 IN (SELECT key1 FROM s2 WHERE key1 = 'a' UNION SELECT key1 FROM s1 WHERE key1 = 'b'); # DEPENDENT UNION

EXPLAIN SELECT * FROM (SELECT key1, count(*) AS c FROM s1 GROUP BY key1) AS derived_s1 WHERE c > 1; # DERIVED

EXPLAIN SELECT * FROM s1 WHERE key1 IN (SELECT key1 FROM s2); # MATERIALIZED

EXPLAIN SELECT key1, key_part2 FROM s1 WHERE key_part3 = 'a'; # ALL


##############  Extra ######################
EXPLAIN select 1 # no tables used

EXPLAIN select * FROM s1 WHERE 1 != 1; # impossible where

EXPLAIN select * FROM s1 WHERE common_field = 'a'; # where

EXPLAIN SELECT MIN(key1) FROM s1 WHERE key1 = 'adqwdqweqwe'; # no matching min/max row

EXPLAIN SELECT MIN(key1) FROM s1; # Select tables optimized away

EXPLAIN SELECT key1 FROM s1 WHERE key1 = 'a'; # using index

EXPLAIN SELECT * FROM s1 WHERE key1 > 'z' AND key1 like '%a%'; # using index condition

EXPLAIN SELECT * FROM s1 INNER JOIN s2 ON s1.common_field = s2.common_field; # using join buffer

EXPLAIN SELECT * FROM s1 LEFT JOIN s2 on s1.key1 = s2.key1 WHERE s2.id IS NULL; # not exists

EXPLAIN SELECT * FROM s1 WHERE key1 = 'a' OR key3 = 'a'; # using union(index_merge)

EXPLAIN SELECT * FROM s1 ORDER BY key1 LIMIT 10; #

EXPLAIN SELECT * FROM s1 ORDER BY key1 LIMIT 97; # Using filesort

EXPLAIN SELECT * FROM s1 ORDER BY common_field LIMIT 10; # Using filesort

EXPLAIN SELECT DISTINCT common_field FROM s1; #  Using temporary

EXPLAIN SELECT * FROM s1 INNER JOIN s2 on s1.key1 = s2.key2 WHERE s1.common_field = 'a'; # Using index condition  Using where


EXPLAIN FORMAT=JSON SELECT * FROM s1 INNER JOIN s2 on s1.key1 = s2.key2 WHERE s1.common_field = 'a';

select version();

EXPLAIN FORMAT=TREE SELECT * FROM s1 INNER JOIN s2 on s1.key1 = s2.key2 WHERE s1.common_field = 'a'; 