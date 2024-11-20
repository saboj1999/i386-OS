#ifndef OS_INCLUDE_GRAPHICS_H
#define OS_INCLUDE_GRAPHICS_H
#include <stdint.h>
#include "bool.h"
#include "vga_video_color.h"

void InitFrameBuffer();
void ColorBars(int mod);

void SetVideoMode();
void SetTextMode();
void SetPixel(int x, int y, VideoColor color);
void UpdateScreen();

void DrawPixel(int x, int y, VideoColor color);
void DrawChar(int x, int y, char c, VideoColor color);
void DrawInt(int x, int y, int n, VideoColor color);
void DrawString(int x, int y, const char* str, VideoColor color);
void DrawBackGround(VideoColor color);
void DrawRectangle(int x, int y, int height, int width, VideoColor color, Bool fill);

#endif