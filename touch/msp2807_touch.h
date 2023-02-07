///////////////////////////////////////////////////////////////////////////////
//
//  Roman Piksaykin [piksaykin@gmail.com], R2BDY
//  https://www.qrz.com/db/r2bdy
//
///////////////////////////////////////////////////////////////////////////////
//
//
//  msp2807_touch.h - Touch screen library for MSP2807 display.
// 
//
//  DESCRIPTION
//
//      The library interfaces touch screen of MSP2807 display.
//
//  PLATFORM
//      Raspberry Pi pico.
//
//  REVISION HISTORY
// 
//      Rev 0.9   05 Jan 2023
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
#ifndef _MSP2807_TOUCH_H
#define _MSP2807_TOUCH_H

#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

#include "../lib/assert.h"

#include "msp2807_calibration.h"

#define MSP2807_CMD_READ_X  0xD8
#define MSP2807_CMD_READ_Y  0x98
#define MSP2807_CMD_READ_Z1 0xB0
#define MSP2807_CMD_READ_Z2 0xC0

#define CS_ENABLE   0
#define CS_DISABLE  1

typedef struct
{
    spi_inst_t *mpSPIPort;

    int mGPIO_miso;
    int mGPIO_cs;
    int mGPIO_sck;
    int mGPIO_mosi;
    int mGPIO_ispressed;

} touch_hwconfig_t;

typedef struct
{
    touch_hwconfig_t 
            *mpHWConfig;    // Device hardware config.

    bool mIsPressed;        // Is screen now pressed.
    uint64_t mTmOfLastTouch;// System time of last screen press.
    bool mIsProcessed;      // Is current state was processed.

    int mX, mY;             // Values
    int mZ[2];              // have been read from MSP2807. [*]

    int mXf, mYf;           // mX, mY filtered * 16384.

    const int mkTmMinFlick; // Minimum time btw adjasent
                            // touches forms a group to filter.
    const int mkTmLongFlick;// Min time btw different touches.
    const int mkBetaShft;   // Touch position low-pass filer beta.

} touch_control_t;

void TouchInitHW(touch_hwconfig_t *phwconfig, spi_inst_t *pspi_port,
                    int spi_clock_freq, int gpio_MISO, int gpio_CS, 
                    int gpio_SCK, int gpio_MOSI, int gpio_ISPRESSED);

void TouchInitCtl(touch_control_t *pcontrol, touch_hwconfig_t *phwconfig,
                    int min_flick_ms, int long_press, int beta);

static inline void TouchCS_Set(const touch_hwconfig_t *phwconfig, int state);

void TouchReadRegisters(touch_control_t *pcontrol);

int CheckTouch(touch_control_t *pcontrol);

#endif
