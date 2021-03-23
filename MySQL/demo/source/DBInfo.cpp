#include "DBInfo.h"
#include <string.h>
#define STRNCPY(dst,src,sz) strncpy_s(dst,src,sz)

DBInfo::DBInfo()
{
    memset(this, 0, sizeof(DBInfo));
}

void DBInfo::setUserName(const S8 *user)
{
    STRNCPY(userName, user, sizeof(userName));
}

void DBInfo::setPassword(const S8 *password)
{
    STRNCPY(passWord, password, sizeof(passWord));
}

void DBInfo::setDBName(const S8 *name)
{
    STRNCPY(dBName, name, sizeof(dBName));
}

void DBInfo::setHost(const S8 *dbHost)
{
    STRNCPY(host, dbHost, sizeof(host));
}

void DBInfo::setPort(U16 port)
{
    this->port = port;
}