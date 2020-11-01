#include <MIDI.h>
#include <HardwareSerial.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define LED 2
#define USB_MONITOR_BAUDRATE 115200
#define MIDI_BAUDRATE 31250
#define RXD2 16
#define TXD2 17

#define SERVICE_UUID "03b80e5a-ede8-4b33-a751-6ce34ec4c700"
#define CHARACTERISTIC_UUID "7772e5db-3868-4112-a1a9-f2669d106bf3"

bool debug = false;
int numClients = 0;

BLEServer *_server = nullptr;
BLEAdvertising *_advertising = nullptr;
BLECharacteristic *_characteristic = nullptr;

MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI);

// -----------------------------------------------------------------------------
//  FUNCTIONS
// -----------------------------------------------------------------------------
void ledState(bool state)
{
  if (state)
    digitalWrite(LED, HIGH);
  else
    digitalWrite(LED, LOW);
}

void debugOutput(String output)
{
  if (debug)
  {
    Serial.println(output);
  }
}

// -----------------------------------------------------------------------------
//  Callbacks
// -----------------------------------------------------------------------------
class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *_server)
  {
    numClients++;
    debugOutput("NumClients: " + String(numClients));
    ledState(numClients > 0);
  };

  void onDisconnect(BLEServer *_server)
  {
    if (numClients > 0)
      numClients--;
    debugOutput("NumClients: " + String(numClients));
    ledState(numClients > 0);
  }
};

class MyCharacteristicCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    std::string rxValue = pCharacteristic->getValue();

    if (rxValue.length() > 0)
    {
      debugOutput("Received Value: ");
      for (int i = 0; i < rxValue.length(); i++) {
        debugOutput(String(rxValue[i]));
        Serial2.write(rxValue[i]); // Proxy BLE to serial MIDI
      }
    }
  }
};

// -----------------------------------------------------------------------------
//  SETUP
// -----------------------------------------------------------------------------
void setup()
{
  // Serial (USB) for debug output
  Serial.begin(USB_MONITOR_BAUDRATE);

  // Serial2 (GPIO 16 & 17) for hardware-midi in/out
  Serial2.begin(MIDI_BAUDRATE, SERIAL_8N1, RXD2, TXD2);

  // LED for signalizing if BLE client is connected
  pinMode(LED, OUTPUT);

  // BLE Setup
  BLEDevice::init("BLE Midi Proxy");
  _server = BLEDevice::createServer();
  _server->setCallbacks(new MyServerCallbacks());

  auto service = _server->createService(BLEUUID(SERVICE_UUID));

  _characteristic = service->createCharacteristic(
      BLEUUID(CHARACTERISTIC_UUID),
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE |
          BLECharacteristic::PROPERTY_NOTIFY |
          BLECharacteristic::PROPERTY_WRITE_NR);
  _characteristic->addDescriptor(new BLE2902());
  _characteristic->setCallbacks(new MyCharacteristicCallbacks());

  auto _security = new BLESecurity();
  _security->setAuthenticationMode(ESP_LE_AUTH_BOND);

  service->start();

  _advertising = _server->getAdvertising();
  _advertising->addServiceUUID(service->getUUID());
  _advertising->setAppearance(0x00);
  _advertising->start();

  debugOutput("End Setup");
}

// -----------------------------------------------------------------------------
//  LOOP
// -----------------------------------------------------------------------------
void loop()
{
  /*
    * Nothing to do here because
    * we're working with callbacks
    */
}

// ====================================================================================
// Event handlers for incoming MIDI messages (via BLE)
// ====================================================================================

void onBleMidiConnected()
{
  numClients++;
  debugOutput("NumClients: " + String(numClients));
  ledState(numClients > 0);
}

void onBleMidiDisconnected()
{
  if (numClients > 0)
    numClients--;
  debugOutput("NumClients: " + String(numClients));
  ledState(numClients > 0);
}

void onBleMidiControlChange(byte channel, byte number, byte value)
{
  debugOutput("ControlChange, Channel: " + String(channel) +
              " number:" + String(number) +
              " value:" + String(value));
  MIDI.sendControlChange(number, value, channel);
}

void onBleMidiProgramChange(byte channel, byte number)
{
  debugOutput("ProgramChange, Channel: " + String(channel) +
              " number:" + String(number));
  MIDI.sendProgramChange(number, channel);
}

void onBleMidiNoteOn(byte channel, byte note, byte velocity)
{
  MIDI.sendNoteOn(note, velocity, channel);
}

void onBleMidiNoteOff(byte channel, byte note, byte velocity)
{
  MIDI.sendNoteOff(note, velocity, channel);
}
