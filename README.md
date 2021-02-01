# APM - Automatic Power Monitoring

This project is a multiple PZEM004T reader using a Software Serial Driver(Made by me, hightly functional and low CPU usage, [Driver](https://github.com/0x3333/chibios_svn_mirror/blob/master/os/hal/lib/complex/soft_serial]).

It uses Opus Framework, a easy to use STM32F1 framework with some drivers build in and ready to use. The RTOS is ChibiosRT, but FreeRTOS can be used.

I'm using some [chinese boards](https://www.aliexpress.com/item/32910310363.html) with ESP8266 header builtin. The ESP8266 module(ESP-01) is using espurna firmware with a custom driver builtin(See folder, espurna).

![board photo](https://github.com/0x3333/apm/blob/master/.github/board.jpg)

This project is in a working condition(I'm using with 5 PZEM004T) but has not been tested outside my environment.

![espurna photo](https://github.com/0x3333/apm/blob/master/.github/espurna.png)

The protocol between devices is basically a struct sent multiple times with a CRC(The same as PZEM004T). The request is a magic(`{ 0x42, 0xAA, 0x42, 0x2E }`), when received, will respond with the structs with the last measure.

The STM32 firmware will reply instantaneously with the last measurement, so we can have several PZEM004T devices in the espurna firmware without timing issues, and as we are using a high performant Software Serial, we can have several devices without affecting the CPU consumption(Most code is done in IRQ).

If you have any questions, just let me know.
