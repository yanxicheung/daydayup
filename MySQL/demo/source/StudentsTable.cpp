#include "StudentsTable.h"
#include <stdio.h>
#include <iostream>
using namespace std;

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
