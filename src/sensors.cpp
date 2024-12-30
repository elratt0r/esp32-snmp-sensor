#include "main.h"

dht22_t sensor1;

DHT_Unified dht(DHT_PIN, DHT22);
unsigned long minDelay = 5000;
unsigned long lastRead = 0;

void sensors_setup(void)
{
    Serial.println("[SENSOR] setup begin");

    // start sensor
    dht.begin();

    // Set delay between sensor readings based on sensor details.
    sensor_t sensor;
    dht.temperature().getSensor(&sensor);

    unsigned long delayMS = sensor.min_delay / 1000;
    if (delayMS > minDelay) {
        minDelay = delayMS;
    }
    Serial.printf("[SENSOR] setting sensor delay to: %lums\r\n", minDelay);

    Serial.println("[SENSOR] setup done");
}

void sensors_loop()
{
    // check read time
    if ((millis() - lastRead) < minDelay) {
        // Serial.printf("%lu - %lu < %lu\r\n", millis(), lastRead, minDelay);
        return;
    }

    // show uptime
    Serial.print(F("[SENSOR] uptime: "));
    Serial.print(millis());
    Serial.println(F("ms"));

    // Get temperature event and print its value.
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature)) {
        Serial.println(F("[SENSOR] error reading temperature!"));
    } else {
        Serial.print(F("[SENSOR] temperature: "));
        Serial.print(event.temperature);
        Serial.println(F("°C"));
        sensor1.temperature = event.temperature;
    }
    // Get humidity event and print its value.
    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity)) {
        Serial.println(F("[SENSOR] error reading humidity!"));
    } else {
        Serial.print(F("[SENSOR] humidity: "));
        Serial.print(event.relative_humidity);
        Serial.println(F("%"));
        sensor1.humidity = event.relative_humidity;
    }

    lastRead = millis();
}
