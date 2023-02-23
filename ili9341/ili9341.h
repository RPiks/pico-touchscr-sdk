///////////////////////////////////////////////////////////////////////////////
//
//  Roman Piksaykin [piksaykin@gmail.com], R2BDY
//  https://www.qrz.com/db/r2bdy
//
///////////////////////////////////////////////////////////////////////////////
//
//
//  ili9341.h - ILI9341-based display library (MSP2807 etc).
// 
//
//  DESCRIPTION
//
//      The library provides a memory efficient set of functions
//  to work with ILI9341-based display 320x240 pixels.
//
//      It implements screen buffer which consists of two planes : graphics
//  and color. The size of the buffer is 10800 bytes. Most of text, menus
//  and technoir graphics look quite the same as original mode which 
//  consumes 320*240*2 = 153600 bytes. The formula for proposed mode is:
//  320*240/8 + (320/8)*(240/8) = 10800.
//  ^-B/W plane 1bpp; ^^^^^^^^^-color plane as a set of boxes of 8x8 pixels.
//
//      The screen is being updated selectively using `change detection` algo.
//
//      It implements boxy color plane which mimics Zx-Spectrum screen memory
//  structure.
//
//      8x8 bitmap font is being used.
//
//  PLATFORM
//      Raspberry Pi pico.
//
//  REVISION HISTORY
// 
//      Rev 0.99   05 Jan 2023
//  Initial release.
//
//  LICENCE
//      MIT License (http://www.opensource.org/licenses/mit-license.php)
//
//  Copyright (c) 2023 by Roman Piksaykin
//  
//  Permission is hereby granted, free of charge,to any person obtaining a copy
//  of this software and associated documentation files (the Software), to deal
//  in the Software without restriction,including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY,WHETHER IN AN ACTION OF CONTRACT,TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.
///////////////////////////////////////////////////////////////////////////////
#ifndef _ILI9341_H
#define _ILI9341_H

#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

#include "../lib/assert.h"

#include "ili9341hw.h"
#include "font_8x8.h"

#define GET_DATA_BIT(p, n)  ((*((uint32_t *)(p) + ((n) >> 5)) \
        >> (31 - ((n) & 31))) & 1)
#define SET_DATA_BIT(p, n)  (*((uint32_t *)(p) + ((n) >> 5)) \
        |= (0x80000000 >> ((n) & 31)))
#define CLR_DATA_BIT(p, n)  (*((uint32_t *)(p) + ((n) >> 5)) \
        &=~(0x80000000 >> ((n) & 31)))

typedef enum 
{
    kBlack,
    kBlue,
    kRed,
    kMagenta,
    kGreen,
    kCyan,
    kYellow,
    kWhite
} color_t;

typedef struct 
{
    spi_inst_t *mpSPIPort;

    int mGPIO_miso;
    int mGPIO_cs;
    int mGPIO_sck;
    int mGPIO_mosi;
    int mGPIO_reset;
    int mGPIO_dc;

} ili9341_config_t;

typedef struct
{
    ili9341_config_t *mpHWConfig;           // Device hardware config.

    int16_t mCursorX;                       // Cursor-
    int16_t mCursorY;                       // position.
    uint8_t mCursorType;                    // Not yet implemented. [*]

    color_t mCanvasPaper;                   // Default- 
    color_t mCanvasInk;                     // canvas colors.

    uint32_t mpPixBuffer[PIX_W32COUNT];     // Black-white 1bpp canvas.

    uint8_t mpColorBuffer[TEXT_CHARCOUNT];  // 8x8 block attributes:
                                // Flash|Changed|Pap2|Pap1|Pap0|Ink2|Ink1|Ink0.
                                // `Flash' blinking attribute (cursors) [*].
                                // `Changed` need to send to device flag.
                                // `Paper` color, `Ink` color [0..7].
} screen_control_t;

/* Hardware I/O low level operations. */
static inline void ILI9341_CS_Set(const ili9341_config_t *pconfig, int state);

void ILI9341_Init(ili9341_config_t *pconfig, spi_inst_t *pspi_port, 
                    int spi_clock_freq, int gpio_MISO, int gpio_CS, 
                    int gpio_SCK, int gpio_MOSI, int gpio_RS, int gpio_DC);

void ILI9341_SetCommand(const ili9341_config_t *pconfig, uint8_t cmd);
void ILI9341_CommandParam(const ili9341_config_t *pconfig,uint8_t data);

void ILI9341_SetOutWriting(const ili9341_config_t *pconfig,
                            const int start_col, const int end_col,
                            const int start_page,const int end_page);

void ILI9341_WriteData(const ili9341_config_t *pconfig,void *buffer,int bytes);


/* Screen buffer operations - text &. */
void TftClearScreenBuffer(screen_control_t *pscr, color_t paper, color_t ink);
void TftSetCursor(screen_control_t *pscr, int x, int y);

void TftPutChar(screen_control_t *pscr, int x, int y, int paper, int ink, 
                char chr);

void TftPutColorAttr(screen_control_t *pscr, int x, int y, int paper, int ink);

void TftPutString(screen_control_t *pscr, const char* str, int top_y, 
                int bot_y, int paper, int ink);

void TftPrintf(screen_control_t *pscr, int top_y, int bot_y, int paper,
                int ink, const char* str, ...);

void TftScrollVerticalZone(screen_control_t *pscr, int top_y, int bot_y);

/* Screen buffer operations - graphics. */
void TftPutPixel(screen_control_t *pscr, int x, int y, color_t paper,color_t ink);
void TftPutLine(screen_control_t *pscr, int x0, int y0, int x1, int y1);
void TftPutTextLabel(screen_control_t *pscr, const char *pstr, int x_pix, 
                    int y_pix, bool over);
void TftClearRect8(screen_control_t *pscr, int x, int y);

/* Hardware I/O operations. */
void TftFullScreenWrite(screen_control_t *pscr);
int TftFullScreenSelectiveWrite(screen_control_t *pscr, int nblock_max);
void TftSymbolWrite(screen_control_t *pscr, int sym_x, int sym_y);

static const uint16_t spPalette[8] = 
{
    0x0000, // Black.
    0x1F00, // Blue.
    0x00F8, // Red.
    0x1FF8, // Magenta.
    0xE007, // Green 0,0xFF,0.
    0xFF07, // Cyan 0xFF,0xFF,0.
    0xE0FF, // Yellow 0xFF,0xFF,0.
    0xffff  // White.
};

#endif
