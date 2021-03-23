#ifndef _DBINFO_H_
#define _DBINFO_H_

#include "BaseTypes.h"

#define  MAX_LEN_USER_NAME      64      //最长数据库用户名长度
#define  MAX_LEN_PASSWORD       64      //最长密码长度
#define  MAX_LEN_DB_NAME        64      //最长数据库名长度
#define  MAX_LEN_HOST           16      //最长数据库IP字符串长度

struct DBInfo
{
    DBInfo();
    void setUserName(const S8 *user);
    void setPassword(const S8 *password);
    void setDBName(const S8 *name);
	void setHost(const S8 *dbHost);
    void setPort(U16 port);

    S8 userName[MAX_LEN_USER_NAME];  // 登陆用户名
    S8 passWord[MAX_LEN_PASSWORD];   // 密码
    S8 host[MAX_LEN_HOST];           // 数据库服务器地址(字符串IP)
    U16 port;                        // 端口
    S8 dBName[MAX_LEN_DB_NAME];      // 数据库名
};

#endif