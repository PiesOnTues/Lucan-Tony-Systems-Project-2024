#include <stdio.h>
 #include <math.h> 

void printVal(double value) { if (value == (int)value) { printf("%d\n", (int)value); } else { printf("%f\n", value); } }
 
double printsum(double a, double b) { 
	printVal(a+b);
	return 0;
 }
int main() { 
printsum (12, 6);
return 0;
 }
