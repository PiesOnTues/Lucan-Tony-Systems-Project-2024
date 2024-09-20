#include <stdio.h>
#include <math.h> 
double x = 8;
double y = 3;
double x = 0;
void printVal(double value) { if (value == (int)value) { printf("%d\n", (int)value); } else { printf("%f\n", value); } }
 
int main() { 
printVal(0*y);
return 0;
 }
