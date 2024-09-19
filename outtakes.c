// Checks line for the word "function", 
        else 

        // If the line(s) after the function don't contain either a tab or a blank space at the beginning 
        // We can assume that those lines aren't part of the function and that the function has ended
        // Once the function has ended we append the compiled function to the compiled code
        else if ((inFunc == true) && (line[0] != ' ' || line[0] != '\t')) {
            inFunc = false;
            strcat(compiledCode, compiledFunc);
            strcat(compiledCode, " }\n");
        } 

        // If we are inside a function we simply process the lines and then append them to the funciton
        else if (inFunc == true) { 
            
        }
        
        