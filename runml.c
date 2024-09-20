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
char compiledCode[10000] = "";
// mainCode stores code of the main function
char mainCode[BUFSIZ];
// funcCode stores all code under non-main functions
char funcCode[BUFSIZ];

// funcArr stores the name of each new function when it is compiled
char funcArr[BUFSIZ][100];
// funcIndex stores the current index for referencing the funcList array
int funcIndex = 0;

// varArr stores the name of each new variable when it is compiled
char varArr[BUFSIZ][100];
// varIndex stores the current index for referencing the varArr
int varIndex = 0;

// inFunc Tells us if we are in a function or not
bool inFunc = false;

// invalidId stores a boolean value which indicates wether or not invalid identifiers are present within the program
bool invalidId = false;

// Counts number of identifiers 
int identifierCount = 0;



// Checks if the string passed in satisfies the condition for a valid ml identifier or not, and flags the error accordingly
void validateIdentifier(char *id) {

    // Check if the function name is between 1-12 characters
    if (strlen(id) < 1 || strlen(id) > 12) {
        // flags error
        invalidId = true;
        
    } else {
        // check if its fully alphabetical or not
        for (int i = 0; i < strlen(id); i++) {
            if (!isalpha(id[i])) {
                // flags error
                invalidId = true;
            }
        }
    }
}



// Processes a single function definition line
char* functionHeader(char *line) {
    
    // designates a place to store the header line as it is compiled
    static char funcDef[BUFSIZ] = "";

    // Assumes that the first call occurs with the function definition line
    char *word = strtok(line, " ");
    
    // Skip "function" keyword
    word = strtok(NULL, " ");
    strcat(funcDef, "double ");
    strcat(funcDef, word);  // Function name
    strcat(funcDef, "(");

    // check function name identifier validity
    validateIdentifier(word);

    // stores function name in funcName array, and iterates the indexer
    strcpy(funcArr[funcIndex++], word);

    // iterates the total identifer count
    identifierCount++;

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



// checks if variable already exists or not
bool varExists(const char *varName) {

    // checks if item is in the list
    for (int i = 0; i < varIndex; i++) {
        if (varArr[i] == NULL) {
            break;
        }
        if (strcmp(varName, varArr[i]) == 0) {
            return true;
        }
    }

    return false;

}



// checks if inputted string is a number
int isNum(const char *str) {
    char *endptr;
    // Use strtod to attempt conversion to double
    double val = strtod(str, &endptr);
    // Checks if string was succesfully converted 
    return endptr != str;
    return val;
}



// function to check if a token exists as a predefined function
bool funcExists(const char *funcName) {

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

    // Stores the processed line as a whole
    static char compiledLine[LINELENGTH];

    // Initialize to empty
    compiledLine[0] = '\0'; 

    // Tokenizes each line of code into individual words
    char *word = strtok(line, " \t");

    // Loops through the line
    while (word != NULL) {
        
        if (funcExists(word) == true) {
            // Once a function name is found a the rest of the line is printed
            strcat(compiledLine, word);
            while ((word = strtok(NULL, " ")) != NULL) {
                strcat(compiledLine, word);
            }
            strcat(compiledLine, ";\n");
        }

        // Handles print statements
        if (word == NULL) {
            break;
        }
        if (strcmp(word, "print") == 0) {
                    
            // concatenates printf function for float vals
            strcat(compiledLine, "printVal(");

            // checks if the next word is an already defined variable or a double
            word = strtok(NULL, " \t");
            if (varExists(word) || isNum(word)) {
                // concatenates first variable/float
                if (word != NULL) {
                    strcat(compiledLine, word);
                }
            // Checks if next word is a function name
            } else if (funcExists(word)) {
                strcat(compiledLine, word);
                while ((word = strtok(NULL, " ")) != NULL) {
                    strcat(compiledLine, word);
                }
            } else {
                // If the variable hasn't been defined it simply defines the varaible with the value of 0 and prints 0
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
                    
            // If it is a operation print it concatenates the operation symbol and var/number and the continues this till line end
            strcat(compiledLine, word);
            word = strtok(NULL, " ");
            while (word != NULL) {
                strcat(compiledLine, word);
                word = strtok(NULL, " ");
            }
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

            bool varVal = false;
            char tempVarCode[BUFSIZ] = "";

            // concatenates: "double" + variable name + "="
            strcat(tempVarCode, "double ");
            strcat(tempVarCode, prev);  // varname
            strcat(tempVarCode, " = ");

            // validates variable name
            validateIdentifier(prev);

            // Generates and concatenates variable's assigned value
            word = strtok(NULL, " ");
            while (word != NULL) {
                // Check if the assigned value is another variable and if so it must be printed in main
                if (varExists(word)) {
                    varVal = true;
                }

                strcat(tempVarCode, word);
                word = strtok(NULL, " ");
                }
            
            // make sure variable doesnt already exist
            if (!varExists(prev)) {
                // stores var name in varArr and iterates the indexer
                strcpy(varArr[varIndex++], prev);
                identifierCount++;
            }
            
            // line end characters
            strcat(tempVarCode, ";\n");

            // Checks if the variable needs to be defined in a function, in main or globally
            if (inFunc) {
                strcat(funcCode, tempVarCode);
            } else if (varVal) {
                strcat(mainCode, tempVarCode);
            } else {
                strcat(compiledCode, tempVarCode);
            }
        }

        // If the token isn't recognized, check if it is meaningful or null
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
                strcat(funcCode, functionHeader(line));
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



// Main
int main(int argc, char *argv[]) {

    // gets filename from command line input
    const char *filename = argv[1];

    // sets correct file extension
    const char *extension = ".ml";

    // checks that number of args is correct
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Check if the filename ends with the required ".ml" extension
    if (strlen(filename) < strlen(extension) || strcmp(filename + strlen(filename) - strlen(extension), extension) != 0) {
        fprintf(stderr, "Error: Input file must have the '.ml' extension.\n");
        exit(EXIT_FAILURE);
    }

    // Opens .ml file and then compiles it in processFile function 
    FILE *file_in = fopen(filename, "r");
    if (file_in == NULL) {
        perror("error opening input file");
        return 1;
    }

    processFile(file_in);
    fclose(file_in);

    // checks for invalid identifiers and handles the error in the case they do exist
    if (invalidId) {
        fprintf(stderr, "invalid identifiers present, ensure all identifiers consist only of 1-12 alphabetical characters\n");
        exit(EXIT_FAILURE);
    }
        
    // Checks if the identifier limit has been surpassed
    if (identifierCount > 50) {
        fprintf(stderr, "%d unique identifiers present, maximum of 50 allowed.\n", identifierCount);
        exit(EXIT_FAILURE);
    }

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
    remove(tempC);

    return 0;
}