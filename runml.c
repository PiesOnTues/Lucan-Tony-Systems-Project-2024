#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>

#define LINELENGTH 256

char compiledCode[BUFSIZ]; 

void processFile(FILE *file) {
    char line[LINELENGTH];
    
    char baseCode[] = "#include <stdio.h> \n int main() { ";
    strcat(compiledCode, baseCode);

    // Remove newline character
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';  

        // Skip comments
        if (strchr(line, '#') != NULL) {  
            continue;
        }
        
        // Initialize array to NULL
        char *processedLine[LINELENGTH] = {0};  
        int i = 0;

        char *word = strtok(line, " ");
        while (word != NULL) {
            processedLine[i++] = word; 

            // Handles print statements
            if (strcmp(word, "print") == 0) {
                
                // Appends printf function for float vals
                strcat(compiledCode, "printf(\"%f\", ");

                // Appends first variable
                word = strtok(NULL, " ");
                if (word != NULL) {
                    strcat(compiledCode, word);
                }

                // Checks if this is a one variable/number print (e.g. print 3.5) and closes print with a bracket if it is 
                word = strtok(NULL, " ");
                if (word == NULL) {
                    strcat(compiledCode, ");");
                    break;
                }
                
                // Appends operation symbol, appends second variable and then closes the printf with a bracket
                strcat(compiledCode, word);
                word = strtok(NULL, " ");
                strcat(compiledCode, word);
                strcat(compiledCode, ");");

            }

            // Handle variable assignment
            else if (strcmp(word, "<-") == 0) {

                // Appends: double varaible name =
                strcat(compiledCode, "double ");
                strcat(compiledCode, processedLine[0]);  
                strcat(compiledCode, " = ");

                // Generates and appends variables assigned value
                word = strtok(NULL, " ");
                strcat(compiledCode, word);
                strcat(compiledCode, ";");
            // If the token isn't recognized it will simply generate the next word
            } else {
                word = strtok(NULL, " ");
            }
        }

        // Adds \n for readability
        strcat(compiledCode, "\n");
    }
    strcat(compiledCode, "return 0; }");
}

int main(int argc, char *argv[]) {
    if (argc != 2) { 
        fprintf(stderr, "usage: %s <input file>\n", argv[0]);
        return 1;
    }

    const char *file_in_path = argv[1];

    // Opens .ml file and then compiles it in processFile function 
    FILE *file_in = fopen(file_in_path, "r");
    if (file_in == NULL) {
        perror("error opening input file");
        return 1;
    }
    processFile(file_in);
    fclose(file_in);

    // The compiled code is stored in a char which is then written to a new temporary c file
    char tempC[] = "XXXXXX.c";
    mkstemp(tempC);
    FILE *file = fopen(tempC, "w");
    fprintf(file, "%s", compiledCode);
    fclose(file);

    // The c file is then compiled by executing a compilation command (gcc -o) using the system() function
    char tempExec[] = "XXXXXX";
    char compCmd[LINELENGTH];
    mkstemp(tempExec);
    snprintf(compCmd, LINELENGTH, "gcc -o %s %s", tempExec, tempC);
    system(compCmd);

    // The resulting exec file that is created is then executed
    char execCmd[LINELENGTH];
    snprintf(execCmd, sizeof(compCmd), "./%s", tempExec);
    system(execCmd);

    // Deletes the temporary c and exec files
    unlink(tempC);
    unlink(tempExec);

    return 0;
}