///////////////////////////////////////////////////////////////////////////////
//
//  Roman Piksaykin [piksaykin@gmail.com], R2BDY
//  https://www.qrz.com/db/r2bdy
//
///////////////////////////////////////////////////////////////////////////////
//
//
//  ili9341.c - ILI9341-based display library (MSP2807 etc).
// 
//
//  DESCRIPTION
//
//      The library provides a memory efficient set of functions to work with
//  ILI9341-based display 320x240 pixels.
//
//      It implements screen buffer which consists of two planes : graphics and
//  color. The size of the buffer is 10800 bytes. Most of text, menus and
//  technoir graphics look quite the same as original mode which consumes 
//  320*240*2 = 153600 bytes.
//      The formula for proposed mode is:
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
#include "ili9341.h"

static inline void ILI9341_CS_Set(const ili9341_config_t *pconfig, int state) 
{
    asm volatile("nop \n nop \n nop");
    gpio_put(pconfig->mGPIO_cs, state);
    asm volatile("nop \n nop \n nop");
}

void ILI9341_SetCommand(const ili9341_config_t *pconfig, uint8_t cmd) 
{
    ILI9341_CS_Set(pconfig, CS_ENABLE);
    gpio_put(pconfig->mGPIO_dc, 0);
    asm volatile("nop \n nop \n nop");
    spi_write_blocking(pconfig->mpSPIPort, &cmd, 1);
    gpio_put(pconfig->mGPIO_dc, 1);
    ILI9341_CS_Set(pconfig, CS_DISABLE);
}

void ILI9341_CommandParam(const ili9341_config_t *pconfig, uint8_t data) 
{
    ILI9341_CS_Set(pconfig, CS_ENABLE);
    spi_write_blocking(pconfig->mpSPIPort, &data, 1);
    ILI9341_CS_Set(pconfig, CS_DISABLE);
}

/// @brief Starts HW writing session of arbitrary area of screen.
/// @param pconfig Control structure.
/// @param start_col Start column of the area.
/// @param end_col Finish column of the area.
/// @param start_page Start row of the area.
/// @param end_page Finish row of the area.
void ILI9341_SetOutWriting(const ili9341_config_t *pconfig,
                            const int start_col, const int end_col,
                            const int start_page,const int end_page)
{
    assert_(pconfig);
    assert_(start_col != end_col);
    assert_(start_page != end_page);

    // Column address set.
    ILI9341_SetCommand(pconfig, ILI9341_CASET);
    
    ILI9341_CommandParam(pconfig, (start_col >> 8) & 0xFF);
    ILI9341_CommandParam(pconfig, start_col & 0xFF);

    ILI9341_CommandParam(pconfig, (end_col >> 8) & 0xFF);
    ILI9341_CommandParam(pconfig, end_col & 0xFF);

    // Page address set.
    ILI9341_SetCommand(pconfig, ILI9341_PASET);

    ILI9341_CommandParam(pconfig, (start_page >> 8) & 0xFF);
    ILI9341_CommandParam(pconfig, start_page & 0xFF);

    ILI9341_CommandParam(pconfig, (end_page >> 8) & 0xFF);
    ILI9341_CommandParam(pconfig, end_page & 0xFF);

    // Start writing.
    ILI9341_SetCommand(pconfig, ILI9341_RAMWR);

    // ...Need further writing in accordance with volume has set.
}

/// @brief Writes a data buffer to display.
/// @param pconfig Control structure of display.
/// @param buffer Data buffer.
/// @param bytes Size of the buffer in bytes.
void ILI9341_WriteData(const ili9341_config_t *pconfig, void *buffer,int bytes)
{
    ILI9341_CS_Set(pconfig, CS_ENABLE);
    spi_write_blocking(pconfig->mpSPIPort, buffer, bytes);
    ILI9341_CS_Set(pconfig, CS_DISABLE);
}

/// @brief Inits pre-allocated control structure.
/// @param pconfig Control structure for init.
/// @param pspi_port SPI port of device.
/// @param spi_clock_freq SPI clock freq (SCK pin).
/// @param gpio_MISO Master input slave output pin.
/// @param gpio_CS Chis select pin, active low.
/// @param gpio_SCK Serial clock pin.
/// @param gpio_MOSI Master output slave input pin.
/// @param gpio_RS Reset pin, active low.
/// @param gpio_DC Data/command switch pin.
void ILI9341_Init(ili9341_config_t *pconfig, spi_inst_t *pspi_port, 
                    int spi_clock_freq, int gpio_MISO, int gpio_CS, 
                    int gpio_SCK, int gpio_MOSI, int gpio_RS, int gpio_DC)
{
    assert_(pconfig);
    assert_(pspi_port);
    assert_(spi_clock_freq);

    pconfig->mpSPIPort = pspi_port;
    pconfig->mGPIO_miso = gpio_MISO;
    pconfig->mGPIO_cs = gpio_CS;
    pconfig->mGPIO_sck = gpio_SCK;
    pconfig->mGPIO_mosi = gpio_MOSI;
    pconfig->mGPIO_reset = gpio_RS;
    pconfig->mGPIO_dc = gpio_DC;

    spi_init(pconfig->mpSPIPort, spi_clock_freq);
    const int baudrate = spi_set_baudrate(pconfig->mpSPIPort, spi_clock_freq);

    gpio_set_function(pconfig->mGPIO_miso, GPIO_FUNC_SPI);
    gpio_set_function(pconfig->mGPIO_sck, GPIO_FUNC_SPI);
    gpio_set_function(pconfig->mGPIO_mosi, GPIO_FUNC_SPI);

    gpio_init(pconfig->mGPIO_cs);
    gpio_set_dir(pconfig->mGPIO_cs, GPIO_OUT);
    gpio_put(pconfig->mGPIO_cs, 1);

    gpio_init(pconfig->mGPIO_reset);
    gpio_set_dir(pconfig->mGPIO_reset, GPIO_OUT);
    gpio_put(pconfig->mGPIO_reset, 1);

    gpio_init(pconfig->mGPIO_dc);
    gpio_set_dir(pconfig->mGPIO_dc, GPIO_OUT);
    gpio_put(pconfig->mGPIO_dc, 0);

    sleep_ms(10);
    gpio_put(pconfig->mGPIO_reset, 0);
    sleep_ms(10);
    gpio_put(pconfig->mGPIO_reset, 1);

    ILI9341_SetCommand(pconfig, 0x01);  // Do reset.
    sleep_ms(100);

    ILI9341_SetCommand(pconfig, ILI9341_GAMMASET);
    ILI9341_CommandParam(pconfig, 0x01);

    // positive gamma correction.
    ILI9341_SetCommand(pconfig, ILI9341_GMCTRP1);
    ILI9341_WriteData(pconfig, (uint8_t[15])
        { 0x0f, 0x31, 0x2b, 0x0c, 0x0e, 0x08, 0x4e, 0xf1, 
          0x37, 0x07, 0x10, 0x03, 0x0e, 0x09, 0x00 
        }, 15);

    // negative gamma correction.
    ILI9341_SetCommand(pconfig, ILI9341_GMCTRN1);
    ILI9341_WriteData(pconfig, (uint8_t[15])
        { 0x00, 0x0e, 0x14, 0x03, 0x11, 0x07, 0x31, 0xc1, 
          0x48, 0x08, 0x0f, 0x0c, 0x31, 0x36, 0x0f 
        }, 15);

    ILI9341_SetCommand(pconfig, ILI9341_MADCTL);
    ILI9341_CommandParam(pconfig, 0x48);
   
    ILI9341_SetCommand(pconfig, ILI9341_PIXFMT);
    ILI9341_CommandParam(pconfig, 0x55);    // 16-bit pixel format.

    ILI9341_SetCommand(pconfig, ILI9341_FRMCTR1);
    ILI9341_CommandParam(pconfig, 0x00);
    ILI9341_CommandParam(pconfig, 0x1B);

    // Set display on.
    ILI9341_SetCommand(pconfig, ILI9341_SLPOUT);
    ILI9341_SetCommand(pconfig, ILI9341_DISPON);
}

/// @brief Clears screen buffer.
/// @param pscr Control structure.
/// @param paper Paper color, [0..7].
/// @param ink Ink color, [0..7].
void TftClearScreenBuffer(screen_control_t *pscr, color_t paper, color_t ink)
{
    assert_(pscr);
    assert_(pscr->mpPixBuffer);
    assert_(pscr->mpColorBuffer);

    memset(pscr->mpPixBuffer, 0, sizeof(pscr->mpPixBuffer));

    uint8_t attr_val;
    if(paper < 0)               // Special case: default canvas.
    {
        attr_val = pscr->mCanvasInk | (pscr->mCanvasPaper << 3)
                 | (1 << 6);    // Update flag.
    }
    else
    {
        attr_val = ink | (paper << 3) | (1 << 6);
    }

    memset(pscr->mpColorBuffer, attr_val, sizeof(pscr->mpColorBuffer));

    pscr->mCursorX = pscr->mCursorY = 0;
}

/// @brief Writes screen buffer to device rapidly in one transaction.
/// @param pscr Control structure.
void TftFullScreenWrite(screen_control_t *pscr)
{
    assert_(pscr);
    assert_(pscr->mpHWConfig);
    assert_(pscr->mpPixBuffer);
    assert_(pscr->mpColorBuffer);

    ILI9341_SetOutWriting(pscr->mpHWConfig, 0, PIX_WIDTH-1, 0, PIX_HEIGHT-1);

    uint16_t sBufLine[PIX_WIDTH];
    for(int j = 0; j < PIX_HEIGHT; ++j)
    {
        const int line_of_symbol = (j >> 3) * TEXT_WIDTH;
        for(int i = 0; i < PIX_WIDTH; ++i)
        {
            const int ix_of_symbol = line_of_symbol + (i >> 3);
            const int paper = (pscr->mpColorBuffer[ix_of_symbol] >> 3) & 0b111;
            const int ink = pscr->mpColorBuffer[ix_of_symbol] & 0b111;

            sBufLine[i] = GET_DATA_BIT(pscr->mpPixBuffer, j * PIX_WIDTH + i) 
                        ? spPalette[ink]
                        : spPalette[paper];

            pscr->mpColorBuffer[ix_of_symbol] &= ~(1<<6);
        }

        ILI9341_WriteData(pscr->mpHWConfig, sBufLine, 
                            PIX_WIDTH * sizeof(uint16_t));
    }
}

/// @brief Looks for blocks awaiting update and writes max N of them to device.
/// @param pscr Control structure.
/// @param nblock_max Max. count of blocks for writing.
/// @return 0 - perhaps there are still pending blocks.
/// @return 1 - no pending blocks.
int TftFullScreenSelectiveWrite(screen_control_t *pscr, int nblock_max)
{
    assert_(pscr);
    assert_(pscr->mpHWConfig);

    // Look for blocks awaiting for update.
    for(int j = 0; j < TEXT_HEIGHT; ++j)
    {
        const int line = j * TEXT_WIDTH;
        for(int i = 0; i < TEXT_WIDTH; ++i)
        {
            if((pscr->mpColorBuffer[line + i] >> 6) & 1)
            {
                TftSymbolWrite(pscr, i, j);
                if(!--nblock_max)
                {
                    return 0;
                }
            }
        }
    }

    return 1;
}

/// @brief Writes a color symbol to screen. Doesn't look at `need update` bit.
/// @param pscr Control structure.
/// @param sym_x Symbol x coord, 0...TEXT_WIDTH-1
/// @param sym_y Symbol y coord, 0...TEXT_HEIGHT-1
void TftSymbolWrite(screen_control_t *pscr, int sym_x, int sym_y)
{
    const int pix_tl_x = sym_x << 3;
    const int pix_tl_y = sym_y << 3;
    
    ILI9341_SetOutWriting(pscr->mpHWConfig, pix_tl_x, pix_tl_x + 7, pix_tl_y,
                            pix_tl_y + 7);

    ILI9341_CS_Set(pscr->mpHWConfig, CS_ENABLE);
    
    uint8_t *psym_box = pscr->mpColorBuffer + sym_y * TEXT_WIDTH + sym_x;
    const int paper = (*psym_box >> 3) & 0b111;
    const int ink = *psym_box & 0b111;

    static uint16_t sBuf[8];
    for(int j = 0; j < 8; ++j)
    {
        const int txt_line = j * TEXT_WIDTH;
        const int pix_line = (pix_tl_y + j) * PIX_WIDTH + pix_tl_x;
        for(int i = 0; i < 8; ++i)
        {
            sBuf[i] = GET_DATA_BIT(pscr->mpPixBuffer, pix_line + i)
                    ? spPalette[ink]
                    : spPalette[paper];
        }
        spi_write_blocking(pscr->mpHWConfig->mpSPIPort, (uint8_t *)sBuf, 
                            8 * sizeof(uint16_t));
    }

    ILI9341_CS_Set(pscr->mpHWConfig, CS_DISABLE);
    
    *psym_box &= ~(1<<6);  // Clear 'need update' bit.
}

/// @brief Sets cursor on the desired position.
/// @param pscr Control structure
/// @param x X coord, 0...TEXT_WIDTH-1
/// @param y Y coord, 0...TEXT_HEIGHT-1
void TftSetCursor(screen_control_t *pscr, int x, int y)
{
    assert_(pscr);

    pscr->mCursorX = x;
    pscr->mCursorY = y;
}

/// @brief Draws a char onto the screen buffer.
/// @param pscr Control structure.
/// @param x X coord, 0...TEXT_WIDTH-1.
/// @param y Y coord, 0...TEXT_HEIGHT-1.
/// @param paper Paper color, 0..7.
/// @param ink Ink color, 0..7.
/// @param chr Char for draw.
void TftPutChar(screen_control_t *pscr, int x, int y, int paper, int ink,
                char chr)
{
    if(chr > 0x7E || chr < 0x20) 
    {
        return; // chr > '~'
    }

    chr -= 0x20;

    const int x_pix = x<<3;
    const int y_pix = y<<3;

    const uint8_t *pfchar = kFONT_ + 2 + 8 * (int)chr;
    for(int j = 0; j < 8; ++j)
    {
        const int bit_line = (y_pix + j) * PIX_WIDTH + x_pix;
        for(int i = 0; i < 8; ++i)
        {
            const bool bit_val = (pfchar[j] >> (8-i)) & 1;
            bit_val ? SET_DATA_BIT(pscr->mpPixBuffer, bit_line + i)
                    : CLR_DATA_BIT(pscr->mpPixBuffer, bit_line + i);
        }
    }

    uint8_t *pbox = pscr->mpColorBuffer + x + TEXT_WIDTH * y;

    *pbox &= 0b11000000;    // clear color attrs.
    *pbox |= ink & 0b111;
    *pbox |= (paper & 0b111) << 3;   
    *pbox |= (1<<6);        // Set for update.
}

/// @brief Puts the null-terminated string onto the screen buffer.
/// @param pscr Control structure.
/// @param str Null-terminated string.
/// @param top_y Top symbol Y of scroll area.
/// @param bot_y Top symbol X of scroll area.
/// @param paper Paper color, [0..7].
/// @param ink Ink color, [0..7].
void TftPutString(screen_control_t *pscr, const char* str, 
                    int top_y, int bot_y, int paper, int ink)
{
    assert_(pscr);
    assert_(str);
    assert_(bot_y);

    for(int i = 0; str[i]; ++i)
    {
        if(pscr->mCursorY >= bot_y)
        {
            TftScrollVerticalZone(pscr, top_y, bot_y);
            --pscr->mCursorY;
        }

        const char ch = str[i];
        if('\n' == ch || '\r' == ch)
        {
            pscr->mCursorX = 0;
            ++pscr->mCursorY;
        }
        else
        {
            TftPutChar(pscr, pscr->mCursorX, pscr->mCursorY, paper, ink, ch);
            if(++pscr->mCursorX >= TEXT_WIDTH)
            {
                pscr->mCursorX = 0;
                ++pscr->mCursorY;
            }
        }
    }
}

/// @brief Prints the formatted string onto the screen buffer.
/// @param pscr Control structure.
/// @param top_y Top symbol Y of scroll area.
/// @param bot_y Bottom symbol Y of scroll area.
/// @param paper Paper color.
/// @param ink Ink color.
/// @param str Formatted string.
void TftPrintf(screen_control_t *pscr, int top_y, int bot_y, int paper,
                int ink, const char* str, ...)
{
    assert_(pscr);

    va_list argptr;
    va_start(argptr, str);
    
    char buf[TEXT_WIDTH * 2];           // 2 rows of text max.
    vsnprintf(buf, sizeof(buf), str, argptr);
    va_end(argptr);

    TftPutString(pscr, buf, top_y, bot_y, paper, ink);
}

/// @brief Changes symbol's color attrs & sets the symbol to update.
/// @param pscr Control structure.
/// @param x X coord, 0...TEXT_WIDTH-1.
/// @param y Y coord, 0...TEXT_HEIGHT-1.
/// @param paper Paper color.
/// @param ink Ink color.
void TftPutColorAttr(screen_control_t *pscr, int x, int y, int paper, int ink)
{
    uint8_t *pbox = pscr->mpColorBuffer + x + TEXT_WIDTH * y;
    
    *pbox &= 0b11000000;    // clear attrs.
    *pbox |= ink & 0b111;
    *pbox |= (paper & 0b111) << 3;
    *pbox |= 1 << 6;        // Set for update.
}

/// @brief Scrolls the screen area of [top_y...bot_y] for 1 symbol (8 pixels)
/// @brief higher.
/// @param pscr Control structure.
/// @param top_y The top of the scroll area.
/// @param bot_y The bottom of the scroll area, [0..TEXT_HEIGHT].
void TftScrollVerticalZone(screen_control_t *pscr, int top_y, int bot_y)
{
    assert_(pscr);
    assert_(top_y <= bot_y);

    const int kscroll_area_height = bot_y - top_y;
    const int kscroll_area_bytes = ((kscroll_area_height - 1)<<3) 
                                 * PIX_BYTE_STRIDE;
    
    /* Move the zone btw top_y & bot_y higher. */
    uint8_t *ppixdest = (uint8_t *)pscr->mpPixBuffer + top_y * TEXT_WIDTH * 8;
    const uint8_t *ppixsrc = ppixdest + PIX_BYTE_STRIDE * 8;
    memmove(ppixdest, ppixsrc, kscroll_area_bytes);

    // Clear the bottom symbols' line.
    memset(ppixdest + kscroll_area_bytes, 0, 8 * PIX_BYTE_STRIDE);

    /* Move attrs. */
    const int kscroll_attr_bytes = kscroll_area_height * TEXT_WIDTH;
    uint8_t *pattrdest = pscr->mpColorBuffer + top_y * TEXT_WIDTH;
    const uint8_t *pattrsrc = pattrdest + TEXT_WIDTH;
    memmove(pattrdest, pattrsrc, kscroll_attr_bytes);

    /* Set the updated zone as `changed`. */
    for(int j = top_y * TEXT_WIDTH; j < bot_y * TEXT_WIDTH; ++j)
    {
        pscr->mpColorBuffer[j] |= 1 << 6;
    }
}

/// @brief Puts a pixel on screen buf & sets the element of color plane
/// @brief for update.
/// @param pscr Control structure.
/// @param x Pix's x coord.
/// @param y Pix's y coord.
/// @param paper Paper color.
/// @param ink Ink color.
void TftPutPixel(screen_control_t *pscr, int x, int y, color_t paper, color_t ink)
{
    assert_(pscr);
    
    if(x < 0 || y < 0 || x > PIX_WIDTH - 1 || y > PIX_HEIGHT - 1)
    {
        return;
    }

    SET_DATA_BIT(pscr->mpPixBuffer, x + y * PIX_WIDTH);
    
    TftPutColorAttr(pscr, x>>3, y>>3, paper, ink);
}

/// @brief Puts a line on screen buffer & sets appropriate zones to update.
/// @param pscr Control structure.
/// @param x0 Line begin (end) X coord.
/// @param y0 Line begin (end) Y coord.
/// @param x1 Line end (begin) X coord.
/// @param y1 Line end (begin) Y coord.
void TftPutLine(screen_control_t *pscr, int x0, int y0, int x1, int y1)
{
    assert_(pscr);

    if (x0 < 0 || y0 < 0 || x1 < 0 || y1 < 0) 
    {
        return;
    }
    if (x0 > PIX_WIDTH || y0 > PIX_HEIGHT || x1 > PIX_WIDTH || y1 > PIX_HEIGHT)
    {
        return;
    }

    const int sx = x0 < x1 ? 1 : -1; 
    const int sy = y0 < y1 ? 1 : -1;

    int dx = x1 > x0 ? x1 - x0 : x0 - x1,
        dy = y1 > y0 ? y1 - y0 : y0 - y1,
        err = (dx > dy ? dx : -dy) / 2,
        e2, 
        cnt = 1000L;

    for (; cnt; --cnt) 
    {
        SET_DATA_BIT(pscr->mpPixBuffer, x0 + y0 * PIX_WIDTH);
        pscr->mpColorBuffer[(x0 >> 3) + (y0 >> 3) * TEXT_WIDTH] |= 1 << 6;

        if (x0 == x1 && y0 == y1) 
        {
            break;
        }

        e2 = err;

        if (e2 > -dx) 
        {
            err -= dy;
            x0 += sx;
        }

        if (e2 < dy) 
        {
            err += dx;
            y0 += sy;
        }
    }

}

/// @brief Puts a short text label on the screen buffer using the graphical
/// @brief coordinate system. This is useful in widgets and menus.
/// @param pscr Control structure.
/// @param pstr Null terminated string.
/// @param x_pix Top-left point of the first symbol X coord.
/// @param y_pix Top-left point of the first symbol Y coord.
void TftPutTextLabel(screen_control_t *pscr, const char *pstr, int x_pix, 
                    int y_pix, bool over)
{
    assert_(pscr);
    assert_(pstr);

    if(x_pix > PIX_WIDTH - 8 || y_pix > PIX_HEIGHT - 8)
    {
        return;
    }

    int max_len = (PIX_WIDTH - x_pix) >> 3;
    for(int s = 0; pstr[s] && max_len; ++s, --max_len)
    {
        char chr = pstr[s];
        if(chr > 0x7E || chr < 0x20)
        {
            return; // chr > '~'
        }

        chr -= 0x20;

        const uint8_t *pfchar = kFONT_ + 2 + 8 * (int)chr;
        for(int j = 0; j < 8; ++j)
        {
            const int bit_line = (y_pix + j) * PIX_WIDTH + x_pix;
            const int blk_line = TEXT_WIDTH * ((y_pix + j) >> 3);
            for(int i = 0; i < 8; ++i)
            {
                const bool bit_val = (pfchar[j] >> (8-i)) & 1;
                if(bit_val)
                {
                    SET_DATA_BIT(pscr->mpPixBuffer, bit_line + i);
                }
                else if(over) 
                {
                    CLR_DATA_BIT(pscr->mpPixBuffer, bit_line + i);
                }

                pscr->mpColorBuffer[((x_pix + i) >> 3) + blk_line] |= (1 << 6);
            }
        }
        x_pix += 8;
    }
}

/// @brief Clears the 8x8 rectangle of pixel buffer.
/// @param pscr Control structure.
/// @param x X coord of
/// @param y 8x8 block.
void TftClearRect8(screen_control_t *pscr, int x, int y)
{
    assert_(pscr);

    for(int j = 0; j < 8; ++j)
    {
        const int shft = (x<<3) + (j + (y<<3)) * PIX_WIDTH;
        pscr->mpPixBuffer[shft>>5] = 0x00000000;

    }

    uint8_t *pbox = pscr->mpColorBuffer + x + TEXT_WIDTH * y;
    *pbox |= 1 << 6;        // Set for update.
}
