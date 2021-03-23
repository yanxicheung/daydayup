#include <iostream>
#include <winsock.h> //注意顺序，要放在mysql.h前
#include <mysql.h>   //控制台项目中要在mysql.h之前include <winsock.h>
#include "MySQLDB.h"

using namespace std;

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