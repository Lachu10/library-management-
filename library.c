#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "library.h"
#include "utils.h"

// Case-insensitive substring matching helper
static int contains_substring(const char *str, const char *sub) {
    char str_lower[256] = {0};
    char sub_lower[256] = {0};
    int i;
    
    for (i = 0; str[i] && i < 255; i++) {
        str_lower[i] = tolower((unsigned char)str[i]);
    }
    str_lower[i] = '\0';
    
    for (i = 0; sub[i] && i < 255; i++) {
        sub_lower[i] = tolower((unsigned char)sub[i]);
    }
    sub_lower[i] = '\0';
    
    return strstr(str_lower, sub_lower) != NULL;
}

// Helper to find book by ID
static int find_book_by_id(Book *books, int count, int id) {
    for (int i = 0; i < count; i++) {
        if (books[i].book_id == id) {
            return i;
        }
    }
    return -1;
}

// Load database files
int load_library_data(Book **books, int *book_count, IssueRecord **issues, int *issue_count) {
    // 1. Load Books
    FILE *b_file = fopen(BOOKS_FILE, "rb");
    if (!b_file) {
        *books = NULL;
        *book_count = 0;
    } else {
        fseek(b_file, 0, SEEK_END);
        long b_size = ftell(b_file);
        fseek(b_file, 0, SEEK_SET);
        int num_books = b_size / sizeof(Book);
        if (num_books <= 0) {
            *books = NULL;
            *book_count = 0;
        } else {
            *books = (Book *)malloc(num_books * sizeof(Book));
            if (*books == NULL) {
                printf(ANSI_COLOR_RED "🔴 Error: Out of memory loading books!" ANSI_COLOR_RESET "\n");
                fclose(b_file);
                return 0;
            }
            size_t read_bytes = fread(*books, sizeof(Book), num_books, b_file);
            if (read_bytes != (size_t)num_books) {
                printf(ANSI_COLOR_YELLOW "⚠️ Warning: Books read mismatch." ANSI_COLOR_RESET "\n");
            }
            *book_count = num_books;
        }
        fclose(b_file);
    }

    // 2. Load Issue Records
    FILE *i_file = fopen(ISSUES_FILE, "rb");
    if (!i_file) {
        *issues = NULL;
        *issue_count = 0;
    } else {
        fseek(i_file, 0, SEEK_END);
        long i_size = ftell(i_file);
        fseek(i_file, 0, SEEK_SET);
        int num_issues = i_size / sizeof(IssueRecord);
        if (num_issues <= 0) {
            *issues = NULL;
            *issue_count = 0;
        } else {
            *issues = (IssueRecord *)malloc(num_issues * sizeof(IssueRecord));
            if (*issues == NULL) {
                printf(ANSI_COLOR_RED "🔴 Error: Out of memory loading issues!" ANSI_COLOR_RESET "\n");
                fclose(i_file);
                return 0;
            }
            size_t read_bytes = fread(*issues, sizeof(IssueRecord), num_issues, i_file);
            if (read_bytes != (size_t)num_issues) {
                printf(ANSI_COLOR_YELLOW "⚠️ Warning: Issues read mismatch." ANSI_COLOR_RESET "\n");
            }
            *issue_count = num_issues;
        }
        fclose(i_file);
    }
    return 1;
}

// Save database files
int save_library_data(Book *books, int book_count, IssueRecord *issues, int issue_count) {
    // 1. Save Books
    FILE *b_file = fopen(BOOKS_FILE, "wb");
    if (!b_file) {
        printf(ANSI_COLOR_RED "🔴 Error: Cannot open books database for writing!" ANSI_COLOR_RESET "\n");
        return 0;
    }
    if (book_count > 0 && books != NULL) {
        size_t written = fwrite(books, sizeof(Book), book_count, b_file);
        if (written != (size_t)book_count) {
            printf(ANSI_COLOR_RED "🔴 Error: Failed to write all books to file!" ANSI_COLOR_RESET "\n");
            fclose(b_file);
            return 0;
        }
    }
    fclose(b_file);

    // 2. Save Issues
    FILE *i_file = fopen(ISSUES_FILE, "wb");
    if (!i_file) {
        printf(ANSI_COLOR_RED "🔴 Error: Cannot open issues database for writing!" ANSI_COLOR_RESET "\n");
        return 0;
    }
    if (issue_count > 0 && issues != NULL) {
        size_t written = fwrite(issues, sizeof(IssueRecord), issue_count, i_file);
        if (written != (size_t)issue_count) {
            printf(ANSI_COLOR_RED "🔴 Error: Failed to write all issue records to file!" ANSI_COLOR_RESET "\n");
            fclose(i_file);
            return 0;
        }
    }
    fclose(i_file);
    return 1;
}

// Add New Book
void add_book(Book **books, int *book_count) {
    print_header("Add New Book");

    int id = get_positive_int("Enter Book ID: ");
    if (find_book_by_id(*books, *book_count, id) != -1) {
        printf(ANSI_COLOR_RED "🔴 Error: Book ID %d already exists!" ANSI_COLOR_RESET "\n", id);
        return;
    }

    char title[MAX_TITLE];
    get_string("Enter Book Title: ", title, MAX_TITLE);
    if (strlen(title) == 0) {
        printf(ANSI_COLOR_RED "🔴 Error: Title cannot be empty!" ANSI_COLOR_RESET "\n");
        return;
    }

    char author[MAX_AUTHOR];
    get_string("Enter Author: ", author, MAX_AUTHOR);
    if (strlen(author) == 0) {
        printf(ANSI_COLOR_RED "🔴 Error: Author cannot be empty!" ANSI_COLOR_RESET "\n");
        return;
    }

    int qty = get_positive_int("Enter Quantity: ");

    // Reallocate
    int new_count = *book_count + 1;
    Book *temp = (Book *)realloc(*books, new_count * sizeof(Book));
    if (temp == NULL) {
        printf(ANSI_COLOR_RED "🔴 Error: Out of memory!" ANSI_COLOR_RESET "\n");
        return;
    }
    *books = temp;

    (*books)[*book_count].book_id = id;
    strcpy((*books)[*book_count].title, title);
    strcpy((*books)[*book_count].author, author);
    (*books)[*book_count].quantity = qty;

    *book_count = new_count;

    if (save_library_data(*books, *book_count, NULL, 0)) { // Temp save books
        printf(ANSI_COLOR_GREEN "🟢 Success: Book added successfully!" ANSI_COLOR_RESET "\n");
    }
}

// Update Book Details
void update_book(Book *books, int book_count) {
    print_header("Update Book Details");

    if (book_count == 0 || books == NULL) {
        printf(ANSI_COLOR_YELLOW "📁 No books in library." ANSI_COLOR_RESET "\n");
        return;
    }

    printf("1. Update by Book ID\n");
    printf("2. Update by Book Title\n");
    printf("3. Back to Main Menu\n");
    int choice = get_int("Enter selection (1-3): ");

    int idx = -1;
    if (choice == 1) {
        int id = get_positive_int("Enter Book ID: ");
        idx = find_book_by_id(books, book_count, id);
    } else if (choice == 2) {
        char search_title[MAX_TITLE];
        get_string("Enter Book Title (Exact or Partial): ", search_title, MAX_TITLE);
        
        // Find first partial match
        for (int i = 0; i < book_count; i++) {
            if (contains_substring(books[i].title, search_title)) {
                idx = i;
                break;
            }
        }
    } else {
        return;
    }

    if (idx == -1) {
        printf(ANSI_COLOR_RED "❌ Book not found." ANSI_COLOR_RESET "\n");
        return;
    }

    printf("\n✏️ Modifying details for " ANSI_BOLD "%s" ANSI_COLOR_RESET " (ID: %d)\n", books[idx].title, books[idx].book_id);
    printf("[Press Enter to keep current value]\n\n");

    char buffer[100];

    // Title
    printf("Current Title: %s\n", books[idx].title);
    get_string("Enter New Title: ", buffer, MAX_TITLE);
    if (strlen(buffer) > 0) {
        strcpy(books[idx].title, buffer);
    }

    // Author
    printf("Current Author: %s\n", books[idx].author);
    get_string("Enter New Author: ", buffer, MAX_AUTHOR);
    if (strlen(buffer) > 0) {
        strcpy(books[idx].author, buffer);
    }

    // Quantity
    printf("Current Quantity: %d\n", books[idx].quantity);
    while (1) {
        printf("Enter New Quantity: ");
        get_string("", buffer, sizeof(buffer));
        if (strlen(buffer) == 0) break; // keep current
        
        int qty = atoi(buffer);
        if (qty >= 0) {
            books[idx].quantity = qty;
            break;
        }
        printf(ANSI_COLOR_RED "⚠️ Quantity must be 0 or positive." ANSI_COLOR_RESET "\n");
    }

    printf(ANSI_COLOR_GREEN "🟢 Success: Book details updated." ANSI_COLOR_RESET "\n");
}

// Remove Book
void remove_book(Book **books, int *book_count, IssueRecord *issues, int issue_count) {
    print_header("Remove Book");

    if (*book_count == 0 || *books == NULL) {
        printf(ANSI_COLOR_YELLOW "📁 No books in library." ANSI_COLOR_RESET "\n");
        return;
    }

    printf("1. Remove by Book ID\n");
    printf("2. Remove by Book Title\n");
    printf("3. Back to Main Menu\n");
    int choice = get_int("Enter selection (1-3): ");

    int idx = -1;
    if (choice == 1) {
        int id = get_positive_int("Enter Book ID to remove: ");
        idx = find_book_by_id(*books, *book_count, id);
    } else if (choice == 2) {
        char title[MAX_TITLE];
        get_string("Enter Book Title: ", title, MAX_TITLE);
        for (int i = 0; i < *book_count; i++) {
            if (contains_substring((*books)[i].title, title)) {
                idx = i;
                break;
            }
        }
    } else {
        return;
    }

    if (idx == -1) {
        printf(ANSI_COLOR_RED "❌ Book not found." ANSI_COLOR_RESET "\n");
        return;
    }

    // Critical constraint check: Ensure the book is not currently issued to any user!
    int target_id = (*books)[idx].book_id;
    for (int i = 0; i < issue_count; i++) {
        if (issues[i].book_id == target_id && issues[i].is_returned == 0) {
            printf(ANSI_COLOR_RED "🔴 Error: Cannot remove book. It is currently issued to User '%s' (ID: %s)." ANSI_COLOR_RESET "\n", 
                   issues[i].user_name, issues[i].user_id);
            return;
        }
    }

    printf(ANSI_COLOR_YELLOW "⚠️ Are you sure you want to delete '%s' (ID: %d)? (y/n): " ANSI_COLOR_RESET, (*books)[idx].title, (*books)[idx].book_id);
    char confirm[10];
    get_string("", confirm, sizeof(confirm));
    if (confirm[0] != 'y' && confirm[0] != 'Y') {
        printf(ANSI_COLOR_RED "❌ Deletion canceled." ANSI_COLOR_RESET "\n");
        return;
    }

    // Shift left
    for (int i = idx; i < *book_count - 1; i++) {
        (*books)[i] = (*books)[i + 1];
    }

    int new_count = *book_count - 1;
    if (new_count > 0) {
        Book *temp = (Book *)realloc(*books, new_count * sizeof(Book));
        if (temp != NULL) {
            *books = temp;
        }
    } else {
        free(*books);
        *books = NULL;
    }
    *book_count = new_count;

    printf(ANSI_COLOR_GREEN "🟢 Success: Book removed successfully!" ANSI_COLOR_RESET "\n");
}

// Search Book
void search_book(Book *books, int book_count) {
    print_header("Search Book");

    if (book_count == 0 || books == NULL) {
        printf(ANSI_COLOR_YELLOW "📁 Library is empty." ANSI_COLOR_RESET "\n");
        return;
    }

    printf("1. Search by Book ID\n");
    printf("2. Search by Book Title (Partial)\n");
    printf("3. Search by Author Name (Partial)\n");
    printf("4. Back to Main Menu\n");
    int choice = get_int("Enter selection (1-4): ");

    if (choice == 1) {
        int id = get_positive_int("Enter Book ID to search: ");
        int idx = find_book_by_id(books, book_count, id);
        if (idx != -1) {
            printf("\n🔍 Book Found:\n");
            printf("----------------------------------\n");
            printf("ID:        %d\n", books[idx].book_id);
            printf("Title:     %s\n", books[idx].title);
            printf("Author:    %s\n", books[idx].author);
            printf("Stock Qty: %d\n", books[idx].quantity);
            printf("----------------------------------\n");
        } else {
            printf(ANSI_COLOR_RED "❌ Book with ID %d not found." ANSI_COLOR_RESET "\n", id);
        }
    } else if (choice == 2 || choice == 3) {
        char query[100];
        get_string("Enter search term: ", query, sizeof(query));
        
        int found_count = 0;
        printf("\n🔍 Search Results:\n");
        printf(ANSI_COLOR_BLUE ANSI_BOLD "%-8s | %-35s | %-25s | %-8s" ANSI_COLOR_RESET "\n", "ID", "Title", "Author", "Stock");
        for (int i = 0; i < 80; i++) printf("-");
        printf("\n");

        for (int i = 0; i < book_count; i++) {
            int match = 0;
            if (choice == 2) {
                match = contains_substring(books[i].title, query);
            } else {
                match = contains_substring(books[i].author, query);
            }

            if (match) {
                if (found_count % 2 == 0) {
                    printf(ANSI_COLOR_CYAN);
                }
                printf("%-8d | %-35s | %-25s | %-8d" ANSI_COLOR_RESET "\n",
                       books[i].book_id, books[i].title, books[i].author, books[i].quantity);
                found_count++;
            }
        }
        if (found_count == 0) {
            printf(ANSI_COLOR_RED "❌ No books matching your query found." ANSI_COLOR_RESET "\n");
        } else {
            printf("\n" ANSI_BOLD "Total Matches: %d" ANSI_COLOR_RESET "\n", found_count);
        }
    }
}

// View All Books
void view_all_books(Book *books, int book_count) {
    print_header("All Books In Library");

    if (book_count == 0 || books == NULL) {
        printf(ANSI_COLOR_YELLOW "📁 Library is empty. No books registered." ANSI_COLOR_RESET "\n");
        return;
    }

    printf(ANSI_COLOR_BLUE ANSI_BOLD "%-8s | %-35s | %-25s | %-8s" ANSI_COLOR_RESET "\n", "ID", "Title", "Author", "Stock");
    for (int i = 0; i < 80; i++) printf("-");
    printf("\n");

    for (int i = 0; i < book_count; i++) {
        if (i % 2 == 0) {
            printf(ANSI_COLOR_CYAN);
        }
        printf("%-8d | %-35s | %-25s | %-8d" ANSI_COLOR_RESET "\n",
               books[i].book_id, books[i].title, books[i].author, books[i].quantity);
    }
    printf("\n" ANSI_BOLD "Total Books Types: %d" ANSI_COLOR_RESET "\n", book_count);
}

// Issue Book
void issue_book(Book *books, int book_count, IssueRecord **issues, int *issue_count) {
    print_header("Issue Book");

    if (book_count == 0 || books == NULL) {
        printf(ANSI_COLOR_YELLOW "📁 No books available in library." ANSI_COLOR_RESET "\n");
        return;
    }

    int b_id = get_positive_int("Enter Book ID: ");
    int b_idx = find_book_by_id(books, book_count, b_id);
    if (b_idx == -1) {
        printf(ANSI_COLOR_RED "❌ Book ID %d not found." ANSI_COLOR_RESET "\n", b_id);
        return;
    }

    // Availability validation check
    if (books[b_idx].quantity <= 0) {
        printf(ANSI_COLOR_RED "🔴 Error: Book '%s' is out of stock!" ANSI_COLOR_RESET "\n", books[b_idx].title);
        return;
    }

    char u_id[MAX_USER_ID];
    get_string("Enter User ID: ", u_id, MAX_USER_ID);
    if (strlen(u_id) == 0) {
        printf(ANSI_COLOR_RED "🔴 Error: User ID cannot be empty!" ANSI_COLOR_RESET "\n");
        return;
    }

    char u_name[MAX_NAME];
    get_string("Enter User Name: ", u_name, MAX_NAME);
    if (strlen(u_name) == 0) {
        printf(ANSI_COLOR_RED "🔴 Error: User Name cannot be empty!" ANSI_COLOR_RESET "\n");
        return;
    }

    // Reallocate issues list
    int new_count = *issue_count + 1;
    IssueRecord *temp = (IssueRecord *)realloc(*issues, new_count * sizeof(IssueRecord));
    if (temp == NULL) {
        printf(ANSI_COLOR_RED "🔴 Error: Out of memory!" ANSI_COLOR_RESET "\n");
        return;
    }
    *issues = temp;

    // Set records
    time_t now = normalize_date(get_current_date());
    time_t due = add_days(now, 7); // Loan period = 7 days

    (*issues)[*issue_count].issue_id = *issue_count + 10001; // Offset IDs to make them look premium
    (*issues)[*issue_count].book_id = b_id;
    strcpy((*issues)[*issue_count].user_id, u_id);
    strcpy((*issues)[*issue_count].user_name, u_name);
    (*issues)[*issue_count].issue_date = now;
    (*issues)[*issue_count].due_date = due;
    (*issues)[*issue_count].return_date = 0;
    (*issues)[*issue_count].fine_amount = 0.0f;
    (*issues)[*issue_count].is_returned = 0;

    *issue_count = new_count;

    // Reduce stock
    books[b_idx].quantity--;

    char date_buf[20];
    format_date(due, date_buf, sizeof(date_buf));
    printf(ANSI_COLOR_GREEN "🟢 Success: Book issued successfully!" ANSI_COLOR_RESET "\n");
    printf("📅 Due Date: " ANSI_BOLD "%s" ANSI_COLOR_RESET "\n", date_buf);
}

// Return Book
void return_book(Book *books, int book_count, IssueRecord *issues, int issue_count) {
    print_header("Return Book");

    if (issue_count == 0 || issues == NULL) {
        printf(ANSI_COLOR_YELLOW "📁 No active loan history in system." ANSI_COLOR_RESET "\n");
        return;
    }

    int b_id = get_positive_int("Enter Book ID: ");
    char u_id[MAX_USER_ID];
    get_string("Enter User ID: ", u_id, MAX_USER_ID);

    // Look for active match
    int active_idx = -1;
    for (int i = 0; i < issue_count; i++) {
        if (issues[i].book_id == b_id && strcmp(issues[i].user_id, u_id) == 0 && issues[i].is_returned == 0) {
            active_idx = i;
            break;
        }
    }

    if (active_idx == -1) {
        printf(ANSI_COLOR_RED "❌ No active issue record found for Book ID %d and User ID '%s'." ANSI_COLOR_RESET "\n", b_id, u_id);
        return;
    }

    // Determine return time
    time_t ret_time = 0;
    printf("Enter Return Date (YYYY-MM-DD) or press Enter for [Today]: ");
    char buffer[100];
    get_string("", buffer, sizeof(buffer));
    if (strlen(buffer) > 0) {
        ret_time = parse_date(buffer);
        if (ret_time == (time_t)-1) {
            printf(ANSI_COLOR_RED "🔴 Error: Invalid date format. Aborting return." ANSI_COLOR_RESET "\n");
            return;
        }
    } else {
        ret_time = normalize_date(get_current_date());
    }

    // Update issue record
    issues[active_idx].return_date = ret_time;
    issues[active_idx].is_returned = 1;

    // Return stock
    int b_idx = find_book_by_id(books, book_count, b_id);
    if (b_idx != -1) {
        books[b_idx].quantity++;
    }

    // Calculate fine (₹5 per late day)
    int late_days = calculate_days_difference(issues[active_idx].due_date, ret_time);
    float fine = 0.0f;
    if (late_days > 0) {
        fine = late_days * 5.0f;
    }
    issues[active_idx].fine_amount = fine;

    printf(ANSI_COLOR_GREEN "🟢 Success: Book returned successfully!" ANSI_COLOR_RESET "\n");
    if (fine > 0.0f) {
        printf("⚠️ Loan is overdue by " ANSI_BOLD "%d days" ANSI_COLOR_RESET "! Fine calculated: " ANSI_COLOR_RED "₹%.2f" ANSI_COLOR_RESET "\n", late_days, fine);
    } else {
        printf("🟢 Book returned on time. No fine incurred.\n");
    }
}

// List Issued Books
void list_issued_books(Book *books, int book_count, IssueRecord *issues, int issue_count) {
    print_header("Issue Logs & Active Loans");

    if (issue_count == 0 || issues == NULL) {
        printf(ANSI_COLOR_YELLOW "📁 No loan history in system." ANSI_COLOR_RESET "\n");
        return;
    }

    printf(ANSI_COLOR_BLUE ANSI_BOLD "%-6s | %-25s | %-8s | %-15s | %-12s | %-12s | %-12s | %-6s | %-9s" ANSI_COLOR_RESET "\n",
           "ID", "Book Title", "User ID", "User Name", "Issue Date", "Due Date", "Return Date", "Fine", "Status");
    for (int i = 0; i < 120; i++) printf("-");
    printf("\n");

    for (int i = 0; i < issue_count; i++) {
        // Find title
        char title[MAX_TITLE] = "Deleted Book";
        int b_idx = find_book_by_id(books, book_count, issues[i].book_id);
        if (b_idx != -1) {
            strcpy(title, books[b_idx].title);
        }

        char issue_d[15], due_d[15], return_d[15];
        format_date(issues[i].issue_date, issue_d, sizeof(issue_d));
        format_date(issues[i].due_date, due_d, sizeof(due_d));
        if (issues[i].return_date > 0) {
            format_date(issues[i].return_date, return_d, sizeof(return_d));
        } else {
            strcpy(return_d, "-");
        }

        if (issues[i].is_returned == 0) {
            printf(ANSI_COLOR_CYAN); // highlight active issues
            printf("%-6d | %-25.25s | %-8s | %-15.15s | %-12s | %-12s | %-12s | ₹%-5.2f | %-9s" ANSI_COLOR_RESET "\n",
                   issues[i].issue_id, title, issues[i].user_id, issues[i].user_name, issue_d, due_d, return_d, issues[i].fine_amount, "Active");
        } else {
            printf("%-6d | %-25.25s | %-8s | %-15.15s | %-12s | %-12s | %-12s | ₹%-5.2f | %-9s\n",
                   issues[i].issue_id, title, issues[i].user_id, issues[i].user_name, issue_d, due_d, return_d, issues[i].fine_amount, "Returned");
        }
    }
}

// Reports
void show_reports(Book *books, int book_count, IssueRecord *issues, int issue_count) {
    print_header("Library Analytics & Reports");

    int total_books = 0;
    for (int i = 0; i < book_count; i++) {
        total_books += books[i].quantity;
    }

    int active_loans = 0;
    float total_fine_collected = 0.0f;
    int overdue_count = 0;
    time_t today = normalize_date(get_current_date());

    for (int i = 0; i < issue_count; i++) {
        if (issues[i].is_returned == 0) {
            active_loans++;
            if (today > issues[i].due_date) {
                overdue_count++;
            }
        } else {
            total_fine_collected += issues[i].fine_amount;
        }
    }

    printf(ANSI_BOLD "General Library Summary:" ANSI_COLOR_RESET "\n");
    printf("📊 Total Unique Titles:    " ANSI_COLOR_CYAN "%d" ANSI_COLOR_RESET "\n", book_count);
    printf("📚 Total Stock Copies:     " ANSI_COLOR_CYAN "%d" ANSI_COLOR_RESET "\n", total_books);
    printf("📖 Active Issued Loans:    " ANSI_COLOR_CYAN "%d" ANSI_COLOR_RESET "\n", active_loans);
    printf("💰 Total Fines Collected:  " ANSI_COLOR_GREEN "₹%.2f" ANSI_COLOR_RESET "\n", total_fine_collected);
    printf("⚠️ Overdue Books:          " ANSI_COLOR_RED "%d" ANSI_COLOR_RESET "\n", overdue_count);
    printf("----------------------------------------\n\n");

    if (overdue_count > 0) {
        printf(ANSI_COLOR_RED ANSI_BOLD "⚠️ Overdue Loans Details:" ANSI_COLOR_RESET "\n");
        printf("%-8s | %-25s | %-12s | %-12s | %-10s\n", "User ID", "Book Title", "Due Date", "Days Late", "Est. Fine");
        for (int i = 0; i < 75; i++) printf("-");
        printf("\n");

        for (int i = 0; i < issue_count; i++) {
            if (issues[i].is_returned == 0 && today > issues[i].due_date) {
                char title[MAX_TITLE] = "Deleted Book";
                int b_idx = find_book_by_id(books, book_count, issues[i].book_id);
                if (b_idx != -1) {
                    strcpy(title, books[b_idx].title);
                }

                char due_d[15];
                format_date(issues[i].due_date, due_d, sizeof(due_d));
                int late_days = calculate_days_difference(issues[i].due_date, today);
                float est_fine = late_days * 5.0f;

                printf("%-8s | %-25.25s | %-12s | %-12d | ₹%-9.2f\n",
                       issues[i].user_id, title, due_d, late_days, est_fine);
            }
        }
    } else {
        printf(ANSI_COLOR_GREEN "🟢 Excellent: No loans are currently overdue." ANSI_COLOR_RESET "\n");
    }
}
