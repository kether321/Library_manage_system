#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#define MAX_BOOKS 100
#define MAX_READERS 50
#define MAX_BORROW 100
#define DATE_LEN 20

// 图书结构
typedef struct {
    char id[20];        // 图书编号
    char name[50];      // 书名
    int total;          // 总数量
    int available;      // 可用数量
} Book;

// 读者结构
typedef struct {
    int id;             // 读者ID
    char name[20];      // 姓名
    int borrowed;       // 当前借阅数量
    int fine;           // 欠费
} Reader;

// 借阅记录结构
typedef struct {
    int reader_id;      // 读者ID
    char book_id[20];   // 图书编号
    char borrow_date[20];  // 借阅日期
    char due_date[20];     // 应还日期
    int overdue_days;   // 超期天数
    int fine;           // 罚款
} BorrowRecord;

// 系统配置
typedef struct {
    int max_borrow;     // 最大借阅数量
    int borrow_days;    // 借阅天数
    int max_renew;      // 最大续借次数
    int fine_per_day;   // 每日罚款
} Config;

// 全局数据
Book books[MAX_BOOKS];
Reader readers[MAX_READERS];
BorrowRecord borrows[MAX_BORROW];
Config config = {5, 30, 1, 10};  // 默认配置

int book_count = 0;
int reader_count = 0;
int borrow_count = 0;

// 函数声明
void show_menu();
void borrow_book();
void return_book();
int days_between(char *date1, char *date2);

int main() {
    int choice;
    
    printf("=== 图书馆管理系统 ===\n");
    
    do {
        show_menu();
        printf("请选择操作: ");
        scanf("%d", &choice);
        
        switch(choice) {
            case 1: borrow_book(); break;
            case 2: return_book(); break;
            case 0: printf("谢谢使用！\n"); break;
            default: printf("无效选择！\n");
        }
    } while(choice != 0);
    
    return 0;
}
// 显示菜单
void show_menu() {
    printf("\n======= 主菜单 =======\n");
    printf("1. 借阅图书\n");
    printf("2. 归还图书\n");
    printf("0. 退出\n");
}

// 借阅图书
void borrow_book() {
    int reader_id;
    char book_id[20];
    
    printf("\n=== 借阅图书 ===\n");
    
    // 输入读者ID
    printf("请输入读者ID: ");
    scanf("%d", &reader_id);
    
    int r_index = find_reader(reader_id);
    if(r_index == -1) {
        printf("读者不存在！\n");
        return;
    }
    
    // 检查读者权限
    if(readers[r_index].fine > 0) {
        printf("读者有欠费 %d 元，请先缴费！\n", readers[r_index].fine);
        return;
    }
    
    if(readers[r_index].borrowed >= config.max_borrow) {
        printf("借阅数量已达上限！(最多 %d 本)\n", config.max_borrow);
        return;
    }
    
    // 输入图书编号
    printf("请输入图书编号: ");
    scanf("%s", book_id);
    
    int b_index = find_book(book_id);
    if(b_index == -1) {
        printf("图书不存在！\n");
        return;
    }
    
    if(books[b_index].available <= 0) {
        printf("图书已全部借出！\n");
        return;
    }
    
    // 检查是否已借阅此书
    if(find_borrow(reader_id, book_id) != -1) {
        printf("已借阅此书，请先归还！\n");
        return;
    }
    
    // 创建借阅记录
    BorrowRecord *record = &borrows[borrow_count];
    record->reader_id = reader_id;
    strcpy(record->book_id, book_id);
    
    get_date(record->borrow_date);
    add_days(record->borrow_date, config.borrow_days, record->due_date);
    
    record->overdue_days = 0;
    record->fine = 0;
    
    // 更新数据
    books[b_index].available--;
    readers[r_index].borrowed++;
    borrow_count++;
    
    printf("\n借阅成功！\n");
    printf("图书: %s\n", books[b_index].name);
    printf("读者: %s\n", readers[r_index].name);
    printf("借阅日期: %s\n", record->borrow_date);
    printf("应还日期: %s\n", record->due_date);
}

// 归还图书
void return_book() {
    int reader_id;
    char book_id[20];
    char return_date[20];
    
    printf("\n=== 归还图书 ===\n");
    
    // 输入读者ID和图书编号
    printf("请输入读者ID: ");
    scanf("%d", &reader_id);
    printf("请输入图书编号: ");
    scanf("%s", book_id);
    
    // 查找借阅记录
    int b_index = find_borrow(reader_id, book_id);
    if(b_index == -1) {
        printf("未找到借阅记录！\n");
        return;
    }
    
    // 输入归还日期
    printf("请输入归还日期(yyyy-mm-dd): ");
    scanf("%s", return_date);
    
    // 计算超期天数和罚款
    BorrowRecord *record = &borrows[b_index];
    int overdue = days_between(record->due_date, return_date);
    
    if(overdue > 0) {
        record->overdue_days = overdue;
        record->fine = overdue * config.fine_per_day;
        
        // 更新读者欠费
        int r_index = find_reader(reader_id);
        if(r_index != -1) {
            readers[r_index].fine += record->fine;
        }
        
        printf("超期 %d 天，罚款 %d 元\n", overdue, record->fine);
    }
    
    // 更新数据
    int book_index = find_book(book_id);
    if(book_index != -1) {
        books[book_index].available++;
    }
    
    int reader_index = find_reader(reader_id);
    if(reader_index != -1) {
        readers[reader_index].borrowed--;
    }
    
    printf("归还成功！\n");
}
// 在日期上加天数
void add_days(char *date, int days, char *result) {
    int year, month, day;
    sscanf(date, "%d-%d-%d", &year, &month, &day);
    
    day += days;
    while(day > 30) {
        day -= 30;
        month++;
        if(month > 12) {
            month = 1;
            year++;
        }
    }
    
    sprintf(result, "%04d-%02d-%02d", year, month, day);
}

// 计算两个日期之间的天数
int days_between(char *date1, char *date2) {
    int y1, m1, d1, y2, m2, d2;
    sscanf(date1, "%d-%d-%d", &y1, &m1, &d1);
    sscanf(date2, "%d-%d-%d", &y2, &m2, &d2);
    int days1 = y1 * 360 + m1 * 30 + d1;
    int days2 = y2 * 360 + m2 * 30 + d2;
    
    int diff = days2 - days1;
    return diff > 0 ? diff : 0;
}