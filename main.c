#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "library.h"
#include "utils.h"

#ifdef _WIN32
    #include <conio.h>
    #include <io.h>
#else
    #include <termios.h>
    #include <unistd.h>
#endif

// Cross-platform key reading helper supporting automated input piping
int get_key(void) {
#ifdef _WIN32
    if (_isatty(_fileno(stdin))) {
        return _getch();
    } else {
        return getchar();
    }
#else
    if (isatty(fileno(stdin))) {
        struct termios oldt, newt;
        int ch;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return ch;
    } else {
        return getchar();
    }
#endif
}

#define ADMIN_USER "admin"
#define ADMIN_PASS "admin123"

void get_password(char *password, int max_len) {
    int i = 0;
    int ch;
    while (1) {
        ch = get_key();
        if (ch == 13 || ch == 10 || ch == EOF) {
            password[i] = '\0';
            break;
        } else if (ch == 8 || ch == 127) {
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        } else if (i < max_len - 1 && isprint(ch)) {
            password[i++] = (char)ch;
            printf("*");
        }
    }
    printf("\n");
}

int authenticate_admin(void) {
    char username[50];
    char password[50];

    print_header("Administrator Login");

    printf(ANSI_BOLD "Username: " ANSI_COLOR_RESET);
    get_string("", username, sizeof(username));

    printf(ANSI_BOLD "Password: " ANSI_COLOR_RESET);
    get_password(password, sizeof(password));

    if (strcmp(username, ADMIN_USER) == 0 && strcmp(password, ADMIN_PASS) == 0) {
        printf("\n" ANSI_COLOR_GREEN "🟢 Login Successful! Redirecting to dashboard..." ANSI_COLOR_RESET "\n");
        delay_ms(1000);
        return 1;
    } else {
        printf("\n" ANSI_COLOR_RED "🔴 Invalid Username or Password!" ANSI_COLOR_RESET "\n");
        printf("Press any key to retry...\n");
        get_key();
        return 0;
    }
}

int main(void) {
    init_terminal();

    Book *books = NULL;
    int book_count = 0;
    IssueRecord *issues = NULL;
    int issue_count = 0;

    // Load DB files
    if (!load_library_data(&books, &book_count, &issues, &issue_count)) {
        printf(ANSI_COLOR_RED "🔴 Failed to initialize library database. Exiting..." ANSI_COLOR_RESET "\n");
        return 1;
    }

    clear_screen();

    // Authenticate
    int authenticated = 0;
    int attempts = 0;
    while (!authenticated && attempts < 3) {
        clear_screen();
        if (authenticate_admin()) {
            authenticated = 1;
        } else {
            attempts++;
        }
    }

    if (!authenticated) {
        clear_screen();
        printf("\n" ANSI_COLOR_RED ANSI_BOLD "🔴 Maximum login attempts exceeded. Access Denied!" ANSI_COLOR_RESET "\n");
        if (books != NULL) free(books);
        if (issues != NULL) free(issues);
        return 1;
    }

    int choice;
    do {
        clear_screen();
        print_header("Library Management System (LMS)");
        
        printf(ANSI_COLOR_CYAN "Main Dashboard:\n" ANSI_COLOR_RESET);
        printf("1.  " ANSI_BOLD "Add" ANSI_COLOR_RESET " New Book\n");
        printf("2.  " ANSI_BOLD "Update" ANSI_COLOR_RESET " Book Details\n");
        printf("3.  " ANSI_BOLD "Remove" ANSI_COLOR_RESET " Book\n");
        printf("4.  " ANSI_BOLD "Search" ANSI_COLOR_RESET " Book\n");
        printf("5.  " ANSI_BOLD "View All" ANSI_COLOR_RESET " Books\n");
        printf("6.  " ANSI_BOLD "Issue" ANSI_COLOR_RESET " Book\n");
        printf("7.  " ANSI_BOLD "Return" ANSI_COLOR_RESET " Book\n");
        printf("8.  " ANSI_BOLD "List" ANSI_COLOR_RESET " Issued Books\n");
        printf("9.  " ANSI_BOLD "Analytics" ANSI_COLOR_RESET " & Reports\n");
        printf("10. " ANSI_BOLD "Save" ANSI_COLOR_RESET " Data\n");
        printf("11. " ANSI_COLOR_RED "Exit" ANSI_COLOR_RESET " System\n\n");
        
        choice = get_int("Enter your choice (1-11): ");

        switch (choice) {
            case 1:
                clear_screen();
                add_book(&books, &book_count);
                printf("\nPress any key to return to menu...");
                get_key();
                break;
            case 2:
                clear_screen();
                update_book(books, book_count);
                printf("\nPress any key to return to menu...");
                get_key();
                break;
            case 3:
                clear_screen();
                remove_book(&books, &book_count, issues, issue_count);
                printf("\nPress any key to return to menu...");
                get_key();
                break;
            case 4:
                clear_screen();
                search_book(books, book_count);
                printf("\nPress any key to return to menu...");
                get_key();
                break;
            case 5:
                clear_screen();
                view_all_books(books, book_count);
                printf("\nPress any key to return to menu...");
                get_key();
                break;
            case 6:
                clear_screen();
                issue_book(books, book_count, &issues, &issue_count);
                printf("\nPress any key to return to menu...");
                get_key();
                break;
            case 7:
                clear_screen();
                return_book(books, book_count, issues, issue_count);
                printf("\nPress any key to return to menu...");
                get_key();
                break;
            case 8:
                clear_screen();
                list_issued_books(books, book_count, issues, issue_count);
                printf("\nPress any key to return to menu...");
                get_key();
                break;
            case 9:
                clear_screen();
                show_reports(books, book_count, issues, issue_count);
                printf("\nPress any key to return to menu...");
                get_key();
                break;
            case 10:
                clear_screen();
                printf("\n💾 Saving library data...\n");
                if (save_library_data(books, book_count, issues, issue_count)) {
                    printf(ANSI_COLOR_GREEN "🟢 Success: Data saved successfully!" ANSI_COLOR_RESET "\n");
                }
                printf("\nPress any key to return to menu...");
                get_key();
                break;
            case 11:
                clear_screen();
                printf("\n💾 Saving database and shutting down...\n");
                save_library_data(books, book_count, issues, issue_count);
                printf(ANSI_COLOR_GREEN "👋 Goodbye!" ANSI_COLOR_RESET "\n");
                break;
            default:
                printf("\n" ANSI_COLOR_RED "⚠️ Invalid choice! Please select between 1 and 11." ANSI_COLOR_RESET "\n");
                printf("Press any key to continue...");
                get_key();
                break;
        }
    } while (choice != 11);

    // Free dynamic allocations
    if (books != NULL) free(books);
    if (issues != NULL) free(issues);

    return 0;
}
