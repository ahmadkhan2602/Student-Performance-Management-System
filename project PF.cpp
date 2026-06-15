/* SPMS.cpp
   Student Performance Management System (CS118 - Fall 2025)
   - Console-based C++ program
   - Uses arrays, functions, and file handling (text file)
   - No STL containers (no vector/map). Uses std::string for convenience.
   - Features: add/update/delete, save/load, calculate totals/percentage/grade,
               display table, search by roll, class stats, sort by percentage,
               subject-wise stats, export report.
*/

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cctype>


using namespace std;

const int MAX_STUDENTS = 200;      // Maximum students supported
const int NUM_QUIZZES = 3;
const int NUM_ASSIGN = 2;
const int NUM_COMPONENTS = NUM_QUIZZES + NUM_ASSIGN + 2; // quizzes + assignments + midterm + final
const string DATA_FILENAME = "students.txt";
const string REPORT_FILENAME = "report.txt";

// Index mapping for marks array:
enum ComponentIndex {
    QUIZ1 = 0,
    QUIZ2 = 1,
    QUIZ3 = 2,
    ASSIGN1 = 3,
    ASSIGN2 = 4,
    MIDTERM = 5,
    FINAL_EXAM = 6
};

struct Student {
    string name;
    int roll;
    string section;
    double marks[NUM_COMPONENTS]; // store marks for 3 quizzes, 2 assignments, midterm, final
    double total;
    double percentage;
    char grade;
    bool active; // true if the record exists (not deleted)
};

// Global array and count
Student students[MAX_STUDENTS];
int studentCount = 0;

// Forward declarations
void loadFromFile();
void saveToFile();
void exportReportToFile(const string& filename);
void calculateTotalsAndGrades();
char calculateGrade(double percentage);
int findStudentIndexByRoll(int roll);
void addStudent();
void updateStudent();
void deleteStudent();
void displayAllStudents();
void displayStudentHeader();
void displayStudentRow(const Student& s);
void searchStudent();
void showClassStatistics();
void sortStudentsByPercentageDesc();
void showSubjectWiseStatistics();
int getIntInput(const string& prompt, int minVal, int maxVal);
double getDoubleInput(const string& prompt, double minVal, double maxVal);
string getLineNonEmpty(const string& prompt);
void pressEnterToContinue();
void safeFlush();

// -------------------------------------------------
// Helper functions
// -------------------------------------------------
void safeFlush() {
    cin.clear();
    cin.ignore();
}

int getIntInput(const string& prompt, int minVal, int maxVal) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            if (value < minVal || value > maxVal) {
                cout << "  >> Enter value between " << minVal << " and " << maxVal << ".\n";
            }
            else {
                safeFlush();
                return value;
            }
        }
        else {
            cout << "  >> Invalid input. Please enter a number.\n";
            safeFlush();
        }
    }
}

double getDoubleInput(const string& prompt, double minVal, double maxVal) {
    double value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            if (value < minVal || value > maxVal) {
                cout << "  >> Enter value between " << minVal << " and " << maxVal << ".\n";
            }
            else {
                safeFlush();
                return value;
            }
        }
        else {
            cout << "  >> Invalid input. Please enter a number.\n";
            safeFlush();
        }
    }
}

string getLineNonEmpty(const string& prompt) {
    string line;
    while (true) {
        cout << prompt;
        getline(cin, line);
        if (line.size() == 0) {
            cout << "  >> Input cannot be empty. Try again.\n";
        }
        else {
            return line;
        }
    }
}

void pressEnterToContinue() {
    cout << "\nPress Enter to continue...";
    cin.get();
}

// -------------------------------------------------
// Core functions
// -------------------------------------------------

// Load students from text file
void loadFromFile() {
    ifstream fin(DATA_FILENAME.c_str());
    if (!fin) {
        // File doesn't exist yet
        cout << "No data file found (" << DATA_FILENAME << "). Starting with empty data.\n";
        return;
    }

    fin >> studentCount;
    if (!fin) {
        cout << "Data file corrupted or empty. Starting with empty data.\n";
        studentCount = 0;
        fin.close();
        return;
    }
    

    for (int i = 0; i < studentCount; ++i) {
        Student s;
        s.active = true;

        // Name (may contain spaces)
        getline(fin, s.name);
        if (s.name.size() == 0) getline(fin, s.name); // handle newline leftover

        fin >> s.roll;
       

        getline(fin, s.section);
        if (s.section.size() == 0) getline(fin, s.section);

        for (int j = 0; j < NUM_COMPONENTS; ++j) {
            fin >> s.marks[j];
        }
        fin >> s.total;
        fin >> s.percentage;
        fin >> s.grade;
        int activeFlag;
        fin >> activeFlag;
        s.active = (activeFlag == 1);
        

        students[i] = s;
    }

    cout << "Loaded " << studentCount << " records from " << DATA_FILENAME << ".\n";
    fin.close();
}

// Save students to text file
void saveToFile() {
    ofstream fout(DATA_FILENAME.c_str());
    if (!fout) {
        cout << "Error: Unable to open file for saving: " << DATA_FILENAME << "\n";
        return;
    }
    fout << studentCount << "\n";
    for (int i = 0; i < studentCount; ++i) {
        Student& s = students[i];
        fout << s.name << "\n";
        fout << s.roll << "\n";
        fout << s.section << "\n";
        for (int j = 0; j < NUM_COMPONENTS; ++j) fout << s.marks[j] << " ";
        fout << "\n";
        fout << s.total << "\n";
        fout << s.percentage << "\n";
        fout << s.grade << "\n";
        fout << (s.active ? 1 : 0) << "\n";
    }
    fout.close();
    cout << "Data saved to " << DATA_FILENAME << ".\n";
}

// Export a human-readable report to a text file
void exportReportToFile(const string& filename) {
    ofstream fout(filename.c_str());
    if (!fout) {
        cout << "Error: Unable to open file for writing: " << filename << "\n";
        return;
    }

    fout << "Student Performance Management System - Report\n";
    fout << "=============================================\n";
    fout << left << setw(20) << "Name" << setw(8) << "Roll" << setw(8) << "Section"
        << setw(10) << "Total" << setw(12) << "Percentage" << setw(6) << "Grade" << "\n";
    fout << "------------------------------------------------------------------\n";

    for (int i = 0; i < studentCount; ++i) {
        Student& s = students[i];
        if (!s.active) continue;
        fout << left << setw(20) << s.name
            << setw(8) << s.roll
            << setw(8) << s.section
            << setw(10) << fixed << setprecision(2) << s.total
            << setw(12) << fixed << setprecision(2) << s.percentage
            << setw(6) << s.grade << "\n";
    }

    // Add class stats
    double avg = 0, high = -1e9, low = 1e9;
    int count = 0;
    for (int i = 0; i < studentCount; ++i) {
        if (!students[i].active) continue;
        ++count;
        avg += students[i].percentage;
        if (students[i].percentage > high) high = students[i].percentage;
        if (students[i].percentage < low) low = students[i].percentage;
    }
    if (count > 0) {
        avg /= count;
        fout << "\nClass Statistics:\n";
        fout << "Average Percentage: " << fixed << setprecision(2) << avg << "\n";
        fout << "Highest Percentage: " << fixed << setprecision(2) << high << "\n";
        fout << "Lowest Percentage: " << fixed << setprecision(2) << low << "\n";
    }
    else {
        fout << "\nNo student records to compute class statistics.\n";
    }

    fout.close();
    cout << "Report exported to " << filename << ".\n";
}

// Calculate totals, percentage, and grade for all students
void calculateTotalsAndGrades() {
    for (int i = 0; i < studentCount; ++i) {
        if (!students[i].active) continue;
        double sum = 0.0;
        for (int j = 0; j < NUM_COMPONENTS; ++j) sum += students[i].marks[j];
        students[i].total = sum;
        // Each component assumed out of 100, so percentage is average of components
        students[i].percentage = (sum / (NUM_COMPONENTS * 100.0)) * 100.0;
        students[i].grade = calculateGrade(students[i].percentage);
    }
}

// Grade mapping
char calculateGrade(double percentage) {
    if (percentage >= 85.0) return 'A';
    if (percentage >= 70.0) return 'B';
    if (percentage >= 60.0) return 'C';
    if (percentage >= 50.0) return 'D';
    return 'F';
}

// Find student index by roll (returns -1 if not found or inactive)
int findStudentIndexByRoll(int roll) {
    for (int i = 0; i < studentCount; ++i) {
        if (students[i].active && students[i].roll == roll) return i;
    }
    return -1;
}

// Add a new student
void addStudent() {
    if (studentCount >= MAX_STUDENTS) {
        cout << "Cannot add more students. Max limit reached.\n";
        return;
    }

    Student s;
    cout << "\n--- Add New Student ---\n";
    s.name = getLineNonEmpty("Enter student's full name: ");
    s.roll = getIntInput("Enter roll number (positive integer): ", 1, 9999999);

    if (findStudentIndexByRoll(s.roll) != -1) {
        cout << "A student with roll " << s.roll << " already exists. Use update option instead.\n";
        return;
    }

    s.section = getLineNonEmpty("Enter section (e.g., A, B, C): ");

    cout << "\nEnter marks for each component. Each component is assumed out of 100.\n";
    for (int j = 0; j < NUM_COMPONENTS; ++j) {
        string prompt;
        switch (j) {
        case QUIZ1: prompt = "Quiz 1 (0-100): "; break;
        case QUIZ2: prompt = "Quiz 2 (0-100): "; break;
        case QUIZ3: prompt = "Quiz 3 (0-100): "; break;
        case ASSIGN1: prompt = "Assignment 1 (0-100): "; break;
        case ASSIGN2: prompt = "Assignment 2 (0-100): "; break;
        case MIDTERM: prompt = "Midterm (0-100): "; break;
        case FINAL_EXAM: prompt = "Final Exam (0-100): "; break;
        default: prompt = "Component: "; break;
        }
        s.marks[j] = getDoubleInput(prompt, 0.0, 100.0);
    }

    s.active = true;
    s.total = 0;
    s.percentage = 0;
    s.grade = 'F';
    students[studentCount++] = s;
    calculateTotalsAndGrades();
    saveToFile();
    cout << "Student added successfully.\n";
}

// Update an existing student
void updateStudent() {
    cout << "\n--- Update Student Record ---\n";
    int roll = getIntInput("Enter roll number to update: ", 1, 9999999);
    int idx = findStudentIndexByRoll(roll);
    if (idx == -1) {
        cout << "Student with roll " << roll << " not found.\n";
        return;
    }
    Student& s = students[idx];
    cout << "\nUpdating record for " << s.name << " (Roll: " << s.roll << ")\n";
    cout << "Leave input blank to keep existing value.\n";

    // Update name
    cout << "Current name: " << s.name << "\n";
    cout << "Enter new name or press Enter to keep: ";
    string line;
    getline(cin, line);
    if (!line.empty()) s.name = line;

    // Update section
    cout << "Current section: " << s.section << "\n";
    cout << "Enter new section or press Enter to keep: ";
    getline(cin, line);
    if (!line.empty()) s.section = line;

    // Update marks
    for (int j = 0; j < NUM_COMPONENTS; ++j) {
        string prompt;
        switch (j) {
        case QUIZ1: prompt = "Quiz 1 (0-100) [" + to_string((int)s.marks[j]) + "]: "; break;
        case QUIZ2: prompt = "Quiz 2 (0-100) [" + to_string((int)s.marks[j]) + "]: "; break;
        case QUIZ3: prompt = "Quiz 3 (0-100) [" + to_string((int)s.marks[j]) + "]: "; break;
        case ASSIGN1: prompt = "Assignment 1 (0-100) [" + to_string((int)s.marks[j]) + "]: "; break;
        case ASSIGN2: prompt = "Assignment 2 (0-100) [" + to_string((int)s.marks[j]) + "]: "; break;
        case MIDTERM: prompt = "Midterm (0-100) [" + to_string((int)s.marks[j]) + "]: "; break;
        case FINAL_EXAM: prompt = "Final Exam (0-100) [" + to_string((int)s.marks[j]) + "]: "; break;
        default: prompt = "Component: "; break;
        }
        cout << prompt;
        string in;
        getline(cin, in);
        if (in.size() == 0) {
            // keep
        }
        else {
            // parse number safely
            try {
                double val = stod(in);
                if (val < 0.0 || val > 100.0) {
                    cout << "  >> Value out of range. Keeping old value.\n";
                }
                else {
                    s.marks[j] = val;
                }
            }
            catch (...) {
                cout << "  >> Invalid number. Keeping old value.\n";
            }
        }
    }

    calculateTotalsAndGrades();
    saveToFile();
    cout << "Student record updated.\n";
}

// Delete a student (we will compact the array to remove record)
void deleteStudent() {
    cout << "\n--- Delete Student ---\n";
    int roll = getIntInput("Enter roll number to delete: ", 1, 9999999);
    int idx = findStudentIndexByRoll(roll);
    if (idx == -1) {
        cout << "Student with roll " << roll << " not found.\n";
        return;
    }
    cout << "Are you sure you want to delete record of " << students[idx].name << " (Roll " << roll << ")? (Y/N): ";
    char c;
    cin >> c;
    safeFlush();
    if (toupper(c) == 'Y') {
        // shift left to remove
        for (int i = idx; i < studentCount - 1; ++i) {
            students[i] = students[i + 1];
        }
        studentCount--;
        calculateTotalsAndGrades();
        saveToFile();
        cout << "Record deleted.\n";
    }
    else {
        cout << "Deletion cancelled.\n";
    }
}

// Display table header
void displayStudentHeader() {
    cout << left << setw(4) << "No" << setw(20) << "Name" << setw(8) << "Roll" << setw(8) << "Sec";
    cout << setw(6) << "Q1" << setw(6) << "Q2" << setw(6) << "Q3";
    cout << setw(8) << "A1" << setw(8) << "A2" << setw(8) << "Mid" << setw(8) << "Final";
    cout << setw(10) << "Total" << setw(12) << "Percent" << setw(6) << "Grade" << "\n";
    cout << string(110, '-') << "\n";
}

// Display single student row
void displayStudentRow(const Student& s) {
    cout << left << setw(4) << " " << setw(20) << s.name << setw(8) << s.roll << setw(8) << s.section;
    cout << setw(6) << int(s.marks[QUIZ1]) << setw(6) << int(s.marks[QUIZ2]) << setw(6) << int(s.marks[QUIZ3]);
    cout << setw(8) << int(s.marks[ASSIGN1]) << setw(8) << int(s.marks[ASSIGN2]);
    cout << setw(8) << int(s.marks[MIDTERM]) << setw(8) << int(s.marks[FINAL_EXAM]);
    cout << setw(10) << fixed << setprecision(2) << s.total << setw(12) << fixed << setprecision(2) << s.percentage << setw(6) << s.grade << "\n";
}

// Display all students
void displayAllStudents() {
    cout << "\n--- All Student Records ---\n";
    if (studentCount == 0) {
        cout << "No student records.\n";
        return;
    }
    displayStudentHeader();
    int displayNo = 1;
    for (int i = 0; i < studentCount; ++i) {
        if (!students[i].active) continue;
        cout << setw(4) << displayNo;
        displayStudentRow(students[i]);
        ++displayNo;
    }
}

// Search by roll
void searchStudent() {
    cout << "\n--- Search Student by Roll Number ---\n";
    int roll = getIntInput("Enter roll number: ", 1, 9999999);
    int idx = findStudentIndexByRoll(roll);
    if (idx == -1) {
        cout << "Student with roll " << roll << " not found.\n";
        return;
    }
    Student& s = students[idx];
    cout << "\nStudent found:\n";
    cout << "Name: " << s.name << "\n";
    cout << "Roll: " << s.roll << "\n";
    cout << "Section: " << s.section << "\n";
    cout << "Marks:\n";
    cout << "  Quiz1: " << s.marks[QUIZ1] << "\n";
    cout << "  Quiz2: " << s.marks[QUIZ2] << "\n";
    cout << "  Quiz3: " << s.marks[QUIZ3] << "\n";
    cout << "  Assignment1: " << s.marks[ASSIGN1] << "\n";
    cout << "  Assignment2: " << s.marks[ASSIGN2] << "\n";
    cout << "  Midterm: " << s.marks[MIDTERM] << "\n";
    cout << "  Final: " << s.marks[FINAL_EXAM] << "\n";
    cout << "Total: " << fixed << setprecision(2) << s.total << "\n";
    cout << "Percentage: " << fixed << setprecision(2) << s.percentage << "\n";
    cout << "Grade: " << s.grade << "\n";
}

// Show class average, highest, lowest, toppers
void showClassStatistics() {
    cout << "\n--- Class Statistics ---\n";
    if (studentCount == 0) {
        cout << "No student records.\n";
        return;
    }
    double sumPerc = 0.0;
    double highest = -1e9, lowest = 1e9;
    int count = 0;
    for (int i = 0; i < studentCount; ++i) {
        if (!students[i].active) continue;
        double p = students[i].percentage;
        sumPerc += p;
        if (p > highest) highest = p;
        if (p < lowest) lowest = p;
        ++count;
    }
    double avg = (count > 0) ? (sumPerc / count) : 0.0;
    cout << "Class average percentage: " << fixed << setprecision(2) << avg << "\n";
    cout << "Highest percentage: " << fixed << setprecision(2) << highest << "\n";
    cout << "Lowest percentage: " << fixed << setprecision(2) << lowest << "\n";

    // Toppers (may be more than one)
    cout << "\nTopper(s):\n";
    for (int i = 0; i < studentCount; ++i) {
        if (!students[i].active) continue;
        if (abs(students[i].percentage - highest) < 1e-6) {
            cout << "  " << students[i].name << " (Roll: " << students[i].roll << ") - " << fixed << setprecision(2) << students[i].percentage << "%\n";
        }
    }
}

// Sort students by percentage descending (simple selection sort)
void sortStudentsByPercentageDesc() {
    if (studentCount <= 1) {
        cout << "Not enough students to sort.\n";
        return;
    }
    // We will create a temporary array of indices for active students and sort them,
    // then print sorted result without modifying the main storage (to keep original order).
    int activeCount = 0;
    for (int i = 0; i < studentCount; ++i) if (students[i].active) ++activeCount;
    if (activeCount == 0) {
        cout << "No active records to sort.\n";
        return;
    }

    // Create a small array of indices
    int indices[MAX_STUDENTS];
    int idx = 0;
    for (int i = 0; i < studentCount; ++i) {
        if (students[i].active) {
            indices[idx++] = i;
        }
    }

    // Selection sort indices by students[indices[*]].percentage
    for (int i = 0; i < idx - 1; ++i) {
        int maxPos = i;
        for (int j = i + 1; j < idx; ++j) {
            if (students[indices[j]].percentage > students[indices[maxPos]].percentage) {
                maxPos = j;
            }
        }
        if (maxPos != i) {
            int tmp = indices[i];
            indices[i] = indices[maxPos];
            indices[maxPos] = tmp;
        }
    }

    // Display sorted list
    cout << "\n--- Students Sorted by Percentage (Descending) ---\n";
    cout << left << setw(4) << "No" << setw(20) << "Name" << setw(8) << "Roll" << setw(12) << "Percent" << setw(6) << "Grade" << "\n";
    cout << string(60, '-') << "\n";
    for (int i = 0; i < idx; ++i) {
        Student& s = students[indices[i]];
        cout << setw(4) << i + 1 << setw(20) << s.name << setw(8) << s.roll << setw(12) << fixed << setprecision(2) << s.percentage << setw(6) << s.grade << "\n";
    }
}

// Subject-wise statistics (average per component)
void showSubjectWiseStatistics() {
    cout << "\n--- Subject-wise Statistics ---\n";
    if (studentCount == 0) {
        cout << "No records.\n";
        return;
    }
    double sums[NUM_COMPONENTS];
    for (int j = 0; j < NUM_COMPONENTS; ++j) sums[j] = 0.0;
    int count = 0;
    for (int i = 0; i < studentCount; ++i) {
        if (!students[i].active) continue;
        for (int j = 0; j < NUM_COMPONENTS; ++j) sums[j] += students[i].marks[j];
        ++count;
    }
    if (count == 0) {
        cout << "No active records.\n";
        return;
    }
    cout << left << setw(20) << "Component" << setw(12) << "Average" << "\n";
    cout << string(35, '-') << "\n";
    cout << setw(20) << "Quiz 1" << setw(12) << fixed << setprecision(2) << (sums[QUIZ1] / count) << "\n";
    cout << setw(20) << "Quiz 2" << setw(12) << fixed << setprecision(2) << (sums[QUIZ2] / count) << "\n";
    cout << setw(20) << "Quiz 3" << setw(12) << fixed << setprecision(2) << (sums[QUIZ3] / count) << "\n";
    cout << setw(20) << "Assignment 1" << setw(12) << fixed << setprecision(2) << (sums[ASSIGN1] / count) << "\n";
    cout << setw(20) << "Assignment 2" << setw(12) << fixed << setprecision(2) << (sums[ASSIGN2] / count) << "\n";
    cout << setw(20) << "Midterm" << setw(12) << fixed << setprecision(2) << (sums[MIDTERM] / count) << "\n";
    cout << setw(20) << "Final Exam" << setw(12) << fixed << setprecision(2) << (sums[FINAL_EXAM] / count) << "\n";
}

// -------------------------------------------------
// Menu & main
// -------------------------------------------------

void showMainMenu() {
    cout << "\n------------------------------------------------\n";
    cout << " Student Performance Management System (SPMS) \n";
    cout << "------------------------------------------------\n";
    cout << "1. Add new student\n";
    cout << "2. Update existing student\n";
    cout << "3. Delete student\n";
    cout << "4. Display all students\n";
    cout << "5. Search student by roll number\n";
    cout << "6. Show class statistics (average, highest, lowest, topper)\n";
    cout << "7. Sort students by percentage (descending)\n";
    cout << "8. Show subject-wise statistics\n";
    cout << "9. Export report to text file\n";
    cout << "10. Save & Exit\n";
    cout << "0. Exit without saving\n";
    cout << "------------------------------------------------\n";
}

int main() {
    // Initialize
    for (int i = 0; i < MAX_STUDENTS; ++i) students[i].active = false;
    studentCount = 0;

    // Load data from file if exists
    loadFromFile();
    calculateTotalsAndGrades(); // ensure totals/grades computed

    // Main loop
    while (true) {
        showMainMenu();
        int choice = getIntInput("Enter your choice: ", 0, 10);
        switch (choice) {
        case 1:
            addStudent();
            break;
        case 2:
            updateStudent();
            break;
        case 3:
            deleteStudent();
            break;
        case 4:
            displayAllStudents();
            break;
        case 5:
            searchStudent();
            break;
        case 6:
            showClassStatistics();
            break;
        case 7:
            sortStudentsByPercentageDesc();
            break;
        case 8:
            showSubjectWiseStatistics();
            break;
        case 9: {
            exportReportToFile(REPORT_FILENAME);
            break;
        }
        case 10:
            calculateTotalsAndGrades();
            saveToFile();
            cout << "Saved and exiting program. Goodbye!\n";
            return 0;
        case 0:
            cout << "Exit without saving. Are you sure? (Y/N): ";
            char c;
            cin >> c;
            safeFlush();
            if (toupper(c) == 'Y') {
                cout << "Exiting without saving. Goodbye!\n";
                return 0;
            }
            else {
                cout << "Returning to main menu.\n";
            }
            break;
        default:
            cout << "Invalid choice.\n";
        }
        pressEnterToContinue();
    }

    return 0;
}
