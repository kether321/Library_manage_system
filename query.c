#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_BOOKS 1000
#define MAX_READERS 500
#define MAX_BORROW_RECORDS 2000
#define MAX_CATEGORIES 10
#define MAX_STRING_LENGTH 100
#define MAX_ID_LENGTH 20

// 数据结构定义
typedef struct {
    char book_id[MAX_ID_LENGTH];
    char title[MAX_STRING_LENGTH];
    char author[MAX_STRING_LENGTH];
    char category[MAX_STRING_LENGTH];
    char isbn[MAX_ID_LENGTH];
    int total_copies;        // 馆藏总量
    int available_copies;    // 可借数量
    int borrow_count;        // 借阅次数
    int publication_year;
} Book;

typedef struct {
    char reader_id[MAX_ID_LENGTH];
    char name[MAX_STRING_LENGTH];
    char department[MAX_STRING_LENGTH];
    char phone[MAX_STRING_LENGTH];
    int max_borrow_limit;
    int current_borrowed;
    double fine_amount;      // 未缴罚款
} Reader;

typedef struct {
    char record_id[MAX_ID_LENGTH];
    char book_id[MAX_ID_LENGTH];
    char reader_id[MAX_ID_LENGTH];
    char borrow_date[11];    // YYYY-MM-DD格式
    char due_date[11];       // 应还日期
    char return_date[11];    // 实际归还日期，空表示未还
    int is_overdue;          // 是否超期
    double fine;             // 产生的罚款
} BorrowRecord;

typedef struct {
    char category_name[MAX_STRING_LENGTH];
    int book_count;
    int borrow_count;
} CategoryStat;

// 全局变量
Book books[MAX_BOOKS];
Reader readers[MAX_READERS];
BorrowRecord records[MAX_BORROW_RECORDS];
CategoryStat categories[MAX_CATEGORIES];

int book_count = 0;
int reader_count = 0;
int record_count = 0;
int category_count = 0;

// 当前登录用户信息
char current_user_id[MAX_ID_LENGTH];
int is_admin = 0;  // 0: 读者, 1: 管理员

// 函数声明
void init_system();
void login_menu();
void reader_menu();
void admin_menu();

// 读者端功能
void search_books();
void view_borrow_records();
void view_current_borrows();
void view_overdue_reminders();
void view_payment_records();
void search_personal_info();

// 管理员端功能
void admin_search_books();
void view_collection_stats();
void view_category_stats();
void view_borrow_rate();
void view_overdue_rate();
void generate_statistics_report();

// 工具函数
int calculate_overdue_days(const char* due_date);
void get_current_date(char* date_str);
int date_compare(const char* date1, const char* date2);
void clear_input_buffer();
int find_book_by_id(const char* book_id);
int find_reader_by_id(const char* reader_id);
void load_sample_data();

// 初始化系统
void init_system() {
    // 初始化分类
    strcpy(categories[0].category_name, "计算机科学");
    strcpy(categories[1].category_name, "文学");
    strcpy(categories[2].category_name, "历史");
    strcpy(categories[3].category_name, "自然科学");
    strcpy(categories[4].category_name, "艺术");
    category_count = 5;
    
    load_sample_data();
}

// 登录菜单
void login_menu() {
    int choice;
    
    while (1) {
        printf("\n======= 图书借阅管理系统 =======\n");
        printf("1. 读者登录\n");
        printf("2. 管理员登录\n");
        printf("3. 退出系统\n");
        printf("===============================\n");
        printf("请选择: ");
        scanf("%d", &choice);
        clear_input_buffer();
        
        switch (choice) {
            case 1:
                printf("请输入读者ID: ");
                fgets(current_user_id, MAX_ID_LENGTH, stdin);
                current_user_id[strcspn(current_user_id, "\n")] = 0;
                
                if (find_reader_by_id(current_user_id) != -1) {
                    is_admin = 0;
                    reader_menu();
                } else {
                    printf("读者ID不存在！\n");
                }
                break;
                
            case 2:
                printf("请输入管理员密码: ");
                char password[MAX_STRING_LENGTH];
                fgets(password, MAX_STRING_LENGTH, stdin);
                password[strcspn(password, "\n")] = 0;
                
                // 简单密码验证
                if (strcmp(password, "admin123") == 0) {
                    is_admin = 1;
                    strcpy(current_user_id, "admin");
                    admin_menu();
                } else {
                    printf("密码错误！\n");
                }
                break;
                
            case 3:
                printf("感谢使用，再见！\n");
                exit(0);
                
            default:
                printf("无效选择！\n");
        }
    }
}

// 读者端菜单
void reader_menu() {
    int choice;
    
    while (1) {
        printf("\n======= 读者服务菜单 =======\n");
        printf("1. 图书查询\n");
        printf("2. 个人借阅记录\n");
        printf("3. 待还图书查询\n");
        printf("4. 超期提醒\n");
        printf("5. 缴纳记录查询\n");
        printf("6. 个人信息查询\n");
        printf("7. 返回上级菜单\n");
        printf("===========================\n");
        printf("请选择: ");
        scanf("%d", &choice);
        clear_input_buffer();
        
        switch (choice) {
            case 1:
                search_books();
                break;
            case 2:
                view_borrow_records();
                break;
            case 3:
                view_current_borrows();
                break;
            case 4:
                view_overdue_reminders();
                break;
            case 5:
                view_payment_records();
                break;
            case 6:
                search_personal_info();
                break;
            case 7:
                return;
            default:
                printf("无效选择！\n");
        }
    }
}

// 管理员端菜单
void admin_menu() {
    int choice;
    
    while (1) {
        printf("\n======= 管理员服务菜单 =======\n");
        printf("1. 图书查询\n");
        printf("2. 馆藏统计\n");
        printf("3. 分类统计\n");
        printf("4. 借阅率统计\n");
        printf("5. 超期率统计\n");
        printf("6. 生成统计报表\n");
        printf("7. 返回上级菜单\n");
        printf("============================\n");
        printf("请选择: ");
        scanf("%d", &choice);
        clear_input_buffer();
        
        switch (choice) {
            case 1:
                admin_search_books();
                break;
            case 2:
                view_collection_stats();
                break;
            case 3:
                view_category_stats();
                break;
            case 4:
                view_borrow_rate();
                break;
            case 5:
                view_overdue_rate();
                break;
            case 6:
                generate_statistics_report();
                break;
            case 7:
                return;
            default:
                printf("无效选择！\n");
        }
    }
}

// 读者端：图书查询
void search_books() {
    int choice;
    char search_term[MAX_STRING_LENGTH];
    int found = 0;
    
    printf("\n--- 图书查询 ---\n");
    printf("1. 按书名查询\n");
    printf("2. 按作者查询\n");
    printf("3. 按分类查询\n");
    printf("4. 按ISBN查询\n");
    printf("请选择查询方式: ");
    scanf("%d", &choice);
    clear_input_buffer();
    
    printf("请输入查询关键词: ");
    fgets(search_term, MAX_STRING_LENGTH, stdin);
    search_term[strcspn(search_term, "\n")] = 0;
    
    printf("\n查询结果:\n");
    printf("%-12s %-30s %-20s %-15s %-10s %-10s\n", 
           "图书ID", "书名", "作者", "分类", "馆藏量", "可借数");
    printf("--------------------------------------------------------------------------------\n");
    
    for (int i = 0; i < book_count; i++) {
        int match = 0;
        
        switch (choice) {
            case 1:
                if (strstr(books[i].title, search_term) != NULL) match = 1;
                break;
            case 2:
                if (strstr(books[i].author, search_term) != NULL) match = 1;
                break;
            case 3:
                if (strstr(books[i].category, search_term) != NULL) match = 1;
                break;
            case 4:
                if (strcmp(books[i].isbn, search_term) == 0) match = 1;
                break;
        }
        
        if (match) {
            printf("%-12s %-30s %-20s %-15s %-10d %-10d\n",
                   books[i].book_id,
                   books[i].title,
                   books[i].author,
                   books[i].category,
                   books[i].total_copies,
                   books[i].available_copies);
            found = 1;
        }
    }
    
    if (!found) {
        printf("未找到相关图书！\n");
    }
}

// 读者端：查询个人借阅记录
void view_borrow_records() {
    printf("\n--- 个人借阅记录 ---\n");
    printf("%-10s %-30s %-12s %-12s %-12s %-8s\n",
           "记录ID", "书名", "借阅日期", "应还日期", "归还日期", "状态");
    printf("----------------------------------------------------------------------------\n");
    
    int count = 0;
    for (int i = 0; i < record_count; i++) {
        if (strcmp(records[i].reader_id, current_user_id) == 0) {
            // 查找图书信息
            int book_idx = find_book_by_id(records[i].book_id);
            if (book_idx == -1) continue;
            
            char status[10];
            if (strlen(records[i].return_date) == 0) {
                strcpy(status, "未还");
            } else {
                strcpy(status, "已还");
            }
            
            printf("%-10s %-30s %-12s %-12s %-12s %-8s\n",
                   records[i].record_id,
                   books[book_idx].title,
                   records[i].borrow_date,
                   records[i].due_date,
                   records[i].return_date,
                   status);
            count++;
        }
    }
    
    if (count == 0) {
        printf("暂无借阅记录！\n");
    } else {
        printf("共 %d 条记录\n", count);
    }
}

// 读者端：查询待还图书
void view_current_borrows() {
    printf("\n--- 待还图书 ---\n");
    printf("%-30s %-12s %-12s %-10s\n",
           "书名", "借阅日期", "应还日期", "剩余天数");
    printf("--------------------------------------------------------\n");
    
    int count = 0;
    char current_date[11];
    get_current_date(current_date);
    
    for (int i = 0; i < record_count; i++) {
        if (strcmp(records[i].reader_id, current_user_id) == 0 &&
            strlen(records[i].return_date) == 0) {
            
            int book_idx = find_book_by_id(records[i].book_id);
            if (book_idx == -1) continue;
            
            int days_left = calculate_overdue_days(records[i].due_date);
            
            printf("%-30s %-12s %-12s %-10d\n",
                   books[book_idx].title,
                   records[i].borrow_date,
                   records[i].due_date,
                   days_left);
            count++;
        }
    }
    
    if (count == 0) {
        printf("暂无待还图书！\n");
    } else {
        printf("共 %d 本待还图书\n", count);
    }
}

// 读者端：超期提醒
void view_overdue_reminders() {
    printf("\n--- 超期提醒 ---\n");
    printf("%-30s %-12s %-12s %-10s %-10s\n",
           "书名", "借阅日期", "应还日期", "超期天数", "罚款金额");
    printf("------------------------------------------------------------------------\n");
    
    int count = 0;
    char current_date[11];
    get_current_date(current_date);
    
    for (int i = 0; i < record_count; i++) {
        if (strcmp(records[i].reader_id, current_user_id) == 0 &&
            strlen(records[i].return_date) == 0 &&
            date_compare(current_date, records[i].due_date) > 0) {
            
            int book_idx = find_book_by_id(records[i].book_id);
            if (book_idx == -1) continue;
            
            int overdue_days = calculate_overdue_days(records[i].due_date);
            double fine = overdue_days * 0.5; // 每天0.5元罚款
            
            printf("%-30s %-12s %-12s %-10d ¥%-9.2f\n",
                   books[book_idx].title,
                   records[i].borrow_date,
                   records[i].due_date,
                   overdue_days,
                   fine);
            count++;
        }
    }
    
    if (count == 0) {
        printf("暂无超期图书！\n");
    } else {
        printf("共 %d 本超期图书\n", count);
    }
}

// 读者端：缴纳记录查询
void view_payment_records() {
    printf("\n--- 缴纳记录 ---\n");
    printf("（示例功能：显示所有已产生罚款的记录）\n");
    printf("%-30s %-12s %-12s %-10s %-10s\n",
           "书名", "借阅日期", "归还日期", "罚款原因", "金额");
    printf("------------------------------------------------------------------------\n");
    
    int count = 0;
    for (int i = 0; i < record_count; i++) {
        if (strcmp(records[i].reader_id, current_user_id) == 0 &&
            records[i].fine > 0) {
            
            int book_idx = find_book_by_id(records[i].book_id);
            if (book_idx == -1) continue;
            
            printf("%-30s %-12s %-12s %-10s ¥%-9.2f\n",
                   books[book_idx].title,
                   records[i].borrow_date,
                   records[i].return_date,
                   "超期罚款",
                   records[i].fine);
            count++;
        }
    }
    
    if (count == 0) {
        printf("暂无缴纳记录！\n");
    } else {
        printf("共 %d 条缴纳记录\n", count);
    }
}

// 读者端：个人信息查询
void search_personal_info() {
    int reader_idx = find_reader_by_id(current_user_id);
    if (reader_idx == -1) {
        printf("用户信息不存在！\n");
        return;
    }
    
    printf("\n--- 个人信息 ---\n");
    printf("读者ID: %s\n", readers[reader_idx].reader_id);
    printf("姓名: %s\n", readers[reader_idx].name);
    printf("院系: %s\n", readers[reader_idx].department);
    printf("联系电话: %s\n", readers[reader_idx].phone);
    printf("最大借阅量: %d\n", readers[reader_idx].max_borrow_limit);
    printf("当前借阅数: %d\n", readers[reader_idx].current_borrowed);
    printf("未缴罚款: ¥%.2f\n", readers[reader_idx].fine_amount);
    
    // 计算借阅统计
    int total_borrowed = 0;
    int current_borrowed = 0;
    for (int i = 0; i < record_count; i++) {
        if (strcmp(records[i].reader_id, current_user_id) == 0) {
            total_borrowed++;
            if (strlen(records[i].return_date) == 0) {
                current_borrowed++;
            }
        }
    }
    
    printf("累计借阅: %d 本\n", total_borrowed);
    printf("当前借阅: %d 本\n", current_borrowed);
}

// 管理员端：图书查询（增强版）
void admin_search_books() {
    search_books(); // 复用读者查询功能
    
    // 添加管理员特有的统计信息
    printf("\n--- 图书流通统计 ---\n");
    int total_borrowed = 0;
    for (int i = 0; i < book_count; i++) {
        total_borrowed += books[i].borrow_count;
    }
    
    printf("总馆藏量: %d 本\n", book_count);
    printf("总借阅次数: %d 次\n", total_borrowed);
    printf("平均借阅率: %.2f%%\n", (float)total_borrowed / book_count * 100);
}

// 管理员端：馆藏统计
void view_collection_stats() {
    printf("\n--- 馆藏统计 ---\n");
    
    int total_copies = 0;
    int total_available = 0;
    int total_borrowed_copies = 0;
    
    for (int i = 0; i < book_count; i++) {
        total_copies += books[i].total_copies;
        total_available += books[i].available_copies;
        total_borrowed_copies += (books[i].total_copies - books[i].available_copies);
    }
    
    printf("图书种类数: %d\n", book_count);
    printf("总馆藏册数: %d\n", total_copies);
    printf("可借阅册数: %d\n", total_available);
    printf("已借出册数: %d\n", total_borrowed_copies);
    printf("借出率: %.2f%%\n", (float)total_borrowed_copies / total_copies * 100);
    
    // 显示最近添加的图书
    printf("\n最近添加的图书（示例）:\n");
    int count = (book_count < 5) ? book_count : 5;
    for (int i = 0; i < count; i++) {
        printf("%d. %s (%s)\n", i+1, books[i].title, books[i].author);
    }
}

// 管理员端：分类统计
void view_category_stats() {
    printf("\n--- 分类统计 ---\n");
    printf("%-15s %-10s %-10s %-10s %-10s\n",
           "分类", "图书数量", "占比", "借阅次数", "借阅率");
    printf("------------------------------------------------------------\n");
    
    // 初始化分类统计
    for (int i = 0; i < category_count; i++) {
        categories[i].book_count = 0;
        categories[i].borrow_count = 0;
    }
    
    // 统计各分类数据
    for (int i = 0; i < book_count; i++) {
        for (int j = 0; j < category_count; j++) {
            if (strcmp(books[i].category, categories[j].category_name) == 0) {
                categories[j].book_count++;
                categories[j].borrow_count += books[i].borrow_count;
                break;
            }
        }
    }
    
    // 显示统计结果
    for (int i = 0; i < category_count; i++) {
        if (categories[i].book_count > 0) {
            float percentage = (float)categories[i].book_count / book_count * 100;
            float borrow_rate = (categories[i].book_count > 0) ? 
                (float)categories[i].borrow_count / categories[i].book_count : 0;
            
            printf("%-15s %-10d %-9.1f%% %-10d %-9.1f\n",
                   categories[i].category_name,
                   categories[i].book_count,
                   percentage,
                   categories[i].borrow_count,
                   borrow_rate);
        }
    }
    
    // 找出最受欢迎的分类
    int max_borrow = 0;
    char popular_category[MAX_STRING_LENGTH] = "";
    for (int i = 0; i < category_count; i++) {
        if (categories[i].borrow_count > max_borrow) {
            max_borrow = categories[i].borrow_count;
            strcpy(popular_category, categories[i].category_name);
        }
    }
    
    printf("\n最受欢迎分类: %s (借阅次数: %d)\n", popular_category, max_borrow);
}

// 管理员端：借阅率统计
void view_borrow_rate() {
    printf("\n--- 借阅率统计 ---\n");
    
    // 按借阅次数排序
    Book sorted_books[MAX_BOOKS];
    memcpy(sorted_books, books, sizeof(Book) * book_count);
    
    // 简单冒泡排序
    for (int i = 0; i < book_count - 1; i++) {
        for (int j = 0; j < book_count - i - 1; j++) {
            if (sorted_books[j].borrow_count < sorted_books[j+1].borrow_count) {
                Book temp = sorted_books[j];
                sorted_books[j] = sorted_books[j+1];
                sorted_books[j+1] = temp;
            }
        }
    }
    
    printf("借阅排行榜（Top 10）:\n");
    printf("%-4s %-30s %-20s %-15s %-10s\n",
           "排名", "书名", "作者", "分类", "借阅次数");
    printf("----------------------------------------------------------------\n");
    
    int count = (book_count < 10) ? book_count : 10;
    for (int i = 0; i < count; i++) {
        printf("%-4d %-30s %-20s %-15s %-10d\n",
               i + 1,
               sorted_books[i].title,
               sorted_books[i].author,
               sorted_books[i].category,
               sorted_books[i].borrow_count);
    }
    
    // 计算总体借阅率
    int total_borrow_count = 0;
    for (int i = 0; i < book_count; i++) {
        total_borrow_count += books[i].borrow_count;
    }
    
    float avg_borrow_rate = (float)total_borrow_count / book_count;
    printf("\n平均每本图书借阅次数: %.2f\n", avg_borrow_rate);
    
    // 统计借阅率分布
    printf("\n借阅率分布:\n");
    int ranges[5] = {0}; // 0次, 1-5次, 6-10次, 11-20次, 20+次
    for (int i = 0; i < book_count; i++) {
        int count = books[i].borrow_count;
        if (count == 0) ranges[0]++;
        else if (count <= 5) ranges[1]++;
        else if (count <= 10) ranges[2]++;
        else if (count <= 20) ranges[3]++;
        else ranges[4]++;
    }
    
    printf("0次: %d 本 (%.1f%%)\n", ranges[0], (float)ranges[0]/book_count*100);
    printf("1-5次: %d 本 (%.1f%%)\n", ranges[1], (float)ranges[1]/book_count*100);
    printf("6-10次: %d 本 (%.1f%%)\n", ranges[2], (float)ranges[2]/book_count*100);
    printf("11-20次: %d 本 (%.1f%%)\n", ranges[3], (float)ranges[3]/book_count*100);
    printf("20+次: %d 本 (%.1f%%)\n", ranges[4], (float)ranges[4]/book_count*100);
}

// 管理员端：超期率统计
void view_overdue_rate() {
    printf("\n--- 超期率统计 ---\n");
    
    char current_date[11];
    get_current_date(current_date);
    
    int total_borrowed = 0;
    int total_overdue = 0;
    double total_fine = 0;
    
    // 统计所有借阅记录
    for (int i = 0; i < record_count; i++) {
        if (strlen(records[i].return_date) == 0) { // 未归还
            total_borrowed++;
            if (date_compare(current_date, records[i].due_date) > 0) {
                total_overdue++;
                
                // 计算当前超期天数
                int overdue_days = calculate_overdue_days(records[i].due_date);
                total_fine += overdue_days * 0.5;
            }
        }
    }
    
    printf("当前借出图书总数: %d\n", total_borrowed);
    printf("超期图书数量: %d\n", total_overdue);
    
    if (total_borrowed > 0) {
        float overdue_rate = (float)total_overdue / total_borrowed * 100;
        printf("超期率: %.2f%%\n", overdue_rate);
    }
    
    printf("预估罚款总额: ¥%.2f\n", total_fine);
    
    // 显示超期详情
    if (total_overdue > 0) {
        printf("\n超期详情:\n");
        printf("%-10s %-30s %-12s %-12s %-10s\n",
               "读者ID", "书名", "应还日期", "超期天数", "预估罚款");
        printf("----------------------------------------------------------------\n");
        
        for (int i = 0; i < record_count; i++) {
            if (strlen(records[i].return_date) == 0 &&
                date_compare(current_date, records[i].due_date) > 0) {
                
                int book_idx = find_book_by_id(records[i].book_id);
                if (book_idx == -1) continue;
                
                int overdue_days = calculate_overdue_days(records[i].due_date);
                double fine = overdue_days * 0.5;
                
                printf("%-10s %-30s %-12s %-12d ¥%-9.2f\n",
                       records[i].reader_id,
                       books[book_idx].title,
                       records[i].due_date,
                       overdue_days,
                       fine);
            }
        }
    }
}

// 管理员端：生成统计报表
void generate_statistics_report() {
    printf("\n=== 图书借阅统计报表 ===\n");
    printf("生成时间: ");
    
    // 获取当前时间
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    printf("%d-%02d-%02d %02d:%02d:%02d\n",
           tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
           tm.tm_hour, tm.tm_min, tm.tm_sec);
    
    printf("\n一、馆藏概况\n");
    printf("----------------\n");
    int total_copies = 0;
    for (int i = 0; i < book_count; i++) {
        total_copies += books[i].total_copies;
    }
    printf("图书种类: %d 种\n", book_count);
    printf("总馆藏量: %d 册\n", total_copies);
    printf("读者总数: %d 人\n", reader_count);
    printf("借阅记录: %d 条\n", record_count);
    
    printf("\n二、分类统计\n");
    printf("----------------\n");
    view_category_stats();
    
    printf("\n三、借阅分析\n");
    printf("----------------\n");
    int total_borrow_count = 0;
    for (int i = 0; i < book_count; i++) {
        total_borrow_count += books[i].borrow_count;
    }
    printf("总借阅次数: %d 次\n", total_borrow_count);
    printf("平均每本借阅: %.2f 次\n", (float)total_borrow_count / book_count);
    
    printf("\n四、超期情况\n");
    printf("----------------\n");
    view_overdue_rate();
    
    printf("\n五、热门图书\n");
    printf("----------------\n");
    
    // 找出借阅次数最多的5本书
    Book sorted_books[MAX_BOOKS];
    memcpy(sorted_books, books, sizeof(Book) * book_count);
    
    for (int i = 0; i < book_count - 1; i++) {
        for (int j = 0; j < book_count - i - 1; j++) {
            if (sorted_books[j].borrow_count < sorted_books[j+1].borrow_count) {
                Book temp = sorted_books[j];
                sorted_books[j] = sorted_books[j+1];
                sorted_books[j+1] = temp;
            }
        }
    }
    
    int count = (book_count < 5) ? book_count : 5;
    for (int i = 0; i < count; i++) {
        printf("%d. 《%s》 - %s (借阅: %d次)\n",
               i + 1,
               sorted_books[i].title,
               sorted_books[i].author,
               sorted_books[i].borrow_count);
    }
    
    printf("\n=== 报表结束 ===\n");
}

// 工具函数：计算超期天数
int calculate_overdue_days(const char* due_date) {
    char current_date[11];
    get_current_date(current_date);
    
    // 简化计算：假设每个月30天
    int due_year, due_month, due_day;
    int cur_year, cur_month, cur_day;
    
    sscanf(due_date, "%d-%d-%d", &due_year, &due_month, &due_day);
    sscanf(current_date, "%d-%d-%d", &cur_year, &cur_month, &cur_day);
    
    int due_days = due_year * 365 + due_month * 30 + due_day;
    int cur_days = cur_year * 365 + cur_month * 30 + cur_day;
    
    int overdue = cur_days - due_days;
    return (overdue > 0) ? overdue : 0;
}

// 工具函数：获取当前日期
void get_current_date(char* date_str) {
    // 使用预定义的日期，避免time.h的跨平台问题
    strcpy(date_str, "2024-03-20"); // 示例日期
}

// 工具函数：比较日期
int date_compare(const char* date1, const char* date2) {
    // 简单字符串比较，要求日期格式为YYYY-MM-DD
    return strcmp(date1, date2);
}

// 工具函数：清空输入缓冲区
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// 工具函数：查找图书
int find_book_by_id(const char* book_id) {
    for (int i = 0; i < book_count; i++) {
        if (strcmp(books[i].book_id, book_id) == 0) {
            return i;
        }
    }
    return -1;
}

// 工具函数：查找读者
int find_reader_by_id(const char* reader_id) {
    for (int i = 0; i < reader_count; i++) {
        if (strcmp(readers[i].reader_id, reader_id) == 0) {
            return i;
        }
    }
    return -1;
}

// 加载示例数据
void load_sample_data() {
    // 添加示例图书
    Book book1 = {"B001", "C程序设计语言", "K&R", "计算机科学", "9787111128069", 10, 5, 156, 2004};
    Book book2 = {"B002", "数据结构", "严蔚敏", "计算机科学", "9787302147510", 8, 3, 203, 2007};
    Book book3 = {"B003", "红楼梦", "曹雪芹", "文学", "9787020002207", 15, 10, 89, 2008};
    Book book4 = {"B004", "百年孤独", "马尔克斯", "文学", "9787544253994", 6, 2, 67, 2011};
    Book book5 = {"B005", "人类简史", "尤瓦尔", "历史", "9787508647357", 12, 8, 145, 2014};
    
    books[book_count++] = book1;
    books[book_count++] = book2;
    books[book_count++] = book3;
    books[book_count++] = book4;
    books[book_count++] = book5;
    
    // 添加示例读者
    Reader reader1 = {"R001", "张三", "计算机学院", "13800138000", 10, 2, 0};
    Reader reader2 = {"R002", "李四", "文学院", "13900139000", 10, 1, 5.50};
    
    readers[reader_count++] = reader1;
    readers[reader_count++] = reader2;
    
    // 添加示例借阅记录
    BorrowRecord record1 = {"REC001", "B001", "R001", "2024-03-01", "2024-04-01", "", 0, 0};
    BorrowRecord record2 = {"REC002", "B002", "R001", "2024-03-10", "2024-03-20", "", 1, 0};
    BorrowRecord record3 = {"REC003", "B003", "R002", "2024-02-15", "2024-03-15", "2024-03-10", 0, 0};
    BorrowRecord record4 = {"REC004", "B001", "R002", "2024-02-01", "2024-03-01", "", 1, 2.5};
    
    records[record_count++] = record1;
    records[record_count++] = record2;
    records[record_count++] = record3;
    records[record_count++] = record4;
}

// 主函数
int main() {
    init_system();
    login_menu();
    return 0;
}