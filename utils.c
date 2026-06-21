#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"

#ifdef _WIN32
    #include <windows.h>
    #ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
        #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
    #endif
#else
    #include <unistd.h>
#endif

void init_terminal(void) {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }
#endif
}

void delay_ms(int ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

void clear_screen(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void print_header(const char *title) {
    int len = strlen(title);
    int width = 60;
    int padding = (width - len - 2) / 2;

    printf("\n" ANSI_COLOR_CYAN ANSI_BOLD);
    for (int i = 0; i < width; i++) printf("=");
    printf("\n");

    printf("|");
    for (int i = 0; i < padding; i++) printf(" ");
    printf("%s", title);
    for (int i = 0; i < width - padding - len - 2; i++) printf(" ");
    printf("|\n");

    for (int i = 0; i < width; i++) printf("=");
    printf(ANSI_COLOR_RESET "\n\n");
}

void get_string(const char *prompt, char *buffer, int max_len) {
    printf("%s", prompt);
    if (fgets(buffer, max_len, stdin) != NULL) {
        // Remove trailing newline
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        } else {
            // Clear buffer if input was too long
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }
    }
}

int get_int(const char *prompt) {
    char buffer[100];
    int val;
    while (1) {
        get_string(prompt, buffer, sizeof(buffer));
        int valid = 1;
        int len = strlen(buffer);
        if (len == 0) valid = 0;
        for (int i = 0; i < len; i++) {
            if (i == 0 && (buffer[i] == '-' || buffer[i] == '+')) continue;
            if (!isdigit((unsigned char)buffer[i])) {
                valid = 0;
                break;
            }
        }
        if (valid) {
            val = atoi(buffer);
            return val;
        }
        printf(ANSI_COLOR_RED "⚠️ Invalid input! Please enter a valid integer." ANSI_COLOR_RESET "\n");
    }
}

int get_positive_int(const char *prompt) {
    int val;
    while (1) {
        val = get_int(prompt);
        if (val > 0) {
            return val;
        }
        printf(ANSI_COLOR_RED "⚠️ Input must be a positive integer greater than 0." ANSI_COLOR_RESET "\n");
    }
}

float get_float(const char *prompt) {
    char buffer[100];
    float val;
    char *endptr;
    while (1) {
        get_string(prompt, buffer, sizeof(buffer));
        if (strlen(buffer) == 0) {
            printf(ANSI_COLOR_RED "⚠️ Input cannot be empty." ANSI_COLOR_RESET "\n");
            continue;
        }
        val = strtof(buffer, &endptr);
        if (*endptr == '\0') {
            return val;
        }
        printf(ANSI_COLOR_RED "⚠️ Invalid input! Please enter a valid decimal number." ANSI_COLOR_RESET "\n");
    }
}

// Date helpers
time_t get_current_date(void) {
    return time(NULL);
}

time_t normalize_date(time_t t) {
    struct tm *info = localtime(&t);
    if (!info) return t;
    struct tm tm_norm = *info;
    tm_norm.tm_hour = 12;
    tm_norm.tm_min = 0;
    tm_norm.tm_sec = 0;
    tm_norm.tm_isdst = -1;
    return mktime(&tm_norm);
}

void format_date(time_t t, char *buffer, size_t max_len) {
    if (t == 0) {
        strncpy(buffer, "N/A", max_len);
        buffer[max_len - 1] = '\0';
        return;
    }
    struct tm *info = localtime(&t);
    if (info) {
        strftime(buffer, max_len, "%Y-%m-%d", info);
    } else {
        strncpy(buffer, "Error", max_len);
        buffer[max_len - 1] = '\0';
    }
}

time_t add_days(time_t t, int days) {
    return t + ((time_t)days * 24 * 60 * 60);
}

int calculate_days_difference(time_t start, time_t end) {
    double diff = difftime(end, start);
    int days = (int)(diff / (24 * 60 * 60));
    return days;
}

time_t parse_date(const char *date_str) {
    struct tm tm = {0};
    int year, month, day;
    if (sscanf(date_str, "%d-%d-%d", &year, &month, &day) == 3) {
        tm.tm_year = year - 1900;
        tm.tm_mon = month - 1;
        tm.tm_mday = day;
        tm.tm_hour = 12; // Noon to avoid timezone anomalies
        tm.tm_min = 0;
        tm.tm_sec = 0;
        tm.tm_isdst = -1;
        return mktime(&tm);
    }
    return (time_t)-1;
}
