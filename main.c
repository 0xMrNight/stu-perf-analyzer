#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_NAME "students.dat"
#define MAX_NAME 50
#define NUM_SUBJECTS 3

typedef struct {
    int regNo;
    char name[MAX_NAME];
    float marks[NUM_SUBJECTS];
    float total;
    float average;
    char grade;
} Student;

// Function Prototypes
void addStudent();
void displayAllStudents();
void searchStudent();
void updateMarks();
void displayStatistics();
void calculateResult(Student *s);
char determineGrade(float average);
void clearBuffer();

int main() {
    int choice;

    while (1) {
        printf("\n--- Smart Student Performance Analyzer ---\n");
        printf("1. Add Student Record\n");
        printf("2. Display All Records\n");
        printf("3. Search Student by RegNo\n");
        printf("4. Update Marks\n");
        printf("5. View Class Statistics & Grade Distribution\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            clearBuffer();
            continue;
        }

        switch (choice) {
            case 1: addStudent(); break;
            case 2: displayAllStudents(); break;
            case 3: searchStudent(); break;
            case 4: updateMarks(); break;
            case 5: displayStatistics(); break;
            case 6: printf("Exiting program.\n"); exit(0);
            default: printf("Invalid choice. Try again.\n");
        }
    }
    return 0;
}

void calculateResult(Student *s) {
    s->total = 0;
    for (int i = 0; i < NUM_SUBJECTS; i++) {
        s->total += s->marks[i];
    }
    s->average = s->total / NUM_SUBJECTS;
    s->grade = determineGrade(s->average);
}

char determineGrade(float average) {
    if (average >= 85) return 'A';
    if (average >= 70) return 'B';
    if (average >= 50) return 'C';
    return 'F';
}

void addStudent() {
    FILE *fp = fopen(FILE_NAME, "ab");
    if (!fp) {
        printf("Error opening file!\n");
        return;
    }

    Student s;
    printf("Enter Registration Number: ");
    if (scanf("%d", &s.regNo) != 1) {
        printf("Invalid Registration Number.\n");
        clearBuffer();
        fclose(fp);
        return;
    }
    clearBuffer();

    printf("Enter Name: ");
    if (fgets(s.name, MAX_NAME, stdin)) {
        s.name[strcspn(s.name, "\n")] = 0;
    }

    for (int i = 0; i < NUM_SUBJECTS; i++) {
        do {
            printf("Enter marks for Subject %d (0-100): ", i + 1);
            if (scanf("%f", &s.marks[i]) != 1) {
                printf("Invalid input. Setting marks to 0.\n");
                s.marks[i] = 0;
                clearBuffer();
            }
        } while (s.marks[i] < 0 || s.marks[i] > 100);
    }

    calculateResult(&s);
    fwrite(&s, sizeof(Student), 1, fp);
    fclose(fp);
    printf("Record added successfully!\n");
}

void displayAllStudents() {
    FILE *fp = fopen(FILE_NAME, "rb");
    if (!fp) {
        printf("No records found.\n");
        return;
    }

    Student s;
    printf("\n%-10s %-20s %-8s %-8s %-5s\n", "RegNo", "Name", "Total", "Avg", "Grade");
    printf("------------------------------------------------------------\n");
    int count = 0;
    while (fread(&s, sizeof(Student), 1, fp)) {
        printf("%-10d %-20s %-8.2f %-8.2f %-5c\n", s.regNo, s.name, s.total, s.average, s.grade);
        count++;
    }
    if (count == 0) printf("No records found in file.\n");
    fclose(fp);
}

void searchStudent() {
    FILE *fp = fopen(FILE_NAME, "rb");
    if (!fp) {
        printf("No records found.\n");
        return;
    }

    int reg, found = 0;
    printf("Enter Registration Number to search: ");
    if (scanf("%d", &reg) != 1) {
        printf("Invalid input.\n");
        clearBuffer();
        fclose(fp);
        return;
    }

    Student s;
    while (fread(&s, sizeof(Student), 1, fp)) {
        if (s.regNo == reg) {
            printf("\nRecord Found:\n");
            printf("Name: %s\n", s.name);
            printf("Total: %.2f\n", s.total);
            printf("Average: %.2f\n", s.average);
            printf("Grade: %c\n", s.grade);
            found = 1;
            break;
        }
    }
    if (!found) printf("Student with RegNo %d not found.\n", reg);
    fclose(fp);
}

void updateMarks() {
    FILE *fp = fopen(FILE_NAME, "rb+");
    if (!fp) {
        printf("No records found.\n");
        return;
    }

    int reg, found = 0;
    printf("Enter RegNo to update marks: ");
    if (scanf("%d", &reg) != 1) {
        printf("Invalid input.\n");
        clearBuffer();
        fclose(fp);
        return;
    }

    Student s;
    while (fread(&s, sizeof(Student), 1, fp)) {
        if (s.regNo == reg) {
            printf("Updating marks for %s\n", s.name);
            for (int i = 0; i < NUM_SUBJECTS; i++) {
                do {
                    printf("New marks for Subject %d (0-100): ", i + 1);
                    if (scanf("%f", &s.marks[i]) != 1) {
                        clearBuffer();
                        continue;
                    }
                } while (s.marks[i] < 0 || s.marks[i] > 100);
            }
            calculateResult(&s);
            fseek(fp, -sizeof(Student), SEEK_CUR);
            fwrite(&s, sizeof(Student), 1, fp);
            found = 1;
            break;
        }
    }
    if (found) printf("Record updated.\n");
    else printf("Record not found.\n");
    fclose(fp);
}

void displayStatistics() {
    FILE *fp = fopen(FILE_NAME, "rb");
    if (!fp) {
        printf("No records found.\n");
        return;
    }

    Student s, topper;
    float classTotal = 0, highest = -1, lowest = 1001;
    int count = 0, aCount = 0, bCount = 0, cCount = 0, fCount = 0;

    while (fread(&s, sizeof(Student), 1, fp)) {
        classTotal += s.average;
        if (s.total > highest) {
            highest = s.total;
            topper = s;
        }
        if (s.total < lowest) lowest = s.total;

        if (s.grade == 'A') aCount++;
        else if (s.grade == 'B') bCount++;
        else if (s.grade == 'C') cCount++;
        else fCount++;
        
        count++;
    }
    fclose(fp);

    if (count == 0) {
        printf("No records to analyze.\n");
        return;
    }

    printf("\n--- Class Statistics ---\n");
    printf("Total Students: %d\n", count);
    printf("Class Average: %.2f\n", classTotal / count);
    printf("Highest Total: %.2f (Topper: %s)\n", highest, topper.name);
    printf("Lowest Total: %.2f\n", lowest);
    printf("\n--- Grade Distribution ---\n");
    printf("A: %d\nB: %d\nC: %d\nFail: %d\n", aCount, bCount, cCount, fCount);
}

void clearBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
