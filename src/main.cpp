#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

const char *WIFI_SSID = "Fiercelooo";
const char *WIFI_PASSWORD = "10012009";

#define PIN_RELAY 5
#define PIN_LED 8
#define PIN_BUZZER 6

#define RELAY_ON HIGH
#define RELAY_OFF LOW

#define BUZZER_ON LOW
#define BUZZER_OFF HIGH

WebServer server(80);

// STATUS (biar stabil)
bool statusLed = false;
bool statusRelay = false;

void allIdle()
{
  digitalWrite(PIN_RELAY, RELAY_OFF);
  digitalWrite(PIN_LED, LOW);
  digitalWrite(PIN_BUZZER, BUZZER_OFF);

  statusRelay = false;
  statusLed = false;
}

void beepShort(int times)
{
  for (int i = 0; i < times; i++)
  {
    digitalWrite(PIN_BUZZER, BUZZER_ON);
    delay(150);
    digitalWrite(PIN_BUZZER, BUZZER_OFF);
    delay(150);
  }
}

// akses diterima
void aksesDiterima(String name)
{
  Serial0.println("[ACTION] AKSES DITERIMA -> " + name);

  digitalWrite(PIN_LED, HIGH);
  statusLed = true;

  digitalWrite(PIN_RELAY, RELAY_ON);
  statusRelay = true;

  digitalWrite(PIN_BUZZER, BUZZER_ON);
  delay(100);
  digitalWrite(PIN_BUZZER, BUZZER_OFF);
}

// AKSES DITOLAK
void aksesDitolak(String reason)
{
  Serial0.println("[ACTION] AKSES DITOLAK -> " + reason);

  for (int i = 0; i < 3; i++)
  {
    digitalWrite(PIN_LED, HIGH);
    delay(120);
    digitalWrite(PIN_LED, LOW);
    delay(120);
  }

  beepShort(3);
}

// POST /trigger
void handleTrigger()
{
  if (!server.hasArg("plain"))
  {
    server.send(400, "application/json", "{\"error\":\"Body tidak ditemukan\"}");
    return;
  }

  String body = server.arg("plain");
  Serial0.println("[HTTP] Received: " + body);

  StaticJsonDocument<256> doc;
  DeserializationError err = deserializeJson(doc, body);

  if (err)
  {
    server.send(400, "application/json", "{\"error\":\"JSON tidak valid\"}");
    return;
  }

  String action = doc["action"] | "denied";

  if (action == "granted")
  {
    String name = doc["name"] | "user";
    aksesDiterima(name);
    server.send(200, "application/json", "{\"status\":\"ok\",\"action\":\"granted\"}");
  }
  else
  {
    String reason = doc["reason"] | "unknown";
    aksesDitolak(reason);
    server.send(200, "application/json", "{\"status\":\"ok\",\"action\":\"denied\"}");
  }
}

// GET /status
void handleStatus()
{
  server.send(200, "application/json",
              "{\"status\":\"online\",\"ip\":\"" + WiFi.localIP().toString() + "\"}");
}

// GET /device/state
void handleDeviceState()
{
  String json = "{";
  json += "\"led\":" + String(statusLed ? "true" : "false") + ",";
  json += "\"relay\":" + String(statusRelay ? "true" : "false");
  json += "}";

  server.send(200, "application/json", json);
}

// CONTROL RELAY MANUAL
void handleRelayOn()
{
  digitalWrite(PIN_LED, HIGH);
  digitalWrite(PIN_RELAY, RELAY_ON);

  statusLed = true;
  statusRelay = true;

  server.send(200, "application/json", "{\"relay\":true}");
}

void handleRelayOff()
{
  allIdle();
  server.send(200, "application/json", "{\"relay\":false}");
}

// TEST ENDPOINT
void handleTestOn()
{
  digitalWrite(PIN_LED, HIGH);
  digitalWrite(PIN_RELAY, RELAY_ON);
  digitalWrite(PIN_BUZZER, BUZZER_ON);

  statusLed = true;
  statusRelay = true;

  server.send(200, "application/json", "{\"status\":\"all_on\"}");
}

void handleTestOff()
{
  allIdle();
  server.send(200, "application/json", "{\"status\":\"all_off\"}");
}

// CONNECT WIFI
void connectWiFi()
{
  Serial0.print("[WIFI] Connecting ke ");
  Serial0.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(PIN_LED, HIGH);
    delay(200);
    digitalWrite(PIN_LED, LOW);
    delay(200);
    Serial0.print(".");
  }

  Serial0.println();
  Serial0.println("[WIFI] Connected!");
  Serial0.print("IP: ");
  Serial0.println(WiFi.localIP());
}

void setup()
{
  Serial0.begin(115200);
  delay(1000);

  pinMode(PIN_RELAY, OUTPUT);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);

  allIdle();

  connectWiFi();

  server.on("/trigger", HTTP_POST, handleTrigger);
  server.on("/status", HTTP_GET, handleStatus);
  server.on("/device/state", HTTP_GET, handleDeviceState);

  // endpoint baru
  server.on("/relay/on", HTTP_GET, handleRelayOn);
  server.on("/relay/off", HTTP_GET, handleRelayOff);

  server.on("/test/on", HTTP_GET, handleTestOn);
  server.on("/test/off", HTTP_GET, handleTestOff);

  server.begin();

  Serial0.println("[SERVER] Ready");
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial0.println("\n[WIFI] Putus! Reconnecting...");
    connectWiFi();
  }

  server.handleClient();
}