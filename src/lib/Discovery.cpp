#ifdef ESP8266
#include "Discovery.h"
#include <ESPAsyncTCP.h>

static char httpStr[160];
static AsyncClient *discoveryClient = nullptr;
static uint32_t discoveryStarted = 0;

static void discoveryCleanup(AsyncClient *c) {
  if (discoveryClient == c) discoveryClient = nullptr;
  delete c;
}

void send_discovery_request(uint32_t ip, const char *name, const char *type) {
  if (discoveryClient) {
    // Previous request still in flight. If it has been stuck for a long time
    // (e.g. connect failed after DNS without any callback), forget it.
    if (millis() - discoveryStarted < 20000) return;
    discoveryClient = nullptr;
  }

  snprintf(httpStr, sizeof(httpStr),
           "POST /?address=%u.%u.%u.%u&name=%s&type=%s HTTP/1.1\r\nHost: %s\r\nContent-Length: 0\r\nConnection: close\r\n\r\n",
           (unsigned)(ip & 0xFF), (unsigned)((ip >> 8) & 0xFF),
           (unsigned)((ip >> 16) & 0xFF), (unsigned)((ip >> 24) & 0xFF),
           name, type, DISCOVERY_HOST);

  AsyncClient *c = new AsyncClient();
  if (!c) return;
  discoveryClient = c;
  discoveryStarted = millis();

  c->onConnect([](void *, AsyncClient *client) {
    client->write(httpStr);
  });
  c->onData([](void *, AsyncClient *client, void *, size_t) {
    client->close();
  });
  c->onDisconnect([](void *, AsyncClient *client) {
    discoveryCleanup(client);
  });
  c->onError([](void *, AsyncClient *client, int8_t err) {
    Serial.printf("Discovery error: %s\n", client->errorToString(err));
  });
  c->onTimeout([](void *, AsyncClient *client, uint32_t) {
    client->close();
  });

  if (!c->connect(DISCOVERY_HOST, DISCOVERY_PORT)) {
    discoveryCleanup(c);
  }
}

#endif
