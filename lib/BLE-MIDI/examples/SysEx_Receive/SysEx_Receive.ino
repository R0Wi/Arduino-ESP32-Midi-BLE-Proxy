#include <BLEMIDI_Transport.h>

//#include <hardware/BLEMIDI_ESP32_NimBLE.h>
#include <hardware/BLEMIDI_ESP32.h>
//#include <hardware/BLEMIDI_nRF52.h>
//#include <hardware/BLEMIDI_ArduinoBLE.h>

#define LED 2
#define USB_MONITOR_BAUDRATE 115200
#define MIDI_BAUDRATE 31250
#define RXD2 16
#define TXD2 17

bool debug = true;
int numClients = 0;

MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, SERIAL_MIDI_OUTPUT);

// This creates :
//  MIDI -> Object from Midi-Library representing BLE Midi input
//  BLEMIDI -> Special object from BLE Midi library for adding some more events
BLEMIDI_CREATE_INSTANCE("CustomName", MIDI)

bool isConnected = false;

//#define LOGGING

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void setup()
{
  Serial.begin(115200);
  while (!Serial);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

    // Serial2 (GPIO 16 & 17) for hardware-midi in/out
  Serial2.begin(MIDI_BAUDRATE, SERIAL_8N1, RXD2, TXD2);

  BLEMIDI.setHandleConnected(onBleMidiConnected);
  BLEMIDI.setHandleDisconnected(onBleMidiDisconnected);
 
  MIDI.setHandleNoteOn(onBleMidiNoteOn);
  MIDI.setHandleNoteOff(onBleMidiNoteOff);
  MIDI.setHandleControlChange(onBleMidiControlChange);
  MIDI.setHandleProgramChange(onBleMidiProgramChange);
  MIDI.setHandleSystemExclusive(onBleMidiSysEx);

  MIDI.begin();

  debugOutput("End setup");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void loop()
{
  // Listen on all channels
  // for(byte i = 1; i <= 16; i++) {
  //   MIDI.read(i);
  // }
  MIDI.read();
}

// -----------------------------------------------------------------------------
//  FUNCTIONS
// -----------------------------------------------------------------------------

void debugOutput(String output){
  if (debug){
    Serial.println(output);
  }
}

void ledState(bool state) {
  if (state)
    digitalWrite(LED, HIGH);
  else
    digitalWrite(LED, LOW);
}

// ====================================================================================
// Event handlers for incoming MIDI messages (via BLE)
// ====================================================================================

void onBleMidiConnected() {
  numClients++;
  debugOutput("NumClients: " + String(numClients));
  ledState(numClients > 0);
}

void onBleMidiDisconnected() {
  if (numClients > 0)
    numClients--;
  debugOutput("NumClients: " + String(numClients));
  ledState(numClients > 0);
}

void onBleMidiSysEx(byte* data, unsigned length) {
  Serial.print(F("SYSEX: ("));
  Serial.print(length);
  Serial.print(F(" bytes) "));
  for (uint16_t i = 0; i < length; i++)
  {
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  SERIAL_MIDI_OUTPUT.sendSysEx(length, data);
}

void onBleMidiControlChange(byte channel, byte number, byte value){
  debugOutput("ControlChange, Channel: " + String(channel) + 
               " number:" + String(number) +
               " value:" + String(value));
  SERIAL_MIDI_OUTPUT.sendControlChange(number, value, channel);
}

void onBleMidiProgramChange(byte channel, byte number){
  debugOutput("ProgramChange, Channel: " + String(channel) + 
              " number:" + String(number));
  SERIAL_MIDI_OUTPUT.sendProgramChange(number, channel);
}

void onBleMidiNoteOn(byte channel, byte note, byte velocity) {
  SERIAL_MIDI_OUTPUT.sendNoteOn(note, velocity, channel);
}

void onBleMidiNoteOff(byte channel, byte note, byte velocity) {
  SERIAL_MIDI_OUTPUT.sendNoteOff(note, velocity, channel);
}
