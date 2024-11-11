#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX 100
#define MAX_SCHEMES 50
#define MAX_COLUMNS 10
#define FILE_NAME "schemes.txt"
#define MAX_LENGTH 100

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

void readTable(const scheme *s) {
    if (s == NULL || s->table == NULL || s->table->rowCount == 0) {
        printf("Table is empty or not initialized.\n");
        return;
    }

    printf("Reading table data for scheme: %s\n", s->name);
    printf("+------------------------------------------------+\n");
    printf("|                  %s                    |\n", s->name);
    printf("+------------------------------------------------+\n");

    for (register int j = 0; j < MAX_COLUMNS && s->table->columns->name[j][0] != '\0'; j++) {
        printf("| %-15s ", s->table->columns->name[j]);
    }
    printf("|\n");
    printf("+------------------------------------------------+\n");

    for (register int i = 0; i < s->table->rowCount; i++) {
        printf("| ");
        for (int j = 0; j < MAX_COLUMNS && s->table->columns->contents[j][i] != NULL; j++) {
            printf("%-15s ", s->table->columns->contents[j][i]);
        }
        printf("|\n");
    }
    printf("+------------------------------------------------+\n");
}
void writeTable(FILE *fp, const scheme *s) {
    if (s == NULL || s->table == NULL || s->table->rowCount == 0) {
        fprintf(fp,"Table is empty or not initialized.\n");
        return;
    }

    fprintf(fp,"Reading table data for scheme: %s\n", s->name);
    fprintf(fp,"+------------------------------------------------+\n");
    fprintf(fp,"|                  %s                    |\n", s->name);
    fprintf(fp,"+------------------------------------------------+\n");

    for (register int j = 0; j < MAX_COLUMNS && s->table->columns->name[j][0] != '\0'; j++) {
        fprintf(fp,"| %-15s ", s->table->columns->name[j]);
    }
    fprintf(fp,"|\n");
    fprintf(fp,"+------------------------------------------------+\n");

    for (register int i = 0; i < s->table->rowCount; i++) {
        fprintf(fp,"| ");
        for (int j = 0; j < MAX_COLUMNS && s->table->columns->contents[j][i] != NULL; j++) {
            fprintf(fp,"%-15s ", s->table->columns->contents[j][i]);
        }
        fprintf(fp,"|\n");
    }
    fprintf(fp,"+------------------------------------------------+\n");
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

    for (register int j = 0; j < colCount; j++) {
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
        char input[MAX_LENGTH];

        printf("Enter values for the columns separated by commas (e.g., value1,value2,...): ");
        scanf(" %[^\n]", input);

        if (strcmp(input, "end") == 0) {
            break;
        }


        char *token = strtok(input, ",");
        int j = 0;

        while (token != NULL && j < colCount) {
            s->table->columns->contents[j][s->table->rowCount] = malloc(sizeof(char) * 100);
            strncpy(s->table->columns->contents[j][s->table->rowCount], token, 100);
            token = strtok(NULL, ",");
            j++;
        }


        if (j < colCount) {
            printf("Not enough values provided for all columns. Expected %d but got %d.\n", colCount, j);

            for (register int k = 0; k < j; k++) {
                free(s->table->columns->contents[k][s->table->rowCount]);
            }
        } else {
            s->table->rowCount++;
        }
    }

    printf("Table creation completed with %d rows.\n", s->table->rowCount);
}

void makeScheme(const char *name) {
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

scheme *findScheme(const char *name) {
    for (register int i = 0; i < dbs.schemeCount; i++) {
        if (strcmp(dbs.schemes[i]->name, name) == 0) {
            return dbs.schemes[i];
        }
    }
    return NULL;
}

void deleteScheme(char *name) {
    for (register int i = 0; i < dbs.schemeCount; i++) {
        if (strcmp(dbs.schemes[i]->name, name) == 0) {
            free(dbs.schemes[i]->name);
            if (dbs.schemes[i]->table != NULL) {
                for (register int j = 0; j < dbs.schemes[i]->table->rowCount; j++) {
                    for (register int k = 0; k < MAX_COLUMNS; k++) {
                        if (dbs.schemes[i]->table->columns->contents[k][j] != NULL) {
                            free(dbs.schemes[i]->table->columns->contents[k][j]);
                        }
                    }
                }
                free(dbs.schemes[i]->table->columns);
                free(dbs.schemes[i]->table);
            }
            free(dbs.schemes[i]);

            for (register int j = i; j < dbs.schemeCount - 1; j++) {
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
    for (register int i = 0; i < dbs.schemeCount; i++) {
        free(dbs.schemes[i]->name);
        if (dbs.schemes[i]->table != NULL) {
            for (register int j = 0; j < dbs.schemes[i]->table->rowCount; j++) {
                for (register int k = 0; k < MAX_COLUMNS; k++) {
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
    printf("|                   EASY DBMS                    |\n");
    printf("+------------------------------------------------+\n");
    printf("| 1. Create Scheme                               |\n");
    printf("| 2. Create Table                                |\n");
    printf("| 3. Read Table                                  |\n");
    printf("| 4. Delete Scheme                               |\n");
    printf("| 5. Save                                        |\n");
    printf("| 6. Exit                                        |\n");
    printf("| 7. Load Previous Schemes                       |\n");
    printf("| 8. Show Current Schemes                        |\n");
    printf("| 9. Input Query                                 |\n");
    printf("+------------------------------------------------+\n");
}

void loadSchemes() {
    char *fileName = malloc(MAX_LENGTH * sizeof(char));
    FILE *fp = NULL;
    printf("Enter Scheme filename: ");
    scanf("%s", fileName);
    getchar();

    if (access(fileName, R_OK) == 0) {
        printf("file exists\n");
        fp = fopen(fileName, "r");
    } else {
        printf("file not exists. Running default file\n");
        fp = fopen(FILE_NAME, "r");
    }

    if (fp == NULL) {
        printf("Failed to open the file.\n");
        free(fileName);
        return;
    }

    char line[256];
    scheme *currentScheme = NULL;
    int rowIndex = 0;
    int columnHeaderRead = 0;

    printf("Starting to load schemes...\n");

    while (fgets(line, sizeof(line), fp) != NULL) {
        printf("Read line: %s", line);

        if (strncmp(line, "Reading table data for scheme:", 30) == 0) {
            if (currentScheme != NULL) {
                dbs.schemes[dbs.schemeCount++] = currentScheme;
            }

            char schemeName[50];
            sscanf(line, "Reading table data for scheme: %s", schemeName);
            currentScheme = malloc(sizeof(scheme));
            currentScheme->name = strdup(schemeName);
            currentScheme->table = malloc(sizeof(table));
            currentScheme->table->columns = malloc(sizeof(columns));
            memset(currentScheme->table->columns, 0, sizeof(columns));
            currentScheme->table->rowCount = 0;
            rowIndex = 0;
            columnHeaderRead = 0;
            printf("Created new scheme: %s\n", schemeName);

        } else if (currentScheme && strstr(line, "|") != NULL) {
            if (!columnHeaderRead && strstr(line, "name") != NULL) {
                int colIndex = 0;
                char *token = strtok(line, "|");
                while (token != NULL && colIndex < MAX_COLUMNS) {
                    sscanf(token, " %49s", currentScheme->table->columns->name[colIndex]);
                    token = strtok(NULL, "|");
                    colIndex++;
                }
                columnHeaderRead = 1;
                printf("Column headers loaded\n");

            } else if (columnHeaderRead) {
                int colIndex = 0;
                char *token = strtok(line, "|");
                while (token != NULL && colIndex < MAX_COLUMNS) {
                    currentScheme->table->columns->contents[colIndex][rowIndex] = strdup(token);
                    token = strtok(NULL, "|");
                    colIndex++;
                }
                rowIndex++;
                currentScheme->table->rowCount++;
                printf("Row %d loaded\n", rowIndex);
            }
        }
    }

    if (currentScheme != NULL) {
        dbs.schemes[dbs.schemeCount++] = currentScheme;
    }

    printf("Finished loading schemes.\n");
    fclose(fp);
    free(fileName);
}
void showCurrentSchemes() {
    for (register int i = 0; i < dbs.schemeCount; i++) {
        printf("Scheme: %s\n", dbs.schemes[i]->name);
    }
}
void bufferClear() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void easyDbmsQuery(const char *query) {
    int columnIdx = -1;
    char command[10] = {0};
    char column[50] = {0};
    char from[10] = {0};
    char schemeName[50] = {0};

    sscanf(query, "%s %s %s %s", command, column, from, schemeName);

    if (strcmp(command, "select") == 0) {
        scheme *targetScheme = findScheme(schemeName);
        if (targetScheme != NULL) {
            for (register int i = 0; i < MAX_COLUMNS; i++) {
                if (strcmp(targetScheme->table->columns->name[i], column) == 0) {
                    columnIdx = i;
                    break;
                }
            }

            if (columnIdx == -1) {
                printf("Column '%s' not found in scheme '%s'.\n", column, schemeName);
                return;
            }

            printf("Contents of column '%s' in scheme '%s':\n", column, schemeName);
            for (int i = 0; i < targetScheme->table->rowCount; i++) {
                if (targetScheme->table->columns->contents[columnIdx][i] != NULL) {
                    printf("%s\n", targetScheme->table->columns->contents[columnIdx][i]);
                }
            }
        } else {
            printf("Scheme '%s' not found.\n", schemeName);
        }
    }else if(strcmp(command, "delete") == 0) {
        scheme *targetScheme = findScheme(schemeName);
        if (targetScheme != NULL) {
            deleteScheme(schemeName);
        }else {
            printf("Delete Scheme Failed\n");
            printf("Scheme '%s' not found.\n", schemeName);
        }
    }else {
        printf("Unsupported command '%s'.\n", command);
    }
}




int main(void) {
    int choice;
    char schemeName[50];
    char query[100];
    FILE *file = fopen(FILE_NAME, "a");
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
                printf("Enter scheme name to save table: ");
                scanf("%s", schemeName);
                scheme *sWrite = findScheme(schemeName);
                writeTable(file, sWrite);
                printf("saved scheme '%s' to: %s\n", schemeName, schemeName);
                break;
            case 6:
                freeMemory();
                printf("Exiting...\n");
                fclose(file);
                return 0;
            case 7:
                loadSchemes();
                printf("Load schemes done.\n");
                break;
            case 8:
                showCurrentSchemes();
                break;
            case 9:
                printf("input query:");
                bufferClear();
                fgets(query, sizeof(query), stdin);
                query[strcspn(query, "\n")] = 0;
                easyDbmsQuery(query);
                break;


            default:
                printf("Invalid option, please try again.\n");
        }
    }

}