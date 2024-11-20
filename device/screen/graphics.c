#include "../../include/portmap.h"
#include "../../include/bool.h"
#include "../../include/font.h"
#include "../../include/math.h"
#include "../../include/graphics.h"

#define VIDEO_MEMORY ((uint8_t*)0xA0000)

#define CRTC_REGISTER_COUNT 17

static const int screen_width = 320;
static const int screen_height = 200;

static uint8_t* crtc_text_values = {0};

static uint8_t frame_buffer[320*200] = {0};

void write_to_port_at_index(uint16_t port, uint8_t index, uint8_t data)
{
    outb(port, index);
    outb(port+1, data);
}

void write_sequencer_register(uint8_t index, uint8_t data) {
    outb(0x3C4, index);
    outb(0x3C5, data);
}

void write_crtc_register(uint8_t index, uint8_t data) {
    outb(0x3D4, index);
    outb(0x3D5, data);
}

void write_graphics_register(uint8_t index, uint8_t data) {
    outb(0x3CE, index);
    outb(0x3CF, data);
}

void wait_for_retrace() {
    // Wait until vertical retrace starts
    while (inb(0x3DA) & 0x08);
    // Wait until vertical retrace ends
    while (!(inb(0x3DA) & 0x08));
}

void io_delay() {
    for (volatile int i = 0; i < 1000; i++);
}

void unlock_crtc_registers() {
    outb(0x3D4, 0x11);
    outb(0x3D5, 0x1F); // Standard value to unlock CRTC registers
    io_delay(); // Optional: Delay to ensure the value is set
}

void sequencer_reset() {
    // Step 1: Put Sequencer into Reset
    write_sequencer_register(0x00, 0x01); // Set Bit 0 (Reset)
    io_delay(); // Optional: Small delay to ensure the reset is registered

    // Step 2: Release Sequencer from Reset and set Clocking Mode
    write_sequencer_register(0x00, 0x03); // Clear Bit 0 and set Bit 1
    io_delay(); // Optional: Small delay
}

void InitFrameBuffer()
{
    for(int i = 0; i < screen_width * screen_height; i++)
    {
        frame_buffer[i] = 0; // Initialize all pixels to color index 0 (typically black)
    }
}

/**
 * @brief Reads all CRTC registers and stores their values into an array.
 *
 * @param crtc_values Pointer to an array of at least CRTC_REGISTER_COUNT bytes.
 * @param size The size of the crtc_values array.
 */
void ReadCRTCRegisters(uint8_t* crtc_values, int size) {
    //==============CHATGPT===============//
    // Ensure the provided array is large enough
    if (size < CRTC_REGISTER_COUNT) {
        // Handle error: Array too small
        // You can return an error code or handle it as per your requirements
        return;
    }

    // Synchronize with the vertical retrace to prevent race conditions
    wait_for_retrace();

    // Iterate through all CRTC register indices
    for (uint8_t i = 0; i < CRTC_REGISTER_COUNT; i++) {
        // Write the register index to the CRTC Index Port (0x3D4)
        outb(0x3D4, i);

        // Read the register value from the CRTC Data Port (0x3D5)
        uint8_t value = inb(0x3D5);

        // Store the value in the array
        crtc_values[i] = value;
    }
}

/*                              Port    Index   Text    Vid12   Vid13   VidX
Mode Control	                0x3C0	0x10	0x0C	0x01	0x41	0x41
Overscan Register	            0x3C0	0x11	0x00	0x00	0x00	0x00
Color Plane Enable	            0x3C0	0x12	0x0F	0x0F	0x0F	0x0F
Horizontal Panning	            0x3C0	0x13	0x08	0x00	0x00	0x00
Color Select	                0x3C0	0x14	0x00	0x00	0x00	0x00
Miscellaneous Output Register	0x3C2	N/A	    0x67	0xE3	0x63	0xE3
Clock Mode Register	            0x3C4	0x01	0x00	0x01	0x01	0x01
Character select	            0x3C4	0x03	0x00	0x00	0x00	0x00
Memory Mode Register	        0x3C4	0x04	0x07	0x02	0x0E	0x06
Mode Register	                0x3CE	0x05	0x10	0x00	0x40	0x40
Miscellaneous Register	        0x3CE	0x06	0x0E	0x05	0x05	0x05
Horizontal Total	            0x3D4	0x00	0x5F	0x5F	0x5F	0x5F
Horizontal Display Enable End	0x3D4	0x01	0x4F	0x4F	0x4F	0x4F
Horizontal Blank Start	        0x3D4	0x02	0x50	0x50	0x50	0x50
Horizontal Blank End	        0x3D4	0x03	0x82	0x82	0x82	0x82
Horizontal Retrace Start	    0x3D4	0x04	0x55	0x54	0x54	0x54
Horizontal Retrace End	        0x3D4	0x05	0x81	0x80	0x80	0x80
Vertical Total	                0x3D4	0x06	0xBF	0x0B	0xBF	0x0D
Overflow Register	            0x3D4	0x07	0x1F	0x3E	0x1F	0x3E
Preset row scan	                0x3D4	0x08	0x00	0x00	0x00	0x00
Maximum Scan Line	            0x3D4	0x09	0x4F	0x40	0x41	0x41
Vertical Retrace Start	        0x3D4	0x10	0x9C	0xEA	0x9C	0xEA
Vertical Retrace End	        0x3D4	0x11	0x8E	0x8C	0x8E	0xAC
Vertical Display Enable End	    0x3D4	0x12	0x8F	0xDF	0x8F	0xDF
Logical Width	                0x3D4	0x13	0x28	0x28	0x28	0x28
Underline Location	            0x3D4	0x14	0x1F	0x00	0x40	0x00
Vertical Blank Start	        0x3D4	0x15	0x96	0xE7	0x96	0xE7
Vertical Blank End	            0x3D4	0x16	0xB9	0x04	0xB9	0x06
Mode Control	                0x3D4	0x17	0xA3	0xE3	0xA3	0xE3
*/

void SetVideoMode() {
    // Working!
    wait_for_retrace();
    ReadCRTCRegisters(crtc_text_values, 17);
    wait_for_retrace();
    // Set Miscellaneous Output Register for Mode 13h
    outb(0x3C2, 0x63); // Standard value for Mode 13h
    // Unlock CRTC Registers: Register 0x11, bit 7 must be 0
    unlock_crtc_registers();
    // Reset Sequencer
    sequencer_reset();

    write_to_port_at_index(0x3C0, 0x10, 0x41);
    write_to_port_at_index(0x3C0, 0x11, 0x00);
    write_to_port_at_index(0x3C0, 0x12, 0x0F);
    write_to_port_at_index(0x3C0, 0x13, 0x00);
    write_to_port_at_index(0x3C0, 0x14, 0x00);

    io_delay();
    outb(0x3C2, 0x63); // misc output reg
    io_delay();

    // Sequencer Registers
    write_sequencer_register(0x00, 0x03); // Reset Register (not in reset)
    write_sequencer_register(0x01, 0x01); // Clocking Mode
    write_sequencer_register(0x02, 0x0F); // Map Mask (enable all planes)
    write_sequencer_register(0x03, 0x00); // Character Map Select
    write_sequencer_register(0x04, 0x0E); // Memory Mode

    io_delay();
    // Bring the sequencer out of reset
    write_sequencer_register(0x00, 0x03 & ~0x01); // Clear Bit 0

    // Graphics Controller Registers
    write_graphics_register(0x00, 0x00); // Set/Reset
    write_graphics_register(0x01, 0x00); // Enable Set/Reset
    write_graphics_register(0x02, 0x00); // Color Compare
    write_graphics_register(0x03, 0x00); // Data Rotate
    write_graphics_register(0x04, 0x00); // Read Map Select
    write_graphics_register(0x05, 0x40); // Graphics Mode
    write_graphics_register(0x06, 0x05); // Miscellaneous
    write_graphics_register(0x07, 0x0F); // Color Don't Care
    write_graphics_register(0x08, 0xFF); // Bit Mask

    io_delay();
    // Wait for the sequencer to finish resetting
    // Implement a small delay if necessary

    // CRTC Registers (Standard Mode 13h Values)
    uint8_t crtc_values[] = {
        0x5F, // Register 0: Horizontal Total
        0x4F, // Register 1: Horizontal Display End
        0x50, // Register 2: Start Horizontal Blank
        0x82, // Register 3: End Horizontal Blank
        0x54, // Register 4: Start Horizontal Retrace
        0x80, // Register 5: End Horizontal Retrace
        0xBF, // Register 6: Vertical Total
        0x1F, // Register 7: Overflow Register
        0x00, // Register 8: Preset Row Scan
        0x41, // Register 9: Maximum Scan Line // THIS WAS THE PROBLEM AHHHHHHHHHHH was 0x4F frick man
        0x0D, // Register 10: Cursor Start (Corrected)
        0x0E, // Register 11: Cursor End (Corrected)
        0x00, // Register 12: Start Address High
        0x00, // Register 13: Start Address Low
        0x00, // Register 14: Cursor Location High
        0x00, // Register 15: Cursor Location Low
        0x9C, // Register 16: Vertical Retrace Start
        0xA3, // Register 17: Vertical Retrace End
    };

    // Write CRTC registers
    for (int i = 0; i < sizeof(crtc_values); i++) {
        write_crtc_register(i, crtc_values[i]);
    }

    // Attribute Controller Registers
    inb(0x3DA); // Reset flip-flop
    uint8_t attr_values_graphics[16] = {
        0x00, // Attribute 0: Palette entry 0
        0x01, // Attribute 1: Palette entry 1
        0x02, // Attribute 2: Palette entry 2
        0x03, // Attribute 3: Palette entry 3
        0x04, // Attribute 4: Palette entry 4
        0x05, // Attribute 5: Palette entry 5
        0x06, // Attribute 6: Palette entry 6
        0x07, // Attribute 7: Palette entry 7
        0x08, // Attribute 8: Palette entry 8
        0x09, // Attribute 9: Palette entry 9
        0x0A, // Attribute 10: Palette entry 10
        0x0B, // Attribute 11: Palette entry 11
        0x0C, // Attribute 12: Palette entry 12
        0x0D, // Attribute 13: Palette entry 13
        0x0E, // Attribute 14: Palette entry 14
        0x41  // Attribute 15: Miscellaneous (enable video and set graphics mode)
    };
    for (uint8_t i = 0; i < 16; i++) {
        outb(0x3C0, i);                        // Select Attribute Controller register
        outb(0x3C0, attr_values_graphics[i]);  // Write data value
    }

    // Enable video output
    outb(0x3C0, 0x20);
    // InitFrameBuffer();
}

// FIXME: one of the values is no bueno
void SetTextMode() {

    // not working :(
    wait_for_retrace();
    // Set Miscellaneous Output Register for Mode 03h
    outb(0x3C2, 0x67); // Standard value for Mode 03h

    unlock_crtc_registers();
    // Reset Sequencer
    sequencer_reset();

    io_delay();

    write_to_port_at_index(0x3C0, 0x10, 0x0C);
    write_to_port_at_index(0x3C0, 0x11, 0x00);
    write_to_port_at_index(0x3C0, 0x12, 0x0F);
    write_to_port_at_index(0x3C0, 0x13, 0x08);
    write_to_port_at_index(0x3C0, 0x14, 0x00);

    io_delay();
    outb(0x3C2, 0x67); // misc output reg
    io_delay();

    // Sequencer Registers
    write_sequencer_register(0x01, 0x00); // Clocking Mode
    // write_sequencer_register(0x02, 0x0F); // Map mask
    write_sequencer_register(0x03, 0x00); // Character select
    write_sequencer_register(0x04, 0x07); // Memory Mode

    // Graphics Controller Registers for Text Mode
    // write_graphics_register(0x00, 0x00); // Set/Reset
    // write_graphics_register(0x01, 0x00); // Enable Set/Reset
    // write_graphics_register(0x02, 0x00); // Color Compare
    // write_graphics_register(0x03, 0x00); // Data Rotate
    // write_graphics_register(0x04, 0x00); // Read Map Select
    write_graphics_register(0x05, 0x10); // Graphics Mode (Text mode)
    write_graphics_register(0x06, 0x0E); // Miscellaneous
    // write_graphics_register(0x07, 0x0F); // Color Don't Care
    // write_graphics_register(0x08, 0xFF); // Bit Mask

    io_delay();

    // Standard Mode 03h CRTC settings
    uint8_t crtc_values_text[] = {
    0x4F, // Register 0: Horizontal Total
    0x50, // Register 1: Horizontal Display End
    0x82, // Register 2: Start Horizontal Blank
    0x54, // Register 3: End Horizontal Blank
    0x80, // Register 4: Start Horizontal Retrace
    0xBF, // Register 5: End Horizontal Retrace
    0x1F, // Register 6: Vertical Total
    0x1F, // Register 7: Overflow Register
    0x00, // Register 8: Preset Row Scan
    0x4F, // Register 9: Maximum Scan Line
    0x9C, // Register 10: Cursor Start
    0x8E, // Register 11: Cursor End
    0x00, // Register 12: Start Address High
    0x00, // Register 13: Start Address Low
    0x1F, // Register 14: Cursor Location High
    0x96, // Register 15: Cursor Location Low
    0xB9, // Register 16: Vertical Retrace Start
    0xA3, // Register 17: Vertical Retrace End
};


    // Write CRTC registers for text mode
    for (uint8_t i = 0; i < sizeof(crtc_values_text); i++) {
        write_crtc_register(i, crtc_text_values[i]);
    }

    // Attribute Controller Registers for Text Mode
    inb(0x3DA); // Reset flip-flop
    uint8_t attr_values_text[16] = {
        0x00, // Attribute 0: Palette entry 0
        0x01, // Attribute 1: Palette entry 1
        0x02, // Attribute 2: Palette entry 2
        0x03, // Attribute 3: Palette entry 3
        0x04, // Attribute 4: Palette entry 4
        0x05, // Attribute 5: Palette entry 5
        0x06, // Attribute 6: Palette entry 6
        0x07, // Attribute 7: Palette entry 7
        0x08, // Attribute 8: Palette entry 8
        0x09, // Attribute 9: Palette entry 9
        0x0A, // Attribute 10: Palette entry 10
        0x0B, // Attribute 11: Palette entry 11
        0x0C, // Attribute 12: Palette entry 12
        0x0D, // Attribute 13: Palette entry 13
        0x0E, // Attribute 14: Palette entry 14
        0x07  // Attribute 15: Text mode, enable blinking
    };
    for (uint8_t i = 0; i < 16; i++) {
        outb(0x3C0, i);                       // Select Attribute Controller register
        outb(0x3C0, attr_values_text[i]);     // Write data value
    }

    // Enable video output
    outb(0x3C0, 0x20);
}


// x,y (320x200, 256 colors) might be 16 colors
void SetPixel(int x, int y, VideoColor color) {
    // Check for valid coordinates
    if (x < 0 || x >= screen_width || y < 0 || y >= screen_height) {
        return;
    }
    VIDEO_MEMORY[y * screen_width + x] = color;
}

void ColorBars(int mod)
{
    for(int i = 0; i < screen_width; i++)
    {
        for(int j = 0; j < screen_height; j++)
        {
            SetPixel(i, j, i/mod);
        }
    }
}

void UpdateScreen()
{
    wait_for_retrace();
    for(int i = 0; i < screen_width; i++)
    {
        for(int j = 0; j < screen_height; j++)
        {
            int pixel = j*screen_width+i;
            SetPixel(i, j, frame_buffer[pixel]);
            frame_buffer[pixel] = 0;
        }
    }
}

void DrawPixel(int x, int y, VideoColor color)
{
    frame_buffer[y*screen_width+x] = color;
}

// Function to draw a single character
void DrawChar(int x, int y, char c, VideoColor color) {
    //=============CHATGPT-GENERATED===============//    

    // Calculate the index in the font array
    int index = c - FONT_FIRST_CHAR;

    // Iterate through each row of the character
    for (int row = 0; row < FONT_HEIGHT; row++) {
        uint8_t row_bitmap = font8x8_basic[index][row];
        for (int col = 0; col < FONT_WIDTH; col++) {
            // Check if the bit at the current column is set
            if (row_bitmap & (1 << (7 - col))) { // MSB first
                DrawPixel(x + col, y + row, color); // Draw foreground pixel
            }
            // Optionally, you can handle background pixels here if needed
            // else {
            //     DrawPixel(x + col, y + row, bg_color); // Draw background pixel
            // }
        }
    }
}

void DrawInt(int x, int y, int n, VideoColor color)
{
    int temp, l;
    l = GetIntLength(n);
    if(n < 0) { DrawChar(x, y, '-', color); n *= -1; x += FONT_WIDTH;}
    while (l > 0){
        temp = n;
        for(int i = 0; i < l - 1; i++) { temp /= 10; }
        DrawChar(x, y, (temp % 10)+'0', color);
        x += FONT_WIDTH;
        l--;
    }
}

void DrawString(int x, int y, const char* str, VideoColor color)
{
    int start_x = x;
    while(*str != '\0')
    {
        if(*str == '\n') 
        {
            y += FONT_HEIGHT * 1.25;
            x = start_x;
        }
        else
        {
            DrawChar(x, y, *str, color);
            x += FONT_WIDTH;
        }
        str++;
    }
}

void DrawBackGround(VideoColor color)
{
    for(int i = 0; i < screen_width; i++)
    {
        for(int j = 0; j < screen_height; j++)
        {
            frame_buffer[j*screen_width+i] = color;
        }
    }
}

void DrawRectangle(int x, int y, int height, int width, VideoColor color, Bool fill)
{
    // draw rectangle from center (x, y)
    int x_min = x - (width / 2);
    int x_max = x + (width / 2);
    int y_min = y - (height / 2);
    int y_max = y + (height / 2);

    if(fill)
    {
        for(int i = x_min; i < x_max; i++)
        {
            for(int j = y_min; j < y_max; j++)
            {
                DrawPixel(i, j, color);
            }
        }
    }
    else
    {
        for(int i = x_min; i < x_max; i++)
        {
            DrawPixel(i, y_min, color);
            DrawPixel(i, y_max, color);
        }

        for(int j = y_min; j < y_max; j++)
        {
            DrawPixel(x_min, j, color);
            DrawPixel(x_max, j, color);
        }
    }
}