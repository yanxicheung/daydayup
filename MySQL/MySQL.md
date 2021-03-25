# 概念：

`SQL` 是用于访问和处理数据库的标准的计算机语言。全称为`Structured Query Language`，即结构化查询语言。



# 准备工作：

可以按照[MySQL 安装教程](https://www.cnblogs.com/zhang1f/p/12985780.html)安装`MySQL`数据库。

数据库连接工具可以使用`HeidiSql`。

在window cmd下的常用操作：

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



常用语句参考：[MySQL 基本语法](https://www.jianshu.com/p/b252f97afed0)



# C++ API：

使用 `MySQL`提供的接口时，只需要包含`mysql.h`这个文件，该文件还包含了其他的头文件，所以项目还需要将其他文件添加进来。

以下是高频使用的C++ API：

```
mysql_init
mysql_real_connect
mysql_close

mysql_query
mysql_real_query
mysql_store_result
mysql_free_result

mysql_fetch_row


mysql_error
```



其他的一些接口：

```
mysql_select_db
mysql_num_rows
```



# 示例：

## main：

```c++
int main(int argc, char **argv)
{
    MySQLDB &db = MySQLDB::getInstance();

    DBInfo dbInfo;
    dbInfo.setDBName("test");
    dbInfo.setHost("localhost");
    dbInfo.setPassword("admin123");
    dbInfo.setPort(3306);
    dbInfo.setUserName("root");
    db.connect(dbInfo);

	db.createDataBase("test");

    db.ROLE(StudentsTable).createTable();
	db.isTableExist("students");

    db.ROLE(StudentsTable).add(Student(11, 1, "daniel", 'M', 90));

    db.ROLE(StudentsTable).modify(Student(11, 1, "daniel", 'M', 95));


	list<Student> students;
	db.ROLE(StudentsTable).getAll(students);
	for (auto &student : students)
	{
		printf("##########");
		student.dump();
		printf("##########");
	}

    db.ROLE(StudentsTable).remove(Student(11, 1, "daniel", 'M', 95));

    db.ROLE(StudentsTable).destoryTable();

	db.disconnect();

    system("pause");
    return 0;
}
```



## MySQLDB：

```c++
struct MySQLDB: private StudentsTable
{
    static MySQLDB& getInstance();
    void disconnect();
    bool connect(const DBInfo &dbInfo);
	bool createDataBase(const char *name);
	bool isTableExist(const char *name);
    IMPL_ROLE(StudentsTable)
private:
    MYSQL *getHandle() override;
private:
    MySQLDB();
    MySQLDB(const MySQLDB&);
    MySQLDB& operator=(const MySQLDB&);
private:
    MYSQL *handle;
	DBInfo dbInfo;
};
```



```c++
MySQLDB::MySQLDB() :handle(NULL)
{
 
}

MySQLDB& MySQLDB::getInstance()
{
    static MySQLDB db;
    return db;
}

MYSQL *MySQLDB::getHandle()
{
    return handle;
}

void MySQLDB::disconnect()
{
	mysql_close(handle);
}

bool MySQLDB::connect(const DBInfo &dbInfo)
{
	this->dbInfo = dbInfo;

    handle = new MYSQL();
    if (handle == NULL)
        return false;

    mysql_init(handle);

    bool failed = (mysql_real_connect(handle, dbInfo.host, dbInfo.userName, dbInfo.passWord, NULL, dbInfo.port, NULL, 0) == NULL);
    if (failed)
    {
        printf("connected database failed, error :%s", mysql_error(handle));
        return false;
    }
    //防止乱码。设置和数据库的编码一致就不会乱码
    mysql_query(handle, "set names gbk");
    return true;
}

bool MySQLDB::createDataBase(const char *name)
{
	S8 sql[1024] = { 0 };
	sprintf(sql, "CREATE DATABASE IF NOT EXISTS %s;", name);
	bool succ = (mysql_real_query(handle, sql, strlen(sql) + 1) == 0);
	if (!succ)
	{
		printf("create database failed, %s\n", mysql_error(handle));
		return false;
	}

	sprintf(sql, "USE %s;", name); 	//后面操作都在这个数据库里面操作;
	succ = (mysql_real_query(handle, sql, strlen(sql) + 1) == 0);
	if (!succ)
	{
		printf("use database failed, %s\n", mysql_error(handle));
		return false;
	}
	return true;
}

bool MySQLDB::isTableExist(const char *name)
{
	S8 sql[1024] = { 0 };
	sprintf(sql, "SELECT 1 FROM %s.%s", dbInfo.dBName, name);
	bool exist = (mysql_real_query(handle, sql, strlen(sql) + 1) == 0);

	// 以释放目前mysql数据库query返回所占用的内存
	MYSQL_RES* results = mysql_store_result(handle);
	mysql_free_result(results);
	return exist;
}
```

## StudentsTable：

```c++
struct Student
{
    Student(int id, int classId, const char* name, char gender, int score)
    {
        this->id = id;
        this->classId = classId;
        strcpy(this->name,name);
        this->gender = gender;
        this->score = score;
    }

	void dump() const
	{
		printf("\nid: %u\n",id);
		printf("class id: %u\n", classId);
		printf("name: %s\n", name);
		printf("gender: %c\n", gender);
		printf("score: %u\n", score);
	}

	void setValue(const MYSQL_ROW& row)
	{
		S32 nIndex = 0;
		this->id = strtoul(row[nIndex++], 0, 10);
		this->classId = strtoul(row[nIndex++], 0, 10);
		strcpy(this->name, row[nIndex++]);
		this->gender = *row[nIndex++];
		this->score = strtoul(row[nIndex], 0, 10);
	}

	Student(){}
    int id;
    int classId;
    char name[100];
    char gender;
    int score;
};

struct StudentsTable
{
    bool createTable();
    bool destoryTable();

    bool add(const Student& s);
    bool remove(const Student& s);
    bool modify(const Student& s);
    bool clear();
	bool getAll(list<Student> &students);
private:
    virtual  MYSQL *getHandle() = 0;
private:
    bool query(const char* sql);
};
```



```c++
bool StudentsTable::query(const char* sql)
{
    MYSQL *handle = getHandle();
    if (handle == NULL)
        return false;

    bool succ = (mysql_query(handle, sql) == 0);
    if (!succ)
    {
        cout << mysql_error(handle) << endl;
    }
    return succ;
}

bool StudentsTable::createTable()
{
    S8 sql[1024] = { 0 };
    sprintf(sql, "CREATE TABLE IF NOT EXISTS students(id BIGINT NOT NULL AUTO_INCREMENT,\
                  class_id BIGINT NOT NULL,\
                  name VARCHAR(100) NOT NULL,\
                  gender VARCHAR(1) NOT NULL,\
                  score INT NOT NULL,\
                  PRIMARY KEY(id));");
    return query(sql);
}

bool StudentsTable::destoryTable()
{
    S8 sql[1024] = { 0 };
    sprintf(sql, "DROP TABLE students");
    return query(sql);
}

bool StudentsTable::add(const Student& s)
{
    S8 sql[1024] = { 0 };
    sprintf(sql, "INSERT INTO students(id, class_id, name, gender, score) VALUES (\
                 %u, %u, '%s', '%c', %u);",\
                 s.id, s.classId, s.name, s.gender, s.score);
    return query(sql);
}

bool StudentsTable::remove(const Student& s)
{
    S8 sql[1024] = { 0 };
    sprintf(sql, "DELETE FROM students WHERE id=%d", s.id);
    return query(sql);
}

bool StudentsTable::modify(const Student& s)
{
    S8 sql[1024] = { 0 };
    sprintf(sql, "UPDATE students SET class_id=%u, name='%s', gender='%c', score=%u WHERE id=%u;",\
            s.classId, s.name, s.gender, s.score, s.id);
    return query(sql);
}

bool StudentsTable::clear()
{
    S8 sql[1024] = { 0 };
    sprintf(sql, "DELETE FROM students;");
    return query(sql);
}


bool StudentsTable::getAll(list<Student> &students)
{
	S8 sql[1024] = { 0 };
	sprintf(sql, "SELECT * FROM students;");

	MYSQL *handle = getHandle();
	if (handle == NULL)
		return false;

	bool succ = (mysql_query(handle, sql) == 0);
	if (!succ)
	{
		cout << mysql_error(handle) << endl;
		return false;
	}

	MYSQL_RES* res = mysql_store_result(handle);
	if (!res)
	{
		return false;
	}

	MYSQL_ROW row;
	Student info;
	while (row = mysql_fetch_row(res))
	{
		info.setValue(row);
		students.push_back(info);
	}
	mysql_free_result(res);
	return true;
}
```



demo代码链接：https://github.com/yanxicheung/daydayup/tree/main/MySQL/demo



# 参考文献：

2. [MySQL修改密码](https://www.cnblogs.com/lqtbk/p/10156981.html)
3. [数据库连接工具介绍](https://www.cnblogs.com/coding400/p/9715882.html)
4. [卸载MySQL数据库](https://blog.csdn.net/weixin_41792162/article/details/89921559)
5. [VS中MFC连接MySQL由于系统不同位（32/64）引起的错误：无法解析的外部符号 _mysql_init@4,_mysql_query,_mysql_error](https://blog.csdn.net/u010385646/article/details/45488675)
6. [C++连接并使用MySQL数据库](https://blog.csdn.net/weixin_43155866/article/details/88837424)
7. [C++操作MYSQL数据库](https://blog.csdn.net/qq_22203741/article/details/79962981)
8. [MySQL 基本语法](https://www.jianshu.com/p/b252f97afed0)