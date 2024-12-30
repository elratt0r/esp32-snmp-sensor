#include "main.h"

void ota_setup(void)
{
    Serial.println("[OTA] setup begin");

    // set hostname
    ArduinoOTA.setHostname(cfg.wifi_hostname);

    // init ota
    ArduinoOTA
        .onStart([]() {
            String type;
            if (ArduinoOTA.getCommand() == U_FLASH)
                type = "sketch";
            else // U_SPIFFS
                type = "filesystem";

            // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
            Serial.println("[OTA] start updating " + type);
        })
        .onEnd([]() {
            Serial.println("\r\n[OTA]End");
        })
        .onProgress([](unsigned int progress, unsigned int total) {
            Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
        })
        .onError([](ota_error_t error) {
            Serial.printf("[OTA] error[%u]: ", error);
            if (error == OTA_AUTH_ERROR)
                Serial.println("Auth Failed");
            else if (error == OTA_BEGIN_ERROR)
                Serial.println("Begin Failed");
            else if (error == OTA_CONNECT_ERROR)
                Serial.println("Connect Failed");
            else if (error == OTA_RECEIVE_ERROR)
                Serial.println("Receive Failed");
            else if (error == OTA_END_ERROR)
                Serial.println("End Failed");
            else
                Serial.println("Unknown Error");
        });
    ArduinoOTA.begin();

    Serial.println("[OTA] setup done");
}

void ota_loop(void)
{
    ArduinoOTA.handle();
}