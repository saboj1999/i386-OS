#include "../include/bool.h"
#include "../include/string.h"

#define MAX_TOKENS 10  // Maximum number of tokens the Split function can handle

/**
 * Purpose: To compare to strings and return true if they are equal.
 * << CompareString >>
 * @params: 
 *      const char* s1 -> a string
 *      const char* s2 -> a string
 * @returns: 
 *      Bool -> true if strings are equal
 */
Bool CompareString(const char* s1, const char* s2)
{
    while (*s1 || *s2)
    {
        if (*s1 != *s2) return False;
        s1++;
        s2++;
    } 
    return True;
}
/**
 * Purpose: Determine if given char is a space / tab / newline / pagedown.
 * << isSpace >>
 * @params: 
 *      char c -> given char
 * @returns: 
 *      Bool -> true if char is space / tab / newline / pagedown
 */
Bool isSpace(char c)
{
    // https://www.amazon.com/Create-Your-Own-Operating-System/dp/1981624058
    return (c == ' ' || c == '\t' || c == '\n' || c == '\12');
}
/**
 * Purpose: Copies a string from src to dest.
 * << StrCopy >>
 * @params:
 *      char* dest -> the destination buffer
 *      const char* src -> the source string
 * @returns: void
 */
void StrCopy(char* dest, const char* src)
{
    while (*src)
    {
        *dest++ = *src++;
    }
    *dest = '\0';
}
/**
 * Purpose: Determine length of a given string.
 * << StrLen >>
 * @params:
 *      const char* str -> the given string
 * @returns:
 *      int len -> length of given string
 */
int StrLen(const char* str)
{
    int len = 0;
    while (str[len])
    {
        len++;
    }
    return len;
}
/**
 * Purpose: Trim the spaces of left side of string.
 * << LTrim >>
 * @params:
 *      char* s -> the given string
 * @returns:
 *      char* s -> trimmed string
 */
char* LTrim(char* s)
{
    // https://www.amazon.com/Create-Your-Own-Operating-System/dp/1981624058
    while (isSpace(*s)) s++;
    return s;
}
/**
 * Purpose: Trim the spaces of right side of string.
 * << RTrim >>
 * @params:
 *      char* s -> the given string
 * @returns:
 *      char* s -> trimmed string
 */
char* RTrim(char* s)
{
    // https://www.amazon.com/Create-Your-Own-Operating-System/dp/1981624058
    char* back = s + StrLen(s);
    while (isSpace(*--back));
    *(back + 1) = '\0';
    return s;
}
/**
 * Purpose: Split the given string into an array of strings split up by
 * - a given delimiter.
 * << Split >>
 * @params:
 *      char* str -> the given string
 *      char c -> the given delimiter
 * @returns:
 *      char** tokens -> pointer to the pointer of the first string in array
 */
char** Split(char* str, char c)
{
    // +++++++++++ChatGPT-Function+++++++++++ //
    static char* tokens[MAX_TOKENS]; // Array to hold pointers to tokens
    int count = 0;                   // Number of tokens found
    char* p = str;                   // Pointer to iterate through the string

    while (*p)
    {
        // Skip over any leading delimiters
        while (*p == c)
            p++;

        if (*p == '\0')
            break;

        // Store the start of the token
        if (count < MAX_TOKENS - 1)
            tokens[count++] = p;
        else
            break;  // Prevent buffer overflow

        // Move to the end of the token
        while (*p && *p != c)
            p++;

        // If we've found a delimiter, replace it with '\0' to terminate the token
        if (*p == c)
        {
            *p = '\0';
            p++;
        }
    }

    // Null-terminate the tokens array
    tokens[count] = 0;

    return tokens;
    // +++++++++++ChatGPT-Function+++++++++++ //
}
/**
 * Purpose: Strip spaces from both sides of a given string.
 * << Strip >>
 * @params:
 *      char* s -> the given string
 * @returns:
 *      char* s -> stripped string
 */
char* Strip(char* s)
{
    // https://www.amazon.com/Create-Your-Own-Operating-System/dp/1981624058
    return RTrim(LTrim(s)); 
}
/**
 * Purpose: Convert a given string to lowercase in place.
 * << Lower >>
 * @params:
 *      char* str -> the given string
 * @returns:
 *      char* str -> modified string
 */
char* Lower(char* str)
{
    int i = 0;
    while (str[i] != '\0')
    {
        char c = str[i];
        if (c >= 'A' && c <= 'Z')
        {
            str[i] = c + ('a' - 'A');
        }
        i++;
    }
    return str;
}
/**
 * Purpose: Convert a given string to uppercase in place.
 * << Upper >>
 * @params:
 *      char* str -> the given string
 * @returns:
 *      char* str -> modified string
 */
char* Upper(char* str)
{
    int i = 0;
    while (str[i] != '\0')
    {
        char c = str[i];
        if (c >= 'a' && c <= 'z')
        {
            str[i] = c - ('a' - 'A');
        }
        i++;
    }
    return str;
}
/**
 * Purpose: Convert a given string to titlecase in place.
 * << Title >>
 * @params:
 *      char* str -> the given string
 * @returns:
 *      char* str -> modified string
 */
char* Title(char* str)
{
    int i = 0;
    Bool new_word = True;

    while (str[i] != '\0')
    {
        char c = str[i];
        if (isSpace(c))
        {
            new_word = True;
        }
        else
        {
            if (new_word && c >= 'a' && c <= 'z')
            {
                str[i] = c - ('a' - 'A');
            }
            else if (!new_word && c >= 'A' && c <= 'Z')
            {
                str[i] = c + ('a' - 'A');
            }
            new_word = False;
        }
        i++;
    }
    return str;
}
/**
 * Purpose: Determine if all chars in a given string are alphabetical.
 * << isAlpha >>
 * @params:
 *      const char* str -> the given string
 * @returns:
 *      Bool -> true if all chars in string are alphabetical
 */
Bool isAlpha(const char* str)
{
    int i = 0;
    while (str[i] != '\0')
    {
        char c = str[i];
        if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')))
        {
            return False;
        }
        i++;
    }
    return True;
}
/**
 * Purpose: Determine if all chars in a given string are alpha-numeric.
 * << isAlphaNumeric >>
 * @params:
 *      const char* str -> the given string
 * @returns:
 *      Bool -> true if all chars in string are alpha-numeric
 */
Bool isAlphaNumeric(const char* str)
{
    int i = 0;
    while (str[i] != '\0')
    {
        char c = str[i];
        if (!((c >= 'A' && c <= 'Z') ||
              (c >= 'a' && c <= 'z') ||
              (c >= '0' && c <= '9')))
        {
            return False;
        }
        i++;
    }
    return True;
}
/**
 * Purpose: Determine if all chars in a given string are digits.
 * << isDigit >>
 * @params:
 *      const char* str -> the given string
 * @returns:
 *      Bool -> true if all chars in string are digits
 */
Bool isDigit(const char* str)
{
    int i = 0;
    while (str[i] != '\0')
    {
        char c = str[i];
        if (!(c >= '0' && c <= '9'))
        {
            return False;
        }
        i++;
    }
    return True;
}