#ifndef CFG_H
#define CFG_H

#define EEPROM_SIZE 4096
#define OPMODE_WIFI_ACCESSPOINT 0
#define OPMODE_WIFI_STATION 1
#define OPMODE_ETH_CLIENT 2

// settings struct version
#define cfg_ver_num 4

typedef struct {
    // version 1
    byte version;
    char wifi_ssid[32];         // was: 33
    char wifi_secret[64];       // was: 65
    char wifi_hostname[32];     // was: 256
    byte wifi_opmode;
    bool wifi_powersave;
    bool wifi_ap_fallback;

    // version 2
    bool static_ip;
    char ip_addr[16];
    char ip_gw[16];
    char ip_netmask[16];
    char ip_dns[16];

    // version 3
    char snmp_contact[129];
    char snmp_location[129];
    char snmp_ro_community[65];
    char snmp_rw_community[65];
} settings_t;

void config_setup(void);
void write_config(void);
void read_config(void);
void dump_config(void);
String get_settings(void);
boolean parse_settings(JsonDocument json);
void factory_reset(const String& reason);
void esp_reboot(const String& reason);

#endif
