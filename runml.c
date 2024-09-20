//  CITS2002 Project 1 2024
//  Student1:   24214099   Lucan McDonald
//  Student2:   24502509   Anthony Stewart
//  Platform:   Apple

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>

#define LINELENGTH 256

// compiledCode globally stores the summation of all function code and maincode
char compiledCode[BUFSIZ] = "";
// mainCode stores code of the main function
char mainCode[BUFSIZ];
// compiledFunc stores all code under non-main functions
char funcCode[BUFSIZ];

// funcArr stores the name of each new function when it is compiled
char funcArr[50][100];
// funcIndex stores the current index for referencing the funcList array
int funcIndex = 0;

// varArr stores the name of each new variable when it is compiled
char varArr[50][100];
// varIndex stores the current index for referencing the varArr
int varIndex = 0;

// Tells us if we are in a function or not
bool inFunc = false;

// counts number of identifiers 
int identifierCount = 0;



// Processes a single function definition line
char* FunctionHeader(char *line) {
    
    static char funcDef[BUFSIZ];

    // Assumes that the first call occurs with the function definition line
    char *word = strtok(line, " ");
    
    // Skip "function" keyword
    word = strtok(NULL, " ");
    strcat(funcDef, "double ");
    strcat(funcDef, word);  // Function name
    strcat(funcDef, "(");

    if (identifierCount > 50) {
        printf("Identifier limit of 50 exceeded");
    } else {
        // stores function name in funcName array, and iterates the indexer
        strcpy(funcArr[funcIndex++], word);
        identifierCount++;
    }

    // Process function parameters
    while ((word = strtok(NULL, " ")) != NULL) {
        strcat(funcDef, "double ");
        strcat(funcDef, word); // perameter name
        strcat(funcDef, ", ");
        // Adds parameters to list of variables
        strcpy(varArr[varIndex++], word);
    }

    // Removes trailing comma
    if (strlen(funcDef) > 2) {
        funcDef[strlen(funcDef) - 2] = '\0';
    }

    // Adds closing characters
    strcat(funcDef, ") { \n");

    return funcDef;
}

bool isVar(const char *varName) {
    // checks if item is in the list
    for (int i = 0; i < varIndex; i++) {
        if (varArr[i] == NULL) {
            break;
        }
        if (strcmp(varName, varArr[i])) {
            return true;
        }
    }

    return false;

}

int isNum(const char *str) {
    char *endptr;
    // Use strtod to attempt conversion to double
    double val = strtod(str, &endptr);
    // Checks if string was succesfully converted 
    return endptr != str;
    return val;
}



// function to check if a token exists as a predefined function
bool isFunc(const char *funcName) {

    // checks if item is in the list
    for (int i = 0; i < funcIndex; i++) {
        if (funcArr[i] == NULL) {
            break;
        }
        if (strstr(funcName, funcArr[i])) {
            return true;
        }
    }
    
    return false;
}

// processes a single line of ml
char *processLine(char *line) {
    // Stores previous word
    char prev[50] = {0};

    // Copies line for later use
    char line1[LINELENGTH];
    if (line != NULL) {
        strcpy(line1, line);
    }
    // Stores the processed line as a whole
    static char compiledLine[LINELENGTH];

    // Initialize to empty
    compiledLine[0] = '\0'; 

    // Tokenizes each line of code into individual words
    char *word = strtok(line, " \t");

    // Loops through the line
    while (word != NULL) {
    
        if (isFunc(word) == true) {
            strcat(compiledLine, line1);
            strcat(compiledLine, ";\n");
        }

        // Handles print statements
        if (strcmp(word, "print") == 0) {
                    
            // concatenates printf function for float vals
            strcat(compiledLine, "printVal(");

            // checks if first variable/float is a already defined variable or is a double
            word = strtok(NULL, " \t");
            if (isVar(word) || isNum(word)) {
                // concatenates first variable/float
                if (word != NULL) {
                    strcat(compiledLine, word);
                }
            } else {
                // if the variable hasn't been defined it simply defines the varaible with the value of 0 and prints 0
                strcat(compiledCode, "double ");
                strcat(compiledCode, word);
                strcat(compiledCode, " = 0;");
                strcat(compiledLine, "0");



                
            }

            // Ends print statement if it is a one variable/number print (e.g. print 3.5) 
            word = strtok(NULL, " \t");
            if (word == NULL) {
                strcat(compiledLine, ");\n");
                break;
            }
                    
            // If it is a operation print it concatenates the operation symbol, concatenates second variable and then closes the print
            strcat(compiledLine, word);
            word = strtok(NULL, " ");
            strcat(compiledLine, word);
            strcat(compiledLine, ");\n");
            
                
            

            

        }

        // Handle return statement
        else if (strcmp(word, "return") == 0) {
            
            strcat(compiledLine, "return ");

            // Concatenate the expression after "return"
            word = strtok(NULL, " ");
            while (word != NULL) {
                strcat(compiledLine, word);
                word = strtok(NULL, " ");
                if (word != NULL) {
                    strcat(compiledLine, " ");
                }
            }

            // end line correctly
            strcat(compiledLine, ";\n");
            break;
        }

        // Handle variable assignment
        // Concatenates variable directly to compiledCode so variables global scope
        else if (strcmp(word, "<-") == 0) {

            // concatenates: "double" + variable name + "="
            strcat(compiledCode, "double ");
            strcat(compiledCode, prev);  
            strcat(compiledCode, " = ");

            // Generates and concatenates variable's assigned value
            word = strtok(NULL, " ");
            // Checks if identifier count has been exceeded
            if (identifierCount > 50) {
                printf("Identifier limit of 50 exceeded");
            } else {
                // stores var name in varArr and iterates the indexer
                strcpy(varArr[varIndex++], word);
                identifierCount++;
            }
            strcat(compiledCode, word);
            strcat(compiledCode, ";\n");
        }

        // If the token isn't recognized it will simply generate the next word
        else {
            strncpy(prev, word, sizeof(prev) - 1);
            word = strtok(NULL, " \t");
        }
    }

    return compiledLine;

}



// Joins multiple compiled functions into one consolidated output string
void processFile(FILE *file) {

    // defines line character, setting max
    char line[LINELENGTH];

    // Adds lines to import C11 standard libraries
    strcat(compiledCode, "#include <stdio.h>\n#include <math.h> \n");

    // Reads file line by line
    while (fgets(line, sizeof(line), file)) {

        // strips newline character from line
        line[strcspn(line, "\n")] = '\0';  
        
        // Skip comments
        if (strchr(line, '#') != NULL) {  
            continue;
        } 

        // checks if we are in main or in a function
        if (inFunc) {
            // Checks that we are still in a function (by checking if lines are indented)
            if (line[0] == ' ' || line[0] == '\t') {
                strcat(funcCode, "\t");
                strcat(funcCode, processLine(line));
            }

            // reset flag and add closing chars if the end of the function has been reached
            else {
                inFunc = false;
                strcat(funcCode, "\treturn 0;\n }\n");
                strcat(mainCode, processLine(line));
            }
        } 
        
        else {
            // checks if we have hit a function definition line
            if (strstr(line, "function") != NULL) {
                // sets inFunc flag to true to indicate that we should compile code to the funcCode variable
                inFunc = true;
                strcat(funcCode, FunctionHeader(line));
            } 
            
            // if we are not in a function code gets compiled to mainCode
            else {
                strcat(mainCode, processLine(line));
            }
        }
    }

    // the next few lines set up the structure of the fully compiled output file

    // adds function that accounts for integer printing
    strcat(compiledCode, "\nvoid printVal(double value) { if (value == (int)value) { printf(\"%d\\n\", (int)value); } else { printf(\"%f\\n\", value); } }\n \n"); 

    // adds code of non-main functions
    strcat(compiledCode, funcCode);

    // opening lines of main function
    strcat(compiledCode, "int main() { \n");

    // concatenates all code within main 
    strcat(compiledCode, mainCode);

    // sets main to return nothing
    strcat(compiledCode, "return 0;\n }\n");
}



//main
int main(int argc, char *argv[]) {

    // tony put a comment here
    if (argc != 2) { 
        fprintf(stderr, "usage: %s <input file>\n", argv[0]);
        return 1;
    }

    // gets file name from command line input
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
    snprintf(compCmd, LINELENGTH, "cc -std=c11 -o %s %s", tempExec, tempC);
    system(compCmd);

    // The resulting exec file that is created is then executed
    char execCmd[LINELENGTH];
    snprintf(execCmd, sizeof(execCmd), "./%s", tempExec);
    system(execCmd);

    remove(tempExec);

    return 0;
}