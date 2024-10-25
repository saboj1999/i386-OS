#ifndef OS_INCLUDE_MATH_H
#define OS_INCLUDE_MATH_H
#include "bool.h"

/* Even Odd Functions*/
Bool isEven(int n);
Bool isOdd(int n);

/* Absolute Value Functions */
double Dabs(double d);
int Iabs(int n);

/* Power Function */
double Pow(double base, int exponent);

/* Floor and Ceiling Functions */
int Ceil(double d);
int Floor(double d);

/* Get Length Functions */
int GetIntLength (int n);
int GetDecimalLength(double d);

#endif