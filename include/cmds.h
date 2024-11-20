#ifndef OS_INCLUDE_CMDS_H
#define OS_INCLUDE_CMDS_H
#include "bool.h"
#include "vga_color.h"

/* Validation */
Bool isValidCommand(char* command);
Bool isValidColor(char* color);

/* Color Converter String -> VGA */
Color ConvertValidStrToColor(char* color);

/* Print Color Options */
void PrintTextColorOptions();
void PrintBackGroundColorOptions();

/* Test Print Functionality */
void __PrintTest__();

/* Commands */
void Help();
void Man(char* input);
void Echo(char* input);
void STC(char* input);
void SBC(char* input);

/* Process Command */
void ProcessCommand(char* str);

#endif