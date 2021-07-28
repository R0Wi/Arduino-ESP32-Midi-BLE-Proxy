#include <BLEMIDI_Transport.h>
#include <hardware/BLEMIDI_ESP32.h>

// Uncomment to get debug output via USB serial
#define DEBUG

// BLE libs come from ESP32 board repo
// https://github.com/espressif/arduino-esp32
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>



// Lib for parsing BLE MIDI data messages
#include "lib/blidino/nRF51822-BLEMIDI/BLEParser.h"
// Class which writes parsed MIDI messages to serial
#include "SerialMidiReceiver.h"
// Logging via USB serial
#include "SerialLogger.h"

#define BLE_NAME "BLE Midi Proxy"
#define LED 2
#define USB_MONITOR_BAUDRATE 115200
#define MIDI_BAUDRATE 31250
#define RXD2 16
#define TXD2 17

// As specified in
// Specification for MIDI over Bluetooth Low Energy (BLE-MIDI)
// Version 1.0a, NOvember 1, 2015
// 3. BLE Service and Characteristics Definitions
#define SERVICE_UUID "03b80e5a-ede8-4b33-a751-6ce34ec4c700"
#define CHARACTERISTIC_UUID "7772e5db-3868-4112-a1a9-f2669d106bf3"

int numClients = 0;

BLEMIDI_CREATE_INSTANCE(BLE_NAME, _bleMidiInstance)

SerialLogger *_logger = nullptr;
BLEServer *_server = nullptr;
BLEAdvertising *_advertising = nullptr;
BLECharacteristic *_characteristic = nullptr;
mfk::midi::BLEMIDIParser<256, SerialMidiReceiver> _midiParser;

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

// -----------------------------------------------------------------------------
//  Callbacks
// -----------------------------------------------------------------------------
class MyServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer, esp_ble_gatts_cb_param_t *param)
    {
      numClients++;
      _logger->logDebug("NumClients: " + String(numClients));
      ledState(numClients > 0);
    };

    void onDisconnect(BLEServer *pServer)
    {
      if (numClients > 0)
        numClients--;
      _logger->logDebug("NumClients: " + String(numClients));
      ledState(numClients > 0);
    }
};

class MyCharacteristicCallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic)
    {
      std::string rxValue = pCharacteristic->getValue();

      if (rxValue.length() <= 0)
        return;

      _logger->logDebug("Received Value: ");
      _logger->logDebug(String(rxValue[0], HEX));
      for (int i = 1; i < rxValue.length(); i++) {
        _logger->logDebug(String(rxValue[i], HEX));
        _midiParser.parseMidiEvent(rxValue[0], rxValue[i]);
      }
    }
};

// -----------------------------------------------------------------------------
//  SETUP
// -----------------------------------------------------------------------------
void setup()
{
  // Serial (USB) for debug output
#ifdef DEBUG
  Serial.begin(USB_MONITOR_BAUDRATE);
#endif

  _logger = new SerialLogger();
  _logger->logDebug("Begin setup");

  // LED for signalizing if BLE client is connected
  pinMode(LED, OUTPUT);

  BLEMIDI.setHandleConnected(OnConnected);
  BLEMIDI.setHandleDisconnected(OnDisconnected);
 
  _bleMidiInstance.setHandleSystemExclusive(OnMidiSysEx);

  // Listen for MIDI messages on channel 1
  _bleMidiInstance.begin();

  // Setup for parsing MIDI messages
  // setupMidiMessagesParser();

  // // BLE setup
  // setupBleServer();

  _logger->logDebug("End setup");
}

void setupMidiMessagesParser() {
    // Serial2 (GPIO 16 & 17) for hardware-midi in/out
  Serial2.begin(MIDI_BAUDRATE, SERIAL_8N1, RXD2, TXD2);

  SerialMidiReceiver* receiver = new SerialMidiReceiver(&Serial2, _logger);
  _midiParser.setReceiver(receiver);
}

void setupBleServer() {
  // Strongly inspired by
  // https://github.com/lathoub/Arduino-BLE-MIDI/blob/aa0f6bc44aa59995fc91493a046959619efa9df5/src/hardware/BLEMIDI_ESP32.h#L113
  BLEDevice::init(BLE_NAME);
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
}

// -----------------------------------------------------------------------------
//  LOOP
// -----------------------------------------------------------------------------
void loop()
{
  _bleMidiInstance.read();
  /*
      Nothing to do here because
      we're working with callbacks
  */
}
