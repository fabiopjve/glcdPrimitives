/*
 * glcdPrimitives.c
 *
 *  Created on  : July, 14th, 2017
 *  Author      : Fabio Pereira
 *  Description :
 *      This is a library with common functions for drawing
 *  on a color graphic LCD. It is completely hardware inde-
 *  pendent as all functions are performed on a framebuffer
 *      All functions are designed for RGB565 16-bit color
 */

#ifndef GLCDPRIMITIVES_H_
#define GLCDPRIMITIVES_H_

#include <stdint.h>
#include "fonts.h"

#define GLCDROWS 320u
#define GLCDCOLS 256u

/*
 * RGB565 Colors
 */
#define clBLACK     0x0000
#define clBLUE      0x001F
#define clRED       0xF800
#define clGREEN     0x07E0
#define clCYAN      0x07FF
#define clMAGENTA   0xF81F
#define clYELLOW    0xFFE0
#define clWHITE     0xFFFF

/*
 * Prototypes
 */
void glcd_setFont(const uint8_t* font);
void glcd_setFrameBuffer(uint16_t * frameBuffer);
void glcd_setForegroundColor(uint16_t color);
void glcd_setBackgroundColor(uint16_t color);
void glcd_fillScreen(uint16_t color);
void glcd_printCharXY(uint16_t x, uint16_t y, char c);
void glcd_printCharXY_TEST(uint16_t x, uint16_t y, char c);
void glcd_printStringXY(uint16_t x, uint16_t y, char *string);
void glcd_plotPixel(uint16_t x, uint16_t y, uint16_t color);
void glcd_plotLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void glcd_plotRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void glcd_fillRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void glcd_plotCircle(uint16_t x, uint16_t y, int16_t radius, uint16_t color);
void glcd_fillCircle(uint16_t x, uint16_t y, int16_t radius, uint16_t color);
#endif /* GLCDPRIMITIVES_H_ */
