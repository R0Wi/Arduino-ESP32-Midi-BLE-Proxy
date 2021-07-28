#ifndef ____SerialMidiReceiver__
#define ____SerialMidiReceiver__

#include <HardwareSerial.h>
#include "SerialLogger.h"

class SerialMidiReceiver {
    private:
        HardwareSerial* _serial = nullptr;
        SerialLogger* _logger = nullptr;
    public:
        SerialMidiReceiver(HardwareSerial* serial, SerialLogger* logger) 
        {
            _serial = serial;
            _logger = logger;
        }

        uint8_t SendData(uint8_t *dataptr, uint8_t datasize, uint8_t nCable=0)
        {
            _logger->logDebug("SendData");
            for (int i = 0; i < datasize; i++) {
                _logger->logDebug(String(dataptr[i], HEX));
            }
            _serial->write(dataptr, datasize);
            return 0;
        }

        uint8_t SendSysEx(uint8_t *dataptr, uint16_t datasize, uint8_t nCable=0)
        {
            _logger->logDebug("SendSysEx");
            for (int i = 0; i < datasize; i++) {
                _logger->logDebug(String((char)dataptr[i], HEX));
            }
            _serial->write(dataptr, datasize);
            return 0;
        }
};
#endif