#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>

// -----------------------------------------------------------------------------
// SMART GAS SYSTEM - FINAL ROBUST VERSION
// -----------------------------------------------------------------------------

// ---------- WiFi Credentials ----------
const char* WIFI_SSID = "Mital";
const char* WIFI_PASSWORD = "12346789";

// ---------- Firebase Configuration ----------
const String FIREBASE_DB_URL = "https://smartgas-c6396-default-rtdb.firebaseio.com/";
const String FIREBASE_AUTH = "fJV8LMi0I28bvVdYShi8iROpz9nFPXjM8CYNHINs";

// ---------- Pin Definitions ----------
static const int MQ6_PIN = 34;   // MQ6 Analog Input (ADC1)
static const int BUZZER_PIN = 25; 
static const int RELAY_PIN = 26;  // Exhaust Fan Relay
static const int SERVO_PIN = 14;  // Gas Valve Servo

// Relay Logic (Change to false if your relay is active HIGH)
static const bool RELAY_ACTIVE_LOW = true;

// Servo Angles (Adjust to match your physical setup)
static const int SERVO_OPEN = 90;
static const int SERVO_CLOSED = 0;

// ---------- Constants ----------
const unsigned long WARMUP_TIME_MS = 20000; // 20s warmup
const unsigned long TELEMETRY_INTERVAL_MS = 1500;
const unsigned long FETCH_INTERVAL_MS = 2000;

// ---------- Runtime Variables ----------
Servo myServo;
int gasValue = 0;
int gasThreshold = 1600;
String systemMode = "AUTO"; // "AUTO" or "MANUAL"

// Actuator States
bool fanActive = false;
bool buzzerActive = false;
bool servoClosed = false;

// Latches for safety logic
bool gasDetected = false;
bool systemWarmedUp = false;
unsigned long startTime = 0;
unsigned long lastTelemetryMs = 0;
unsigned long lastFetchMs = 0;

// ---------- Firebase Helpers ----------
String makeUrl(const String& path) {
  String url = FIREBASE_DB_URL + path + ".json?auth=" + FIREBASE_AUTH;
  return url;
}

bool firebaseSet(const String& path, const String& json) {
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  http.begin(client, makeUrl(path));
  http.addHeader("Content-Type", "application/json");
  int code = http.PUT(json);
  http.end();
  return (code >= 200 && code < 300);
}

String firebaseGet(const String& path) {
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  http.begin(client, makeUrl(path));
  int code = http.GET();
  String body = (code == 200) ? http.getString() : "";
  http.end();
  return body;
}

// ---------- Logic Functions ----------
void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;
  
  Serial.print("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 25) {
    delay(500);
    Serial.print(".");
    retry++;
  }
  Serial.println("\nWiFi Status: " + String(WiFi.status() == WL_CONNECTED ? "CONNECTED" : "FAILED"));
}

void applyOutputs() {
  // Relay
  digitalWrite(RELAY_PIN, RELAY_ACTIVE_LOW ? (fanActive ? LOW : HIGH) : (fanActive ? HIGH : LOW));
  // Buzzer
  digitalWrite(BUZZER_PIN, buzzerActive ? HIGH : LOW);
  // Servo
  myServo.write(servoClosed ? SERVO_CLOSED : SERVO_OPEN);
}

void syncWithCloud() {
  // 1. Fetch Mode and Threshold
  String modeBody = firebaseGet("/settings/mode");
  if (modeBody != "" && modeBody != "null") {
    modeBody.replace("\"", "");
    systemMode = modeBody;
  }
  
  String threshBody = firebaseGet("/settings/threshold");
  if (threshBody != "" && threshBody != "null") {
    gasThreshold = threshBody.toInt();
  }

  // 2. Fetch Manual Overrides if in MANUAL mode
  if (systemMode == "MANUAL") {
    String controlBody = firebaseGet("/controls");
    if (controlBody != "" && controlBody != "null") {
      StaticJsonDocument<256> doc;
      deserializeJson(doc, controlBody);
      // Only allow manual control if no danger
      if (!gasDetected) {
        fanActive = doc["fan"] | false;
        buzzerActive = doc["buzzer"] | false;
        servoClosed = doc["servo"] | false;
      }
    }
  }

  // 3. Check for Resets (if app uses specialized reset flags)
  String resetAllBody = firebaseGet("/controls/resetAll");
  if (resetAllBody == "true") {
    if (!gasDetected) {
      fanActive = false;
      buzzerActive = false;
      servoClosed = false;
      firebaseSet("/controls/resetAll", "false");
    }
  }
}

void publishTelemetry() {
  StaticJsonDocument<512> doc;
  doc["gasLevel"] = gasValue;
  doc["threshold"] = gasThreshold;
  doc["gasDetected"] = gasDetected;
  doc["sensorWarmedUp"] = systemWarmedUp;
  doc["fanOn"] = fanActive;
  doc["buzzerOn"] = buzzerActive;
  doc["servoAt90"] = (servoClosed && SERVO_CLOSED == 90) || (!servoClosed && SERVO_OPEN == 90);
  doc["servoClosed"] = servoClosed;
  doc["mode"] = systemMode;
  
  String status = "NORMAL";
  if (!systemWarmedUp) status = "WARMING_UP";
  else if (gasDetected) status = "GAS_DETECTED";
  else if (fanActive || servoClosed) status = "MANUAL_OVERRIDE";
  
  doc["status"] = status;
  doc["lastUpdated"] = millis();

  String payload;
  serializeJson(doc, payload);
  firebaseSet("/device", payload);
}

// ---------- Main Setup & Loop ----------
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  pinMode(MQ6_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  
  // Set Safe Initial States
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(RELAY_PIN, RELAY_ACTIVE_LOW ? HIGH : LOW);
  
  myServo.attach(SERVO_PIN);
  myServo.write(SERVO_OPEN);
  
  connectWiFi();
  startTime = millis();
}

void loop() {
  connectWiFi();
  unsigned long now = millis();

  // Warm-up logic
  if (!systemWarmedUp && (now - startTime > WARMUP_TIME_MS)) {
    systemWarmedUp = true;
    Serial.println("System Warmed Up!");
  }

  // Read Sensor
  gasValue = analogRead(MQ6_PIN);
  gasDetected = systemWarmedUp && (gasValue >= gasThreshold);

  // Core Safety Logic
  if (gasDetected) {
    // Danger State: Force Safety Mechanisms (Fan ON, Buzzer ON, Valve CLOSED)
    fanActive = true;
    buzzerActive = true;
    servoClosed = true;
  } else {
    // Safe State: Follow Mode
    if (systemMode == "AUTO") {
      fanActive = false;
      buzzerActive = false;
      servoClosed = false;
    }
    // If Mode is MANUAL, states are updated via syncWithCloud()
  }

  applyOutputs();

  // Periodic Tasks
  if (now - lastFetchMs > FETCH_INTERVAL_MS) {
    lastFetchMs = now;
    syncWithCloud();
  }

  if (now - lastTelemetryMs > TELEMETRY_INTERVAL_MS) {
    lastTelemetryMs = now;
    publishTelemetry();
    Serial.print("Gas: "); Serial.print(gasValue);
    Serial.print(" Thresh: "); Serial.print(gasThreshold);
    Serial.print(" Mode: "); Serial.println(systemMode);
  }

  delay(10);
}
