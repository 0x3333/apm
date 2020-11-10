/**
 *      ____
 *     / __ \____  __  _______
 *    / / / / __ \/ / / / ___/
 *   / /_/ / /_/ / /_/ (__  )
 *   \____/ .___/\__,_/____/
 *       /_/         Framework
 *   Copyright (C) 2019 Tercio Gaudencio Filho.
 *
 *   This file is part of Opus Framework.
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */
#ifndef _APPCONF_H
#define _APPCONF_H

/**
 * ===========================================================================
 * Opus Configurations
 * ===========================================================================
 */

// Application Version
#define OP_APP_MAJOR /*                */ 1
#define OP_APP_MINOR /*                */ 0
#define OP_APP_PATCH /*                */ 0

// Board Serial Number
// Note: Must be UTF-16, append a 0 after each char
#define OP_SERIAL_NUMBER /*            */         \
    /*                                 */ '0', 0, \
        /*                             */ '0', 0, \
        /*                             */ '0', 0, \
        /*                             */ '0', 0, \
        /*                             */ '1', 0

// Boards
#define OP_BOARD_BLUEPILL /*           */ 1
#define OP_BOARD_COREBOARD /*          */ 2

// Board
#define OP_BOARD /*                    */ OP_BOARD_COREBOARD

// Idle Hook
// #define OP_IDLE_HOOK

// Watchdog LED
#define OP_HEARTBEAT_LED
#define OP_HEARTBEAT_LED_DELAY /*       */ 1000
#if OP_BOARD == OP_BOARD_BLUEPILL
#define OP_HEARTBEAT_LED_LINE /*        */ PAL_LINE(GPIOC, 13)
#elif OP_BOARD == OP_BOARD_COREBOARD
#define OP_HEARTBEAT_LED_LINE /*        */ PAL_LINE(GPIOA, 1)
#endif

// Hardware Serial Driver
#define OP_SERIAL
// Hardware Serial Port 1
#define OP_SERIAL_1
#define OP_SERIAL_1_BAUD /*            */ 115200
#define OP_SERIAL_1_TX_LINE /*         */ PAL_LINE(GPIOA, 9)
#define OP_SERIAL_1_RX_LINE /*         */ PAL_LINE(GPIOA, 10)
// Hardware Serial Port 2
#define OP_SERIAL_2
#define OP_SERIAL_2_BAUD /*            */ 115200
#define OP_SERIAL_2_TX_LINE /*         */ PAL_LINE(GPIOA, 2)
#define OP_SERIAL_2_RX_LINE /*         */ PAL_LINE(GPIOA, 3)

// Software Serial Driver
#define OP_SOFT_SERIAL
#define OP_SOFT_SERIAL_BAUD /*         */ 9600
#define OP_SOFT_SERIAL_BITRATE_MULTIPLIER 4
// Software Serial Port 1
#define OP_SOFT_SERIAL_1
#define OP_SOFT_SERIAL_1_TX_LINE /*    */ PAL_LINE(GPIOB, 0)
#define OP_SOFT_SERIAL_1_RX_LINE /*    */ PAL_LINE(GPIOB, 1)
// Software Serial Port 2
#define OP_SOFT_SERIAL_2
#define OP_SOFT_SERIAL_2_TX_LINE /*    */ PAL_LINE(GPIOB, 8)
#define OP_SOFT_SERIAL_2_RX_LINE /*    */ PAL_LINE(GPIOB, 9)
// Software Serial Port 3
#define OP_SOFT_SERIAL_3
#define OP_SOFT_SERIAL_3_TX_LINE /*    */ PAL_LINE(GPIOB, 10)
#define OP_SOFT_SERIAL_3_RX_LINE /*    */ PAL_LINE(GPIOB, 11)
// Software Serial Port 4
#define OP_SOFT_SERIAL_4
#define OP_SOFT_SERIAL_4_TX_LINE /*    */ PAL_LINE(GPIOB, 12)
#define OP_SOFT_SERIAL_4_RX_LINE /*    */ PAL_LINE(GPIOB, 13)
// Software Serial Port 5
#define OP_SOFT_SERIAL_5
#define OP_SOFT_SERIAL_5_TX_LINE /*    */ PAL_LINE(GPIOB, 14)
#define OP_SOFT_SERIAL_5_RX_LINE /*    */ PAL_LINE(GPIOB, 15)

// USB Serial Port Driver
// #define OP_USB_SERIAL

// Managed Flash Storage Driver
#define OP_MFS
#define OP_MFS_BANK_SIZE /*            */ 4096U
#ifdef OP_BOOTLOADER
#define OP_MFS_BANK0_START /*          */ 8U
#define OP_MFS_BANK1_START /*          */ 12U
#else
#define OP_MFS_BANK0_START /*          */ 120U
#define OP_MFS_BANK1_START /*          */ 124U
#endif

#endif /* _APPCONF_H */