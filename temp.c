#include <stdio.h>
 #include <math.h> 
void printVal(double value) { if (value == (int)value) { printf("%d\n", (int)value); } else { printf("%f\n", value); } }int main() { 
double x = 2.5;
printVal(x);
return 0;
 }
