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


#define BUTTON_PIN 34
#define DHT_PIN 5

#ifndef VERSION_STR
#define VERSION_STR "fixme"
#endif

// settings
extern settings_t cfg;

#endif
