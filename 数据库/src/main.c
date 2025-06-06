#include "sqlite_demo.h"
#include <signal.h>
#include <unistd.h>

// 段错误信号处理函数
void segfault_handler(int sig) {
    printf("捕获到段错误信号: %d\n", sig);
    printf("程序异常终止\n");
    exit(1);
}

void show_menu() {
    printf("\n=== SQLite数据库操作菜单 ===\n");
    printf("1. 创建数据库和表\n");
    printf("2. 插入单条记录\n");
    printf("3. 批量插入5条记录\n");
    printf("4. 显示所有记录\n");
    printf("5. 根据ID查询记录\n");
    printf("6. 更新记录\n");
    printf("7. 删除记录\n");
    printf("0. 退出程序\n");
    printf("请选择操作: ");
}

int main() {
    // 注册段错误信号处理器
    signal(SIGSEGV, segfault_handler);
    
    sqlite3* db = NULL;
    const char* db_name = "person.db";
    int choice, id, rc;
    char first_name[100], last_name[100];
    
    printf("=== SQLite数据库操作演示程序 ===\n");
    
    // 打开数据库连接
    rc = sqlite3_open(db_name, &db);
    if(rc) {
        fprintf(stderr, "无法打开数据库: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    printf("数据库连接成功\n");
    
    while(1) {
        show_menu();
        scanf("%d", &choice);
        
        switch(choice) {
            case 1: // 创建数据库和表
                printf("正在创建表...\n");
                if(create_table(db) == 0) {
                    printf("表创建成功\n");
                } else {
                    printf("表创建失败\n");
                }
                break;
                
            case 2: // 插入单条记录
                printf("请输入姓氏: ");
                scanf("%s", first_name);
                printf("请输入名字: ");
                scanf("%s", last_name);
                insert_record(db, first_name, last_name);
                break;
                
            case 3: // 批量插入5条记录
                printf("正在批量插入5条记录...\n");
                insert_records(db);
                break;
                
            case 4: // 显示所有记录
                show_all_records(db);
                break;
                
            case 5: // 根据ID查询记录
                printf("请输入要查询的ID: ");
                scanf("%d", &id);
                query_record(db, id);
                break;
                
            case 6: // 更新记录
                printf("请输入要更新的ID: ");
                scanf("%d", &id);
                printf("请输入新的姓氏: ");
                scanf("%s", first_name);
                printf("请输入新的名字: ");
                scanf("%s", last_name);
                update_record(db, id, first_name, last_name);
                break;
                
            case 7: // 删除记录
                printf("请输入要删除的ID: ");
                scanf("%d", &id);
                delete_record(db, id);
                break;
                
            case 0: // 退出程序
                printf("程序退出\n");
                goto exit;
                
            default:
                printf("无效选择，请重新输入\n");
                break;
        }
    }
    
exit:
    // 关闭数据库连接
    if(db) {
        sqlite3_close(db);
        printf("数据库连接已关闭\n");
    }
    
    return 0;
}
