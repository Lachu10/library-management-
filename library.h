#ifndef LIBRARY_H
#define LIBRARY_H

#include <time.h>

#define MAX_TITLE 100
#define MAX_AUTHOR 50
#define MAX_NAME 50
#define MAX_USER_ID 20

#define BOOKS_FILE "books.dat"
#define ISSUES_FILE "issues.dat"

typedef struct {
    int book_id;
    char title[MAX_TITLE];
    char author[MAX_AUTHOR];
    int quantity;
} Book;

typedef struct {
    int issue_id;
    int book_id;
    char user_id[MAX_USER_ID];
    char user_name[MAX_NAME];
    time_t issue_date;
    time_t due_date;
    time_t return_date; // 0 if not returned yet
    float fine_amount;
    int is_returned; // 0 = active, 1 = returned
} IssueRecord;

// Global state operations
int load_library_data(Book **books, int *book_count, IssueRecord **issues, int *issue_count);
int save_library_data(Book *books, int book_count, IssueRecord *issues, int issue_count);

// Book CRUD operations
void add_book(Book **books, int *book_count);
void update_book(Book *books, int book_count);
void remove_book(Book **books, int *book_count, IssueRecord *issues, int issue_count);
void search_book(Book *books, int book_count);
void view_all_books(Book *books, int book_count);

// Issue and Return Operations
void issue_book(Book *books, int book_count, IssueRecord **issues, int *issue_count);
void return_book(Book *books, int book_count, IssueRecord *issues, int issue_count);
void list_issued_books(Book *books, int book_count, IssueRecord *issues, int issue_count);

// Reports
void show_reports(Book *books, int book_count, IssueRecord *issues, int issue_count);

#endif // LIBRARY_H
