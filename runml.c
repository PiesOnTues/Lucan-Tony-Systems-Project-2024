#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define LINELENGTH 256

void processFile(FILE *file) {
    char line[LINELENGTH];
    char code[BUFSIZ];
    bool isVar = false;
    bool isStr = false;

    // Remove newline character
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';  

        // Skip comments
        if (strchr(line, '#') != NULL) {  
            continue;
        }
        
        // Reset code buffer
        code[0] = '\0';  

        // Initialize array to NULL
        char *processedLine[LINELENGTH] = {0};  
        int i = 0;

        char *word = strtok(line, " \t");
        while (word != NULL) {
            processedLine[i++] = word; 

            if (strcmp(word, "print") == 0) {
                strcat(code, "printf(\"%f\", ");
                isStr = true;
            } else if (isStr) {
                strcat(code, word);
                strcat(code, ");");
                isStr = false;
            } 

            // Handle variable assignment
            else if (strcmp(word, "<-") == 0) {
                if (i > 0) {
                    strcat(code, "double ");
                    strcat(code, processedLine[0]);  
                    strcat(code, " = ");
                    isVar = true;
                }
            } else if (isVar) {
                strcat(code, word);
                strcat(code, ";");
                isVar = false;
            }

            // Get the next word
            word = strtok(NULL, " \t");
        }

        // Print the processed line if code is not empty
        if (strlen(code) > 0) {
            printf("%s\n", code);
        }
    }

    // Check for errors while reading the file
    if (ferror(file)) {
        perror("error reading input file");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) { 
        fprintf(stderr, "usage: %s <input file>\n", argv[0]);
        return 1;
    }

    const char *file_in_path = argv[1];

    FILE *file_in = fopen(file_in_path, "r");
    if (file_in == NULL) {
        perror("error opening input file");
        return 1;
    }

    processFile(file_in);
    fclose(file_in);

    return 0;
}