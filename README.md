# ESP32 based SNMP sensor

## Features

* Expose sensors via SNMP
* Fallback AP provided if configured
* OTA updates via platformio or webpage upload

### Implemented Sensors

* DHT22

## Setup

Have an ESP32 at hand. I use a [ESP32-POE](https://www.olimex.com/Products/IoT/ESP32/ESP32-POE/open-source-hardware) from Olimex.

Install [PlatformIO](https://platformio.org/).

Build release, flash and monitor output:
```shell
pio run -t upload -t monitor
```

Default mode is ethernet connection with DHCP client enabled.  
Have a look for the assigned IP in the serial log and connect to it via a web browser.

### HTML pages
Regenerate include files:
```shell
cd web
sh regen.sh
```
You have to rebuild the project after that.

To test/debug the website:
```shell
cd web
sh container.sh
```
This starts a nginx container with podman on localhost:8888.  
Events and actions don't work, but you can test the javascript and functions.  
(settings.json is a dummy file for that)

### Build options
Build release:
```shell
pio run
```

Build debug:
```shell
pio run -e debug
```

Upload via serial:
```shell
pio run -t upload
```

Upload via OTA:
```shell
pio run -t upload --upload_port <hostname.local>
```

To monitor serial output without re-flashing:
```shell
pio device monitor
```

## Credits

* HTML design taken from [Tasmota](https://tasmota.github.io/)
* Libraries:
  * [ArduinoJson](https://github.com/bblanchon/ArduinoJson)
  * [AsyncTCP](https://github.com/me-no-dev/AsyncTCP)
  * [ESP Async WebServer](https://github.com/me-no-dev/ESPAsyncWebServer)
  * [SNMP_Agent](https://github.com/0neblock/Arduino_SNMP)
  * [Adafruit Unified Sensor](https://github.com/adafruit/Adafruit_Sensor)
  * [DHT sensor library](https://github.com/adafruit/DHT-sensor-library)
