#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>

#define LINELENGTH 256

// Variable defined oustide of main so it has global scope
char compiledCode[BUFSIZ] = ""; 

int hasDec(double value) {
    // Convert the double to a string
    char buffer[50];
    snprintf(buffer, sizeof(buffer), "%.10f", value);
    
    // Find the decimal point in the string
    char *decimal_point = strchr(buffer, '.');
    
    // Check the characters after the decimal point for a non-zero value
    for (char *p = decimal_point + 1; *p; p++) {
        if (*p != '0') {
            return 1; 
        }
    }
    return 0; 
}

void processLine(char *line) {
    char prev[50];

    // Tokenizes each line of code into individual words
    char *word = strtok(line, " ");
    while (word != NULL) {
        // Handles print statements
        if (strcmp(word, "print") == 0) {
            char expression[50] = "";

            

            // Appends first variable
            word = strtok(NULL, " ");
            strcat(expression, word);

            // Ends print statement if it is a one variable/number print (e.g. print 3.5) 
            word = strtok(NULL, " ");
            if (word != NULL) {
                // If it is a operation print it appends the operation symbol, appends second variable and then closes the print
                strcat(expression, word);
                word = strtok(NULL, " ");
                strcat(expression, word);
            }
    
            strcat(compiledCode, "printf(\"%f\", ");
    
        
            strcat(compiledCode, expression);
            strcat(compiledCode, ");");
            
            
            
        }

        // Handle variable assignment
        else if (strcmp(word, "<-") == 0) {

            // Appends: double varaible name =
            strcat(compiledCode, "double ");
            strcat(compiledCode, prev);  
            strcat(compiledCode, " = ");

            // Generates and appends variables assigned value
            word = strtok(NULL, " ");
            strcat(compiledCode, word);
            strcat(compiledCode, ";");
            // If the token isn't recognized it will simply generate the next word
        } else {
            strncpy(prev, word, sizeof(prev) - 1);
            word = strtok(NULL, " ");
        }
    }
     // Adds \n for readability
    strcat(compiledCode, "\n");
}

void processFile(FILE *file) {
    char line[LINELENGTH];

   // Appends stock c code 
    char baseCode[] = "#include <stdio.h> \n int main() { ";
    strcat(compiledCode, baseCode);

    // Reads file line by line
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';  

        // Skip comments
        if (strchr(line, '#') != NULL) {  
            continue;
        }
        
        // Initialize processedLine array which stores all previous code within the .ml file
        char *processedLine[LINELENGTH] = {0};  
        int i = 0;

        processLine(line);
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
    char tempC[] = "temp.c";
    mkstemp(tempC);
    FILE *file_out = fopen(tempC, "w");
    fprintf(file_out, "%s", compiledCode);
    fclose(file_out);

    // The c file is then compiled by executing a compilation command (gcc -o) using the system() function
    char tempExec[] = "XXXXXX";
    char compCmd[LINELENGTH];
    mkstemp(tempExec);
    snprintf(compCmd, LINELENGTH, " cc -std=c11 -Wall -Werror -o %s %s", tempExec, tempC);
    system(compCmd);

    // The resulting exec file that is created is then executed
    char execCmd[LINELENGTH];
    snprintf(execCmd, sizeof(compCmd), "./%s", tempExec);
    system(execCmd);

    // Deletes the temporary c and exec files
    remove(tempC);
    remove(tempExec);

    return 0;
}