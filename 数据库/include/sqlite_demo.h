#ifndef SQLITE_DEMO_H
#define SQLITE_DEMO_H

#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"
#include <string.h>

// 函数声明
int create_database(const char* db_name);
int create_table(sqlite3* db);
int insert_record(sqlite3* db, const char* first_name, const char* last_name);
int insert_records(sqlite3* db);
int show_all_records(sqlite3* db);
int query_record(sqlite3* db, int id);
int delete_record(sqlite3* db, int id);
int update_record(sqlite3* db, int id, const char* first_name, const char* last_name);
int callback(void* data, int argc, char** argv, char** azColName);

#endif
