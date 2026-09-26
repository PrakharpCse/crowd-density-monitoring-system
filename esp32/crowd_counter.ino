#include <WiFi.h>
#include <HTTPClient.h>

// =========================
// Wi-Fi
// =========================
const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// =========================
// Flask Server
// =========================
const char* SERVER_URL = "http://10.248.32.209:5000/update";
const char* API_KEY = "YOUR_DEVICE_KEY";

// =========================
// Pin Configuration
// =========================
#define ENTRY_IR_PIN 26
#define EXIT_IR_PIN 27

#define ENTRY_LED 2
#define EXIT_LED 33

#define BUZZER 25

#define SENSOR_ACTIVE LOW

// =========================
// Counters
// =========================
int entryCount = 0;
int exitCount = 0;
int currentCrowd = 0;

// =========================
// Debouncing
// =========================
bool entryTriggered = false;
bool exitTriggered = false;

unsigned long lastEntryTrigger = 0;
unsigned long lastExitTrigger = 0;

const unsigned long debounceDelay = 500;

// =========================
// Server Update
// =========================
unsigned long lastUpdateTime = 0;
const unsigned long updateInterval = 2000;

// =========================
// Buzzer
// =========================
void beepBuzzer() {
  digitalWrite(BUZZER, HIGH);
  delay(150);
  digitalWrite(BUZZER, LOW);
}

// =========================
// Wi-Fi Connection
// =========================
void connectWiFi() {

  Serial.print("Connecting to Wi-Fi");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi connected!");

  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());
}

// =========================
// Send Data to Flask
// =========================
void sendData() {

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi disconnected.");
    connectWiFi();
  }

  HTTPClient http;

  http.begin(SERVER_URL);

  http.addHeader("Content-Type", "application/json");
  http.addHeader("X-API-KEY", API_KEY);

  currentCrowd = max(0, entryCount - exitCount);

  String jsonData = "{";
  jsonData += "\"entry\":" + String(entryCount) + ",";
  jsonData += "\"exit\":" + String(exitCount);
  jsonData += "}";

  Serial.println("Sending data:");
  Serial.println(jsonData);

  int responseCode = http.POST(jsonData);

  if (responseCode > 0) {
    Serial.print("Server response code: ");
    Serial.println(responseCode);

    String response = http.getString();

    Serial.println("Server response:");
    Serial.println(response);
  }
  else {
    Serial.print("HTTP request failed: ");
    Serial.println(http.errorToString(responseCode));
  }

  http.end();
}

// =========================
// Entry Sensor
// =========================
void checkEntrySensor() {

  int sensorState = digitalRead(ENTRY_IR_PIN);

  if (sensorState == SENSOR_ACTIVE &&
      !entryTriggered &&
      millis() - lastEntryTrigger >= debounceDelay) {

    entryTriggered = true;
    lastEntryTrigger = millis();

    entryCount++;

    Serial.print("ENTRY detected | Entry Count: ");
    Serial.println(entryCount);

    digitalWrite(ENTRY_LED, HIGH);

    beepBuzzer();

    delay(100);

    digitalWrite(ENTRY_LED, LOW);
  }

  if (sensorState != SENSOR_ACTIVE) {
    entryTriggered = false;
  }
}

// =========================
// Exit Sensor
// =========================
void checkExitSensor() {

  int sensorState = digitalRead(EXIT_IR_PIN);

  if (sensorState == SENSOR_ACTIVE &&
      !exitTriggered &&
      millis() - lastExitTrigger >= debounceDelay) {

    exitTriggered = true;
    lastExitTrigger = millis();

    exitCount++;

    Serial.print("EXIT detected | Exit Count: ");
    Serial.println(exitCount);

    digitalWrite(EXIT_LED, HIGH);

    beepBuzzer();

    delay(100);

    digitalWrite(EXIT_LED, LOW);
  }

  if (sensorState != SENSOR_ACTIVE) {
    exitTriggered = false;
  }
}

// =========================
// Setup
// =========================
void setup() {

  Serial.begin(115200);

  pinMode(ENTRY_IR_PIN, INPUT);
  pinMode(EXIT_IR_PIN, INPUT);

  pinMode(ENTRY_LED, OUTPUT);
  pinMode(EXIT_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  digitalWrite(ENTRY_LED, LOW);
  digitalWrite(EXIT_LED, LOW);
  digitalWrite(BUZZER, LOW);

  Serial.println();
  Serial.println("================================");
  Serial.println(" CROWD DENSITY MONITORING SYSTEM");
  Serial.println("================================");

  connectWiFi();
}

// =========================
// Main Loop
// =========================
void loop() {

  checkEntrySensor();
  checkExitSensor();

  if (millis() - lastUpdateTime >= updateInterval) {

    sendData();

    lastUpdateTime = millis();
  }

  delay(10);
}