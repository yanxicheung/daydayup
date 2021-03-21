# 概念：

`SQL` 是用于访问和处理数据库的标准的计算机语言。全称为`Structured Query Language`，即结构化查询语言。



# 准备工作：

可以按照[MySQL 安装教程](https://www.cnblogs.com/zhang1f/p/12985780.html)安装`MySQL`数据库。

数据库连接工具可以使用`HeidiSql`。

以下操作在window cmd中操作：

```mysql
net start mysql   # 启动服务
net stop mysql    # 停止服务
mysql -u root -p  # 在命令行中连接到数据库
ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password BY '你的新密码';  #修改MySQL密码
```



# 语法：

*SQL 对大小写不敏感！*

可以把 SQL 分为两个部分：数据操作语言 (DML) 和 数据定义语言 (DDL)。

查询和更新指令构成了 SQL 的 DML 部分：

- *SELECT* - 从数据库表中获取数据

- *UPDATE* - 更新数据库表中的数据

- *DELETE* - 从数据库表中删除数据

- *INSERT INTO* - 向数据库表中插入数据

  

SQL 的数据定义语言 (DDL) 部分使我们有能力创建或删除表格。我们也可以定义索引（键），规定表之间的链接，以及施加表间的约束。

SQL 中最重要的 DDL 语句:

- *CREATE DATABASE* - 创建新数据库
- *ALTER DATABASE* - 修改数据库
- *CREATE TABLE* - 创建新表
- *ALTER TABLE* - 变更（改变）数据库表
- *DROP TABLE* - 删除表
- *CREATE INDEX* - 创建索引（搜索键）
- *DROP INDEX* - 删除索引





# C++ API：





# 参考文献：

2. [MySQL修改密码](https://www.cnblogs.com/lqtbk/p/10156981.html)
3. [数据库连接工具介绍](https://www.cnblogs.com/coding400/p/9715882.html)
4. [卸载MySQL数据库](https://blog.csdn.net/weixin_41792162/article/details/89921559)
5. [VS中MFC连接MySQL由于系统不同位（32/64）引起的错误：无法解析的外部符号 _mysql_init@4,_mysql_query,_mysql_error](https://blog.csdn.net/u010385646/article/details/45488675)
6. [C++连接并使用MySQL数据库](https://blog.csdn.net/weixin_43155866/article/details/88837424)
7. [C++操作MYSQL数据库](https://blog.csdn.net/qq_22203741/article/details/79962981)
8. [MySQL 基本语法](https://www.jianshu.com/p/b252f97afed0)