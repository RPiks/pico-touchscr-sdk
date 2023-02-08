#include <string.h>
#include <stdlib.h>

#include "ili9341/ili9341.h"
#include "touch/msp2807_touch.h"

// TODO: PSE uncomment one mode only.

#define MODE_TEST_TOUCH_DRAWING
//#define MODE_TEST_RANDOM_LINES
//#define MODE_TEST_RANDOM_LABELS

void PRN32(uint32_t *val)
{ 
    *val ^= *val << 13;
    *val ^= *val >> 17;
    *val ^= *val << 5;
}

void TestRandomLabels(screen_control_t *p_screen)
{
    assert_(p_screen);

    static uint32_t rnd_seed = 0xa5efddbd;

    PRN32(&rnd_seed);
    const int x = rnd_seed % 240;
    PRN32(&rnd_seed);
    const int y = rnd_seed % 312;

    TftPutTextLabel(p_screen, "Pico RULEZZ", x, y, false);
    
    TftFullScreenSelectiveWrite(p_screen, 10000);
}

void TestRandomLines(screen_control_t *p_screen)
{
    assert_(p_screen);

    static uint32_t rnd_seed = 0xa5efddbd;

    PRN32(&rnd_seed);
    const int x0 = rnd_seed % 240;
    PRN32(&rnd_seed);
    const int x1 = rnd_seed % 240;
    PRN32(&rnd_seed);
    const int y0 = rnd_seed % 320;
    PRN32(&rnd_seed);
    const int y1 = rnd_seed % 320;

    TftPutLine(p_screen, x0, y0, x1, y1);
    
    TftFullScreenSelectiveWrite(p_screen, 10000);
}

int main() 
{
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    gpio_put(PICO_DEFAULT_LED_PIN, 1);

    sleep_ms(250);
    gpio_put(PICO_DEFAULT_LED_PIN, 0);

    static screen_control_t sScreen =
    {
        .mCursorX = 0,
        .mCursorY = 0,
        .mCursorType = 0,
        .mCanvasPaper = kBlack,
        .mCanvasInk = kWhite
    };

    /*
    DISPLAY
    =======
    Pico pin  Disp.pin   Description
    (pin 36)  VCC        3.3V power input.
    (pin 38)  GND        Ground.
    (pin 07)  CS         LCD chip select signal, low level enable.
    (pin 11)  RESET      LCD reset signal, low level reset.
    (pin 12)  DC/RS      LCD register / data selection signal; high level: register.
    (pin 10)  SDI(MOSI)  SPI bus write data signal.
    (pin 09)  SCK        SPI bus clock signal.
    (pin 36)  LED        Backlight control.
    (pin 06)  SDO(MISO)  SPI bus read data signal. Hasn't been used so far in here.
    
    TOUCH PANEL
    ===========
    Pico pin  Dev.pin    Description
    (pin 20)  T_IRQ      Touch event indicator (active low).
    (pin 15)  T_DIN      SPI MOSI signal.
    (pin 14)  T_CLK      SPI SCK signal.
    (pin 17)  T_CS       Device chip select (active low).
    (pin 16)  T_DO       SPI MISO signal.
    */

    ili9341_config_t ili9341_hw_config;
    sScreen.mpHWConfig = &ili9341_hw_config;
    ILI9341_Init(sScreen.mpHWConfig, spi0, 90 * MHz, 4, 5, 6, 7, 8, 9);

    sScreen.mCanvasPaper = kBlack;
    sScreen.mCanvasInk = kMagenta;
    TftClearScreenBuffer(&sScreen, kBlack, kRed);
    TftFullScreenWrite(&sScreen);

#ifdef MODE_TEST_TOUCH_DRAWING
    touch_hwconfig_t touch_hwc;
    TouchInitHW(&touch_hwc, spi1, 1 * MHz, 12, 13, 10, 11, 15);
    
    gpio_put(PICO_DEFAULT_LED_PIN, 1);
    
    touch_control_t touch_config;
    TouchInitCtl(&touch_config, &touch_hwc, 1000, 50000, 5);

    gpio_put(PICO_DEFAULT_LED_PIN, 0);

    calibration_mat_t cmat;
    const int16_t refpoints[] =
    {
        0, 0,
        240, 0,
        0, 320,
        240, 320
    };
    const int16_t smplpoints[] =
    {
        10, 120,
        119, 119,
        9, 11,
        118, 12
    };

    CalculateCalibrationMat(refpoints, smplpoints, 4, &cmat);

    TftPrintf(&sScreen, 0, 8, 0, 7, 
                "Calibration mat:\n %.2f %.2f %.2f\n%.2f %.2f %.2f\n",
                cmat.KX1, cmat.KX2, cmat.KX3, cmat.KY1, cmat.KY2, cmat.KY3);
    TftPrintf(&sScreen, 0, 8, 0, 3, "Please draw using the pen!!!");

    TftFullScreenSelectiveWrite(&sScreen, 10000);
#endif

    int led_state = 0;
    for(;;)
    {
        gpio_put(PICO_DEFAULT_LED_PIN, (led_state & 1));
        ++led_state;

#ifdef MODE_TEST_RANDOM_LINES
        TestRandomLines(&sScreen);
        continue;
#endif
#ifdef MODE_TEST_RANDOM_LABELS
        TestRandomLabels(&sScreen);
        //sleep_ms(250);
        continue;
#endif

#ifdef MODE_TEST_TOUCH_DRAWING
        CheckTouch(&touch_config);
        if(touch_config.mIsProcessed)
        {
            static int touch_tick = 0;
            touch_tick++;

            int32_t x = (touch_config.mXf + 8) >> 4;
            int32_t y = (touch_config.mYf + 8) >> 4;

            TouchTransformCoords(&cmat, &x, &y);

            TftPutPixel(&sScreen, x, y, kBlack, kYellow);
            
            if(0 == led_state % 128)
                TftPrintf(&sScreen, 0, 8, ~(touch_tick&7), touch_tick&7, "%u %d %d %d %d\n", touch_tick,
                    touch_config.mX, touch_config.mY, x, y);
        
            TftFullScreenSelectiveWrite(&sScreen, 10000);

            touch_config.mIsProcessed = false;
        }
#endif
    }
}
