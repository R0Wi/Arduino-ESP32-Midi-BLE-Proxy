# Arduino-ESP32-Midi-BLE-Proxy

## What is it?
This code makes your ESP32 act as a simple proxy which takes MIDI
commands from a Bluetooth Low Engergy (BLE) device (like a tablet or smartphone)
and sends the MIDI commands out to the gear which is wired
to your ESP32. This code does not need any additional libraries,
only the ESP32 core (board) is needed like described below.

## Hardware
<p align="center">
  <img width="75%" src="doc/img/midi_pins.jpg" alt="MIDI pins">
</p>
<p align="center">
  <img width="75%" src="doc/img/midi_spec.png" alt="MIDI spec">
</p>
<p align="center">
  <img width="75%" src="doc/img/platine.jpg" alt="Platine">
</p>
<p align="center">
  <img width="75%" src="doc/img/boxed.jpg" alt="Boxed">
</p>

<p align="center">
  <img width="75%" src="doc/img/esp_32_spec.jpg" alt="ESP 32 spec">
</p>

## Compile
You can compile the source code like any other Arduino sketch by using the Arduino IDE. If you like 
you can also use VSCode in combination with the [Arduino plugin](https://marketplace.visualstudio.com/items?itemName=vsciot-vscode.vscode-arduino). A matching board configuration is
available in `.vscode/arduino.json`.

1. Create a directory `MIDI_BLE_PROXY` and clone this repo 
```bash
mkdir MIDI_BLE_PROXY
cd MIDI_BLE_PROXY
git clone https://github.com/R0Wi/Arduino-ESP32-Midi-BLE-Proxy.git .
```
2. Open Arduino IDE and install ESP32 board
<p align="center">
  <img width="75%" src="doc/img/board.png" alt="Board install">
</p>

> This step will also install the BLE core libraries from https://github.com/espressif/arduino-esp32/tree/master/libraries/BLE used in the sourcecode. Read more about boards [here](https://support.arduino.cc/hc/en-us/articles/360012076960-Where-are-the-installed-cores-located-).

3. Under `Tools`->`Board` choose `ESP32 Dev Module`
4. Compile & have fun.