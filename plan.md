### Receive from BLE
https://github.com/R0Wi/Arduino-BLE-MIDI/blob/master/src/Ble_esp32.h

* Fork
* Make `void BleMidiInterface::receive(uint8_t *buffer, uint8_t bufferSize)` virtual
* Overwrite in custom class `CustomBleMidiInterface<CLASSNAME> : BleMidiInterface` with `CLASSNAME = SERIAL_MIDI_RECEIVER` (Classname = name of class which implements the output for the received events from BLE)
### Parsing
https://github.com/sieren/blidino/blob/master/nRF51822-BLEMIDI/nRF51822-BLEMIDI.ino#L379

```c++
#include "BLEParser.h"
// ...
mfk::midi::BLEMIDIParser<256, SERIAL_MIDI_RECEIVER> parser;
/// ...
void parseIncoming(uint8_t *buffer, uint16_t bytesRead)
{
  for (int i = 1; i < bytesRead; i++)
  {
    parser.parseMidiEvent(buffer[0], buffer[i]);
  }
}
```

### Send via serial
```c++
mfk::midi::BLEMIDIParser<256, USBH_MIDI> parser;
```
https://github.com/YuuichiAkagawa/USBH_MIDI/blob/master/usbh_midi.cpp

```c++
class SERIAL_MIDI_RECEIVER;
class SERIAL_MIDI_RECEIVER {
    public:
        SERIAL_MIDI_RECEIVER();
        uint8_t SendData(uint8_t *dataptr, uint8_t nCable=0);
        uint8_t SendSysEx(uint8_t *dataptr, uint16_t datasize, uint8_t nCable=0);
}
```

