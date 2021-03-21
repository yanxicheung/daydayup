#include "StudentsTable.h"

bool StudentsTable::createTable()
{
    MYSQL *handle = getHandle();
    if (handle == NULL)
        return false;
    bool succ = (mysql_query(handle, "SELECT id,class_id,name,gender,score FROM students;") == 0);
    S32 count = mysql_num_rows(mysql_store_result(handle));
    return true;
}