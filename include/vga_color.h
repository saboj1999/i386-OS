#ifndef OS_INCLUDE_VGA_COLOR_H
#define OS_INCLUDE_VGA_COLOR_H

// VGA Color Macro -> shift background 4 bits left and perform bitwise OR
#define VGA_COLOR(fg, bg) ((bg << 4) | (fg))

typedef enum {
        BLACK = 0,
        BLUE,
        GREEN,
        CYAN,
        RED,
        MAGENTA,
        BROWN,
        LIGHT_GRAY,
        DARK_GRAY,
        LIGHT_BLUE,
        LIGHT_GREEN,
        LIGHT_CYAN,
        LIGHT_RED,
        LIGHT_MAGENTA,
        YELLOW,
        WHITE
} Color;

#endif