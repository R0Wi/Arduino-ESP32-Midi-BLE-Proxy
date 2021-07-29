// Uncomment to get debug output on USB serial
//#define DEBUG

#include <BLEMIDI_Transport.h>
#include <hardware/BLEMIDI_ESP32.h>
#include <HardwareSerial.h>
#include <midi_Defs.h>

#define BLE_NAME "BLE Midi Proxy"
#define LED 2
#define USB_MONITOR_BAUDRATE 115200
#define MIDI_BAUDRATE 31250
#define RXD2 16
#define TXD2 17

int numClients = 0;

// This creates :
//  MIDI -> Object from Midi-Library representing BLE Midi input
//  BLEMIDI -> Special object from BLE Midi library for adding some more events
BLEMIDI_CREATE_INSTANCE(BLE_NAME, MIDI)

// MIDI interface for serial midi output connection
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, SERIAL_MIDI_OUTPUT);

// -----------------------------------------------------------------------------
//  FUNCTIONS
// -----------------------------------------------------------------------------
void debugOutput(String output, bool newline = true){
  #ifdef DEBUG
    Serial.print(output);
    if (newline)
      Serial.println();
  #endif
}

void ledState(bool state) {
  if (state)
    digitalWrite(LED, HIGH);
  else
    digitalWrite(LED, LOW);
}

// -----------------------------------------------------------------------------
//  SETUP
// -----------------------------------------------------------------------------
void setup() {
  debugOutput("Begin setup");

  // Serial (USB) for debug output
  Serial.begin(USB_MONITOR_BAUDRATE);
  
  // Serial2 (GPIO 16 & 17) for hardware-midi in/out
  Serial2.begin(MIDI_BAUDRATE, SERIAL_8N1, RXD2, TXD2);

  // LED for signalizing if BLE client is connected
  pinMode(LED, OUTPUT);

  // BLE MIDI Setup
  BLEMIDI.setHandleConnected(onBleMidiConnected);
  BLEMIDI.setHandleDisconnected(onBleMidiDisconnected);

  MIDI.setHandleNoteOn(onBleMidiNoteOn);
  MIDI.setHandleNoteOff(onBleMidiNoteOff);
  MIDI.setHandleControlChange(onBleMidiControlChange);
  MIDI.setHandleProgramChange(onBleMidiProgramChange);
  MIDI.setHandleSystemExclusive(onBleMidiSysEx);

  // Listen on all MIDI channels
  MIDI.begin(MIDI_CHANNEL_OMNI);

  debugOutput("End setup");
}

// -----------------------------------------------------------------------------
//  LOOP
// -----------------------------------------------------------------------------
void loop() {
   MIDI.read();
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

void onBleMidiSysEx(byte* data, unsigned length) {
  if (length == 0)
    return;
  debugOutput(F("SYSEX: ("), false);
  debugOutput(String(length), false);
  debugOutput(F(" bytes) "), false);
  for (uint16_t i = 0; i < length; i++)
  {
    debugOutput(String(data[i], HEX), false);
    debugOutput(" ", false);
  }
  debugOutput("");

  // Check if data contains sysEx boundary bytes F0 and F7
  bool inArrayContainsBoundaries = 
    data[0] == MIDI_NAMESPACE::MidiType::SystemExclusiveStart &&
    data[length - 1] == MIDI_NAMESPACE::MidiType::SystemExclusiveEnd;

  SERIAL_MIDI_OUTPUT.sendSysEx(length, data, inArrayContainsBoundaries);
}
