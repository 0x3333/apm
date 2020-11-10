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
// =================================================================================================
// Includes
// =================================================================================================

// App
#include "app.h"
#include "pzem.h"

// =================================================================================================
// Definitions
// =================================================================================================

#define APP_THREAD_STACK_SIZE 512

// Delay between retries setting the PZEM004T device address
#define DELAY_SET_ADDRESS 10000

// Delay between reads(This is an extra time, the read by itself takes some time)
#define DELAY_READ 5000

// =================================================================================================
// External Declarations
// =================================================================================================

// =================================================================================================
// Exported Variables
// =================================================================================================

// =================================================================================================
// Local Variables / Types
// =================================================================================================

const uint8_t magic[] = { 0x42, 0xAA, 0x42, 0x2E };

typedef struct {
    uint8_t id;
    SoftSerialDriver *port;
    raw_reading_t reading;
} device_t;

device_t devices[] = {
    { 0, &SSD1, { {0,0,0}, {0,0,0}, {0,0}, {0,0,0} } },
    { 1, &SSD2, { {0,0,0}, {0,0,0}, {0,0}, {0,0,0} } },
    { 2, &SSD3, { {0,0,0}, {0,0,0}, {0,0}, {0,0,0} } },
    { 3, &SSD4, { {0,0,0}, {0,0,0}, {0,0}, {0,0,0} } },
    { 4, &SSD5, { {0,0,0}, {0,0,0}, {0,0}, {0,0,0} } },
};

THD_WORKING_AREA(WorkerWA, APP_THREAD_STACK_SIZE);
THD_WORKING_AREA(DeviceReader1WA, APP_THREAD_STACK_SIZE);
THD_WORKING_AREA(DeviceReader2WA, APP_THREAD_STACK_SIZE);
THD_WORKING_AREA(DeviceReader3WA, APP_THREAD_STACK_SIZE);
THD_WORKING_AREA(DeviceReader4WA, APP_THREAD_STACK_SIZE);
THD_WORKING_AREA(DeviceReader5WA, APP_THREAD_STACK_SIZE);

// =================================================================================================
// Local Functions
// =================================================================================================

static bool read(SoftSerialDriver *port, uint8_t cmd,  uint8_t rsp, uint8_t *dst, uint8_t dst_size) {
    uint8_t tmp[RSP_FRAME_SIZE];
    bool retried = false;

retry:
    pzem_send(port, cmd, 0);
    if(!pzem_receive(port, rsp, tmp)) {
        if(!retried) {
            goto retry;
        }
        return false;
    }
    for(uint8_t i = 0 ; i < dst_size ; i++) {
        dst[i] = tmp[i];
    }
    return true;
}

static THD_FUNCTION(DeviceReader, pvParameters)
{
    device_t *device = &(devices[(int) pvParameters]);
    SoftSerialDriver *port = device->port;
    raw_reading_t *reading = &device->reading;

    LOG_T(NL("Device created!"));

retry:
    // Empty input queue
    while(!iqIsEmptyI(&port->iqueue)) {
        iqGet(&port->iqueue);
    }
    pzem_send(port, CMD_SET_ADDRESS, 0);
    if(!pzem_receive(port, RSP_SET_ADDRESS, 0)) {
        LOG_T(NL("Failed setting address!"));
        osalThreadSleepMilliseconds(DELAY_SET_ADDRESS);
        goto retry;
    }

    while (1)
    {
        // Voltage
        if(!read(port, CMD_VOLTAGE, RSP_VOLTAGE, reading->voltage, sizeof(reading->voltage))) {
            LOG_T(NL("Voltage failed!"));
        }
        float v = (reading->voltage[0] << 8) + reading->voltage[1] + (reading->voltage[2] / 10.0);
        LOG_T(NL("V: %.1f"), v);

        // Current
        if(!read(port, CMD_CURRENT, RSP_CURRENT, reading->current, sizeof(reading->current))) {
            LOG_T(NL("Current failed!"));
        }
        float c = (reading->current[0] << 8) + reading->current[1] + (reading->current[2] / 100.0);
        LOG_T(NL("C: %.2f"), c);

        // Power
        if(!read(port, CMD_POWER, RSP_POWER, reading->power, sizeof(reading->power))) {
            LOG_T(NL("Power failed!"));
        }
        uint32_t p = (reading->power[0] << 8) + reading->power[1];
        LOG_T(NL("P: %.2f"), p);

        // Energy
        if(!read(port, CMD_ENERGY, RSP_ENERGY, reading->energy, sizeof(reading->energy))) {
            LOG_T(NL("Energy failed!"));
        }
        uint32_t e = (reading->energy[0] << 16) + (reading->energy[1] << 8) + reading->energy[2];
        LOG_T(NL("E: %.2f"), e);

        osalThreadSleepMilliseconds(DELAY_READ);
    }
}

THD_FUNCTION(Worker, pvParameters)
{
    UNUSED(pvParameters);

    static uint8_t magic_byte = 0;

    LOG_T(NL("Worker Thread created!"));
    while (1)
    {
        msg_t b = sdGet(&SD2);
        // Check for magic(Read command)
        if (b == magic[magic_byte]) {
            if (++magic_byte == sizeof(magic)) {
                // Reset state
                magic_byte = 0;

                // Send readings
                uint16_t crc = 0;
                for(uint8_t i = 0 ; i < (sizeof(devices) / sizeof(device_t)) ; i++) {
                    sdWrite(&SD2, (uint8_t*) &(devices[i].reading), sizeof(devices[i].reading));
                    calc_crc(&crc, (uint8_t *)&(devices[i].reading), sizeof(devices[i].reading));
                }
                sdPut(&SD2, (uint8_t)(crc & 0xFF));
                LOG_T(NL("CRC: %x"), (uint8_t)(crc & 0xFF));
            }
        } else {
            magic_byte = 0;
        }
    }
}

// =================================================================================================
// Exported Functions
// =================================================================================================

void appInit(void)
{
}

void appMain(void)
{
    THD_CREATE_STATIC(Worker, WorkerWA, APP_THREAD_STACK_SIZE, "Worker", NULL, NORMAL_PRIORITY);

    // This is lazy, but get the job done
    THD_CREATE_STATIC(DeviceReader, DeviceReader1WA, APP_THREAD_STACK_SIZE, "Reader1", (uint8_t*) 0, NORMAL_PRIORITY + 1);
    THD_CREATE_STATIC(DeviceReader, DeviceReader2WA, APP_THREAD_STACK_SIZE, "Reader2", (uint8_t*) 1, NORMAL_PRIORITY + 1);
    THD_CREATE_STATIC(DeviceReader, DeviceReader3WA, APP_THREAD_STACK_SIZE, "Reader3", (uint8_t*) 2, NORMAL_PRIORITY + 1);
    THD_CREATE_STATIC(DeviceReader, DeviceReader4WA, APP_THREAD_STACK_SIZE, "Reader4", (uint8_t*) 3, NORMAL_PRIORITY + 1);
    THD_CREATE_STATIC(DeviceReader, DeviceReader5WA, APP_THREAD_STACK_SIZE, "Reader5", (uint8_t*) 4, NORMAL_PRIORITY + 1);
}

#if defined(OP_IDLE_HOOK)
void vApplicationIdleHook(void)
{
}
#endif