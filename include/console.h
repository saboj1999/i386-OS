#ifndef OS_INCLUDE_CONSOLE_H
#define OS_INCLUDE_CONSOLE_H
#include <stdint.h>
#include "vga_color.h"

/* Cursor Functions */
char GetCharAtCursor();
char GetForeGroundAtCursor();
char GetBackGroundAtCursor();
void CustomizeVGACursor();
void HideCursor();
void ShowCursor();
int GetCursorX();
int GetCursorY();
static void UpdateVGACursorPosition();
uint16_t GetVGACursorPosition();
void SetCursorDefaultPosition();
void SetCursorPosition(int x, int y);

/* Color Attribute Functions */
void UpdateBackgroundColorAttribute(Color prev_bg);
void UpdateForegroundColorAttribute(Color prev_fg);
void SetTextColor(Color color);
void SetBackGroundColor(Color color);
void SetAllTextColor(Color color);
void SetFullBackGroundColor(Color color);
char GetForeGround();
char GetBackGround();
void RemoveHighlights();
void RemoveTextColors();
void ResetColorAttributes();

/* Screen Manipulation Functions */
void ClearScreen();
void ResetScreen();
void ScrollScreenUp();
void ScrollScreenDown(); // TODO: implement pages instead of buffer for chars off screen

/* Move Cursor Function */
void MoveCursorToLastCharOnLine();
void MoveCursor(int x, int y);

/* Print Functions */
void PrintCharAtCursor(char c);
void PrintCharFullColorAtCursor(char c, Color fg, Color bg);
/* Keyboard Press Functions */
void Delete();
void Tab();
void NewLine();
/* Print Char */
void PrintChar(char c);
void PrintCharColor(char c, Color color);
void PrintCharHighlight(char c, Color color);
void PrintCharFullColor(char c, Color fg, Color bg);
/* Print Int */
void PrintIntSingleDigit(int n);
void PrintInt(int n);
void PrintIntColor(int n, Color color);
void PrintIntHighlight(int n, Color color);
void PrintIntFullColor(int n, Color fg, Color bg);
/* Print Double */
void PrintDouble(double d);
void PrintDoubleColor(double d, Color color);
void PrintDoubleHighlight(double d, Color color);
void PrintDoubleFullColor(double d, Color fg, Color bg);
/* Print String */
void PrintString(const char* str);
void PrintLine(const char* str);
void PrintStringColor(const char* str, Color color);
void PrintStringHighlight(const char* str, Color color);
void PrintStringFullColor(const char* str, Color fg, Color bg);
void PrintLineColor(const char* str, Color color);
void PrintLineHighlight(const char* str, Color color);
void PrintLineFullColor(const char* str, Color fg, Color bg);
/* Printf */
void Printf(const char* str, ...);

#endif