#ifndef _STUDENTSTABLE_H_
#define _STUDENTSTABLE_H_
#include <winsock.h>
#include "mysql.h"
#include "BaseTypes.h"
struct student
{
    int id;
    int classId;
    char name[100];
    char gender;
    int score;
};

struct StudentsTable
{
    bool createTable();
    bool add(const student& s);
    bool remove(const student &s);

private:
    virtual  MYSQL *getHandle() = 0;
};

#endif