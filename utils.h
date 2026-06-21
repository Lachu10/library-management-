#ifndef UTILS_H
#define UTILS_H

#include <time.h>

// Terminal initialization for ANSI colors
void init_terminal(void);
void delay_ms(int ms);
void clear_screen(void);
void print_header(const char *title);

// Input validation helpers
void get_string(const char *prompt, char *buffer, int max_len);
int get_int(const char *prompt);
int get_positive_int(const char *prompt);
float get_float(const char *prompt);

// Date helpers
time_t get_current_date(void);
time_t normalize_date(time_t t);
void format_date(time_t t, char *buffer, size_t max_len);
time_t add_days(time_t t, int days);
int calculate_days_difference(time_t start, time_t end);
time_t parse_date(const char *date_str);

// ANSI color escape codes for styled output
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_BOLD          "\x1b[1m"

#endif // UTILS_H
