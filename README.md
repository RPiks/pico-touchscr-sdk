# Display & Touchscreen SDK for Raspberry Pi Pico

The Display & Touchscreen SDK for Raspberry Pi Pico includes the headers and
libraries and all necessary build files to build a custom application which
uses MSP2807 display with touch screen or other one based on ILI9341 driver
IC and TSC2046 touchscreen.

![IMG_8820_](https://user-images.githubusercontent.com/47501785/217345382-35092e75-2a11-4d2f-9d79-4ed864db24b8.jpg)


# Memory Efficient Screenbuffer

Taking in account the quite scarce RAM of Pico, the SDK implements a memory
efficient screenbuffer of 10800 bytes, preserving 320x240 pixel resolution.
The screen buffer is organized in 2 planes: A) graphics 1bit-per-pixel plane
and B) color plane of reduced spatial resolution which uses boxes of 8x8 
pixels holding the color attributes. Whereas screenbuffer memory consumption
is drastically reduced, most of the typical graphics which is used by embedded
systems - text information, menus, lines, widgets - looks quite the same in
comparison with full screenbuffer approach which consumes 14.2x more memory.

# Selective updating

The SDK writes the data to display in selective (adaptive) manner: only
regions which have been changed are being written. In addition, the SPI I/O
is organized using tiny chunks of data. This mitigates the negative effect
of blocking SPI bus I/O which is critical in realtime systems such as ADC
data processing.

# Touch Screen Interface

The SDK incorporates the touch screen interface which consists of the adaptive
polling procedure with anti-flicker filter as well as 2D low pass filter which
helps obtaining more accurate estimations of touch point.

In addition the SDK provides a calibration function which was adopted using 
a prototype AN-1021 obtained from Analog Devices company.

The touchscreen interface is compartible with TSC2046 chip.

# Example Code

It is included (test.c).

# Quick-start

1. Install Raspberry Pi Pico SDK. Configure environment variables. Test
whether it is built successfully.

2. git clone this repository. cd pico-touchscr-sdk ; ./build.sh
Check whether output file ./build/pico-touchsrc-sdk-test.uf2 appears.

3. Assemble the test board using pin connection information provided in test.c

4. Load the .uf2 file (2) into the Pico.

5. You now have the screen with text message and the software is waiting for
stylus operation. Draw curves on your screen! Enjoy! :-)

6. Try other examples provided in test.c by uncommenting macros.

7. Use the SDK on your projects freely. Any possible contribution appreciated.
