#ifndef _STUDENTSTABLE_H_
#define _STUDENTSTABLE_H_
#include <winsock.h>
#include "mysql.h"
#include "BaseTypes.h"
#include <string.h>
#include <list>
using namespace std;

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

#endif