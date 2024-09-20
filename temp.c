#include <stdio.h>
#include <math.h> 
double var = 3;
double vartwo = var;
double vartwo = vartwo*2;
double varthree = vartwo*2;

void printVal(double value) { if (value == (int)value) { printf("%d\n", (int)value); } else { printf("%f\n", value); } }
 
double mult(double a, double b) { 
	return a * b;
	return 0;
 }
double mult(double a, double b) { 
double pmult(double a, double b) { 
	printVal(a*b);
	return 0;
 }
int main() { 
printVal(var);
printVal(vartwo);
printVal(varthree);
pmult(mult(1,mult(1,mult(1,1))),varthree);
printVal(pmult(2,3)+a);
pmult(4,5);
return 0;
 }
