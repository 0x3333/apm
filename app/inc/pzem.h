#ifndef _PZEM_H
#define _PZEM_H

// =================================================================================================
// Includes
// =================================================================================================

// Opus
#include "opus.h"

// =================================================================================================
// Definitions
// =================================================================================================

#define CMD_VOLTAGE (uint8_t)0xB0
#define RSP_VOLTAGE (uint8_t)0xA0

#define CMD_CURRENT (uint8_t)0xB1
#define RSP_CURRENT (uint8_t)0xA1

#define CMD_POWER (uint8_t)0xB2
#define RSP_POWER (uint8_t)0xA2

#define CMD_ENERGY (uint8_t)0xB3
#define RSP_ENERGY (uint8_t)0xA3

#define CMD_SET_ADDRESS (uint8_t)0xB4
#define RSP_SET_ADDRESS (uint8_t)0xA4

#define CMD_POWER_ALARM (uint8_t)0xB5
#define RSP_POWER_ALARM (uint8_t)0xA5

#define RSP_FRAME_SIZE sizeof(pzem_frame_t)
#define RSP_DATA_SIZE RSP_FRAME_SIZE - 2

// =================================================================================================
// Data structures / Types
// =================================================================================================

typedef struct {
    uint8_t command;
    uint8_t addr[4];
    uint8_t data;
    uint8_t crc;
} pzem_frame_t ;

// =================================================================================================
// Macros
// =================================================================================================

// =================================================================================================
// External Declarations
// =================================================================================================

void calc_crc(uint16_t *crc, uint8_t *data, uint8_t len);

bool pzem_send(SoftSerialDriver* port, uint8_t cmd, uint8_t data);

bool pzem_receive(SoftSerialDriver* port, uint8_t cmd_response, uint8_t *data);

#endif /* _PZEM_H */