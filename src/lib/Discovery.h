#ifndef __discovery_h__
#define __discovery_h__

#include "Marceau.h"

#define DISCOVERY_HOST "local.mirobot.io"
#define DISCOVERY_PORT 80

// Announces this device to the discovery server. Non-blocking: DNS, connect
// and send all happen in the background via ESPAsyncTCP.
void send_discovery_request(uint32_t ip, const char *name, const char *type);

#endif
