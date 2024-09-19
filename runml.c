#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>

#define LINELENGTH 256

void processFunction(char *line);

// Variable defined outside of main so it has global scope
char compiledCode[BUFSIZ] = "";
// mainCode stores all code within main function 
char mainCode[BUFSIZ];
// compiledFunc stores all code within functions 
char compiledFunc[BUFSIZ];

// Tells us if we are in a function or not
bool inFunc = false;

// processes a single line of ml
char *processLine(char *line) {

    // Stores previous word
    char prev[50];

    // stores the processed line as a whole
    static char compiledLine[LINELENGTH];
    compiledLine[0] = '\0'; // Initialize to empty

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
                strcat(compiledLine, ");\n");
                break;
            }
                
            // If it is a operation print it appends the operation symbol, appends second variable and then closes the print
            strcat(compiledLine, word);
            word = strtok(NULL, " ");
            strcat(compiledLine, word);
            strcat(compiledLine, ");\n");

        }

        // Handle variable assignment
        else if (strcmp(word, "<-") == 0) {

            // Appends: "double" + variable name + "="
            strcat(compiledLine, "double ");
            strcat(compiledLine, prev);  
            strcat(compiledLine, " = ");

            // Generates and appends variable's assigned value
            word = strtok(NULL, " ");
            strcat(compiledLine, word);
            strcat(compiledLine, ";\n");
        }
        // If the token isn't recognized it will simply generate the next word
        else {
            strncpy(prev, word, sizeof(prev) - 1);
            word = strtok(NULL, " ");
        }
    
    }
    return compiledLine;
}

// Joins multiple functions into one consolidated file
void processFile(FILE *file) {
    char line[LINELENGTH];

    // Adds headers
    strcat(compiledCode, "#include <stdio.h> \n ");

    // Reads file line by line
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';  
        printf("%s\n", line);
        // Skip comments
        if (strchr(line, '#') != NULL) {  
            continue;
        } 
        // Checks line for the word "function" then processes the function declaration and appends to compiledFunc
        else if (strstr(line, "function") != NULL) {
            inFunc = true;
            processFunction(line);
        } 
        // If the line(s) after the function don't contain either a tab or a blank space at the beginning 
        // We can assume that those lines aren't part of the function and that the function has ended
        // Once the function has ended we append the compiled function to the compiled code
        else if ((inFunc == true) && (line[0] != ' ' || line[0] != '\t')) {
            inFunc = false;
            strcat(compiledCode, compiledFunc);
            strcat(compiledCode, " }");
        } 
        // If we are inside a function we simply process the lines and then append them to the funciton
        else if (inFunc == true) { 
            strcat(compiledFunc, processLine(line));
        }
        strcat(mainCode, processLine(line));
    }
    // Appends stock C code 
    strcat(compiledCode, "int main() { ");
    // Appends all code within main 
    strcat(compiledCode, mainCode);
    strcat(compiledCode, "return 0; }\n");
}

// Processes a single function
void processFunction(char *line) {
    compiledFunc[0] = '\0';

    // Assumes that the first call occurs with the function definition line
    char *word = strtok(line, " ");
    
    // Skip "function" keyword
    word = strtok(NULL, " ");
    strcat(compiledFunc, "double ");
    strcat(compiledFunc, word);  // Function name
    strcat(compiledFunc, "(double ");

    // Process function parameters
    while ((word = strtok(NULL, " ")) != NULL) {
        strcat(compiledFunc, word);
        strcat(compiledFunc, ", ");
    }

    // Removes trailing comma
    if (strlen(compiledFunc) > 2) {
        compiledFunc[strlen(compiledFunc) - 2] = '\0';
    }

    // Adds closing characters
    strcat(compiledFunc, ") {\n");
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

    // The compiled code is stored in a char which is then written to a new temporary C file
    char tempC[] = "temp.c";
    mkstemp(tempC);
    FILE *file_out = fopen(tempC, "w");
    fprintf(file_out, "%s", compiledCode);
    fclose(file_out);

    // The C file is then compiled by executing a compilation command (gcc -o) using the system() function
    char tempExec[] = "XXXXXX";
    char compCmd[LINELENGTH];
    mkstemp(tempExec);
    snprintf(compCmd, LINELENGTH, "cc -std=c11 -Wall -Werror -o %s %s", tempExec, tempC);
    system(compCmd);

    // The resulting exec file that is created is then executed
    char execCmd[LINELENGTH];
    snprintf(execCmd, sizeof(execCmd), "./%s", tempExec);
    system(execCmd);

    remove(tempExec);

    return 0;
}



