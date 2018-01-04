#include <stdio.h>
#include <stdlib.h>

/* IMPLEMENT ME: Declare your functions here */
int add (int a, int b);
int sub (int a, int b);
int mult (int a, int b);
int divide (int a, int b);

int main (void)
{
	/* IMPLEMENT ME: Insert your algorithm here */
	int a = 6;
	int b = 3;
	int result = 0;
	int operationIndex = 0;
	int (*operationArr[4]) (int, int) = {&add, &sub, &mult, &divide};

	printf("Operand 'a' : 6 | Operand 'b' : 3\n");
	printf("Specify the operation to perform (0 : add | 1 : subtract | 2 : Multiply | 3 : divide): ");
	scanf("%d", &operationIndex);

	result = operationArr[operationIndex] (a, b);

	printf("x = %d\n", result);;

	return 0;
}

//use array of function pointers to avoid if statements


/* IMPLEMENT ME: Define your functions here */
int add (int a, int b) { printf ("Adding 'a' and 'b'\n"); return (a + b); }
int sub (int a, int b) { printf ("Subtracting 'b' from 'a'\n"); return (a - b); }
int mult (int a, int b) { printf ("Multiplying 'a' and 'b'\n"); return (a * b); }
int divide (int a, int b) { printf ("Dividing 'a' by 'b'\n"); return (int)(a / b); }
