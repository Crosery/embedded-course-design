#include "sqlite_demo.h"

// 回调函数，用于处理查询结果
int callback(void* data, int argc, char** argv, char** azColName) {
    int i;
    printf("记录: ");
    for(i = 0; i < argc; i++) {
        printf("%s = %s  ", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

// 创建数据库 - 简化版本，不重新打开连接
int create_database(const char* db_name) {
    printf("数据库文件: %s 准备就绪\n", db_name);
    return 0;
}

// 创建person表
int create_table(sqlite3* db) {
    char* sql = "CREATE TABLE IF NOT EXISTS person(" \
                "ID INTEGER PRIMARY KEY AUTOINCREMENT," \
                "first_name TEXT NOT NULL," \
                "last_name TEXT NOT NULL);";
    
    char* zErrMsg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
    
    if(rc != SQLITE_OK) {
        fprintf(stderr, "SQL错误: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    } else {
        printf("person表创建成功\n");
    }
    
    return 0;
}

// 插入记录
int insert_record(sqlite3* db, const char* first_name, const char* last_name) {
    char sql[512];
    snprintf(sql, sizeof(sql), "INSERT INTO person (first_name, last_name) VALUES ('%s', '%s');", 
             first_name, last_name);
    
    char* zErrMsg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
    
    if(rc != SQLITE_OK) {
        fprintf(stderr, "插入数据错误: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    } else {
        printf("记录插入成功\n");
    }
    
    return 0;
}

// 批量插入5条记录
int insert_records(sqlite3* db) {
    char* sql = "INSERT INTO person (first_name, last_name) VALUES " \
                "('张', '三')," \
                "('李', '四')," \
                "('王', '五')," \
                "('赵', '六')," \
                "('钱', '七');";
    
    char* zErrMsg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
    
    if(rc != SQLITE_OK) {
        fprintf(stderr, "插入数据错误: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    } else {
        printf("成功插入5条记录\n");
    }
    
    return 0;
}

// 显示所有记录
int show_all_records(sqlite3* db) {
    char* sql = "SELECT * FROM person;";
    char* zErrMsg = 0;
    
    printf("--- 所有记录 ---\n");
    int rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    
    if(rc != SQLITE_OK) {
        fprintf(stderr, "查询错误: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    }
    
    return 0;
}

// 根据ID查询记录
int query_record(sqlite3* db, int id) {
    char sql[256];
    snprintf(sql, sizeof(sql), "SELECT * FROM person WHERE ID = %d;", id);
    
    char* zErrMsg = 0;
    printf("查询ID=%d的记录:\n", id);
    int rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    
    if(rc != SQLITE_OK) {
        fprintf(stderr, "查询错误: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    }
    
    return 0;
}

// 删除指定ID的记录
int delete_record(sqlite3* db, int id) {
    char sql[256];
    snprintf(sql, sizeof(sql), "DELETE FROM person WHERE ID = %d;", id);
    
    char* zErrMsg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
    
    if(rc != SQLITE_OK) {
        fprintf(stderr, "删除错误: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    } else {
        printf("成功删除ID=%d的记录\n", id);
    }
    
    return 0;
}

// 更新记录
int update_record(sqlite3* db, int id, const char* first_name, const char* last_name) {
    char sql[512];
    snprintf(sql, sizeof(sql), "UPDATE person SET first_name='%s', last_name='%s' WHERE ID=%d;", 
             first_name, last_name, id);
    
    char* zErrMsg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
    
    if(rc != SQLITE_OK) {
        fprintf(stderr, "更新错误: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    } else {
        printf("成功更新ID=%d的记录\n", id);
    }
    
    return 0;
}
