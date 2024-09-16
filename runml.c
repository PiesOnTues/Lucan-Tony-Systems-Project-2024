#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define LINELENGTH 256

void processFile(FILE *file) {
    char line[LINELENGTH];
    char code[BUFSIZ];


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

        char *word = strtok(line, " ");
        while (word != NULL) {
            processedLine[i++] = word; 
    
            if (strcmp(word, "print") == 0) {
                
                // appends printf function for float vals
                strcat(code, "printf(\"%f\", ");

                // appends first variable
                word = strtok(NULL, " ");
                if (word != NULL) {
                    strcat(code, word);
                }

                // checks if this is a one variable/number print (e.g. print 3.5) and closes print with a bracket if it is 
                word = strtok(NULL, " ");
                if (word == NULL) {
                    strcat(code, ");");
                    break;
                }
                
                // appends operation symbol, appends second variable and then closes the printf with a bracket
                strcat(code, word);
                word = strtok(NULL, " ");
                strcat(code, word);
                strcat(code, ");");

            }
            // Handle variable assignment
            else if (strcmp(word, "<-") == 0) {
                // Appends: double varaible name =
                strcat(code, "double ");
                strcat(code, processedLine[0]);  
                strcat(code, " = ");
                // Generates and appends variables assigned value
                word = strtok(NULL, " ");
                strcat(code, word);
                strcat(code, ";");
            } else {
                word = strtok(NULL, " ");
            }
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