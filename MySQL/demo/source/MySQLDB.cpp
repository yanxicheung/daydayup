#include "MySQLDB.h"
#include <stdio.h>
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

}

bool MySQLDB::connect(const DBInfo &dbInfo)
{
    handle = new MYSQL();
    if (handle == NULL)
        return false;

    mysql_init(handle);

    bool failed = (mysql_real_connect(handle, dbInfo.host, dbInfo.userName, dbInfo.passWord, dbInfo.dBName, dbInfo.port, NULL, 0) == NULL);
    if (failed)
    {
        printf("connected database failed, error :%s", mysql_error(handle));
        return false;
    }
    //防止乱码。设置和数据库的编码一致就不会乱码
    mysql_query(handle, "set names gbk");
    return true;
}
