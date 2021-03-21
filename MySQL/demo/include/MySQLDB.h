#ifndef _MYSQLDB_H_
#define _MYSQLDB_H_
#include <winsock.h>
#include "mysql.h"
#include "DBInfo.h"
#include "StudentsTable.h"
#include "Role.h"

struct MySQLDB: private StudentsTable
{
    static MySQLDB& getInstance();
    void disconnect();
    bool connect(const DBInfo &dbInfo);

    IMPL_ROLE(StudentsTable)
private:
    MYSQL *getHandle() override;
private:
    MySQLDB();
    MySQLDB(const MySQLDB&);
    MySQLDB& operator=(const MySQLDB&);
private:
    MYSQL *handle;
};

#endif