#ifndef _STUDENTSTABLE_H_
#define _STUDENTSTABLE_H_
#include <winsock.h>
#include "mysql.h"
#include "BaseTypes.h"
#include <string.h>

struct student
{
    student(int id, int classId, const char* name, char gender, int score)
    {
        this->id = id;
        this->classId = classId;
        strcpy(this->name,name);
        this->gender = gender;
        this->score = score;
    }
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

    bool add(const student& s);
    bool remove(const student& s);
    bool modify(const student& s);
    bool clear();
private:
    virtual  MYSQL *getHandle() = 0;
private:
    bool operate(const char* sql);
};

#endif