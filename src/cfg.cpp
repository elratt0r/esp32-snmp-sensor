#include "main.h"

settings_t cfg;

void config_setup(void)
{
    EEPROM.begin(EEPROM_SIZE);
    read_config();
}

void write_config(void)
{
    EEPROM.writeBytes(0, &cfg, sizeof(cfg));
    EEPROM.commit();
}

void read_config(void)
{
    // read eeprom
    EEPROM.readBytes(0, &cfg, sizeof(cfg));

    // different config version?
    if (cfg.version != cfg_ver_num) {
        if (cfg.version == 0xff) {
            uint8_t mac[10];
            WiFi.macAddress(mac);
            snprintf(cfg.wifi_ssid, MAX_SSID_LEN, "OSENS-%02X%02X%02X", mac[3], mac[4], mac[5]);
            snprintf(cfg.wifi_secret, MAX_PASSPHRASE_LEN, "");
            snprintf(cfg.wifi_hostname, MAX_SSID_LEN, cfg.wifi_ssid);
            cfg.wifi_opmode = OPMODE_ETH_CLIENT;
            cfg.wifi_powersave = false;
            cfg.wifi_ap_fallback = true;
        }

        if (cfg.version == 0xff || cfg.version < 2) {
            cfg.static_ip = false;
            cfg.ip_addr[0] = 0;
            cfg.ip_gw[0] = 0;
            cfg.ip_netmask[0] = 0;
            cfg.ip_dns[0] = 0;
        }

        if (cfg.version == 0xff || cfg.version < 3) {
            snprintf(cfg.snmp_contact, sizeof(cfg.snmp_contact), "admin");
            snprintf(cfg.snmp_location, sizeof(cfg.snmp_location), "");
            snprintf(cfg.snmp_ro_community, sizeof(cfg.snmp_ro_community) ,"public");
            snprintf(cfg.snmp_rw_community, sizeof(cfg.snmp_rw_community) ,"private");
        }

        cfg.version = cfg_ver_num;
        write_config();
    }

    // dump info to serial
    dump_config();
}

void dump_config(void)
{
    Serial.printf("Settings:\r\n");
    Serial.printf("cfg version      : %d\r\n", cfg.version);
    Serial.printf("ssid             : %s\r\n", cfg.wifi_ssid);
    Serial.printf("wifi_secret      : %s\r\n", cfg.wifi_secret);
    Serial.printf("wifi_hostname    : %s\r\n", cfg.wifi_hostname);
    Serial.printf("wifi_opmode      : %d\r\n", cfg.wifi_opmode);
    Serial.printf("wifi_powersave   : %d\r\n", cfg.wifi_powersave);
    Serial.printf("wifi_ap_fallback : %d\r\n", cfg.wifi_ap_fallback);
    Serial.printf("static_ip        : %d\r\n", cfg.static_ip);
    Serial.printf("ip_addr          : %s\r\n", cfg.ip_addr);
    Serial.printf("ip_gw            : %s\r\n", cfg.ip_gw);
    Serial.printf("ip_netmask       : %s\r\n", cfg.ip_netmask);
    Serial.printf("ip_dns           : %s\r\n", cfg.ip_dns);
    Serial.printf("snmp_contact     : %s\r\n", cfg.snmp_contact);
    Serial.printf("snmp_location    : %s\r\n", cfg.snmp_location);
    Serial.printf("snmp_ro_community: %s\r\n", cfg.snmp_ro_community);
    Serial.printf("snmp_rw_community: %s\r\n", cfg.snmp_rw_community);
}

String get_settings(void)
{
    JsonDocument json;
    json["version"] = VERSION_STR;
    json["wifi_hostname"] = cfg.wifi_hostname;
    json["wifi_ssid"] = cfg.wifi_ssid;
    json["wifi_opmode"] = cfg.wifi_opmode;
    json["wifi_powersave"] = cfg.wifi_powersave;
    json["wifi_ap_fallback"] = cfg.wifi_ap_fallback;
    json["wifi_secret"] = cfg.wifi_secret;
    json["static_ip"] = cfg.static_ip;
    json["ip_addr"] = cfg.ip_addr;
    json["ip_gw"] = cfg.ip_gw;
    json["ip_netmask"] = cfg.ip_netmask;
    json["ip_dns"] = cfg.ip_dns;
    json["snmp_contact"] = cfg.snmp_contact;
    json["snmp_location"] = cfg.snmp_location;
    json["snmp_ro_community"] = cfg.snmp_ro_community;
    json["snmp_rw_community"] = cfg.snmp_rw_community;

    String output;
    serializeJson(json, output);
    return output;
}

boolean parse_settings(JsonDocument json)
{
    if (json["wifi_hostname"].is<const char*>())
        snprintf(cfg.wifi_hostname, sizeof(cfg.wifi_hostname), "%s", (const char*)json["wifi_hostname"]);
    if (json["wifi_ssid"].is<const char*>())
        snprintf(cfg.wifi_ssid, sizeof(cfg.wifi_ssid), "%s", (const char*)json["wifi_ssid"]);
    if (json["wifi_opmode"].is<int>())
        cfg.wifi_opmode = json["wifi_opmode"];
    if (json["wifi_powersave"].is<bool>())
        cfg.wifi_powersave = json["wifi_powersave"];
    if (json["wifi_ap_fallback"].is<bool>())
        cfg.wifi_ap_fallback = json["wifi_ap_fallback"];
    if (json["wifi_secret"].is<const char*>())
        snprintf(cfg.wifi_secret, sizeof(cfg.wifi_secret), "%s", (const char*)json["wifi_secret"]);
    if (json["static_ip"].is<bool>())
        cfg.static_ip = json["static_ip"];
    if (json["ip_addr"].is<const char*>())
        snprintf(cfg.ip_addr, sizeof(cfg.ip_addr), "%s", (const char*)json["ip_addr"]);
    if (json["ip_gw"].is<const char*>())
        snprintf(cfg.ip_gw, sizeof(cfg.ip_gw), "%s", (const char*)json["ip_gw"]);
    if (json["ip_netmask"].is<const char*>())
        snprintf(cfg.ip_netmask, sizeof(cfg.ip_netmask), "%s", (const char*)json["ip_netmask"]);
    if (json["ip_dns"].is<const char*>())
        snprintf(cfg.ip_dns, sizeof(cfg.ip_dns), "%s", (const char*)json["ip_dns"]);
    if (json["snmp_contact"].is<const char*>())
        snprintf(cfg.snmp_contact, sizeof(cfg.snmp_contact), "%s", (const char*)json["snmp_contact"]);
    if (json["snmp_location"].is<const char*>())
        snprintf(cfg.snmp_location, sizeof(cfg.snmp_location), "%s", (const char*)json["snmp_location"]);
    if (json["snmp_ro_community"].is<const char*>())
        snprintf(cfg.snmp_ro_community, sizeof(cfg.snmp_ro_community), "%s", (const char*)json["snmp_ro_community"]);
    if (json["snmp_rw_community"].is<const char*>())
        snprintf(cfg.snmp_rw_community, sizeof(cfg.snmp_rw_community), "%s", (const char*)json["snmp_rw_community"]);

    dump_config();
    write_config();
    return true;
}

void factory_reset(const String& reason)
{
    Serial.printf("[CFG] FACTORY RESET: %s\r\n", reason);
    Serial.flush();
    // reset eeprom content
    cfg.version = 0xff;
    write_config();
    // restart
    delay(500);
    ESP.restart();
}

void esp_reboot(const String& reason)
{
    Serial.printf("[CFG] REBOOT: %s\r\n", reason);
    Serial.flush();
    // restart
    delay(500);
    ESP.restart();
}
