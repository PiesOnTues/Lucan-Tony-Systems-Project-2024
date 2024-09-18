#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>

#define LINELENGTH 256

// Variable defined oustide of main so it has global scope
char compiledCode[BUFSIZ] = ""; 

char processLine(char *line) {

    // Stores previous word
    char prev[50];

    // stores the processed line as a whole
    char compiledLine[100];

    // Tokenizes each line of code into individual words
    char *word = strtok(line, " ");

    // Loops through the line
    while (word != NULL) {

        // Handles print statements
        if (strcmp(word, "print") == 0) {
                    
            // Appends printf function for float vals
            strcat(compiledLine, "printf(\"%f\", ");

            // Appends first variable
            word = strtok(NULL, " ");
            if (word != NULL) {
                strcat(compiledLine, word);
            }

            // Ends print statement if it is a one variable/number print (e.g. print 3.5) 
            word = strtok(NULL, " ");
            if (word == NULL) {
                strcat(compiledLine, ");");
                break;
            }
                
            // If it is a operation print it appends the operation symbol, appends second variable and then closes the print
            strcat(compiledLine, word);
            word = strtok(NULL, " ");
            strcat(compiledLine, word);
            strcat(compiledLine, ");");

        }

        // Handles variable assignment
        else if (strcmp(word, "<-") == 0) {

            // Appends: "double" + varaible name + "="
            strcat(compiledLine, "double ");
            strcat(compiledLine, prev);  
            strcat(compiledLine, " = ");

            // Generates and appends variables assigned value
            word = strtok(NULL, " ");
            strcat(compiledLine, word);
            strcat(compiledLine, ";");

            // If the token isn't recognized it will simply generate the next word
        
        }
        
        // Handles function definitions
        else if (strcmp(word, "function") == 0) {

            // Contcatenates "double" + function name
            strcat(compiledLine, "double ");
            word = strtok(NULL, " ");
            strcat(compiledLine, word);  
            strcat(compiledLine, "(");

            // Concatenates the parameters of the function
            while (strtok != NULL) {
                word = strtok(NULL, " ");
                strcat(compiledLine, "double ");
                strcat(compiledLine, word); 
                strcat(compiledLine, " ");
            }

            // adds closing characters
            strcat(compiledLine, ");");

        }

        // updates previous word and moves to next word
        else {
            strncpy(prev, word, sizeof(prev) - 1);
            word = strtok(NULL, " ");
        }

        return compiledLine;

    }
     // Adds \n for readability
    strcat(compiledLine, "\n");
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
        } else if (strchr(line, 'function') != NULL) {
            processFunction(line);
        }
        
        // Initialize processedData array which stores all previous code within the .ml file
        char *processedData[LINELENGTH] = {0};  
        int i = 0;

        processLine(line);
    }

    strcat(baseCode, "return 0; }");

}

// Processes a single function
char processFunction(char *line) {

    // Define name and perameters
    char compiledFunc = "";
    processLine(*line);
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
    FILE *file_out = fopen(tempC, "w");
    fprintf(file_out, "%s", compiledCode);
    fclose(file_out);

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
    remove(tempC);
    remove(tempExec);

    return 0;
}