#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100
#define MAX_SCHEMES 50
#define MAX_COLUMNS 10

typedef struct columns {
    char name[MAX_COLUMNS][50];
    char *contents[MAX_COLUMNS][MAX];
} columns;

typedef struct table {
    columns *columns;
    int rowCount;
} table;

typedef struct scheme {
    char *name;
    table *table;
} scheme;

typedef struct db {
    scheme *schemes[MAX_SCHEMES];
    int schemeCount;
} db;

db dbs;

void readTable(scheme *s) {
    if (s == NULL || s->table == NULL || s->table->rowCount == 0) {
        printf("Table is empty or not initialized.\n");
        return;
    }

    printf("Reading table data for scheme: %s\n", s->name);
    printf("+------------------------------------------------+\n");
    printf("|                  %s                    |\n", s->name);
    printf("+------------------------------------------------+\n");

    for (int j = 0; j < MAX_COLUMNS && s->table->columns->name[j][0] != '\0'; j++) {
        printf("| %-15s ", s->table->columns->name[j]);
    }
    printf("|\n");
    printf("+------------------------------------------------+\n");

    for (int i = 0; i < s->table->rowCount; i++) {
        printf("| ");
        for (int j = 0; j < MAX_COLUMNS && s->table->columns->contents[j][i] != NULL; j++) {
            printf("%-15s ", s->table->columns->contents[j][i]);
        }
        printf("|\n");
    }
    printf("+------------------------------------------------+\n");
}

void makeTable(scheme *s) {
    if (s == NULL) {
        printf("Scheme is NULL.\n");
        return;
    }

    s->table = malloc(sizeof(table));
    s->table->columns = malloc(sizeof(columns));
    s->table->rowCount = 0;

    printf("Enter the number of columns (max %d): ", MAX_COLUMNS);
    int colCount;
    scanf("%d", &colCount);

    for (int j = 0; j < colCount; j++) {
        printf("Enter name for column %d: ", j + 1);
        scanf("%s", s->table->columns->name[j]);
    }

    printf("Table created with %d columns.\n", colCount);
    printf("Enter data for the table (type 'end' to finish):\n");

    while (1) {
        if (s->table->rowCount >= MAX) {
            printf("Maximum number of rows reached.\n");
            break;
        }

        printf("Row %d: \n", s->table->rowCount + 1);
        int endInput = 0;
        for (int j = 0; j < colCount; j++) {
            s->table->columns->contents[j][s->table->rowCount] = malloc(sizeof(char) * 100);
            printf("Column %s: ", s->table->columns->name[j]);
            scanf("%s", s->table->columns->contents[j][s->table->rowCount]);

            if (strcmp(s->table->columns->contents[j][s->table->rowCount], "end") == 0) {
                endInput = 1;
                free(s->table->columns->contents[j][s->table->rowCount]);
                s->table->columns->contents[j][s->table->rowCount] = NULL;
            }
        }

        if (endInput) {
            break;
        }

        s->table->rowCount++;
    }

    printf("Table creation completed with %d rows.\n", s->table->rowCount);
}

void makeScheme(char *name) {
    if (dbs.schemeCount >= MAX_SCHEMES) {
        printf("Maximum number of schemes reached.\n");
        return;
    }

    scheme *newScheme = malloc(sizeof(scheme));
    newScheme->name = strdup(name);
    newScheme->table = NULL;

    dbs.schemes[dbs.schemeCount] = newScheme;
    dbs.schemeCount++;
}

scheme *findScheme(char *name) {
    for (int i = 0; i < dbs.schemeCount; i++) {
        if (strcmp(dbs.schemes[i]->name, name) == 0) {
            return dbs.schemes[i];
        }
    }
    return NULL;
}

void deleteScheme(char *name) {
    for (int i = 0; i < dbs.schemeCount; i++) {
        if (strcmp(dbs.schemes[i]->name, name) == 0) {
            // 메모리 해제
            free(dbs.schemes[i]->name);
            if (dbs.schemes[i]->table != NULL) {
                for (int j = 0; j < dbs.schemes[i]->table->rowCount; j++) {
                    for (int k = 0; k < MAX_COLUMNS; k++) {
                        if (dbs.schemes[i]->table->columns->contents[k][j] != NULL) {
                            free(dbs.schemes[i]->table->columns->contents[k][j]);
                        }
                    }
                }
                free(dbs.schemes[i]->table->columns);
                free(dbs.schemes[i]->table);
            }
            free(dbs.schemes[i]);

            // 스키마 배열에서 제거
            for (int j = i; j < dbs.schemeCount - 1; j++) {
                dbs.schemes[j] = dbs.schemes[j + 1];
            }
            dbs.schemeCount--;

            printf("Scheme '%s' deleted successfully.\n", name);
            return;
        }
    }
    printf("Scheme '%s' not found.\n", name);
}

void freeMemory() {
    for (int i = 0; i < dbs.schemeCount; i++) {
        free(dbs.schemes[i]->name);
        if (dbs.schemes[i]->table != NULL) {
            for (int j = 0; j < dbs.schemes[i]->table->rowCount; j++) {
                for (int k = 0; k < MAX_COLUMNS; k++) {
                    if (dbs.schemes[i]->table->columns->contents[k][j] != NULL) {
                        free(dbs.schemes[i]->table->columns->contents[k][j]);
                    }
                }
            }
            free(dbs.schemes[i]->table->columns);
            free(dbs.schemes[i]->table);
        }
        free(dbs.schemes[i]);
    }
}

void showMenu() {
    printf("+------------------------------------------------+\n");
    printf("|                   DBMS Menu                    |\n");
    printf("+------------------------------------------------+\n");
    printf("| 1. Create Scheme                                |\n");
    printf("| 2. Create Table                                 |\n");
    printf("| 3. Read Table                                   |\n");
    printf("| 4. Delete Scheme                                |\n");
    printf("| 5. Exit                                         |\n");
    printf("+------------------------------------------------+\n");
}

int main(void) {
    int choice;
    char schemeName[50];

    while (1) {
        showMenu();
        printf("Select an option: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter scheme name: ");
                scanf("%s", schemeName);
                makeScheme(schemeName);
                printf("Scheme created: %s\n", schemeName);
                break;

            case 2:
                printf("Enter scheme name to create table in: ");
                scanf("%s", schemeName);
                scheme *s = findScheme(schemeName);
                if (s) {
                    makeTable(s);
                } else {
                    printf("Scheme '%s' not found.\n", schemeName);
                }
                break;

            case 3:
                printf("Enter scheme name to read table: ");
                scanf("%s", schemeName);
                scheme *sRead = findScheme(schemeName);
                if (sRead) {
                    readTable(sRead);
                } else {
                    printf("Scheme '%s' not found.\n", schemeName);
                }
                break;

            case 4:
                printf("Enter scheme name to delete: ");
                scanf("%s", schemeName);
                deleteScheme(schemeName);
                break;

            case 5:
                freeMemory();
                printf("Exiting...\n");
                return 0;

            default:
                printf("Invalid option, please try again.\n");
        }
    }

    return 0;
}