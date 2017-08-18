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

#include "glcdPrimitives.h"

typedef struct sFont {
    uint8_t     *font;
    uint8_t     x_size;
    uint8_t     y_size;
    uint8_t     offset;
    uint16_t    numchars;
} tFont;

static tFont currentFont;
static uint8_t transparentMode = 0;
static uint16_t foregroundColor = clWHITE;
static uint16_t backgroundColor = clBLACK;
static uint16_t * currentFrameBuffer = 0;

/*
 * Function     : glcd_setFont
 * Description  : select a bitmapped font
 * Input        : const uint8_t* font
 * Returns      : void
 */
void glcd_setFont(const uint8_t* font)
{
    currentFont.font=font;
    currentFont.x_size=font[0];
    currentFont.y_size=font[1];
    currentFont.offset=font[2];
    currentFont.numchars=font[3];
}

/*
 * Function     : glcd_setFrameBuffer
 * Description  : set frame buffer for all operations
 * Input        : uint16_t * frameBuffer
 * Returns      : void
 */
void glcd_setFrameBuffer(uint16_t * frameBuffer)
{
    currentFrameBuffer = frameBuffer;
}

/*
 * Function     : glcd_setForegroundColor
 * Description  : set foreground color
 * Input        : uint16_t color
 * Returns      : void
 */
void glcd_setForegroundColor(uint16_t color)
{
    foregroundColor = color;
}

/*
 * Function     : glcd_setBackgroundColor
 * Description  : set background color
 * Input        : uint16_t color
 * Returns      : void
 */
void glcd_setBackgroundColor(uint16_t color)
{
    backgroundColor = color;
}

/*
 * Function     : glcd_fillScreen
 * Description  : fill the screen with specified color
 * Input        : uint16_t color
 * Returns      : void
 */
void glcd_fillScreen(uint16_t color)
{
    uint16_t x = 0, y = 0;
    uint16_t *fb = currentFrameBuffer;
    for (x = 0; x < GLCDROWS; x++){
        for (y = 0; y < GLCDCOLS; y++){
            *fb = color;
            fb++;
        }
    }
}

/*
 * Function     : glcd_printCharXY
 * Description  : draw a character using current font and current
 *                foreground color on screen
 * Input        : uint16_t x, uint16_t y, char c
 * Returns      : void
 */
void glcd_printCharXY(uint16_t x, uint16_t y, char c)
{
    uint16_t fbX = 0, fbY = y;
    uint16_t tempX = 0, tempY = 0;
    uint16_t bitIndex = (4 +(c - currentFont.offset)*(((uint16_t)currentFont.x_size * (uint16_t)currentFont.y_size)/8)) * 8;
    for (tempY = 0; tempY<currentFont.y_size; tempY++) {
        fbX = x;
        for (tempX = 0; tempX<currentFont.x_size; tempX++) {
            if (currentFont.font[bitIndex / 8] & (1<<(7-bitIndex % 8))) {
                // pixel font is on
                *(currentFrameBuffer + fbX + fbY*GLCDCOLS) = foregroundColor;
            } else {
                // pixel font is off
                if (!transparentMode) {
                    *(currentFrameBuffer + fbX + fbY*GLCDCOLS) = backgroundColor;
                }
            }
            fbX++;
            bitIndex++;
        }
        fbY++;
    }
}

/*
 * Function     : glcd_printCharXY_TEST
 * Description  : draw a character using current font on screen
 *                THIS IS A DEMONSTRATION FUNCTION ONLY!!!!!!!!
 *                ITS PURPOSE IS TO DEMONSTRATE SHARED OBJECTS
 *                ISSUES ON MULTI-THREAD SYSTEMS !!!!!!!
 * Input        : uint16_t x, uint16_t y, char c
 * Returns      : void
 */
void glcd_printCharXY_TEST(uint16_t x, uint16_t y, char c)
{
    uint16_t fbX = 0, fbY = y;
    static uint16_t tempX = 0, tempY = 0;
    uint16_t bitIndex = (4 +(c - currentFont.offset)*(((uint16_t)currentFont.x_size * (uint16_t)currentFont.y_size)/8)) * 8;
    for (tempY = 0; tempY<currentFont.y_size; tempY++) {
        fbX = x;
        for (tempX = 0; tempX<currentFont.x_size; tempX++) {
            if (currentFont.font[bitIndex / 8] & (1<<(7-bitIndex % 8))) {
                // pixel font is on
                *(currentFrameBuffer + fbX + fbY*GLCDCOLS) = foregroundColor;
            } else {
                // pixel font is off
                if (!transparentMode) {
                    *(currentFrameBuffer + fbX + fbY*GLCDCOLS) = backgroundColor;
                }
            }
            fbX++;
            bitIndex++;
        }
        fbY++;
    }
}

/*
 * Function     : glcd_printStringXY
 * Description  : draw a string using current font and current
 *                foreground color on screen
 * Input        : uint16_t x, uint16_t y, char *string
 * Returns      : void
 */
void glcd_printStringXY(uint16_t x, uint16_t y, char *string)
{
    uint16_t index = 0;
    while (*string) {
        glcd_printCharXY(x + (index*(currentFont.x_size)), y, *string++);
        index++;
    }
}

/*
 * Function     : glcd_plotPixel
 * Description  : plot a pixel of specified color
 * Input        : uint16_t x, uint16_t y, uint16_t color
 * Returns      : void
 */
void glcd_plotPixel(uint16_t x, uint16_t y, uint16_t color)
{
    *(currentFrameBuffer + x + y*GLCDCOLS) = color;
}

/*
 * Function     : glcd_plotLine
 * Description  : plot a line of specified color
 * Input        : uint16_t x1, uint16_t y1 - line's initial point
 *                uint16_t x2, uint16_t y2 - line's final point
 *                uint16_t color
 * Returns      : void
 */
void glcd_plotLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    int16_t addx, addy, dx, dy, error;
    uint16_t x,y,i;
    // calculate the distance module
    if (x2>=x1) dx= x2-x1; else dx = x1-x2;
    if (y2>=y1) dy = y2-y1; else dy = y1-y2;
    // initial line point
    x = x1;
    y = y1;
    if (x1 > x2) addx = -1; else addx = 1;
    if (y1 > y2) addy = -1; else addy = 1;
    if (!dx) {
        // vertical line
        if (addy>0) {
            for( i=y1; i <= y2; i++ ) glcd_plotPixel(x, i, color);
        }  else {
            for( i=y2; i <= y1; i++ ) glcd_plotPixel(x, i, color);
        }
    } else if (!dy) {
        // horizontal line
        if (addx>0) {
            for( i=x1; i <= x2; i++ ) glcd_plotPixel(i, y, color);
        } else {
            for( i=x2; i <= x1; i++ ) glcd_plotPixel(i, y, color);
        }
    } else {
        if (dx >= dy) {
            // calculate error factor
            error = (dy*2) - dx;
            for(i=0; i<=dx; ++i) {
                glcd_plotPixel(x, y, color);
                if(error < 0) {
                    error += dy * 2;
                    x += addx;
                } else {
                    error += (dy - dx) * 2;
                    x += addx;
                    y += addy;
                }
            }
        } else {
            // calculate error factor
            error = (dx*2) - dy;
            for (i=0; i<=dy; ++i) {
                glcd_plotPixel(x, y, color);
                if (error < 0) {
                    error += dx*2;
                    y += addy;
                } else {
                    error += (dx - dy) * 2;
                    x += addx;
                    y += addy;
                }
            }
        }
    }
}

/*
 * Function     : glcd_plotRect
 * Description  : plot a rectangle of specified color
 * Input        : uint16_t x1, uint16_t y1 - rectangle's top right point
 *                uint16_t x2, uint16_t y2 - rectangle's bottom left point
 *                uint16_t color
 * Returns      : void
 */
void glcd_plotRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
  glcd_plotLine(x1,y1,x2,y1,color);
  glcd_plotLine(x2,y1,x2,y2,color);
  glcd_plotLine(x2,y2,x1,y2,color);
  glcd_plotLine(x1,y2,x1,y1,color);
}

/*
 * Function     : glcd_fillRect
 * Description  : plot a filled rectangle of specified color
 * Input        : uint16_t x1, uint16_t y1 - rectangle's top right point
 *                uint16_t x2, uint16_t y2 - rectangle's bottom left point
 *                uint16_t color
 * Returns      : void
 */
void glcd_fillRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
  uint16_t lines;
  if (y1<y2){
    for (lines=y1;lines<=y2;lines++) glcd_plotLine(x1,lines,x2,lines,color);
  } else {
    for (lines=y2;lines<=y1;lines++) glcd_plotLine(x1,lines,x2,lines,color);
  }
}

/*
 * Function     : glcd_plotCircle
 * Description  : plot a circle of specified color and radius
 * Input        : uint16_t x, uint16_t y - circle's center point
 *                uint16_t radius - circle's radius in pixels
 *                uint16_t color
 * Returns      : void
 */
void glcd_plotCircle(uint16_t x, uint16_t y, int16_t radius, uint16_t color)
{
    int16_t f = 1 - radius;
    uint16_t ddF_x = 1;
    uint16_t ddF_y = -2 * radius;
    uint16_t x1 = 0;
    uint16_t y1 = radius;

    glcd_plotPixel(x,y+radius,color);
    glcd_plotPixel(x,y-radius,color);
    glcd_plotPixel(x+radius,y,color);
    glcd_plotPixel(x-radius,y,color);
    while(x1 < y1) {
        if(f >= 0) {
            y1--;
            ddF_y += 2;
            f += ddF_y;
        }
        x1++;
        ddF_x += 2;
        f += ddF_x;
        glcd_plotPixel(x+x1,y+y1,color);
        glcd_plotPixel(x-x1,y+y1,color);
        glcd_plotPixel(x+x1,y-y1,color);
        glcd_plotPixel(x-x1,y-y1,color);
        glcd_plotPixel(x+y1,y+x1,color);
        glcd_plotPixel(x-y1,y+x1,color);
        glcd_plotPixel(x+y1,y-x1,color);
        glcd_plotPixel(x-y1,y-x1,color);
    }
}

/*
 * Function     : glcd_fillCircle
 * Description  : plot a filled circle of specified color and radius
 * Input        : uint16_t x, uint16_t y - circle's center point
 *                uint16_t radius - circle's radius in pixels
 *                uint16_t color
 * Returns      : void
 */
void glcd_fillCircle(uint16_t x, uint16_t y, int16_t radius, uint16_t color)
{
    int16_t x1,y1;
    for (y1=-radius; y1<=0; y1++) {
        for (x1=-radius; x1<=0; x1++) {
            if(x1*x1+y1*y1 <= radius*radius) {
                glcd_plotLine(x+x1, y+y1, x+x1+2*(-x1)-1,y+y1, color);
                glcd_plotLine(x+x1, y-y1, x+x1+2*(-x1)-1,y-y1, color);
                break;
            }
        }
    }
}
