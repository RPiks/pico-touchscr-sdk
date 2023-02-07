///////////////////////////////////////////////////////////////////////////////
//
//  Roman Piksaykin [piksaykin@gmail.com], R2BDY
//  https://www.qrz.com/db/r2bdy
//
///////////////////////////////////////////////////////////////////////////////
//
//
//  msp2807_touch.c - Touch screen library for MSP2807 display.
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
#include "msp2807_touch.h"

/// @brief Initialize the control structure.
/// @param phwconfig Ptr to the hw control structure.
/// @param pspi_port SPI port instance of touch screen device, such as spi1.
/// @param spi_clock_freq SPI port clock freq, such as 1 MHz.
/// @param gpio_MISO Master input slave output pin.
/// @param gpio_CS Chip select pin.
/// @param gpio_SCK Serial clock pin.
/// @param gpio_MOSI Master output slave input pin.
/// @param gpio_ISPRESSED Hardware input pin for press (touch) detection.
void TouchInitHW(touch_hwconfig_t *phwconfig, spi_inst_t *pspi_port, 
                    int spi_clock_freq, int gpio_MISO, int gpio_CS, 
                    int gpio_SCK, int gpio_MOSI, int gpio_ISPRESSED)
{
    assert_(phwconfig);
    assert_(pspi_port);
    assert_(spi_clock_freq);

    phwconfig->mpSPIPort = pspi_port;
    phwconfig->mGPIO_miso= gpio_MISO;
    phwconfig->mGPIO_cs  = gpio_CS;
    phwconfig->mGPIO_sck = gpio_SCK;
    phwconfig->mGPIO_mosi= gpio_MOSI;
    phwconfig->mGPIO_ispressed = gpio_ISPRESSED;

    // Use SPI at ~0.5 ... 13MHz.
    spi_init(phwconfig->mpSPIPort, spi_clock_freq);
    //spi_set_format(phwconfig->mpSPIPort, 16, 0, 0, 1);
    const int baudrate = spi_set_baudrate(phwconfig->mpSPIPort, spi_clock_freq);

    gpio_set_function(phwconfig->mGPIO_miso, GPIO_FUNC_SPI);
    gpio_set_function(phwconfig->mGPIO_sck, GPIO_FUNC_SPI);
    gpio_set_function(phwconfig->mGPIO_mosi, GPIO_FUNC_SPI);

    gpio_init(phwconfig->mGPIO_cs);
    gpio_set_dir(phwconfig->mGPIO_cs, GPIO_OUT);
    gpio_put(phwconfig->mGPIO_cs, 1);

    gpio_init(phwconfig->mGPIO_ispressed);
    gpio_set_dir(phwconfig->mGPIO_ispressed, GPIO_IN);
    gpio_pull_up(phwconfig->mGPIO_ispressed);

    sleep_ms(100);
}

/// @brief Touchscreen high level functions init.
/// @param pcontrol Ptr to the control structure.
/// @param phwconfig Ptr to the hw control structure.
/// @param min_flick_ms Min. time for adjacent touch events.
/// @param beta The coeff. of 2d exponential position filter.
void TouchInitCtl(touch_control_t *pcontrol, touch_hwconfig_t *phwconfig,
                    int min_flick_us, int long_press_us, int beta)
{
    memset(pcontrol, 0, sizeof(pcontrol));

    pcontrol->mpHWConfig = phwconfig;
    *(int *)&pcontrol->mkTmMinFlick = min_flick_us;
    *(int *)&pcontrol->mkTmLongFlick= long_press_us;
    *(int *)&pcontrol->mkBetaShft = beta;
}

/// @brief Activates device's bus.
/// @param phwconfig HW config
/// @param state 0 or 1 (0 is active!)
static inline void TouchCS_Set(const touch_hwconfig_t *phwconfig, int state)
{
    asm volatile("nop \n nop \n nop");
    gpio_put(phwconfig->mGPIO_cs, (bool)state);
    asm volatile("nop \n nop \n nop");
}

/// @brief Reads X & Y registers from touch controller and stores it in struct.
/// @brief Reading pressur power (Z) isn't implemented so far.
/// @param pcontrol Control struct.
void TouchReadRegisters(touch_control_t *pcontrol)
{
    TouchCS_Set(pcontrol->mpHWConfig, CS_ENABLE);

    const uint8_t poll_cmds[4] =
    {
        MSP2807_CMD_READ_X,
        MSP2807_CMD_READ_Y,
        MSP2807_CMD_READ_Z1,
        MSP2807_CMD_READ_Z2
    };

    uint8_t res[4];

    for(int i = 0; i < 2; ++i)
    {
        spi_write_blocking(pcontrol->mpHWConfig->mpSPIPort, &poll_cmds[i], 1);
        spi_read_blocking(pcontrol->mpHWConfig->mpSPIPort, 0, &res[i], 1);
    }

    TouchCS_Set(pcontrol->mpHWConfig, CS_DISABLE);

    pcontrol->mX = res[0];
    pcontrol->mY = res[1];
    
    pcontrol->mIsProcessed = true;
}

/// @brief Main function for device polling. Should be inserted into
/// @brief ISR routine periodically processes some data.
/// @param pcontrol Control structure.
/// @return 0 if touch has been pressed and data has been read.
int CheckTouch(touch_control_t *pcontrol)
{
    if(pcontrol)
    {
        if(pcontrol->mpHWConfig && pcontrol->mkBetaShft > 0)
        {
            const bool kb_ispressed 
                        = gpio_get(pcontrol->mpHWConfig->mGPIO_ispressed);
            if(!kb_ispressed)
            {
                const uint32_t klo = timer_hw->timelr;
                const uint32_t khi = timer_hw->timehr;
                const uint64_t ktm64_now = ((uint64_t)khi << 32) | klo;

                if(ktm64_now - pcontrol->mTmOfLastTouch 
                        > pcontrol->mkTmLongFlick)
                {
                    TouchReadRegisters(pcontrol);
                    pcontrol->mTmOfLastTouch = ktm64_now;

                    pcontrol->mXf = pcontrol->mX << 14;
                    pcontrol->mYf = pcontrol->mY << 14;
                }

                if(ktm64_now - pcontrol->mTmOfLastTouch 
                        > pcontrol->mkTmMinFlick)
                {
                    TouchReadRegisters(pcontrol);
                    pcontrol->mTmOfLastTouch = ktm64_now;

                    pcontrol->mXf += ((pcontrol->mX << 14) - pcontrol->mXf
                                   + (1 << (pcontrol->mkBetaShft - 1)))
                                  >> pcontrol->mkBetaShft;

                    pcontrol->mYf += ((pcontrol->mY << 14) - pcontrol->mYf
                                   + (1 << (pcontrol->mkBetaShft - 1)))
                                  >> pcontrol->mkBetaShft;
                }

                return 0;
            }
            return 1;
        }
        return -2;
    }
    return -1;
}
