# Student Performance Management System (SPMS)

A console-based **Student Performance Management System** built in C++ to manage university student records, marks, and grades. Built as a Semester 1 project for **Fundamentals of Programming (CS118)** at FAST University, with a project partner.

## About

SPMS is a complete academic record manager that runs in the terminal. It stores students along with their marks across multiple assessment components (quizzes, assignments, midterm, and final exam), then automatically calculates totals, percentages, and letter grades. It also generates class statistics and exportable reports.

The project was built **without STL containers** (no `vector` or `map`) — using raw arrays, functions, structs, and file handling — to demonstrate a strong grasp of core programming fundamentals.

## Features

- **Add** new student records (name, roll number, section, marks)
- **Update** existing student information
- **Delete** a student record
- **Display** all students in a formatted table
- **Search** for a student by roll number
- **Class statistics** — average, highest, lowest marks, and class topper
- **Sort** students by percentage (descending)
- **Subject-wise statistics** across all assessment components
- **Automatic grade calculation** — totals, percentages, and letter grades
- **File handling** — save and load records from a text file (`students.txt`)
- **Export reports** to a separate text file (`report.txt`)
- **Robust input validation** — handles invalid input gracefully without crashing

## Assessment Components Tracked

Each student has marks for: 3 Quizzes, 2 Assignments, 1 Midterm, and 1 Final Exam — automatically combined into a total, percentage, and grade.

## Technologies Used

- **Language:** C++
- **Concepts:** Arrays, structs, functions, enums, file handling (`fstream`), formatted output (`iomanip`), input validation
- **Storage:** Plain text file (no database, no STL containers)
- **Environment:** Console / terminal application

## How to Run

1. Make sure you have a C++ compiler installed (e.g., g++).
2. Compile the program:
   ```bash
   g++ project_PF.cpp -o spms
   ```
3. Run it:
   ```bash
   ./spms
   ```
   (On Windows you can run `spms.exe`.)

## What I Learned

- Breaking a real-world problem into small, focused functions
- Managing structured data with arrays and structs
- Reading from and writing to files to make data persistent
- Validating user input so the program never crashes on bad input
- Formatting clean, table-style console output
- Collaborating with a partner on a shared codebase

## Authors

- Ahmad Khan
- Muhammad Ahmad

*Built as a Semester 1 project at FAST University (BS Computer Science).*
