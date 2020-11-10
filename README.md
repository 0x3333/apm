# APM - Automatic Power Monitoring

This project is a multiple PZEM004T reader using a Software Serial Driver(Made by me, hightly functional and low CPU usage, (Driver)[https://github.com/0x3333/chibios_svn_mirror/blob/master/os/hal/lib/complex/soft_serial]).

It uses Opus Framework, a easy to use STM32F1 framework with some drivers build in and ready to use. The RTOS is ChibiosRT, but FreeRTOS can be used.

I'm using some (chinese boards)[https://www.aliexpress.com/item/32910310363.html] with ESP8266 header builtin. The ESP8266 module(ESP-01) is using espurna firmware with a custom driver builtin(See folder, espurna).

![board photo](https://github.com/0x3333/apm/blob/master/.github/board.jpg)

This project is in a working condition(I'm using with 5 PZEM004T) but has not been tested outside my environment. If you have any questions, just let me know.

![espurna photo](https://github.com/0x3333/apm/blob/master/.github/espurna.png)

