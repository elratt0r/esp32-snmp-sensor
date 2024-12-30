#ifndef _MAIN_H
#define _MAIN_H

#include <Arduino.h>
#include <WiFi.h>
#include <ETH.h>
#include <EEPROM.h>
#include <ArduinoOTA.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SNMP_Agent.h>
#include <DHT.h>
#include <DHT_U.h>
#include "cfg.h"
#include "ota.h"
#include "network.h"
#include "webserver.h"
#include "button.h"
#include "sensors.h"
#include "snmp.h"

/*
Board: OLIMEX ESP32-POE
Arduino IDE Libraries:
- esp32 by Espressif Systems @ 3.0.7
- ArduinoJson by Benoit Blanchon @ 7.2.1
- AsyncTCP by Me-No-Dev @ 3.3.1
- ESP Async WebServer by Me-No-Dev @ 3.4.5
- SNMP_Agent by Aidan Cyr @ 2.1.0
- Adafruit Unified Sensor by Adafruit @ 1.1.14
- DHT sensor library by Adafruit @ 1.4.6
*/ 

#define BUTTON_PIN 34
#define DHT_PIN 5

#ifndef VERSION_STR
#define VERSION_STR "fixme"
#endif

// settings
extern settings_t cfg;

#endif
