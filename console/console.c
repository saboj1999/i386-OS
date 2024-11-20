#include "../include/math.h"
#include "../include/portmap.h"
#include "../include/console.h"

#define VGA ((char*) 0xb8000)                               // Define VGA buffer at 0xb8000
#define CURSOR_POS ((cursor_y * 80 + cursor_x) * 2)         // Define macro for cursor position
#define VGA_CURRENT_COLOR VGA_COLOR(foreground, background) // Define macro for current VGA color

static const int default_cursor_x = 1;    // Left of Screen -> 0
static const int default_cursor_y = 24;   // Bottom of Screen -> 24
static const int tab = 8;                 // Tab -> Default 8 spaces

static Color foreground = LIGHT_GRAY;     // Foreground (text) 0x0 - 0xf Default Light Grey
static Color background = BLACK;          // Background 0x0 - 0xf Default Black
static int cursor_x = default_cursor_x;   // CursorX 0-79 default 1
static int cursor_y = default_cursor_y;   // CursorY 0-24 default 24
static uint8_t cursor_start = 0;          // Cursor shape - Top 0-15
static uint8_t cursor_end = 15;           // Cursor shape - Bottom 0-15

/**
 * Purpose: Return the character at the current cursor location.
 * << GetCharAtCursor >>
 * @params: none
 * @returns: 
 *      char -> char at current VGA cursor position
 */
char GetCharAtCursor()
{
    return VGA[CURSOR_POS];
}
/**
 * Purpose: Return the foreground byte at the current cursor location.
 * << GetForeGroundAtCursor >>
 * @params: none
 * @returns: 
 *      char -> bits 0-3 of attribute byte at VGA[CURSOR_POS+1]
 */
char GetForeGroundAtCursor()
{
    // Read the current attribute byte
    char attr_byte = VGA[CURSOR_POS+1];
    // Extract the existing foreground color (bits 0-3)
    char existing_foreground = attr_byte & 0x0F;
    return existing_foreground;
}
/**
 * Purpose: Return the background byte at the current cursor location.
 * << GetBackGroundAtCursor >>
 * @params: none
 * @returns: 
 *      char -> bits 4-6 of attribute byte at VGA[CURSOR_POS+1]
 */
char GetBackGroundAtCursor()
{
    // Read the current attribute byte
    char attr_byte = VGA[CURSOR_POS+1];
    // Extract the existing background color (bits 4-6)
    char existing_background = (attr_byte >> 4) & 0x0F;
    return existing_background;
}
/**
 * Purpose: Utilize outb to write the current VGA cursor start and end line
 * - bytes. Allows us to modify the shape of the blinking cursor.
 * << CustomizeVGACursor >>
 * @params: none
 * @returns: void
 */
void CustomizeVGACursor()
{
    // https://wiki.osdev.org/Text_Mode_Cursor
    // (0,15) gives us full coverage of the current location as opposed to the underline it is by default (14, 15)
    outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);

	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}
/**
 * Purpose: Utilize outb to write the current VGA cursor start set to a
 * - value matching the cursor end to effectively hide the cursor.
 * << HideCursor >>
 * @params: none
 * @returns: void
 */
void HideCursor()
{
    outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | 15); // set to 15 to match cursor_end and effectively hide the cursor
}
/**
 * Purpose: Utilize outb to write the current VGA cursor start and set to a
 * - its default value of 0 to make the cursor visible again.
 * << ShowCursor >>
 * @params: none
 * @returns: void
 */
void ShowCursor()
{
    outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start); // return cursor_start to default value
}
/**
 * Purpose: Get the current cursor x position.
 * << GetCursorX >>
 * @params: none
 * @returns:
 *      int cursor_x -> current cursor x position
 */
int GetCursorX()
{
    return cursor_x;
}
/**
 * Purpose: Get the current cursor y position.
 * << GetCursorY >>
 * @params: none
 * @returns:
 *      int cursor_y -> current cursor y position
 */
int GetCursorY()
{
    return cursor_y;
}
/**
 * Purpose: Utilize outb to write the current VGA cursor position
 * - into the correct port.
 * << UpdateVGACursorPosition >>
 * @params: none
 * @returns: void
 */
void UpdateVGACursorPosition() 
{
    uint16_t cursor_position = CURSOR_POS / 2; // VGA buffer 0-3999 we need 0-1999

    outb(0x3D4, 0x0F); // 0x0F is the index for the cursor low register
    outb(0x3D5, (uint8_t)(cursor_position)); // Write the lower 8 bits of the position to the cursor low register

    outb(0x3D4, 0x0E); // 0x0E is the index for the cursor high register
    outb(0x3D5, (uint8_t)(cursor_position >> 8)); // Shift the position value right 8 bits and then write.
}
/**
 * Purpose: Obtain the current VGA cursor position via the inb function
 * - and outb functions. 
 * << GetVGACursorPosition >>
 * @params: none
 * @returns:
 *      uint16_t cursor_position -> current position of the VGA cursor (0-1999)
 */
uint16_t GetVGACursorPosition()
{
    uint16_t cursor_position = 0;

    outb(0x3D4, 0x0F);
    cursor_position |= inb(0x3D5);

    outb(0x3D4, 0x0E);
    cursor_position |= ((uint16_t)inb(0x3D5)) << 8;

    return cursor_position;
}
/**
 * Purpose: To set the default position of the cursor.
 * << SetCursorDefaultPosition >>
 * @params: none
 * @returns: void
 */
void SetCursorDefaultPosition()
{
    cursor_x = default_cursor_x;
    cursor_y = default_cursor_y;
    UpdateVGACursorPosition();
}
/**
 * Purpose: To set the position of the cursor.
 * << SetCursorPosition >>
 * @params: 
 *      int x -> Column position to set cursor (0-79)
 *      int y -> Row position to set cursor (0-24)
 * @returns: void
 */
void SetCursorPosition(int x, int y) 
{
    cursor_x = x;
    cursor_y = y;
    UpdateVGACursorPosition();
}
/**
 * Purpose: To update the background portion of all positions' color attribute
 * - in the VGA Buffer.
 * Note: This function skips memory addresses that do not have the same color
 * - background as the given argument "prevBackGround".
 * <<  UpdateBackgroundColorAttribute  >>
 * @params: 
 *      Color prev_bg -> Previous color of background (0x0 - 0xf) enum
 * @returns: void
 */
void UpdateBackgroundColorAttribute(Color prev_bg)
{
    // +++++++++++ChatGPT-Function+++++++++++ //
    for(int i = 0; i < 2000; i++)
    {
        // Read the current attribute byte
        char attr_byte = VGA[i*2+1];

        // Extract the existing background color (bits 4-6)
        char existing_background = (attr_byte >> 4) & 0x0F;

        // Extract the blinking bit (bit 7) to preserve it
        char blinking_bit = attr_byte & 0x80;

        // Compare with prev_bg (ensure it's in the range 0-7)
        if (existing_background == (prev_bg & 0x0F))
        {
            // Keep the existing foreground color (bits 0-3)
            char foreground = attr_byte & 0x0F;

            // Set the new background color (shifted to bits 4-6)
            char new_background = (background & 0x0F) << 4;

            // Reconstruct the attribute byte
            char new_attr_byte = foreground | new_background | blinking_bit;

            // Write back the new attribute byte
            VGA[i*2+1] = new_attr_byte;
        }
        // If backgrounds don't match, leave the attribute byte unchanged
    }
    // +++++++++++ChatGPT-Function+++++++++++ //
}
/**
 * Purpose: To update the foreground portion of all positions' color attribute
 * - in the VGA Buffer.
 * Note: This function skips memory addresses that do not have the same color
 * - foreground as the given argument "prevForeground".
 * << UpdateForegroundColorAttribute >>
 * @params: 
 *      Color prev_fg -> Previous color of foreground or text (0x0 - 0xf) enum
 * @returns: void
 */
void UpdateForegroundColorAttribute(Color prev_fg)
{
    // +++++++++++ChatGPT-Function+++++++++++ //
    for(int i = 0; i < 2000; i++)
    {
        // Read the current attribute byte
        char attr_byte = VGA[i*2+1];

        // Extract the existing foreground color (bits 0-3)
        char existing_foreground = attr_byte & 0x0F;

        // Compare with prev_fg (ensure it's in the range 0x0-0xF)
        if (existing_foreground == (prev_fg & 0x0F))
        {
            // Keep the existing background color and blinking bit (bits 4-7)
            char background_and_blink = attr_byte & 0xF0;

            // Set the new foreground color (bits 0-3)
            char new_foreground = foreground & 0x0F;

            // Reconstruct the attribute byte
            char new_attr_byte = background_and_blink | new_foreground;

            // Write back the new attribute byte
            VGA[i*2+1] = new_attr_byte;
        }
        // If foregrounds don't match, leave the attribute byte unchanged
    }
    // +++++++++++ChatGPT-Function+++++++++++ //
}
/**
 * Purpose: To directly manipulate the variable "foreground" which will be
 * - utilized in the VGA_CURRENT_COLOR macro.
 * << SetTextColor >>
 * @params: 
 *      Color color -> Color to set foreground or text (0x0 - 0xf) enum
 * @returns: void
 */
void SetTextColor(Color color)
{
    foreground = color;
}
/**
 * Purpose: To directly manipulate the variable "background" which will be
 * - utilized in the VGA_CURRENT_COLOR macro.
 * << SetBackGroundColor >>
 * @params: 
 *      Color color -> Color to set background (0x0 - 0xf) enum
 * @returns: void
 */
void SetBackGroundColor(Color color)
{
    background = color;
}
/**
 * Purpose: To directly manipulate the variable "foreground" which will be
 * - utilized in the VGA_CURRENT_COLOR macro and then Update all of the
 * - correct portions of the color attributes in the VGA Buffer with a
 * - function call.
 * << SetAllTextColor >>
 * @params: 
 *      Color color -> Color to set foreground or text (0x0 - 0xf) enum
 * @returns: void
 */
void SetAllTextColor(Color color) // excludes individually set colors with PrintStringColor
{
    Color prev_fg = foreground;
    foreground = color;
    UpdateForegroundColorAttribute(prev_fg);
}
/**
 * Purpose: To directly manipulate the variable "background" which will be
 * - utilized in the VGA_CURRENT_COLOR macro and then Update all of the
 * - correct portions of the color attributes in the VGA Buffer with a
 * - function call.
 * << SetFullBackGroundColor >>
 * @params: 
 *      Color color -> Color to set background (0x0 - 0xf) enum
 * @returns: void
 */
void SetFullBackGroundColor(Color color) // excludes highlights set with PrintStringHighlight
{
    Color prev_bg = background;
    background = color;
    UpdateBackgroundColorAttribute(prev_bg);
}
/**
 * Purpose: Get current foreground color.
 * << GetForeGround >>
 * @params: none
 * @returns: 
 *      char -> foreground
 */
char GetForeGround()
{
    return foreground;
}
/**
 * Purpose: Get current background color.
 * << GetBackGround >>
 * @params: none
 * @returns: 
 *      char -> backgorund
 */
char GetBackGround()
{
    return background;
}
/**
 * Purpose: Set all background attributes not matching the current background
 * - color to the current background color. Basically gets rid of highlights.
 * << RemoveHighlights >>
 * @params: none
 * @returns: void
 */
void RemoveHighlights()
{
    for(int i = 0; i < 2000; i++)
    {
        // Read the current attribute byte
        char attr_byte = VGA[i*2+1];
        char bg_byte = attr_byte;
        bg_byte &= 0xF0;
        // Clear the background color bits (bits 4-6)
        if (bg_byte != background)
        {
             // Keep bits 0-3 (foreground color), clear bits 4-7
            attr_byte &= 0x0F; 
            // Set the new background color (shifted to bits 4-6)
            attr_byte |= (background << 4);
            VGA[i*2+1] = attr_byte;
        }
    }
}
/**
 * Purpose: Set all foreground attributes not matching the current foreground color
 * - to the current foreground color. Basically gets rid of custom set text colors.
 * << RemoveTextColors >>
 * @params: none
 * @returns: void
 */
void RemoveTextColors()
{
    for(int i = 0; i < 2000; i++)
    {
        // Read the current attribute byte
        char attr_byte = VGA[i*2+1];
        char fg_byte = attr_byte;
        fg_byte &= 0x0F;
        // Clear the background color bits (bits 4-6)
        if (fg_byte != foreground)
        {
            // Keep bits 4-7 (background), clear bits 0-3
            attr_byte &= 0xF0;
            // Set the new foreground color
            attr_byte |= foreground;
            VGA[i*2+1] = attr_byte;
        }
    }
}
/**
 * Purpose: Make calls to SetTextColor and SetFullBackGroundColor to reset to 
 * - their default values of Light Gray (0x7) and Black (0x0) respectively.
 * << ResetColorAttributes >>
 * @params: none
 * @returns: void
 */
void ResetColorAttributes()
{
    SetTextColor(LIGHT_GRAY);
    SetFullBackGroundColor(BLACK);
    RemoveTextColors();
    RemoveHighlights();
}
/**
 * Purpose: To clear the screen of all text characters filling in the VGA buffer
 * - with spaces (0x20) and the VGA_CURRENT_COLOR.
 * << ClearScreen >>
 * @params: none
 * @returns: void
 */
void ClearScreen()
{
    for(int i = 0; i < 2000; i++)
    { 
        VGA[i*2] = 0x20; // ascii space
        VGA[i*2+1] = VGA_CURRENT_COLOR;
    }
    SetCursorDefaultPosition();
    CustomizeVGACursor(); // set cursor to full block
}
/**
 * Purpose: To reset text and background color to default values and then
 * - clear the screen of all text characters with ClearScreen.
 * << ResetScreen >>
 * @params: none
 * @returns: void
 */
void ResetScreen()
{
    ResetColorAttributes(); // more work than needs to be
    ClearScreen();
}
/**
 * Purpose: To move all characters printed to the screen up by 1 in the
 * - screen y. Move one row at a time to the memory of the row above from
 * - top row to bottom row. Finally, clear the last line.
 * << ScrollScreen >>
 * @params: none
 * @returns: void
 */
void ScrollScreenUp() 
{
    for (int y = 1; y < 25; y++) {
        for (int x = 0; x < 80; x++) {
            VGA[(y-1) * 80 * 2 + x * 2] = VGA[y * 80 * 2 + x * 2];
            VGA[(y-1) * 80 * 2 + x * 2 + 1] = VGA[y * 80 * 2 + x * 2 + 1];
        }
    }
    // Clear the last line
    for (int x = 0; x < 80; x++) {
        VGA[(24 * 80 * 2) + x * 2] = 0x20;  // ASCII space
        VGA[(24 * 80 * 2) + x * 2 + 1] = VGA_CURRENT_COLOR;  // Attribute
    }
}
/**
 * Purpose: To move all characters printed to the screen down by 1 in the
 * - screen y. Move one row at a time to the memory of the row below from
 * - bottom row to top row.
 * << ScrollScreenDown >>
 * @params: none 
 * @returns: void
 */
void ScrollScreenDown() 
{
    // FIXME: consider uses the pages feature in VGA? instead of buffer...
    // Move each line down by one, starting from the bottom
    for (int y = 23; y >= 0; y--) {
        for (int x = 0; x < 80; x++) {
            VGA[(y + 1) * 80 * 2 + x * 2] = VGA[y * 80 * 2 + x * 2];
            VGA[(y + 1) * 80 * 2 + x * 2 + 1] = VGA[y * 80 * 2 + x * 2 + 1];
        }
    }
    // // Clear the first line
    // for (int x = 0; x < 80; x++) {
    //     VGA[x * 2] = 0x20;  // ASCII space
    //     VGA[x * 2 + 1] = VGA_CURRENT_COLOR;  // Attribute
    // }
}
/**
 * Purpose: To move the cursor to the space directly after the last
 * - character on the current row.
 * << MoveCursorToLastCharOnLine >>
 * @params: none 
 * @returns: void
 */
void MoveCursorToLastCharOnLine()
{
    int line_start = CURSOR_POS - (CURSOR_POS % 160);
    for(int i = 79; i >= default_cursor_x; i--)
    {
        if(VGA[i*2 + line_start] != ' ')
        {
            cursor_x = i + 1;
            if(cursor_x >= 80)
                cursor_x = 79;
            return;
        }
    }
    // If the line is empty, set cursor_x to default
    cursor_x = default_cursor_x;
}
/**
 * Purpose: To increment to position of the cursor checking for things
 * - such as end of lines and end of screen. This function appropriately 
 * - calls ScrollScreen when cursor_y > 24 and sets cursor_x to its default
 * - when hitting the end of a line or cursor_x > 79.
 * << MoveCursor >>
 * @params: 
 *      int x -> x value to add to cursor_x (0-79)
 *      int y -> y value to add to cursor_y (0-24)
 * @returns: void
 */
void MoveCursor(int x, int y) 
{
    cursor_x += x;
    cursor_y += y;
    if (cursor_x >= 79) { // check 79 to keep last space on each line blank
        cursor_x = default_cursor_x;
        cursor_y++;
    }
    else if (cursor_x < default_cursor_x) { 
        cursor_y--;
        MoveCursorToLastCharOnLine();
    }
    if (cursor_y >= 25) {
        ScrollScreenUp();
        cursor_y = 24;  // Keep cursor at the bottom
    }
    else if (cursor_y < 0) {
        ScrollScreenDown();
        cursor_y = 0;
        MoveCursorToLastCharOnLine();
    }
    UpdateVGACursorPosition();
}
/**
 * Purpose: To move a given char into the memory address of the current 
 * - cursor position and set the color attribute at the next address
 * - to the VGA_CURRENT_COLOR macro.
 * << PrintCharAtCursor >>
 * @params: 
 *      char c -> an ascii character
 * @returns: void
 */
void PrintCharAtCursor(char c) 
{
    VGA[CURSOR_POS] = c;
    VGA[CURSOR_POS + 1] = VGA_CURRENT_COLOR;  // Attribute byte
}
/**
 * Purpose: To move a given char into the memory address of the current 
 * - cursor position and set the color attribute at the next address
 * - to the attribute created with VGA_COLOR and the given fg & bg.
 * << PrintCharFullColorAtCursor >>
 * @params: 
 *      char c -> an ascii character
 *      Color fg -> vga_color 0-15
 *      Color bg -> vga_color 0-15
 * @returns: void
 */
void PrintCharFullColorAtCursor(char c, Color fg, Color bg)
{
    VGA[CURSOR_POS] = c;
    VGA[CURSOR_POS + 1] = VGA_COLOR(fg, bg);  // Attribute byte
}
/**
 * Purpose: To delete the most recently printed char at the previous cursor
 * - position. Move cursor has been updated to ensure the page will scroll
 * - appropriately if deleting the beyond the first index in the VGA buffer.
 * << Delete >>
 * @params: none
 * @returns: void
 */
void Delete()
{
    MoveCursor(-1, 0);
    PrintCharAtCursor(' ');
}
/**
 * Purpose: To move the cursor position to the next position in the VGA
 * - that is divisibly by a specified value. Default -> 8
 * << Tab >>
 * @params: none
 * @returns: void
 */
void Tab()
{
    MoveCursor(cursor_x % tab == 0 ? tab : tab - (cursor_x % tab), 0);
}
/**
 * Purpose: To move the cursor to the the next line and returning cursor_x
 * - to its default position.
 * << NewLine >>
 * @params: none
 * @returns: void
 */
void NewLine()
{
    cursor_x = default_cursor_x;
    MoveCursor(0, 1);
}
/**
 * Purpose: To move a given char into the memory address of the current
 * - cursor position and increment the position of the cursor. This function
 * - also checks for special characters like '\n' and '\t', handling 
 * - their situations accordingly.
 * << PrintChar >>
 * @params: 
 *      char c -> an ascii character
 * @returns: void
 */
void PrintChar(char c) 
{
    switch(c)
    {
        case '\n': NewLine(); break;
        case '\t': Tab(); break;
        case '\b': Delete(); break;
        default:
        {
            PrintCharAtCursor(c);
            MoveCursor(1, 0);
            break;
        }
    }
}
/**
 * Purpose: To print a character at the current cursor location with a given color.
 * - Does this by saving the previous text color in a variable before reassigning it
 * - at the end of the function.
 * << PrintCharColor >>
 * @params: 
 *      char c -> a character -> ascii character
 *      Color color -> Color to set foreground or text (0x0 - 0xf) enum
 * @returns: void
 */
void PrintCharColor(char c, Color color)
{
    Color prev_fg = foreground;
    SetTextColor(color);
    PrintChar(c);
    SetTextColor(prev_fg);
}
/**
 * Purpose: To print a character at the current cursor location with a given 
 * - background color. Does this by saving the previous background color 
 * - in a variable before reassigning it at the end of the function.
 * << PrintCharHighlight >>
 * @params: 
 *      char c -> a character -> ascii character
 *      Color color -> Color to set background (0x0 - 0xf) enum
 * @returns: void
 */
void PrintCharHighlight(char c, Color color)
{
    Color prev_bg = background;
    SetBackGroundColor(color);
    PrintChar(c);
    SetBackGroundColor(prev_bg);
}
/**
 * Purpose: To print a character at the current cursor location with a given 
 * - background color and foreground color. Does this by saving the previous 
 * - background color and foreground color in a variable before reassigning
 * - them at the end of the function.
 * << PrintCharFullColor >>
 * @params: 
 *      char c -> a character -> ascii character
 *      Color fg -> Color to set foreground or text (0x0 - 0xf) enum
 *      Color bg -> Color to set background (0x0 - 0xf) enum
 * @returns: void
 */
void PrintCharFullColor(char c, Color fg, Color bg)
{
    Color prev_bg = background;
    Color prev_fg = foreground;
    SetBackGroundColor(bg);
    SetTextColor(fg);
    PrintChar(c);
    SetBackGroundColor(prev_bg);
    SetTextColor(prev_fg);
}
/**
 * Purpose: To move the ascii value of a given integer into the VGA buffer's
 * - memory at the current cursor location. Does this with a call to PrintChar,
 * - passing in the value of the single digit integer plus 0x30 which corresponds
 * - to the value of the ascii character '0'. This maps int to char.
 * << PrintIntSingleDigit >>
 * @params: 
 *      int n -> an integer
 * @returns: void
 */
void PrintIntSingleDigit(int n) 
{
    PrintChar(n + '0');
}
/**
 * Purpose: To print a given integer to the screen at the cursor's current
 * - location. Does this by getting the length of integer and ensuring we 
 * - have the absolute value of the integer. The outer while loop while loop 
 * - the length of the integer, and the inner loop will loop the current value
 * - of (length - 1) to be left with only the desired digit. We print the number from
 * - left to right or highest order digit to lowest.
 * << PrintInt >>
 * @params: 
 *      int n -> an integer
 * @returns: void
 */
void PrintInt(int n)
{
    HideCursor();
    int temp, l;
    l = GetIntLength(n);
    if(n < 0) { PrintChar('-'); n *= -1;}
    while (l > 0){
        temp = n;
        for(int i = 0; i < l - 1; i++) { temp /= 10; }
        PrintIntSingleDigit(temp % 10);
        l--;
    }
    ShowCursor();
}
/**
 * Purpose: To print an integer at the current cursor location with a given color.
 * - Does this by saving the previous text color in a variable before reassigning it
 * - at the end of the function.
 * << PrintIntColor >>
 * @params: 
 *      int n -> an integer
 *      Color color -> Color to set foreground or text (0x0 - 0xf) enum
 * @returns: void
 */
void PrintIntColor(int n, Color color)
{
    Color prev_fg = foreground;
    SetTextColor(color);
    PrintInt(n);
    SetTextColor(prev_fg);
}
/**
 * Purpose: To print an integer at the current cursor location with a given 
 * - background color. Does this by saving the previous background color 
 * - in a variable before reassigning it at the end of the function.
 * << PrintIntHighlight >>
 * @params: 
 *      int n -> an integer
 *      Color color -> Color to set background (0x0 - 0xf) enum
 * @returns: void
 */
void PrintIntHighlight(int n, Color color)
{
    Color prev_bg = background;
    SetBackGroundColor(color);
    PrintInt(n);
    SetBackGroundColor(prev_bg);
}
/**
 * Purpose: To print an integer at the current cursor location with a given 
 * - background color and foreground color. Does this by saving the previous 
 * - background color and foreground color in a variable before reassigning
 * - them at the end of the function.
 * << PrintIntFullColor >>
 * @params: 
 *      int n -> an integer
 *      Color fg -> Color to set foreground or text (0x0 - 0xf) enum
 *      Color bg -> Color to set background (0x0 - 0xf) enum
 * @returns: void
 */
void PrintIntFullColor(int n, Color fg, Color bg)
{
    Color prev_bg = background;
    Color prev_fg = foreground;
    SetBackGroundColor(bg);
    SetTextColor(fg);
    PrintInt(n);
    SetBackGroundColor(prev_bg);
    SetTextColor(prev_fg);
}
/**
 * Purpose: To print a given double to the screen at the cursor's current
 * - location. Does this by take floor of d and printing it as an integer,
 * - then finding number of digits after decimal point and printing the 
 * - integer equivalent to the remainder times ten to the power of the 
 * - number of digits after the decimal point.
 * << PrintDouble >>
 * @params: 
 *      double d -> a double
 * @returns: void
 */
void PrintDouble(double d)
{
    HideCursor();
    int f, l, dec;
    double r;
    if(d < 0.0) { PrintChar('-'); d *= -1; }
    f = Floor(d);                   // floor
    r = d - f;                      // remainder
    l = GetDecimalLength(d);        // number of digits after decimal point
    dec = r * Pow(10.0, l);         // integer representation of digits after decimal

    PrintInt(f);
    PrintChar('.');
    PrintInt((int)dec);
    ShowCursor();
}
/**
 * Purpose: To print a double at the current cursor location with a given color.
 * - Does this by saving the previous text color in a variable before reassigning it
 * - at the end of the function.
 * << PrintDoubleColor >>
 * @params: 
 *      double d -> a double
 *      Color color -> Color to set foreground or text (0x0 - 0xf) enum
 * @returns: void
 */
void PrintDoubleColor(double d, Color color)
{
    Color prev_fg = foreground;
    SetTextColor(color);
    PrintDouble(d);
    SetTextColor(prev_fg);
}
/**
 * Purpose: To print a double at the current cursor location with a given 
 * - background color. Does this by saving the previous background color 
 * - in a variable before reassigning it at the end of the function.
 * << PrintDoubleHighlight >>
 * @params: 
 *      double d -> a double
 *      Color color -> Color to set background (0x0 - 0xf) enum
 * @returns: void
 */
void PrintDoubleHighlight(double d, Color color)
{
    Color prev_bg = background;
    SetBackGroundColor(color);
    PrintDouble(d);
    SetBackGroundColor(prev_bg);
}
/**
 * Purpose: To print a double at the current cursor location with a given 
 * - background color and foreground color. Does this by saving the previous 
 * - background color and foreground color in a variable before reassigning
 * - them at the end of the function.
 * << PrintDoubleFullColor >>
 * @params: 
 *      double d -> a double
 *      Color fg -> Color to set foreground or text (0x0 - 0xf) enum
 *      Color bg -> Color to set background (0x0 - 0xf) enum
 * @returns: void
 */
void PrintDoubleFullColor(double d, Color fg, Color bg)
{
    Color prev_bg = background;
    Color prev_fg = foreground;
    SetBackGroundColor(bg);
    SetTextColor(fg);
    PrintDouble(d);
    SetBackGroundColor(prev_bg);
    SetTextColor(prev_fg);
}
/**
 * Purpose: To print a string to the screen at the current cursor position.
 * - Does this by looping over the characters and calling PrintChar.
 * << PrintString >>
 * @params: 
 *      const char* str -> a string of ascii characters
 * @returns: void
 */
void PrintString(const char* str)
{
    HideCursor();
    int i = 0;
    while (str[i] != '\0') {
        PrintChar(str[i]);
        i++;
    }
    ShowCursor();
}
/**
 * Purpose: To print a string to the screen at the current cursor position
 * and then go to a new line. Does this with calls to PrintString and PrintChar.
 * << PrintLine >>
 * @params: 
 *      const char* str -> a string of ascii characters
 * @returns: void
 */
void PrintLine(const char* str)
{
    PrintString(str);
    PrintChar('\n');
}
/**
 * Purpose: To print a string at the current cursor location with a given color.
 * - Does this by saving the previous text color in a variable before reassigning it
 * - at the end of the function.
 * << PrintStringColor >>
 * @params: 
 *      const char* str -> a string of ascii characters
 *      Color color -> Color to set foreground or text (0x0 - 0xf) enum
 * @returns: void
 */
void PrintStringColor(const char* str, Color color)
{
    Color prev_fg = foreground;
    SetTextColor(color);
    PrintString(str);
    SetTextColor(prev_fg);
}
/**
 * Purpose: To print a string at the current cursor location with a given 
 * - background color. Does this by saving the previous background color 
 * - in a variable before reassigning it at the end of the function.
 * << PrintStringHighlight >>
 * @params: 
 *      const char* str -> a string of ascii characters
 *      Color color -> Color to set background (0x0 - 0xf) enum
 * @returns: void
 */
void PrintStringHighlight(const char* str, Color color)
{
    Color prev_bg = background;
    SetBackGroundColor(color);
    PrintString(str);
    SetBackGroundColor(prev_bg);
}
/**
 * Purpose: To print a string at the current cursor location with a given 
 * - background color and foreground color. Does this by saving the previous 
 * - background color and foreground color in a variable before reassigning
 * - them at the end of the function.
 * << PrintStringFullColor >>
 * @params: 
 *      const char* str -> a string of ascii characters
 *      Color fg -> Color to set foreground or text (0x0 - 0xf) enum
 *      Color bg -> Color to set background (0x0 - 0xf) enum
 * @returns: void
 */
void PrintStringFullColor(const char* str, Color fg, Color bg)
{
    Color prev_bg = background;
    Color prev_fg = foreground;
    SetBackGroundColor(bg);
    SetTextColor(fg);
    PrintString(str);
    SetBackGroundColor(prev_bg);
    SetTextColor(prev_fg);
}
/**
 * Purpose: To print a string at the current cursor location with a given color
 * and then go to a new line. Does this by saving the previous text color in a 
 * - variable before reassigning it at the end of the function.
 * << PrintLineColor >>
 * @params: 
 *      const char* str -> a string of ascii characters
 *      Color color -> Color to set foreground or text (0x0 - 0xf) enum
 * @returns: void
 */
void PrintLineColor(const char* str, Color color)
{
    Color prev_fg = foreground;
    SetTextColor(color);
    PrintString(str);
    SetTextColor(prev_fg);
    PrintChar('\n');
}
/**
 * Purpose: To print a string at the current cursor location with a given 
 * - background color and then go to a new line. Does this by saving the 
 * - previous background color in a variable before reassigning it at the 
 * - end of the function.
 * << PrintLineHighlight >>
 * @params: 
 *      const char* str -> a string of ascii characters
 *      Color color -> Color to set background (0x0 - 0xf) enum
 * @returns: void
 */
void PrintLineHighlight(const char* str, Color color)
{
    Color prev_bg = background;
    SetBackGroundColor(color);
    PrintString(str);
    SetBackGroundColor(prev_bg);
    PrintChar('\n');
}
/**
 * Purpose: To print a string at the current cursor location with a given 
 * - background color and foreground color and then go to a new line. 
 * - Does this by saving the previous background color and foreground color
 * - in a variable before reassigning them at the end of the function.
 * << PrintLineFullColor >>
 * @params: 
 *      const char* str -> a string of ascii characters
 *      Color fg -> Color to set foreground or text (0x0 - 0xf) enum
 *      Color bg -> Color to set background (0x0 - 0xf) enum
 * @returns: void
 */
void PrintLineFullColor(const char* str, Color fg, Color bg)
{
    Color prev_bg = background;
    Color prev_fg = foreground;
    SetBackGroundColor(bg);
    SetTextColor(fg);
    PrintString(str);
    SetBackGroundColor(prev_bg);
    SetTextColor(prev_fg);
    PrintChar('\n');
}
/**
 * Purpose: Prints to screen a formatted string that contains percent signs,
 * - followed by (d, f, s, c) ex: "%f", where it will print the corresponding
 * - variable based on its type in place of the percent sign and identifier.
 * Note: Following the formatted string can be any combination of ints, doubles,
 * - strings, or chars listed as arguements. Their order matters and is respective
 * - to their order in the formatted string.
 * << Printf >>
 * @params: 
 *      const char* format -> a string of ascii characters containing identifiers "%"
 *      int n -> an integer corresponding with %d in formatted string
 *      double d -> a double corresponding with %f in formatted string
 *      const char* str -> a string corresponding with %s in formatted string
 *      char c -> a char corresponding with %c in formatted string
 * @returns: void
 */
void Printf(const char* format, ...)
{
    // +++++++++++ChatGPT-Function+++++++++++ //
    // Pointer to the first argument after 'format'
    unsigned int* arg_ptr = (unsigned int*)(&format + 1);

    const char* p = format;

    while (*p != '\0') 
    {
        if (*p == '%') 
        {
            p++;
            if (*p == '\0') 
            {
                // Error: Incomplete format specifier at end of string
                PrintStringColor("Error: Incomplete format specifier\n", RED);
                return;
            } 
            else 
            {
                char specifier = *p;

                switch (specifier) 
                {
                    case 'd': 
                    {
                        int n = *arg_ptr;
                        arg_ptr++;
                        PrintInt(n);
                        break;
                    }
                    case 'f': 
                    {
                        double d = *((double*)arg_ptr);
                        arg_ptr += 2; // Move past the double (8 bytes)
                        PrintDouble(d);
                        break;
                    }
                    case 's': 
                    {
                        const char* s = *((const char**)arg_ptr);
                        arg_ptr++;
                        PrintString(s);
                        break;
                    }
                    case 'c': 
                    {
                        char c = (char)(*arg_ptr);
                        arg_ptr++;
                        PrintChar(c);
                        break;
                    }
                    default: 
                    {
                        // Error: Unknown format specifier
                        PrintStringColor("Error: Unknown format specifier ", RED);
                        PrintCharFullColor(specifier, RED, YELLOW);
                        PrintChar('\n');
                        return;
                    }
                }
            }
        } 
        else 
        {
            PrintChar(*p);
        }
        p++;
    }
    // +++++++++++ChatGPT-Function+++++++++++ //
}