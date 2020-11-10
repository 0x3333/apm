// -----------------------------------------------------------------------------
// PZEM004T + STM32F103 based power monitor
// Copyright (C) 2020 by Tercio Gaudencio Filho <terciofilho at gmail dot com>
// -----------------------------------------------------------------------------

//
// You need to define below directives to make it work
// The DEVICES count must match the one in the STM32 firmware
// This is kind of lazy, but does the job
//
// #define MULTI_PZEM004T_SUPPORT  1
// #define MULTI_PZEM004T_DEVICES  5
//


#if SENSOR_SUPPORT && MULTI_PZEM004T_SUPPORT

#pragma once

#include "Arduino.h"
#include "BaseSensor.h"

#define MPZ_BAUD_RATE                        115200
#define MPZ_READ_WAIT_INTERVAL               15000

#define MPZ_MAGNITUDE_COUNT                  4
#define MPZ_MAGNITUDE_CURRENT_INDEX          0
#define MPZ_MAGNITUDE_VOLTAGE_INDEX          1
#define MPZ_MAGNITUDE_POWER_ACTIVE_INDEX     2
#define MPZ_MAGNITUDE_ENERGY_INDEX           3

#define MPZ_DEV_INDEX(slot)                  (slot / MPZ_MAGNITUDE_COUNT)
#define MPZ_MAGNITUDE_INDEX(slot, dev)       (slot - (dev * MPZ_MAGNITUDE_COUNT))

const uint8_t read_magic[] = { 0x42, 0xAA, 0x42, 0x2E };

// Reading state machine
enum {
    SEND_READ_CMD = 0x01,
    WAIT_READ_RESP = 0x02,
    WAIT_NEXT_READ = 0x03
};

class MultiPZEM004TSensor : public BaseSensor {
protected:

        // ---------------------------------------------------------------------
        // Protected
        // ---------------------------------------------------------------------

        // Same structure used in STM32 firmware
        typedef struct {
            uint8_t current[3];
            uint8_t voltage[3];
            uint8_t power[2];
            uint8_t energy[3];
        } raw_reading_t;

        typedef struct {
            float current;
            float voltage;
            float power;
            float energy;
        } reading_t;

        reading_t _readings[MULTI_PZEM004T_DEVICES];

        HardwareSerial * _serial = NULL;

    public:

        // ---------------------------------------------------------------------
        // Public
        // ---------------------------------------------------------------------

        MultiPZEM004TSensor(): BaseSensor() {
            _sensor_id = SENSOR_MULTI_PZEM004T_ID;
        }

        ~MultiPZEM004TSensor() {
            if (_serial) delete _serial;
        }

        // ---------------------------------------------------------------------

        void resetEnergy() {
            // FIXME: Send RESET command to STM32F103
        }

        // ---------------------------------------------------------------------
        // Sensor API
        // ---------------------------------------------------------------------

        void setSerial(HardwareSerial * serial) {
            _serial = serial;
        }

        // Initialization method, must be idempotent
        void begin() {
            _count = MULTI_PZEM004T_DEVICES * MPZ_MAGNITUDE_COUNT;

            // Reset Readings
            for(uint8_t i = 0; i < MULTI_PZEM004T_DEVICES; i++) {
                _readings[i].current = -1;
                _readings[i].voltage = -1;
                _readings[i].power = -1;
                _readings[i].energy = -1;
            }

            _serial->begin(MPZ_BAUD_RATE);

            _ready = true;
        }

        // Descriptive name of the sensor
        String description() {
            char buffer[25];
            snprintf(buffer, sizeof(buffer), "MultiPZEM004T @ HwSerial");
            return String(buffer);
        }

        // Descriptive name of the slot # index
        String slot(unsigned char index) {
            int dev = MPZ_DEV_INDEX(index);
            char buffer[25];
            snprintf(buffer, sizeof(buffer), "(%u/%u)", MPZ_MAGNITUDE_INDEX(index, dev), dev);
            return description() + String(buffer);
        };

        // Address of the sensor (it could be the GPIO or I2C address)
        String address(unsigned char index) {
            int dev = MPZ_DEV_INDEX(index);
            return String(dev);
        }

        // Type for slot # index
        unsigned char type(unsigned char index) {
            int dev = MPZ_DEV_INDEX(index);
            index = MPZ_MAGNITUDE_INDEX(index, dev);
            if (index == MPZ_MAGNITUDE_CURRENT_INDEX)      return MAGNITUDE_CURRENT;
            if (index == MPZ_MAGNITUDE_VOLTAGE_INDEX)      return MAGNITUDE_VOLTAGE;
            if (index == MPZ_MAGNITUDE_POWER_ACTIVE_INDEX) return MAGNITUDE_POWER_ACTIVE;
            if (index == MPZ_MAGNITUDE_ENERGY_INDEX)       return MAGNITUDE_ENERGY;
            return MAGNITUDE_NONE;
        }

        // Current value for slot # index
        double value(unsigned char index) {
            int dev = MPZ_DEV_INDEX(index);
            index = MPZ_MAGNITUDE_INDEX(index, dev);
            double response = 1;
            if (index == MPZ_MAGNITUDE_CURRENT_INDEX)      response = _readings[dev].current;
            if (index == MPZ_MAGNITUDE_VOLTAGE_INDEX)      response = _readings[dev].voltage;
            if (index == MPZ_MAGNITUDE_POWER_ACTIVE_INDEX) response = _readings[dev].power;
            if (index == MPZ_MAGNITUDE_ENERGY_INDEX)       response = _readings[dev].energy * 3600;
            if (response < 0) response = 0;
            return response;
        }

        // Number of decimals for a magnitude (or -1 for default)
	    signed char decimals(unsigned char index) {
            int dev = MPZ_DEV_INDEX(index);
            index = MPZ_MAGNITUDE_INDEX(index, dev);
            double response = -1;
            if (index == MPZ_MAGNITUDE_VOLTAGE_INDEX)      response = 1;
            return response;
        }

        // Post-read hook (usually to reset things)
        void post() {
            _error = SENSOR_ERROR_OK;
        }

        float _current(const uint8_t *data)
        {
            return (data[0] << 8) + data[1] + (data[2] / 100.0);
        }

        float _voltage(const uint8_t *data)
        {
            return (data[0] << 8) + data[1] + (data[2] / 10.0);
        }

        float _power(const uint8_t *data)
        {
            return (data[0] << 8) + data[1];
        }

        float _energy(const uint8_t *data)
        {
            return ((uint32_t)data[0] << 16) + ((uint16_t)data[1] << 8) + data[2];
        }

        static inline bool _is_in_range(unsigned long start, unsigned long end) {
            return (bool)((unsigned long)((unsigned long)millis() - (unsigned long)start) <
                            (unsigned long)((unsigned long)end - (unsigned long)start));
        }

        uint8_t _crc(uint8_t *data, uint8_t len)
        {
            uint16_t crc = 0;
            for(uint8_t i = 0; i < len; i++)
                crc += *data++;
            return (uint8_t)(crc & 0xFF);
        }

        void pre() {
            static raw_reading_t raw_reading[MULTI_PZEM004T_DEVICES];
            static uint8_t * raw_reading_addr = (uint8_t *) &raw_reading;

            // Clear RX buffer
            while(_serial->available()) {
                _serial->read();
            }
            // Send command
            _serial->write(read_magic, sizeof(read_magic));

            // Within response time window
            unsigned long start = millis();
            while((int)_serial->available() < (int)(sizeof(raw_reading) + 1)) { // + 1 byte(CRC)
                if (millis() - start > 100) {
                    _error = 1;
                    DEBUG_MSG_P(PSTR("[MPZEM004T] Timeout reading\n"));
                    return; 
                }
                delay(1);
            }

            for(uint8_t i = 0; i < sizeof(raw_reading); i++) {
                raw_reading_addr[i] = _serial->read();
            }
            uint8_t crc = _serial->read();
            uint8_t calc_crc = _crc(raw_reading_addr, sizeof(raw_reading));
            if(crc != calc_crc) { // If CRC is incorrect, send a read command again
                _error = 2;
                DEBUG_MSG_P(PSTR("[MPZEM004T] CRC Error(0x%x, 0x%x)\n"), crc, calc_crc);
                return;
            } else {
                for(uint8_t i = 0; i < MULTI_PZEM004T_DEVICES; i++) {
                    _readings[i].current = _current(raw_reading[i].current);
                    _readings[i].voltage = _voltage(raw_reading[i].voltage);
                    _readings[i].power = _power(raw_reading[i].power); 
                    _readings[i].energy = _energy(raw_reading[i].energy);
                }
            }
        }

};

#endif // SENSOR_SUPPORT && MULTI_PZEM004T_SUPPORT
