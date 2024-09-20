#include <stdio.h>
#include <math.h> 

void printVal(double value) { if (value == (int)value) { printf("%d\n", (int)value); } else { printf("%f\n", value); } }
 
int main() { 
printVal(3.5);
return 0;
 }
