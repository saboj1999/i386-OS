#ifndef OS_INCLUDE_KEYBOARD_H
#define OS_INCLUDE_KEYBOARD_H
#include <stdint.h>

#define MAX_INPUT_LENGTH 256
// DEPRECATED
// For use before bit manipulation (different scancodes for press and release)
#define LSHIFT_PRESSED  0x2A
#define LSHIFT_RELEASED 0xAA
#define RSHIFT_PRESSED  0x36
#define RSHIFT_RELEASED 0xB6
#define UARROW_PRESSED 0x48
#define DARROW_PRESSED 0x50
#define LARROW_PRESSED 0x4B
#define RARROW_PRESSED 0x4D
#define UARROW_RELEASED 0xC8
#define DARROW_RELEASED 0xD0
#define LARROW_RELEASED 0xCB
#define RARROW_RELEASED 0xCD
#define SPACE_PRESSED 0x39
#define ESCAPE_PRESSED 0x01
#define ENTER_PRESSED 0x1C
// For use after bit manipulation (same scancode for press and release)
#define CAPSLOCK_PRESSED 0x3A

typedef enum {
    //=======CHAT-GPT MADE THIS ENUM======//
    KEY_NONE = 0x00,

    // Escape key
    KEY_ESC = 0x01,

    // Number keys (top row)
    KEY_1 = 0x02,
    KEY_2 = 0x03,
    KEY_3 = 0x04,
    KEY_4 = 0x05,
    KEY_5 = 0x06,
    KEY_6 = 0x07,
    KEY_7 = 0x08,
    KEY_8 = 0x09,
    KEY_9 = 0x0A,
    KEY_0 = 0x0B,
    KEY_MINUS = 0x0C,       // '-'
    KEY_EQUAL = 0x0D,       // '='
    KEY_BACKSPACE = 0x0E,

    // Tab and Q to P
    KEY_TAB = 0x0F,
    KEY_Q = 0x10,
    KEY_W = 0x11,
    KEY_E = 0x12,
    KEY_R = 0x13,
    KEY_T = 0x14,
    KEY_Y = 0x15,
    KEY_U = 0x16,
    KEY_I = 0x17,
    KEY_O = 0x18,
    KEY_P = 0x19,
    KEY_LEFT_BRACKET = 0x1A,  // '['
    KEY_RIGHT_BRACKET = 0x1B, // ']'
    KEY_ENTER = 0x1C,
    KEY_LEFT_CTRL = 0x1D,

    // A to G
    KEY_A = 0x1E,
    KEY_S = 0x1F,
    KEY_D = 0x20,
    KEY_F = 0x21,
    KEY_G = 0x22,
    KEY_H = 0x23,
    KEY_J = 0x24,
    KEY_K = 0x25,
    KEY_L = 0x26,
    KEY_SEMICOLON = 0x27,     // ';'
    KEY_APOSTROPHE = 0x28,    // '''
    KEY_GRAVE = 0x29,         // '`'
    KEY_LEFT_SHIFT = 0x2A,
    KEY_BACKSLASH = 0x2B,     // '\'

    // Z to M
    KEY_Z = 0x2C,
    KEY_X = 0x2D,
    KEY_C = 0x2E,
    KEY_V = 0x2F,
    KEY_B = 0x30,
    KEY_N = 0x31,
    KEY_M = 0x32,
    KEY_COMMA = 0x33,         // ','
    KEY_PERIOD = 0x34,        // '.'
    KEY_SLASH = 0x35,         // '/'
    KEY_RIGHT_SHIFT = 0x36,

    // Special keys
    KEY_KP_ASTERISK = 0x37,   // '*' on keypad
    KEY_LEFT_ALT = 0x38,
    KEY_SPACE = 0x39,
    KEY_CAPS_LOCK = 0x3A,

    // Function keys
    KEY_F1 = 0x3B,
    KEY_F2 = 0x3C,
    KEY_F3 = 0x3D,
    KEY_F4 = 0x3E,
    KEY_F5 = 0x3F,
    KEY_F6 = 0x40,
    KEY_F7 = 0x41,
    KEY_F8 = 0x42,
    KEY_F9 = 0x43,
    KEY_F10 = 0x44,

    // Number lock and scroll lock
    KEY_NUM_LOCK = 0x45,
    KEY_SCROLL_LOCK = 0x46,

    // Numpad keys (with num lock ON)
    KEY_KP_7 = 0x47,
    KEY_KP_8 = 0x48,
    KEY_KP_9 = 0x49,
    KEY_KP_MINUS = 0x4A,
    KEY_KP_4 = 0x4B,
    KEY_KP_5 = 0x4C,
    KEY_KP_6 = 0x4D,
    KEY_KP_PLUS = 0x4E,
    KEY_KP_1 = 0x4F,
    KEY_KP_2 = 0x50,
    KEY_KP_3 = 0x51,
    KEY_KP_0 = 0x52,
    KEY_KP_DECIMAL = 0x53,     // '.'

    // More function keys
    KEY_F11 = 0x57,
    KEY_F12 = 0x58

} KeyPress;


/* Scan Functions */
char ScancodeToChar(unsigned char scancode);
uint8_t Scan(void);

/* Get Input Functions */
char GetChar();
char* GetInput();

#endif