class SERIAL_MIDI_RECEIVER;
class SERIAL_MIDI_RECEIVER {
    public:
        SERIAL_MIDI_RECEIVER();
        uint8_t SendData(uint8_t *dataptr, uint8_t nCable=0);
        uint8_t SendSysEx(uint8_t *dataptr, uint16_t datasize, uint8_t nCable=0);
}