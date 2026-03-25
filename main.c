#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// File names for data storage
#define FILE_NAME "students.dat"
#define TEMP_FILE "temp.dat"

#define MAX_NAME 50
#define NUM_SUBJECTS 3

// Structure to hold student information
typedef struct {
    int regNo;
    char name[MAX_NAME];
    float marks[NUM_SUBJECTS];
    float total;
    float average;
    char grade;
} Student;

// Function prototypes for calculations and buffer management
void calculateResult(Student *s);
char determineGrade(float average);
void clearBuffer();

// Function prototypes for CLI operations
void addStu(int reg, char* name, float m1, float m2, float m3);
int listStu();
void updStu(int reg, float m1, float m2, float m3);
void delStu(int reg);
void stats();

// Menu function
void menu();

int main(int argc, char *argv[]) {
    // Check if command line arguments are provided (CLI mode)
    if (argc > 1) {
        if (strcmp(argv[1], "add") == 0 && argc == 7) {
            addStu(atoi(argv[2]), argv[3], atof(argv[4]), atof(argv[5]), atof(argv[6]));
        } else if (strcmp(argv[1], "list") == 0) {
            listStu();
        } else if (strcmp(argv[1], "update") == 0 && argc == 6) {
            updStu(atoi(argv[2]), atof(argv[3]), atof(argv[4]), atof(argv[5]));
        } else if (strcmp(argv[1], "delete") == 0 && argc == 3) {
            delStu(atoi(argv[2]));
        } else if (strcmp(argv[1], "stats") == 0) {
            stats();
        }
        return 0;
    }

    // Call interactive menu when no arguments are provided
    menu();
    return 0;
}

// Interactive menu that calls CLI functions
void menu() {
    int choice;
    while (1) {
        printf("\n--- Smart Student Performance Analyzer ---\n");
        printf("1. Add Student Record\n");
        printf("2. Display All Records\n");
        printf("3. Update Marks\n");
        printf("4. Delete Record\n");
        printf("5. View Class Statistics\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            clearBuffer();
            continue;
        }

        if (choice == 6) break;

        switch (choice) {
            case 1: {
                int reg;
                char name[MAX_NAME];
                float m[3];
                printf("Enter Registration Number: ");
                if (scanf("%d", &reg) != 1) { printf("Invalid input.\n"); clearBuffer(); break; }
                clearBuffer();
                printf("Enter Name: ");
                fgets(name, MAX_NAME, stdin);
                name[strcspn(name, "\n")] = 0;
                for (int i = 0; i < 3; i++) {
                    do {
                        printf("Enter marks for Subject %d (0-100): ", i + 1);
                        if (scanf("%f", &m[i]) != 1) { m[i] = -1; clearBuffer(); }
                    } while (m[i] < 0 || m[i] > 100);
                }
                addStu(reg, name, m[0], m[1], m[2]);
                printf("Record added successfully.\n");
                break;
            }
            case 2:
                printf("\nRegNo|Name|M1|M2|M3|Total|Avg|Grade\n");
                printf("--------------------------------------\n");
                if (listStu() == 0) {
                    printf("No records found in database.\n");
                }
                break;
            case 3: {
                int reg;
                float m[3];
                if (listStu() == 0) {
                    printf("No records found in database.\n");
                    break;
                }
                printf("Enter RegNo to update: ");
                if (scanf("%d", &reg) != 1) { printf("Invalid input.\n"); clearBuffer(); break; }
                for (int i = 0; i < 3; i++) {
                    do {
                        printf("Enter new marks for Subject %d (0-100): ", i + 1);
                        if (scanf("%f", &m[i]) != 1) { m[i] = -1; clearBuffer(); }
                    } while (m[i] < 0 || m[i] > 100);
                }
                updStu(reg, m[0], m[1], m[2]);
                break;
            }
            case 4: {
                int reg;
                printf("Enter RegNo to delete: ");
                if (scanf("%d", &reg) != 1) { printf("Invalid input.\n"); clearBuffer(); break; }
                delStu(reg);
                printf("Record processed.\n");
                break;
            }
            case 5:
                printf("\nAvg|Topper|High|Low|A|B|C|F\n");
                printf("-------------------------------\n");
                if (listStu() == 0) {
                    printf("No records found in database.\n");
                    break;
                }
                stats();
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }
    }
}

// Calculate average and determine grade for a student
void calculateResult(Student *s) {
    s->total = 0;
    for (int i = 0; i < NUM_SUBJECTS; i++) s->total += s->marks[i];
    s->average = s->total / NUM_SUBJECTS;
    s->grade = determineGrade(s->average);
}

// Assign grade based on average score
char determineGrade(float average) {
    if (average >= 85) return 'A';
    if (average >= 70) return 'B';
    if (average >= 50) return 'C';
    return 'F';
}

// Add a student record to the binary file
void addStu(int reg, char* name, float m1, float m2, float m3) {
    FILE *fp = fopen(FILE_NAME, "ab");
    if (!fp) return;
    Student s;
    s.regNo = reg;
    strncpy(s.name, name, MAX_NAME);
    s.marks[0] = m1; s.marks[1] = m2; s.marks[2] = m3;
    calculateResult(&s);
    fwrite(&s, sizeof(Student), 1, fp);
    fclose(fp);
}

// List all student records in a pipe-delimited format
int listStu() {
    FILE *fp = fopen(FILE_NAME, "rb");
    if (!fp) return 0;
    Student s;
    int count = 0;
    while (fread(&s, sizeof(Student), 1, fp)) {
        printf("%d|%s|%.2f|%.2f|%.2f|%.2f|%.2f|%c\n", 
               s.regNo, s.name, s.marks[0], s.marks[1], s.marks[2], s.total, s.average, s.grade);
        count++;
    }
    fclose(fp);
    return count;
}

// Update marks for a specific student using RegNo
void updStu(int reg, float m1, float m2, float m3) {
    FILE *fp = fopen(FILE_NAME, "rb+");
    if (!fp) return;
    Student s;
    int found = 0;
    while (fread(&s, sizeof(Student), 1, fp)) {
        if (s.regNo == reg) {
            s.marks[0] = m1; s.marks[1] = m2; s.marks[2] = m3;
            calculateResult(&s);
            fseek(fp, -sizeof(Student), SEEK_CUR);
            fwrite(&s, sizeof(Student), 1, fp);
            found = 1; break;
        }
    }
    fclose(fp);
    if (found) printf("SUCCESS\n");
    else printf("Student not found.\n");
}

// Delete a student record by RegNo using a temporary file
void delStu(int reg) {
    FILE *fp = fopen(FILE_NAME, "rb");
    FILE *ft = fopen(TEMP_FILE, "wb");
    if (!fp || !ft) return;
    Student s;
    while (fread(&s, sizeof(Student), 1, fp)) {
        if (s.regNo != reg) fwrite(&s, sizeof(Student), 1, ft);
    }
    fclose(fp); fclose(ft);
    remove(FILE_NAME); rename(TEMP_FILE, FILE_NAME);
}

// Calculate and output class wide statistics
void stats() {
    FILE *fp = fopen(FILE_NAME, "rb");
    if (!fp) return;
    Student s, topper;
    float classTotal = 0, highest = -1, lowest = 1000;
    int count = 0, a = 0, b = 0, c = 0, f = 0;
    while (fread(&s, sizeof(Student), 1, fp)) {
        classTotal += s.average;
        if (s.total > highest) { highest = s.total; topper = s; }
        if (s.total < lowest) lowest = s.total;
        if (s.grade == 'A') a++; else if (s.grade == 'B') b++;
        else if (s.grade == 'C') c++; else f++;
        count++;
    }
    fclose(fp);
    if (count > 0)
        printf("%.2f|%s|%.2f|%.2f|%d|%d|%d|%d\n", classTotal/count, topper.name, highest, lowest, a, b, c, f);
    else
        printf("No records available.\n");
}

// Clear input buffer to handle invalid inputs
void clearBuffer() { int c; while ((c = getchar()) != '\n' && c != EOF); }
