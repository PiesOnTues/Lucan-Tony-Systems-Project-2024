#include <stdio.h>
#include <math.h> 
double one = 1;

void printVal(double value) { if (value == (int)value) { printf("%d\n", (int)value); } else { printf("%f\n", value); } }
 
double increment(double value) { 
	return value + one;
	return 0;
 }
int main() { 
printVal(increment(3)+increment(4));
return 0;
 }
