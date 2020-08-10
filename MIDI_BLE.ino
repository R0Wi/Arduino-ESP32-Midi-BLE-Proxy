#include "lib/MIDI-BLE/src/BleMidi.h" 
#include "lib/MIDI-library/src/MIDI.h"
#include <HardwareSerial.h>

#define LED 2
#define USB_MONITOR_BAUDRATE 115200
#define MIDI_BAUDRATE 31250
#define RXD2 16
#define TXD2 17

bool debug = false;
int numClients = 0;

BLEMIDI_CREATE_INSTANCE(bm);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI);

// -----------------------------------------------------------------------------
//  SETUP
// -----------------------------------------------------------------------------
void setup() {
  // Serial (USB) for debug output
  Serial.begin(USB_MONITOR_BAUDRATE);
  
  // Serial2 (GPIO 16 & 17) for hardware-midi in/out
  Serial2.begin(MIDI_BAUDRATE, SERIAL_8N1, RXD2, TXD2);

  // LED for signalizing if BLE client is connected
  pinMode(LED, OUTPUT);

  // BLE Setup
  bm.begin("BLE Midi Proxy");

  bm.onConnected(onBleMidiConnected);
  bm.onDisconnected(onBleMidiDisconnected);

  bm.setHandleNoteOn(onBleMidiNoteOn);
  bm.setHandleNoteOff(onBleMidiNoteOff);
  bm.setHandleControlChange(onBleMidiControlChange);
  bm.setHandleProgramChange(onBleMidiProgramChange);

  debugOutput("End Setup");
}

// -----------------------------------------------------------------------------
//  LOOP
// -----------------------------------------------------------------------------
void loop() {
   /*
    * Nothing to do here because
    * we're working with callbacks
    */
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

void onBleMidiControlChange(byte channel, byte number, byte value){
  debugOutput("ControlChange, Channel: " + String(channel) + 
               " number:" + String(number) +
               " value:" + String(value));
  MIDI.sendControlChange(number, value, channel);
}

void onBleMidiProgramChange(byte channel, byte number){
  debugOutput("ProgramChange, Channel: " + String(channel) + 
              " number:" + String(number));
  MIDI.sendProgramChange(number, channel);
}

void onBleMidiNoteOn(byte channel, byte note, byte velocity) {
  MIDI.sendNoteOn(note, velocity, channel);
}

void onBleMidiNoteOff(byte channel, byte note, byte velocity) {
  MIDI.sendNoteOff(note, velocity, channel);
}
