//  CITS2002 Project 1 2024
//  Student1:   24214099   Lucan McDonald
//  Student2:   24502509   Anthony Stewart
//  Platform:   Apple b

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

void processFile(FILE *file) {
    // Initializes buffers to store each line from input and code for output
    char line[256];
    char code[1024];

    // Stores the previous word
    char prev[1024] = "";
    
    // Bool stores whether the word is part of a variable definition
    bool isvar = false;
    bool isstr = false;

    // Read file line by line
    while (fgets(line, sizeof(line), file)) {
        // Remove newline character
        line[strcspn(line, "\n")] = '\0';

        // Skip comments
        if (strchr(line, '#') != NULL) {
            continue;
        }
        // Reset code buffer for each line
        code[0] = '\0';
        // Split the line into words
        char *word = strtok(line, " \t");

        // Process each word in the line
        while (word != NULL) {
            // Handle variable assignment 
            if (strcmp(word, "<-") == 0) {
                strcat(code, "double ");
                strcat(code, prev);  
                strcat(code, " = ");
                 // Mark that a variable assignment is found
                isvar = true; 
            } else if (isvar) {
                // Append the value to code and reset isvar
                strcat(code, word);
                strcat(code, ";");
                isvar = false;
            // Handles printing 
            } else if (strcmp(word, "print") == 0) {
                strcat(code, "printf(");
                isstr = true;
            } else if (isstr && isdigit(word[0])) {
                strcat(code, word);
                strcat(code, ");");
                isstr = false;
            } else {
                // Save the current word in prev for future reference
                strcpy(prev, word);
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
    // Check if the correct number of arguments is provided
    if (argc != 2) {
        fprintf(stderr, "usage: %s <input file> \n", argv[0]);
        return 1;
    }

    // File paths from arguments
    const char *file_in_path = argv[1];
    const char *file_out_path = argv[2];

    // Open input file
    FILE *file_in = fopen(file_in_path, "r");
    if (file_in == NULL) {
        perror("error opening input file");
        return 1;
    }

    // Process the file!!
    processFile(file_in);

    // Close the input file
    fclose(file_in);

    return 0;
}

