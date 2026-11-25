Mirobot
=======

This is an Arduino library that allows you to control Mirobot (http://mirobot.io).

It can be used in one of two ways;
 - Controlling Mirobot directly from Arduino code (e.g. mirobot.forward(100)). The logic for how it moves is in the Arduino sketch. See the "basic_example" in the examples directory.
 - Receiving commands remotely. The logic for how it moves is somewhere else, most probably in the browser. See the "socket_example" in the examples directory.

Mirobot v3 (WiFi) firmware
--------------------------

`examples/firmware-v3` is the firmware for the ESP8266-based Mirobot v3. It runs its own WiFi network and serves
the browser UI (built-in drag and drop UI, Blockly, JavaScript, Python, Snap!, Point & Click and Remote Control).

### Requirements

- **Arduino IDE 2.x**
- **ESP8266 board package 3.x**: in Boards Manager, install "esp8266 by ESP8266 Community".
- **Libraries** (install with Library Manager):
  - **Marceau** 1.0.0 or later ([mimeindustries/marceau](https://github.com/mimeindustries/marceau)), the
    version that uses LittleFS
  - **ESP Async WebServer** by ESP32Async (3.6.0 or later)
  - **ESP Async TCP** by ESP32Async (2.0.0 or later). Don't use the older `me-no-dev` version.
  - **Async TCP** by ESP32Async. Marceau lists it as a dependency; it is only used on ESP32.

The AVR firmware (`firmware-v1`, `firmware-v2`) needs the older Marceau from before 1.0.0, which still supports AVR.

### Setup

1. Open **File → Examples → Mirobot → firmware-v3** and **File → Save As** to a normal sketch folder, so the sketch
   has its `data` folder next to it.
2. Select your ESP8266 board and port, and set **Tools → Flash Size** to a layout with a filesystem of at least
   1 MB (the UI takes about 800 KB once LittleFS rounds each file up to whole blocks), e.g. `4MB (FS:2MB OTA:~1019KB)`. Upload as normal.
3. Upload the `data` folder to the filesystem: install the LittleFS upload plugin from
   [arduino-littlefs-upload](https://github.com/earlephilhower/arduino-littlefs-upload/releases) (put the `.vsix`
   in `~/.arduinoIDE/plugins/` and restart the IDE), close the Serial Monitor, then press **Ctrl+Shift+P** and run
   **"Upload LittleFS to Pico/ESP8266/ESP32"**. Use the same Flash Size setting as for the sketch.
4. Join the `Mirobot-XXXX` network and open `http://192.168.4.1/`.

### The UI files

`web.h` and `data/` are built from [mime-app-libs](https://github.com/mimeindustries/mime-app-libs):

    npm run build:mirobot -- --out ../mirobot-arduino/examples/firmware-v3

### Over-the-air updates

`updateFirmware` and `updateUI` fetch `http://downloads.mearm.com/Mirobot/WiFi/v4/firmware-latest.bin` and
`.../v4/ui-latest.bin`. Build `ui-latest.bin` as a **LittleFS** image, not SPIFFS, or the robot won't be able to
read its filesystem after an update:

    mklittlefs -c data -p 256 -b 8192 -s 2072576 ui-latest.bin

`2072576` is the FS size for the 4MB / FS:2MB layout. Robots running older firmware still fetch the SPIFFS files
from `downloads.mime.co.uk/Mirobot/v3/`, so leave those in place.

### Changes for ESP8266 core 3.x (2026)

- **Marceau 1.0.0.** Web server and TCP now come from the ESP32Async libraries, and the UI lives on LittleFS
  instead of SPIFFS.
- **Discovery rewritten.** It used `espconn.h` from the legacy SDK, which clashes with lwIP2 in core 3.x. It now
  uses ESPAsyncTCP's `AsyncClient` and stays non-blocking.
- **OTA calls updated.** `ESPhttpUpdate.update(url)` and `updateSpiffs(url)` were removed from the core; the
  firmware now calls `update(client, url)` and `updateFS(client, url)`, from the v4 location above.
- **`IRAM_ATTR`** replaces the deprecated `ICACHE_RAM_ATTR` in the stepper interrupt code.
- **Macro clashes.** Mirobot's `SETTINGS_VERSION` and `SERIAL_BUFFER_LENGTH` are now `MIROBOT_`-prefixed, so they
  no longer redefine Marceau's macros of the same names.
- **Access point name buffer.** It was 11 bytes for the 13-byte `Mirobot-XXXX`, so naming the network wrote past it.
- **Settings reset once.** Marceau's settings are larger and have a new version number, so the first boot after
  flashing resets the WiFi settings and Mirobot's calibration to their defaults.
