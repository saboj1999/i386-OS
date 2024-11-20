#include "../../include/portmap.h"
#include "../../include/bool.h"
#include "../../include/console.h"
#include "../../include/keyboard.h"

static Bool isShiftPressed = False;
static Bool isCapsLocked = False;
static Bool isArrowPressed = False;

static const char charmap[256] =
{
    0,   0/*0x1B*/, '1',  '2',  '3',  '4',  '5',  '6',
    '7',  '8',  '9',  '0',  '-',  '=',  '\b', '\t',
    'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',
    'o',  'p',  '[',  ']',  '\n', 0,   'a',  's',
    'd',  'f',  'g',  'h',  'j',  'k',  'l',  ';',
    '\'', '`',  0,   '\\', 'z',  'x',  'c',  'v',
    'b',  'n',  'm',  ',',  '.',  '/',  0,   '*', 
    0,   ' ',  0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   '7',
    '8',  '9',  '-',  '4',  '5',  '6',  '+',  '1',
    '2',  '3',  '0',  '.',  0,   0,   0,   0
};

static const char shift_charmap[256] = 
{
    0,   0x1B, '!',  '@',  '#',  '$',  '%',  '^',
    '&', '*',  '(',  ')',  '_',  '+',  '\b', '\t',
    'Q', 'W',  'E',  'R',  'T',  'Y',  'U',  'I',
    'O', 'P',  '{',  '}',  '\n', 0,   'A',  'S',
    'D', 'F',  'G',  'H',  'J',  'K',  'L', ':',
    '"', '~',  0,   '|',  'Z',  'X',  'C', 'V',
    'B', 'N',  'M', '<',  '>',  '?',  0,   '*',
    0,   ' ',  0,    0,    0,    0,    0,    0,
};

/**
 * Purpose: To take a scancode from inb at port 0x60 and return the 
 * - associated ascii character.
 * << ScancodeToChar >>
 * @params: 
 *      unsigned char scancode -> scancode recieved from inb
 * @returns: 
 *      Char -> equivalent ascii character
 */
char ScancodeToChar(unsigned char scancode) 
{
    if (scancode > 0 && scancode <= 255) {
        return (isShiftPressed || isCapsLocked) ? shift_charmap[scancode] : charmap[scancode];
    }
    return 0;
}
/**
 * Purpose: Utilize inb to recieve a byte from port 0x60 (keyboard),
 * - and handle special cases like Shift / Capslock. Returns 0 if not
 * - a valid key.
 * << Scan >>
 * @params: none
 * @returns: 
 *      uint8_t -> byte recieved from keyboard port
 */
uint8_t Scan(void) 
{
    uint8_t brk;
    static uint8_t key = 0;
    uint8_t scan = inb(0x60);
    // Printf("%d\n", scan);

    if (scan == LSHIFT_PRESSED || scan == RSHIFT_PRESSED) {
        isShiftPressed = True;
        return 0;
    } else if (scan == LSHIFT_RELEASED || scan == RSHIFT_RELEASED) {
        isShiftPressed = False;
        return 0;
    } else if (scan == UARROW_PRESSED || scan == DARROW_PRESSED || 
        scan == LARROW_PRESSED || scan == RARROW_PRESSED) 
    {
        if(isArrowPressed) return 0;
        isArrowPressed = True;
        switch (scan)
        {
            case UARROW_PRESSED: MoveCursor(0, -1); break;
            case DARROW_PRESSED: MoveCursor(0, 1); break;
            case LARROW_PRESSED: MoveCursor(-1, 0); break;
            case RARROW_PRESSED: MoveCursor(1, 0); break;
        }
        return 0;
    }
    isArrowPressed = False; // hacky fix

    brk = scan & 0x80;
    scan = scan & 0x7F;

    if (scan == CAPSLOCK_PRESSED) {
        isCapsLocked = !isCapsLocked;
        return 0;
    }

    if (brk) {
        return key = 0;
    } else if (scan != key) {
        return key = scan;
    } else {
        return 0;
    }
}
/**
 * Purpose: Utilize Scan and ScancodeToChar to get a byte from the 
 * - keyboard port and return the ascii equivalent.
 * << Scan >>
 * @params: none
 * @returns: 
 *      char -> scancode converted to ascii from keyboard port
 */
char GetChar()
{
    uint8_t byte = 0;
    while (byte == 0)
    {
        byte = Scan();
    }
    return ScancodeToChar(byte);
}
/**
 * Purpose: Utilize GetChar and PrintChar to allow the user to type
 * - a line of text before hitting enter or \n to submit the input.
 * << Scan >>
 * @params: none
 * @returns: 
 *      char* -> the string of text displayed on screen before next new line
 */
char* GetInput() 
{
    // +++++++++++ChatGPT-Function+++++++++++ //
    static char input_buffer[MAX_INPUT_LENGTH];
    uint16_t index = 0;
    char c;

    // Initialize the buffer with null characters
    for (int i = 0; i < MAX_INPUT_LENGTH; i++) 
    {
        input_buffer[i] = '\0';
    }

    while (True) 
    {
        c = GetChar();
        PrintChar(c);

        // Handle Enter key
        if (c == '\n') break;
        // Handle Backspace
        else if (c == '\b') {
            if (index > 0) {
                index--;
                input_buffer[index] = '\0';
            }
        }
        // Handle Tab
        else if (c == '\t') {
            if (index < MAX_INPUT_LENGTH - 1) {
                input_buffer[index++] = ' ';
            }
        }
        // Handle printable characters
        else if (c >= ' ' && c <= '~') {  // ASCII printable characters
            if (index < MAX_INPUT_LENGTH - 1) {  // Leave space for null terminator
                input_buffer[index++] = c;
            } 
        }
    }

    // Ensure the string is null-terminated
    input_buffer[index] = '\0';

    // Return the input string
    return input_buffer;
    // +++++++++++ChatGPT-Function+++++++++++ //
}
