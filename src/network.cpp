#include "main.h"

bool net_connected = false;

void network_setup(void)
{
    Serial.println("[NET] setup begin");

    // add a handler for network events
    Serial.println("registering event handler for network events...");
    WiFi.onEvent(network_event);

    // dhcp or static?
    IPAddress myIP, myGW, myNM, myDNS;
    if (cfg.static_ip) {
        myIP.fromString(cfg.ip_addr);
        myGW.fromString(cfg.ip_gw);
        myNM.fromString(cfg.ip_netmask);
        myDNS.fromString(cfg.ip_dns);
        Serial.printf(
            "[NET] using static IP: %s/%s (%s)\r\n",
            myIP.toString().c_str(),
            myNM.toString().c_str(),
            myGW.toString().c_str());
    }

    // ethernet
    if (cfg.wifi_opmode == OPMODE_ETH_CLIENT) {
        Serial.println("[NET] mode: ETH");
        ETH.begin();
        ETH.setHostname(cfg.wifi_hostname);
        if (cfg.static_ip) {
            delay(500); // needed to let esp-idf settle
            ETH.config(myIP, myGW, myNM, myDNS);
        }
    } else if (cfg.wifi_opmode == OPMODE_WIFI_STATION) {
        Serial.println("[NET] mode: WIFI_STA");
        WiFi.disconnect();
        WiFi.setAutoReconnect(true);
        WiFi.setHostname(cfg.wifi_hostname);
        WiFi.setSleep(cfg.wifi_powersave);
        WiFi.mode(WIFI_STA);
        if (cfg.static_ip) {
            WiFi.config(myIP, myGW, myNM, myDNS);
        }
        WiFi.begin(cfg.wifi_ssid, cfg.wifi_secret);
    } else if (cfg.wifi_opmode == OPMODE_WIFI_ACCESSPOINT) {
        Serial.println("[NET] mode: WIFI_AP");
        WiFi.softAP(cfg.wifi_ssid, cfg.wifi_secret);
        IPAddress IP = WiFi.softAPIP();
        Serial.printf("[NET] AP IP address: %s\r\n", IP.toString().c_str());
    }

    Serial.println("[NET] setup done");
}

void network_event(WiFiEvent_t event)
{
    // Serial.printf("WifiEvent: %d\r\n", event);

    switch (event) {
    // *** ETH ***
    case ARDUINO_EVENT_ETH_START:
        // This will happen during setup, when the Ethernet service starts
        Serial.println("[NET] ETH started");
        break;

    case ARDUINO_EVENT_ETH_STOP:
        // This will happen when the ETH interface is stopped but this never happens
        Serial.println("[NET] ETH stopped");
        net_connected = false;
        break;

    case ARDUINO_EVENT_ETH_CONNECTED:
        // This will happen when the Ethernet cable is plugged
        Serial.println("[NET] ETH connected");
        break;

    case ARDUINO_EVENT_ETH_DISCONNECTED:
        // This will happen when the Ethernet cable is unplugged
        Serial.println("[NET] ETH disconnected");
        net_connected = false;
        break;

    case ARDUINO_EVENT_ETH_GOT_IP:
        // This will happen when we obtain an IP address through DHCP:
        Serial.print("[NET] got IP for ETH: ");
        Serial.print(ETH.macAddress());
        Serial.print(", IPv4: ");
        Serial.print(ETH.localIP());
        Serial.print(" (");
        if (ETH.fullDuplex()) {
            Serial.print("FULL_DUPLEX");
        } else {
            Serial.print("HALF_DUPLEX");
        }
        Serial.print(", ");
        Serial.print(ETH.linkSpeed());
        Serial.println("Mbps)");
        if (!MDNS.begin(cfg.wifi_hostname)) {
            Serial.println("[NET] MDNS responder failed to start");
        }
        net_connected = true;
        break;

    // *** WIFI STA ***
    case ARDUINO_EVENT_WIFI_STA_START:
        // This will happen during setup, when the WiFi service starts
        Serial.println("[NET] WIFI_STA started");
        break;

    case ARDUINO_EVENT_WIFI_STA_STOP:
        // This will happen when the WiFi interface is stopped but this never happens
        Serial.println("[NET] WIFI_STA stopped");
        net_connected = false;
        break;

    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
        // This will happen when the WiFi is connected
        Serial.println("[NET] WIFI_STA connected");
        break;

    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        // This will happen when the WiFi is disconnected
        Serial.println("[NET] WIFI_STA disconnected");
        WiFi.reconnect();
        net_connected = false;
        break;

    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
        Serial.print("[NET] got IP for WiFi MAC: ");
        Serial.print(WiFi.macAddress());
        Serial.print(", IPv4: ");
        Serial.print(WiFi.localIP());
        Serial.println("");
        if (!MDNS.begin(cfg.wifi_hostname)) {
            Serial.println("[NET] MDNS responder failed to start");
        }
        net_connected = true;
        break;

    // *** WIFI AP ***
    case ARDUINO_EVENT_WIFI_AP_START:
        // This will happen during setup, when the WiFi service starts
        Serial.println("[NET] WIFI_AP started");
        break;

    default:
        break;
    }
}

void network_loop(void)
{
    static bool fallback_started = false;
    static unsigned long last_checked = 0;
    static int nc_counter = 0;

    // check for fallback ap
    if (cfg.wifi_ap_fallback) {
        // check if network got connected after bootup
        if (!net_connected && millis() < NET_BOOT_TIMEOUT_MS) {
            // increment fail counter
            // dirty hack: use 700ms instead of 1000ms to account for loop drift
            if (millis() - last_checked > 700) {
                nc_counter++;
                last_checked = millis();
                Serial.printf("[NET] nc_counter: %d\r\n", nc_counter);
            }
        }

        // start fallback ap in case the counter goes over limit
        if (nc_counter > NET_BOOT_TIMEOUT_SECS) {
            // get WiFi MAC for SSID
            uint8_t tmp_mac[10];
            WiFi.macAddress(tmp_mac);

            // build tmp ssid based on MAC
            char tmp_ssid[32];
            sprintf(tmp_ssid, "OSENS-%02X%02X%02X", tmp_mac[3], tmp_mac[4], tmp_mac[5]);

            // start fallback AP
            Serial.printf("[NET] failed to connect to SSID %s\r\n", cfg.wifi_ssid);
            Serial.printf("[NET] starting fallback AP with SSID %s\r\n", tmp_ssid);
            WiFi.disconnect();
            WiFi.softAP(tmp_ssid, NULL);
            IPAddress IP = WiFi.softAPIP();
            Serial.printf("[NET] fallback AP IP: %s\r\n", IP.toString().c_str());

            // abuse nc_counter to skip multiple startups
            nc_counter = 0;
        }
    }
}
