#ifndef NETWORK_H
#define NETWORK_H

void network_setup(void);
void network_event(WiFiEvent_t event);
void network_loop(void);

#define SECOND 1000
#define NET_BOOT_TIMEOUT_SECS  30
#define NET_BOOT_TIMEOUT_MS (NET_BOOT_TIMEOUT_SECS * SECOND)

#endif