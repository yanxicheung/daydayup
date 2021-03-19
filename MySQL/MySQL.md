# 准备工作：

按照参考文献安装`MySQL`数据库和数据库连接工具`HeidiSql`



# 常用操作：

以下操作在window cmd中操作：

```mysql
net start mysql   # 启动服务

net stop mysql    # 停止服务

mysql -u root -p  # 在命令行中连接到数据库

ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password BY '你的新密码';  #修改MySQL密码
```



# C++操作MySQL：

```c++
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <winsock.h> //注意顺序，要放在mysql.h前
#include <mysql.h>   //控制台项目中要在mysql.h之前include <winsock.h>
#include <cstdio>

#if _MSC_VER
#define snprintf _snprintf
#endif

using namespace std;

//记录结构体
typedef struct tagRecord
{
	char    byType;   // 字段数据的类型
	char  name[36];   // 字段的名字
	void *pValue;     // 字段数据的指针
	uint32_t   dwLen;      // 二进制数据的长度
	uint32_t   dwIndexType;// 索引类型
} TRecord;


class MySql
{
public:
	MySql() :pConn(NULL){}

	bool connect(string host, string user, string passwd, unsigned int port)
	{
		pConn = mysql_init(NULL);  // pConn可以理解为数据库的句柄
		if (pConn == NULL)
			return false;

		//如果连接成功，返回MYSQL*连接句柄。如果连接失败，返回NULL
		bool failed = (mysql_real_connect(pConn, host.c_str(), user.c_str(), passwd.c_str(), NULL, port, NULL, 0) == NULL);
		if (failed)
		{
			printf("connected database failed, error :%s", mysql_error(pConn));
			return false;
		}
		//防止乱码。设置和数据库的编码一致就不会乱码
		mysql_query(pConn, "set names gbk");
		return true;
	}

	bool createDataBase(string dbBaseName)
	{
		this->dbBaseName = dbBaseName;

		//若数据库不存在则先创建数据库，若已经存在则打开数据库
		std::string sql = "create database if not exists ";
		sql += dbBaseName;
		bool succ = (mysql_real_query(pConn, sql.c_str(), sql.size() + 1) == 0);
		if (!succ)
		{
			printf("create database failed");
			return false;
		}

		//后面操作都在dbBaseName这个数据库里面操作;
		sql = "use ";
		sql += dbBaseName;
		succ = (mysql_real_query(pConn, sql.c_str(), sql.size() + 1) == 0);
		if (!succ)
		{
			printf("use database failed");
			return false;
		}
		return true;
	}

	bool isTableExist(string tableName)
	{
		snprintf(m_szSql, sizeof(m_szSql), "select 1 from %s.%s", dbBaseName.c_str(), tableName.c_str());
		bool exist = (mysql_real_query(pConn, m_szSql, strlen(m_szSql) + 1) == 0);

		// 以释放目前mysql数据库query返回所占用的内存
		MYSQL_RES* results = mysql_store_result(pConn);
		mysql_free_result(results);
		return exist;
	}

	bool createTable(string tableName);

	void disconnect()
	{
		mysql_close(pConn);
	}

private:
	string dbBaseName;
	MYSQL *pConn;
private:
	char m_szSql[2048];
};

static MySql mySql;
int main(int argc, char **argv)
{
	const char* host = "localhost";
	const char* user = "root";
	const char* passwd = "admin123";
	unsigned int port = 3306;    // 填0默认为3306

	if (!mySql.connect(host, user, passwd, port))
	{
		return -1;
	}

	const char* dbBaseName = "test_test1";  // 数据库名字;

	if (!mySql.createDataBase(dbBaseName))
	{
		return -1;
	}

	const char *tableName = "classes";   // 表名字
	const unsigned int itemNum = 2;
	TRecord aRecord[itemNum] = // 定义表的结构即字段
	{
		/*	{ EDATATYEP_C64, "id", NULL, 0, EINDEX_PRIM },
			{ EDATATYEP_C64, "name", NULL, 0, EINDEX_NO }*/
	};

	mySql.disconnect();
	system("pause");
	return 0;
}
```



# 参考文献：

1. [MySQL 安装教程](https://www.cnblogs.com/zhang1f/p/12985780.html)
2. [MySQL修改密码](https://www.cnblogs.com/lqtbk/p/10156981.html)
3. [数据库连接工具介绍](https://www.cnblogs.com/coding400/p/9715882.html)
4. [卸载MySQL数据库](https://blog.csdn.net/weixin_41792162/article/details/89921559)
5. [VS中MFC连接MySQL由于系统不同位（32/64）引起的错误：无法解析的外部符号 _mysql_init@4,_mysql_query,_mysql_error](https://blog.csdn.net/u010385646/article/details/45488675)
6. [C++连接并使用MySQL数据库](https://blog.csdn.net/weixin_43155866/article/details/88837424)
7. [C++操作MYSQL数据库](https://blog.csdn.net/qq_22203741/article/details/79962981)
8. [MySQL 基本语法](https://www.jianshu.com/p/b252f97afed0)