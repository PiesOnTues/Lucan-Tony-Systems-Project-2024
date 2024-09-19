#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>



#define LINELENGTH 256



// Variable defined oustide of main so it has global scope
char compiledCode[BUFSIZ] = ""; 

// Tells us if we are in a function or not
int inFunc = 0;


// DELETEME
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


// processes a single line of ml
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
            strcat(compiledCode, "printf(\"%f\", ");

            // Appends first variable
            word = strtok(NULL, " ");
            if (word != NULL) {
                strcat(compiledCode, word);
            }

            // Ends print statement if it is a one variable/number print (e.g. print 3.5) 
            word = strtok(NULL, " ");
            if (word == NULL) {
                strcat(compiledCode, ");");
                break;
            }
                
            // If it is a operation print it appends the operation symbol, appends second variable and then closes the print
            strcat(compiledCode, word);
            word = strtok(NULL, " ");
            strcat(compiledCode, word);
            strcat(compiledCode, ");");

        }

                // Handle variable assignment
        else if (strcmp(word, "<-") == 0) {

            // Appends: "double" + varaible name + "="
            strcat(compiledLine, "double ");
            strcat(compiledLine, prev);  
            strcat(compiledLine, " = ");

            // Generates and appends variables assigned value
            word = strtok(NULL, " ");
            strcat(compiledLine, word);
            strcat(compiledLine, ";");
        
        }
        
        // Handles function definitions
        else if (strcmp(word, "function") == 0) {

            // Set inFunc flag to indicate we are in a function
            inFunc = 1;  

            // send line to processFunction
            processFunction(line);

            // return early when function is defined
            return compiledLine;
        }

        // If the token isn't recognized it will simply generate the next word
        else {
            strncpy(prev, word, sizeof(prev) - 1);
            word = strtok(NULL, " ");
        }

        return compiledLine;

    }
     // Adds \n for readability
    strcat(compiledLine, "\n");
}


// joins multiple functions into one consolidated file
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
void processFunction(char *line) {

    // Stores the processed function body
    char compiledFunc[200] = "";

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

    // Adds function definition to compiled code
    strcat(compiledCode, compiledFunc);

    // Process the function body line by line
    while (fgets(line, LINELENGTH, stdin)) {
        // Checks if line starts with tab character or indent (if we are still in the function scope)
        if (line[0] != '\t' && line[0] != ' ') {
            // reset inFunc flag when end of function is reached
            inFunc = 0;
            break;
        }

        // Process the line inside the function
        processLine(line);

    }

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