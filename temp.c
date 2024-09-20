#include <stdio.h>
#include <math.h> 
double x = 8;
double y = 3;

void printVal(double value) { if (value == (int)value) { printf("%d\n", (int)value); } else { printf("%f\n", value); } }
 
int main() { 
printVal(x*y);
return 0;
 }
