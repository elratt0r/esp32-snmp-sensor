#include "main.h"

void setup()
{
    // wait for the hardware to initialize
    delay(500);

    // log setup start
    Serial.begin(115200);
    Serial.println("[SETUP] begin");

    // read config
    config_setup();

    // button
    button_setup();

    // network
    network_setup();

    // OTA
    ota_setup();

    // webserver
    webserver_setup();

    // sensors
    sensors_setup();

    // snmp
    snmp_setup();

    // ready
    Serial.println("[SETUP] done");
}

void loop()
{
    // network
    network_loop();

    // ota
    ota_loop();

    // button
    button_loop();

    // sensors
    sensors_loop();

    // snmp
    snmp_loop();
}
