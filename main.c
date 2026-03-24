#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_NAME "students.dat"
#define TEMP_FILE "temp.dat"
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
void calculateResult(Student *s);
char determineGrade(float average);
void clearBuffer();

// CLI Wrapper functions for Python
void cliAdd(int reg, char* name, float m1, float m2, float m3);
void cliList();
void cliUpdate(int reg, float m1, float m2, float m3);
void cliDelete(int reg);
void cliStats();

int main(int argc, char *argv[]) {
    if (argc > 1) {
        if (strcmp(argv[1], "add") == 0 && argc == 7) {
            cliAdd(atoi(argv[2]), argv[3], atof(argv[4]), atof(argv[5]), atof(argv[6]));
        } else if (strcmp(argv[1], "list") == 0) {
            cliList();
        } else if (strcmp(argv[1], "update") == 0 && argc == 6) {
            cliUpdate(atoi(argv[2]), atof(argv[3]), atof(argv[4]), atof(argv[5]));
        } else if (strcmp(argv[1], "delete") == 0 && argc == 3) {
            cliDelete(atoi(argv[2]));
        } else if (strcmp(argv[1], "stats") == 0) {
            cliStats();
        }
        return 0;
    }

    printf("Interactive menu is available when running without arguments.\n");
    return 0;
}

void calculateResult(Student *s) {
    s->total = 0;
    for (int i = 0; i < NUM_SUBJECTS; i++) s->total += s->marks[i];
    s->average = s->total / NUM_SUBJECTS;
    s->grade = determineGrade(s->average);
}

char determineGrade(float average) {
    if (average >= 85) return 'A';
    if (average >= 70) return 'B';
    if (average >= 50) return 'C';
    return 'F';
}

void cliAdd(int reg, char* name, float m1, float m2, float m3) {
    FILE *fp = fopen(FILE_NAME, "ab");
    if (!fp) return;
    Student s;
    s.regNo = reg;
    strncpy(s.name, name, MAX_NAME);
    s.marks[0] = m1; s.marks[1] = m2; s.marks[2] = m3;
    calculateResult(&s);
    fwrite(&s, sizeof(Student), 1, fp);
    fclose(fp);
    printf("SUCCESS\n");
}

void cliList() {
    FILE *fp = fopen(FILE_NAME, "rb");
    if (!fp) return;
    Student s;
    while (fread(&s, sizeof(Student), 1, fp)) {
        printf("%d|%s|%.2f|%.2f|%.2f|%.2f|%.2f|%c\n", 
               s.regNo, s.name, s.marks[0], s.marks[1], s.marks[2], s.total, s.average, s.grade);
    }
    fclose(fp);
}

void cliUpdate(int reg, float m1, float m2, float m3) {
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
}

void cliDelete(int reg) {
    FILE *fp = fopen(FILE_NAME, "rb");
    FILE *ft = fopen(TEMP_FILE, "wb");
    if (!fp || !ft) return;
    Student s;
    while (fread(&s, sizeof(Student), 1, fp)) {
        if (s.regNo != reg) fwrite(&s, sizeof(Student), 1, ft);
    }
    fclose(fp); fclose(ft);
    remove(FILE_NAME); rename(TEMP_FILE, FILE_NAME);
    printf("SUCCESS\n");
}

void cliStats() {
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
}

void clearBuffer() { int c; while ((c = getchar()) != '\n' && c != EOF); }
