# Library Management System (LMS) in C

A professional, menu-driven Library Management System implemented in C. Designed for librarians to easily manage book inventory, issue books to borrowers, process returns, calculate late fees, and view analytics logs using flat binary file storage.

---

##  Features

-  Administrator Login: Secure login gate preventing unauthorized access to library controls.
-  Complete Book CRUD Operations:
  - Create: Add new books with unique ID validations.
  - Read: View all books in a formatted tabular interface with stock details.
  - Update: Edit book title, author, and inventory quantity.
  - Delete: Safely remove books (restricted if the book is currently on loan).
-  Issue & Return loan manager:
  - Issue books to users (automatically calculates loan period of 7 days).
  - Return books (automatically calculates overdue calendar days and incurs a late fine of ₹5/day).
- Real Date/Time arithmetic: Fully integrated with C standard `<time.h>` to handle dates and transaction records reliably.
- Library Analytics & Reports: Generates summaries of stock records, active loans, total fines collected, and list of overdue loans.
- Flat-file Database Persistence: Automatically loads and saves all records as binary dat files (`books.dat`, `issues.dat`).
- Premium ANSI Console UI: Colorized menus, borders, and status cues ( Success, Error, Warning) with Windows Command Prompt / Terminal support.

---

## Project Structure

```text
library_management/
├── main.c           # Program entry point, authentication & main loop
├── library.c        # CRUD book inventory, issue/return loan routines & reports
├── library.h        # Structures for Book/IssueRecord & global declarations
├── utils.c          # Input checks, date formatting & ANSI terminal setup
├── utils.h          # Console UI helper declarations & ANSI color macros
└── Makefile         # Automated build script
