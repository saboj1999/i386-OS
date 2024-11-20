#ifndef OS_INCLUDE_STRING_H
#define OS_INCLUDE_STRING_H
#include "bool.h"

/* String Helper Functions */
Bool CompareString(const char* s1, const char* s2);
Bool isSpace(char c);

/* String Copy */
void StrCopy(char* dest, const char* src);

/* String Length */
int StrLen(const char* str);

/* Modification Functions */
char* LTrim(char* s);
char* RTrim(char* s);
char** Split(char* str, char c);
char* Strip(char* s);
char* Lower(char* str);
char* Upper(char* str);
char* Title(char* str);

/* Input Validation */
Bool isAlpha(const char* str);
Bool isAlphaNumeric(const char* str);
Bool isDigit(const char* str);

#endif