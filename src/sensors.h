#ifndef SENSORS_H
#define SENSORS_H

typedef struct {
    float temperature;
    float humidity;
} dht22_t;

extern dht22_t sensor1;

void sensors_setup(void);
void sensors_loop(void);

#endif
