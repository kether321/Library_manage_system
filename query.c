#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 回调函数类型：处理查询结果行
// ncols: 列数, data: 每列值数组 (需自行释放? 无, SQLite自动), colname: 列名数组
typedef void (*QueryCallback)(int ncols, char **data, char **colname);

// 执行通用查询（使用prepared statement，支持参数绑定）
static int execute_prepared_query(sqlite3 *db, const char *sql, int param_count, sqlite3_stmt *params[], QueryCallback cb) {
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL prepare error: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    // 绑定参数
    for (int i = 0; i < param_count; i++) {
        sqlite3_bind_text(stmt, i + 1, (const char *)params[i], -1, SQLITE_STATIC);
    }

    // 执行查询并回调每行
    int ncols = sqlite3_column_count(stmt);
    char *colnames[ncols];
    for (int i = 0; i < ncols; i++) {
        colnames[i] = (char *)sqlite3_column_name(stmt, i);
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        char *rowdata[ncols];
        for (int i = 0; i < ncols; i++) {
            rowdata[i] = (char *)sqlite3_column_text(stmt, i);
        }
        cb(ncols, rowdata, colnames);
    }

    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? 0 : -1;
}

// 读者端：图书查询（书名/作者模糊，支持读者过滤可借阅）
int search_books(sqlite3 *db, const char *query, int reader_id, QueryCallback cb) {
    char like_query[256];
    snprintf(like_query, sizeof(like_query), "%%%s%%", query);

    char *sql_base = "SELECT id, title, author, status FROM books WHERE title LIKE ? OR author LIKE ?";
    char *params[2] = {like_query, like_query};

    if (reader_id > 0) {
        char *sql_reader = " AND status = 0";  // 假设 status=0 为可借
        // 动态SQL拼接（安全，因无user input in append）
        size_t len = strlen(sql_base) + strlen(sql_reader) + 1;
        char *sql = malloc(len);
        snprintf(sql, len, "%s%s", sql_base, sql_reader);
        int rc = execute_prepared_query(db, sql, 2, params, cb);
        free(sql);
        return rc;
    }
    return execute_prepared_query(db, sql_base, 2, params, cb);
}

// 读者端：个人借阅记录
int get_borrowing_history(sqlite3 *db, int reader_id, QueryCallback cb) {
    char reader_str[32];
    snprintf(reader_str, sizeof(reader_str), "%d", reader_id);

    const char *sql = 
        "SELECT b.title, l.borrow_date, l.due_date, l.return_date "
        "FROM loans l JOIN books b ON l.book_id = b.id "
        "WHERE l.reader_id = ? ORDER BY l.borrow_date DESC";
    char *params[1] = {reader_str};
    return execute_prepared_query(db, sql, 1, params, cb);
}

// 读者端：待还图书（未归还且未超期）
int get_due_books(sqlite3 *db, int reader_id, QueryCallback cb) {
    char reader_str[32];
    snprintf(reader_str, sizeof(reader_str), "%d", reader_id);

    const char *sql = 
        "SELECT b.title, l.due_date "
        "FROM loans l JOIN books b ON l.book_id = b.id "
        "WHERE l.reader_id = ? AND l.return_date IS NULL AND l.due_date >= DATE('now')";
    char *params[1] = {reader_str};
    return execute_prepared_query(db, sql, 1, params, cb);
}

// 读者端：超期提醒
int get_overdue_reminders(sqlite3 *db, int reader_id, QueryCallback cb) {
    char reader_str[32];
    snprintf(reader_str, sizeof(reader_str), "%d", reader_id);

    const char *sql = 
        "SELECT b.title, l.due_date, "
        "CAST((julianday(DATE('now')) - julianday(l.due_date)) AS INTEGER) AS overdue_days "
        "FROM loans l JOIN books b ON l.book_id = b.id "
        "WHERE l.reader_id = ? AND l.return_date IS NULL AND l.due_date < DATE('now')";
    char *params[1] = {reader_str};
    return execute_prepared_query(db, sql, 1, params, cb);
}

// 读者端：缴纳记录
int get_payment_records(sqlite3 *db, int reader_id, QueryCallback cb) {
    char reader_str[32];
    snprintf(reader_str, sizeof(reader_str), "%d", reader_id);

    const char *sql = 
        "SELECT amount, payment_date, reason FROM payments WHERE reader_id = ? "
        "ORDER BY payment_date DESC";
    char *params[1] = {reader_str};
    return execute_prepared_query(db, sql, 1, params, cb);
}

// 管理员端：馆藏总量（回调单值）
int get_total_books(sqlite3 *db, QueryCallback cb) {
    const char *sql = "SELECT COUNT(*) FROM books";
    char *params[0] = {0};
    return execute_prepared_query(db, sql, 0, params, cb);
}

// 管理员端：分类占比（回调：category, count, percent。需先总计）
int get_category_distribution(sqlite3 *db, QueryCallback cb) {
    // 先获取总量
    int total = 0;
    QueryCallback total_cb = (QueryCallback)&total;  // 简化：自定义cb处理
    // 注意：为准确，先query total，然后group，并在cb中计算%（但cb外部需total）
    // 此处简化：返回category:count，外部计算%
    const char *sql = "SELECT category, COUNT(*) as count FROM books GROUP BY category";
    char *params[0] = {0};
    return execute_prepared_query(db, sql, 0, params, cb);
}

// 管理员端：借阅率（当前借出/总量 *100，回调单值percent）
double calc_borrowing_rate(sqlite3 *db) {
    int total = 0;
    QueryCallback total_cb = ^(int n, char** d, char** c) { total = atoi(d[0]); };
    get_total_books(db, total_cb);

    int borrowed = 0;
    QueryCallback borrowed_cb = ^(int n, char** d, char** c) { borrowed = atoi(d[0]); };
    const char *sql_b = "SELECT COUNT(*) FROM loans WHERE return_date IS NULL";
    char *p[0]={0};
    execute_prepared_query(db, sql_b, 0, p, borrowed_cb);

    return total > 0 ? (borrowed * 100.0 / total) : 0.0;
}

// 注意：C无闭包，上例伪码。实际分开query并计算。
// 修正：提供独立函数返回double
double get_borrowing_rate(sqlite3 *db) {
    sqlite3_stmt *stmt;
    int total = 0, borrowed = 0;

    // total
    sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM books", -1, &stmt, NULL);
    if (sqlite3_step(stmt) == SQLITE_ROW) total = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    // borrowed
    sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM loans WHERE return_date IS NULL", -1, &stmt, NULL);
    if (sqlite3_step(stmt) == SQLITE_ROW) borrowed = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    return total > 0 ? (borrowed * 100.0 / total) : 0.0;
}

// 管理员端：超期率（当前超期/总借阅 *100）
double get_overdue_rate(sqlite3 *db) {
    sqlite3_stmt *stmt;
    int total_loans = 0, overdue = 0;

    // total_loans
    sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM loans", -1, &stmt, NULL);
    if (sqlite3_step(stmt) == SQLITE_ROW) total_loans = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    // overdue
    const char *sql_o = "SELECT COUNT(*) FROM loans WHERE return_date IS NULL AND due_date < DATE('now')";
    sqlite3_prepare_v2(db, sql_o, -1, &stmt, NULL);
    if (sqlite3_step(stmt) == SQLITE_ROW) overdue = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    return total_loans > 0 ? (overdue * 100.0 / total_loans) : 0.0;
}

// 示例回调：打印结果行（用户可自定义）
void print_row(int ncols, char **data, char **colname) {
    for (int i = 0; i < ncols; i++) {
        printf("%s: %s | ", colname[i], data[i] ? data[i] : "NULL");
    }
    printf("\n");
}

// 示例使用（集成到现有系统）
/*
int main() {
    sqlite3 *db;
    sqlite3_open("library.db", &db);  // 由外部系统提供db

    // 读者查询
    search_books(db, "Python", 123, print_row);

    // 统计
    printf("Borrowing rate: %.2f%%\n", get_borrowing_rate(db));
    printf("Overdue rate: %.2f%%\n", get_overdue_rate(db));

    sqlite3_close(db);
    return 0;
}
*/
