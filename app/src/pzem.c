// =================================================================================================
// Includes
// =================================================================================================

// App
#include "pzem.h"

// =================================================================================================
// Definitions
// =================================================================================================

// =================================================================================================
// External Declarations
// =================================================================================================

// =================================================================================================
// Exported Variables
// =================================================================================================

// =================================================================================================
// Local Variables / Types
// =================================================================================================

// =================================================================================================
// Local Functions
// =================================================================================================

static uint8_t _crc(uint8_t *data, uint8_t len)
{
    uint16_t crc = 0;
    for(uint8_t i = 0; i < len; i++)
        crc += *data++;
    return (uint8_t)(crc & 0xFF);
}

// =================================================================================================
// Exported Functions
// =================================================================================================

bool pzem_send(SoftSerialDriver* port, uint8_t cmd, uint8_t data) {
    pzem_frame_t frame;

    frame.command = cmd;
    frame.addr[0] = 1;
    frame.addr[1] = 2;
    frame.addr[2] = 3;
    frame.addr[3] = 4;
    frame.data = data;
    frame.crc = _crc((uint8_t*)&frame, sizeof(frame) - 1);
    
    unsigned int len = ssdWrite(port, (uint8_t*)&frame, sizeof(frame));

    return len == sizeof(frame);
}

bool pzem_receive(SoftSerialDriver* port, uint8_t rsp, uint8_t *data) {
    static uint8_t buffer[RSP_FRAME_SIZE];

    unsigned int len = ssdReadTimeout(port, buffer, sizeof(buffer), OSAL_MS2I(2000));
    if (len != sizeof(buffer)) {
        return false;
    }

    uint8_t calc_crc = _crc(buffer, sizeof(buffer)-1);
    if(buffer[sizeof(buffer)-1] != calc_crc) {
        return false;
    }

    if(buffer[0] != rsp) {
        return false;
    }

    if(data) {
        for(uint8_t i = 0; i < RSP_DATA_SIZE; i++) {
            data[i] = buffer[i + 1];
        }
    }
    return true;
}
