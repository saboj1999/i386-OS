#include "../include/bool.h"
#include "../include/math.h"

#define EPSILON 1e-8  // Small threshold to account for floating-point errors

/**
 * Purpose: To return True or False whether n is even.
 * << isEven >>
 * @params: 
 *      int n -> an integer
 * @returns:
 *      Bool -> True or False (enum)
 */ 
Bool isEven(int n)
{
  return n % 2 == 0 ? True : False;
}
/**
 * Purpose: To return True or False whether n is odd.
 * << isOdd >>
 * @params: 
 *      int n -> an integer
 * @returns:
 *      Bool -> True or False (enum)
 */ 
Bool isOdd(int n)
{
  return n % 2 == 0 ? False : True;
}
/**
 * Purpose: To return the absolute value of a provided double.
 * << Dabs >>
 * @params: 
 *      double d -> a double
 * @returns:
 *      double d -> absolute value of provided double
 */ 
double Dabs(double d)
{
  return d < 0 ? d * -1 : d;
}
/**
 * Purpose: To return the absolute value of a provided integer.
 * << Iabs >>
 * @params: 
 *      int n -> an integer
 * @returns:
 *      int n -> absolute value of provided integer
 */ 
int Iabs(int n)
{
  return n < 0 ? n * -1 : n;
}
/**
 * Purpose: To return the value of a base raised to some exponent.
 * << Pow >>
 * @params: 
 *      double base -> a double
 *      int exponent -> an integer (doubles not supported yet)
 * @returns:
 *      double value -> value of the exponent operation
 */
double Pow(double base, int exponent)
{
  double value = 1.0;
  if(exponent < 0)
  {
    return 1.0 / Pow(base, Iabs(exponent));
  }
  while(exponent > 0)
  {
    value *= base;
    exponent--;
  }
  return value;
}
/**
 * Purpose: To take the ceiling of a given double. Does this by
 * - checking first if the double has a decimal value of X.0,
 * - next if the double is greater or less than zero.
 * - Casting to an integer chops off the portion after decimal point,
 * - therefore we must add one above zero to ensure ceiling value.
 * << Ceil >>
 * @params: 
 *      double d -> a double
 * @returns:
 *      int c -> the ceiling value of d
 */
int Ceil(double d)
{
    int n = (int)d;
    int c;
    if (Dabs(d - (double)n) < EPSILON) c = n;
    else if (d > 0) c = n + 1;
    else c = n;
    return c;
}
/**
 * Purpose: To take the floor of a given double. Does this by
 * - checking first if the double has a decimal value of X.0,
 * - next if the double is greater or less than zero.
 * - Casting to an integer chops off the portion after decimal point,
 * - therefore we must add one below zero to ensure floor value.
 * << Floor >>
 * @params: 
 *      double d -> a double
 * @returns:
 *      int f -> the floor value of d
 */
int Floor(double d)
{
  int n = (int)d;
  int f;
  if (Dabs(d - (double)n) < EPSILON) f = n;
  else if (d < 0) f = n - 1;
  else f = n;
  return f;
}
/**
 * Purpose: To determine the length of a given integer. Does this by 
 * - looping and continuely incrementing l while dividing the number with
 * - integer division by 10 until n is less than 1.
 * << GetIntLength >>
 * @params: 
 *      int n -> an integer
 * @returns: 
 *      int l -> length of the given integer
 */
int GetIntLength(int n)
{
  int l = 0;
  if (n == 0) return 1;
  n *= n < 0 ? -1 : 1;
  while(n > 0){ l++; n /= 10; }
  return l;
}
/**
 * Purpose: To determine the length of a given double's digits after the decimal.
 * - Does this by looping and continuely incrementing l while multiplying 
 * - the number with by 10 until the double minus its floor is equal to zero.
 * << GetDecimalLength >>
 * @params: 
 *      double d -> a double
 * @returns: 
 *      int l -> number of digits after decimal point
 */
int GetDecimalLength(double d)
{
  int l = 0;
  if (d < 0.0) d = -d;
  d -= Floor(d);
  while (d > EPSILON && l < 15) // Limit l to prevent potential infinite loops
  {
      d *= 10;
      l++;
      d -= Floor(d);
      if (d < EPSILON)  // If d is very close to zero, consider it zero
          break;
  }
  return l;
}