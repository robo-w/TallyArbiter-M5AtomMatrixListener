# Tally Arbiter M5 ATOM Matrix Listener

## EOL Notice

**This version is only compatible with Tally Arbiter versions up to 2.x**

All future versions will be distributed as binaries from the [Tally Arbiter repository](http://github.com/josephdadams/tallyarbiter).
Please make all feature requests, bug reports, and pull requests against the primary repository.

## Fork Notice

This fork (`robo-w`) changes following behavior compared to the upstream:

* The colors of preview and program (green and red) are shown on all pixels of the LED matrix for the default cam number `0`, instead of leaving a dot in the middle.
* On startup the number of the light is still shown, in order to check the currently configured number on the device.
* Fixed ordering of colors for the use with `M5Atom` library 0.0.8 or higher. For older versions reverse red and green colors.
* Re-enabled storing of device-ID in preferences (keep configured device ID on power loss)
* Added blinking indication of missing network connection.

With `M5Atom` library version 0.0.8 the brightness of the LEDs is drastically reduced:

* The `FastLED` library offers a range of 0-255 for the brightness values.
* In `LED_DisPlay.cpp` of `M5Atom` library the accepted brightness values are reduced to `0-100`, but then multiplied by `0.4` before handed over to the `FastLED` library.
* So if you set `M5.dis.setBrightness(100);` you effectively get around 16 percent of the output power.
* Option for a patch:
  * Comment line `brightness = (40 * brightness / 100);` in `LED_DisPlay.cpp:151`, in order to increase the effective range to around `0-40%`.
  * This patch is completely at your own risk! Check the temperature of your device, especially if you run it for a long time continuously.

## Description

Tally Arbiter M5 ATOM Matrix Listener is an accessory program that allows you to connect to a Tally Arbiter server and control an M5Stick-C ESP32 Arduino device from M5Stack based on the incoming tally information.

To learn more about the Tally Arbiter project, [click here](http://github.com/josephdadams/tallyarbiter).

It is not sold, authorized, or associated with any other company or product.

To contact the author or for more information, please visit [www.techministry.blog](http://www.techministry.blog).

You can buy an M5 ATOM Matrix here:
https://m5stack.com/collections/m5-atom/products/atom-matrix-esp32-development-kit
OR
https://www.adafruit.com/product/4497 (Shipped from USA)

# Instructions

1. Go to https://docs.m5stack.com/en/arduino/arduino_development follow the instructions under heading "Boards Manager"
2. Open tallyarbiter-m5atom.ino in Arduino IDE https://www.arduino.cc/en/software
2. Update `networkSSID`, `networkPass` variable to match your Wifi network
3. Update `tallyarbiter_host` values to match your TallyArbiter installation
4. In Library Manager install FastLED, SocketIoClient, Arduino_JSON, WebSockets version 2.3.4, and MultiButton
6. Open `~/Documents/Arduino/libraries/SocketIoClient/SocketIoClient.cpp` in a text editor. Find the line that says `hexdump(payload, length);` and comment it out by adding `//` to the beginning of the line. Save and close that file.
7. Go In the IDE go to Sketch -> Upload

## Troubleshooting

### macOS
If you receive an error similar to `ImportError: No module named serial` reference: https://community.m5stack.com/post/11106

# Videos

* What this does when used with TallyArbiter - https://youtu.be/hNhU22OkXek
* Demo of the M5Atom Matrix code in this git - https://youtu.be/Mc_PCxg6qdc
* Demo running on 10 M5Atom Matrix units via a cloud server - https://youtu.be/TiqjmXdzPic
* RAW Step-By-Step Install on a fresh windows installation - https://youtu.be/soj1Cxv3mLY
* Video Walkthrough: https://youtu.be/WMrRKD63Jrw

# Improvements and Suggestions
I welcome all improvements and suggestions. You can submit issues and pull requests, or contact me through [my blog](http://www.techministry.blog).






