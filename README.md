# Display & Touchscreen SDK for Raspberry Pi Pico.

The Display & Touchscreen SDK for Raspberry Pi pico includes the headers and 
libraries and all necessary build files to build a custom application which
uses MSP2807 display with touch screen or other one based on ILI9341 driver 
IC and TSC2046 touchscreen.

# Memory Efficient Screenbuffer

Taking in account the quite scarce RAM of Pico, the SDK implements a memory
efficient screenbuffer of 10800 bytes (320x240 pixels). The screen buffer
is organized in 2 planes: B/W 1bit-per-pixel plane and `boxycolor` plane.
Having quite the same appearance of text information, menus, lines, widgets
and other tech` graphical objects, this approach provides 14.2x economy in
device's memory consumption.

# Selective updating

The SDK writes the data to display in selective (adaptive) manner: only
regions which have been changed are being written.

# Touch Screen Interface

The touch screen interface consists of the adaptive polling procedure and
2d low pass filter which helps obtaining more accurate estimations of
touch point.

# Touch Screen Calibration

In addition the touch screen interface provides a calibration function which
was adopted using the prototype from Analog Devices company.

# Example Code

It is included (test.c).

# Quick-start

1. Install Raspberry Pi Pico SDK. Configure environment variables. Test
whether it is built successfully.

2. git clone this repository.
cd pico-touchscr-sdk
./build.sh
Check if output file ./build/pico-touchsrc-sdk-test.uf2 appears.

3. Assemble the test board using pin connection information provided in test.c

4. Load the .uf2 file (2) into Pico.

5. You now have the screen with text message and the software is waiting for
stylus operation.
