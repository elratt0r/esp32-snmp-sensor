#include "main.h"

static unsigned long lastUptimeUpdateTime = 0;
static unsigned long lastSensorUpdateTime = 0;

WiFiUDP udp;
SNMPAgent snmp = SNMPAgent();

// RFC1213
std::string sysDescr = "ESP32-SNMP-SENSOR v" VERSION_STR " compiled on " __DATE__ " " __TIME__;
std::string sysObjectID = ".1.3.6.1.4.1.54618.1.11";
uint32_t sysUptime = 0;
char* sysContact = cfg.snmp_contact;
char* sysName = cfg.wifi_hostname;
char* sysLocation = cfg.snmp_location;
int sysServices = 0x41; // Physical and Application

// dummy strings
std::string empty_str = "";
std::string dummyObjectID = ".1.3.6.1.4.1.54618.2.11";
int empty_int = 0;
uint32_t empty_u32 = 0;

// entPhySensorType
// other(1), unknown(2), voltsAC(3), voltsDC(4), amperes(5), watts(6), hertz(7), celsius(8), percentRH(9), rpm(10), cmm(11), truthvalue(12)
int entPhySensorType_celsius = 8;
int entPhySensorType_percentRH = 9;

// entPhySensorScale
// yocto(1), zepto(2), atto(3), femto(4), pico(5), nano(6), micro(7), milli(8), units(9), kilo(10), mega(11), giga(12), tera(13), exa(14), peta(15), zetta(16), yotta(17)
int entPhySensorScale_milli = 8;
int entPhySensorScale_units = 9;

// entPhySensorOperStatus
// ok(1), unavailable(2), nonoperational(3)
int entPhySensorOperStatus_ok = 1;

// entPhySensorUnitsDisplay
std::string entPhySensorUnitsDisplay_Celsius = "Celsius";
std::string entPhySensorUnitsDisplay_percentRH = "%";

// sensors
typedef struct {
    int index;
    std::string name;
    void* pVar;
    int (*get)(float);
    int last_value;
    uint32_t last_update;
} snmp_sensor_t;

snmp_sensor_t sensor_list[] = {
    { 1, "Temperature", &sensor1.temperature, &float2mint, 0, 0 },
    { 2, "Humidity", &sensor1.humidity, &float2mint, 0, 0 }
};

void snmp_setup(void)
{
    Serial.println("[SNMP] setup begin");

    // setup snmp class
    snmp.setUDP(&udp);
    snmp.setReadOnlyCommunity(cfg.snmp_ro_community);
    snmp.setReadWriteCommunity(cfg.snmp_rw_community);
    snmp.begin();

	// add system mib
    addRFC1213MIBHandler();

    // add entities
    // MACRO: addENTITYMIBHandler(id,index,name)
    addENTITYMIBHandler(1, &(sensor_list[0].index), sensor_list[0].name);
    addENTITYMIBHandler(2, &(sensor_list[1].index), sensor_list[1].name);

	// add sensor entities
    // MACRO: addENTITYSENSORMIBHandler(id,type,scale,units,value,last_ts)
    addENTITYSENSORMIBHandler(1, &entPhySensorType_celsius, &entPhySensorScale_milli, entPhySensorUnitsDisplay_Celsius, &(sensor_list[0].last_value), &(sensor_list[0].last_update));
    addENTITYSENSORMIBHandler(2, &entPhySensorType_percentRH, &entPhySensorScale_milli, entPhySensorUnitsDisplay_percentRH, &(sensor_list[1].last_value), &(sensor_list[1].last_update));

    snmp.sortHandlers();

    Serial.println("[SNMP] setup done");
}

void snmp_loop(void)
{
    // This must be called as often as possible to process incoming requests
    snmp.loop();

    // Check for write
    if (snmp.setOccurred) {
        Serial.println("[SNMP] snmpset received, writing config");
        write_config();
        snmp.resetSetOccurred();
    }

    // Periodically update Uptime.
    if (millis() - lastUptimeUpdateTime >= UPTIME_UPDATE_INTERVAL) {
        lastUptimeUpdateTime += UPTIME_UPDATE_INTERVAL;
        sysUptime = getUptime();
    }

    // Read Sensor Values
    if (millis() - lastSensorUpdateTime >= SENSOR_UPDATE_INTERVAL) {
        lastSensorUpdateTime += SENSOR_UPDATE_INTERVAL;
        for (int i = 0; i < NUM(sensor_list); i++) {
            sensor_list[i].last_update = sysUptime;
            sensor_list[i].last_value = sensor_list[i].get(*static_cast<float*>(sensor_list[i].pVar));
            // Serial.printf("[SNMP] ID/Name: %d/%s\r\n", sensor_list[i].index, sensor_list[i].name.c_str());
            // Serial.printf("[SNMP] Update: %d\r\n", sensor_list[i].last_update);
            // Serial.printf("[SNMP] Value: %d\r\n", sensor_list[i].last_value);
        }
    }
}

void addRFC1213MIBHandler(void)
{
    snmp.addReadOnlyStaticStringHandler(".1.3.6.1.2.1.1.1.0", sysDescr);
    snmp.addOIDHandler(".1.3.6.1.2.1.1.2.0", sysObjectID);
    snmp.addTimestampHandler(".1.3.6.1.2.1.1.3.0", &sysUptime);
    snmp.addReadWriteStringHandler(".1.3.6.1.2.1.1.4.0", &sysContact, 128, true);
    snmp.addReadWriteStringHandler(".1.3.6.1.2.1.1.5.0", &sysName, 128, false);
    snmp.addReadWriteStringHandler(".1.3.6.1.2.1.1.6.0", &sysLocation, 128, true);
    snmp.addIntegerHandler(".1.3.6.1.2.1.1.7.0", &sysServices);
}

// Utilities
uint64_t uptimeMillis()
{
    return (esp_timer_get_time() / 1000);
}

uint32_t getUptime()
{
    return (uint32_t)(uptimeMillis() / 10);
}

int float2mint(float a)
{
    return (int)(a * 1000);
}
