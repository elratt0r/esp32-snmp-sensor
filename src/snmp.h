#ifndef SNMP_H
#define SNMP_H

#define UPTIME_UPDATE_INTERVAL 1000 // ms = 1 second
#define SENSOR_UPDATE_INTERVAL 5000 // ms = 5 seconds

void snmp_setup(void);
void snmp_loop(void);

void addRFC1213MIBHandler(void);

uint64_t uptimeMillis(void);
uint32_t getUptime(void);
int float2mint(float);

// macros
#define CC(a, b) a##b
#define NUM(a) (sizeof(a) / sizeof(*a))

// ENTITYMIB
#define oidentPhysicalIndex ".1.3.6.1.2.1.47.1.1.1.1.1."
#define oidentPhysicalDescr ".1.3.6.1.2.1.47.1.1.1.1.2."
#define oidentPhysicalVendorType ".1.3.6.1.2.1.47.1.1.1.1.3."
#define oidentPhysicalContainedIn ".1.3.6.1.2.1.47.1.1.1.1.4."
#define oidentPhysicalClass ".1.3.6.1.2.1.47.1.1.1.1.5."
#define oidentPhysicalParentRelPos ".1.3.6.1.2.1.47.1.1.1.1.6."
#define oidentPhysicalName ".1.3.6.1.2.1.47.1.1.1.1.7."
#define oidentPhysicalHardwareRev ".1.3.6.1.2.1.47.1.1.1.1.8."
#define oidentPhysicalFirmwareRev ".1.3.6.1.2.1.47.1.1.1.1.9."
#define oidentPhysicalSoftwareRev ".1.3.6.1.2.1.47.1.1.1.1.10."
#define oidentPhysicalSerialNum ".1.3.6.1.2.1.47.1.1.1.1.11."
#define oidentPhysicalMfgName ".1.3.6.1.2.1.47.1.1.1.1.12."
#define oidentPhysicalModelName ".1.3.6.1.2.1.47.1.1.1.1.13."
#define oidentPhysicalAlias ".1.3.6.1.2.1.47.1.1.1.1.14."
#define oidentPhysicalAssetID ".1.3.6.1.2.1.47.1.1.1.1.15."
#define oidentPhysicalIsFRU ".1.3.6.1.2.1.47.1.1.1.1.16."
#define oidentPhysicalMfgDate ".1.3.6.1.2.1.47.1.1.1.1.17."
#define oidentPhysicalUris ".1.3.6.1.2.1.47.1.1.1.1.18."

#define addENTITYMIBHandler(id, index, name)                                       \
    {                                                                              \
    snmp.addIntegerHandler(oidentPhysicalIndex #id, index);                        \
    snmp.addReadOnlyStaticStringHandler(oidentPhysicalDescr #id, name);            \
    snmp.addOIDHandler(oidentPhysicalVendorType #id, dummyObjectID);               \
    snmp.addIntegerHandler(oidentPhysicalContainedIn #id, &empty_int);             \
    snmp.addIntegerHandler(oidentPhysicalClass #id, &empty_int);                   \
    snmp.addIntegerHandler(oidentPhysicalParentRelPos #id, &empty_int);            \
    snmp.addReadOnlyStaticStringHandler(oidentPhysicalName #id, empty_str);        \
    snmp.addReadOnlyStaticStringHandler(oidentPhysicalHardwareRev #id, empty_str); \
    snmp.addReadOnlyStaticStringHandler(oidentPhysicalFirmwareRev #id, empty_str); \
    snmp.addReadOnlyStaticStringHandler(oidentPhysicalSoftwareRev #id, empty_str); \
    snmp.addReadOnlyStaticStringHandler(oidentPhysicalSerialNum #id, empty_str);   \
    snmp.addReadOnlyStaticStringHandler(oidentPhysicalMfgName #id, empty_str);     \
    snmp.addReadOnlyStaticStringHandler(oidentPhysicalModelName #id, empty_str);   \
    snmp.addReadOnlyStaticStringHandler(oidentPhysicalAlias #id, empty_str);       \
    snmp.addReadOnlyStaticStringHandler(oidentPhysicalAssetID #id, empty_str);     \
    snmp.addIntegerHandler(oidentPhysicalIsFRU #id, &empty_int);                   \
    snmp.addReadOnlyStaticStringHandler(oidentPhysicalMfgDate #id, empty_str);     \
    snmp.addReadOnlyStaticStringHandler(oidentPhysicalUris #id, empty_str);        \
	}

// ENTITYSENSORMIB
#define oidentPhySensorType ".1.3.6.1.2.1.99.1.1.1.1."
#define oidentPhySensorScale ".1.3.6.1.2.1.99.1.1.1.2."
#define oidentPhySensorPrecision ".1.3.6.1.2.1.99.1.1.1.3."
#define oidentPhySensorValue ".1.3.6.1.2.1.99.1.1.1.4."
#define oidentPhySensorOperStatus ".1.3.6.1.2.1.99.1.1.1.5."
#define oidentPhySensorUnitsDisplay ".1.3.6.1.2.1.99.1.1.1.6."
#define oidentPhySensorValueTimeStamp ".1.3.6.1.2.1.99.1.1.1.7."
#define oidentPhySensorValueUpdateRate ".1.3.6.1.2.1.99.1.1.1.8."

#define addENTITYSENSORMIBHandler(id, type, scale, units, value, last_ts)              \
	{                                                                                  \
    snmp.addIntegerHandler(oidentPhySensorType #id, type);                             \
    snmp.addIntegerHandler(oidentPhySensorScale #id, scale);                           \
    snmp.addIntegerHandler(oidentPhySensorPrecision #id, &empty_int);                  \
    snmp.addIntegerHandler(oidentPhySensorValue #id, value);                           \
    snmp.addIntegerHandler(oidentPhySensorOperStatus #id, &entPhySensorOperStatus_ok); \
    snmp.addReadOnlyStaticStringHandler(oidentPhySensorUnitsDisplay #id, units);       \
    snmp.addTimestampHandler(oidentPhySensorValueTimeStamp #id, last_ts);              \
    snmp.addGaugeHandler(oidentPhySensorValueUpdateRate #id, &empty_u32);              \
	}

#endif