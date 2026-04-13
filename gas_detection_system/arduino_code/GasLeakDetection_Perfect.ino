#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>

// -----------------------------------------------------------------------------
// SMART GAS SYSTEM - LATCHING VERSION (POLISHED)
// -----------------------------------------------------------------------------

// ---------- WiFi Credentials ----------
const char* WIFI_SSID = "Mital";
const char* WIFI_PASSWORD = "12346789";

// ---------- Firebase Configuration ----------
const String FIREBASE_DB_URL = "https://smartgas-c6396-default-rtdb.firebaseio.com/";
const String FIREBASE_AUTH = "fJV8LMi0I28bvVdYShi8iROpz9nFPXjM8CYNHINs";

// ---------- Pin Definitions ----------
static const int MQ6_PIN = 34;
static const int BUZZER = 25;
static const int RELAY = 26;
static const int SERVO_PIN = 14;

// Relay Logic
static const bool RELAY_ACTIVE_LOW = true;

// ---------- Global State ----------
Servo myServo;
int gasThreshold = 1800;
bool gasLatched = false;
bool fanActiveLatched = true;   // Default ON during latch
bool servoClosedLatched = true; // Default CLOSED during latch
bool notified = false;
int gasValue = 0;

unsigned long lastCloudSyncMs = 0;
const unsigned long SYNC_INTERVAL = 1500;

// ---------- Firebase Helpers ----------
String makeUrl(const String& path) {
  return FIREBASE_DB_URL + path + ".json?auth=" + FIREBASE_AUTH;
}

void firebaseSet(const String& path, String value) {
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  http.begin(client, makeUrl(path));
  http.addHeader("Content-Type", "application/json");
  http.PUT(value);
  http.end();
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

// ---------- Connection ----------
void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;
  Serial.print("🔄 Connecting WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 20) {
    delay(500); Serial.print("."); retry++;
  }
  if (WiFi.status() == WL_CONNECTED) Serial.println("\n✅ Connected!");
}

void setRelay(bool state) {
  digitalWrite(RELAY, RELAY_ACTIVE_LOW ? (state ? LOW : HIGH) : (state ? HIGH : LOW));
}

// ---------- Main Logic ----------
void setup() {
  Serial.begin(115200);
  pinMode(BUZZER, OUTPUT);
  pinMode(RELAY, OUTPUT);
  pinMode(MQ6_PIN, INPUT);

  digitalWrite(BUZZER, LOW);
  setRelay(false);

  myServo.attach(SERVO_PIN);
  myServo.write(0); // Open/Safe position

  connectWiFi();
  Serial.println("🔥 Warmup starting (20s)...");
  delay(20000);
  Serial.println("🚀 System Online");
}

void loop() {
  connectWiFi();
  if (WiFi.status() != WL_CONNECTED) return;

  unsigned long now = millis();
  gasValue = analogRead(MQ6_PIN);
  bool gasActive = gasValue >= gasThreshold;

  // 1. Process Logic States
  if (gasActive) {
    // CRITICAL: Gas Currently Detected
    gasLatched = true;
    fanActiveLatched = true;    // Initial latch states
    servoClosedLatched = true;
    
    digitalWrite(BUZZER, HIGH);
    myServo.write(90); // Close Valve
    setRelay(true);    // Fan ON

    if (!notified) {
      firebaseSet("/alerts/lastAlert", "\"🚨 GAS LEAK DETECTED\"");
      notified = true;
    }
  } 
  else if (gasLatched) {
    // WARNING: Gas Cleared but System LATCHED
    digitalWrite(BUZZER, LOW);
    
    // In LATCHED state, we allow manual overrides from the App
    if (now - lastCloudSyncMs > SYNC_INTERVAL) {
      // 1. Check for Full Reset
      String resetVal = firebaseGet("/controls/reset");
      if (resetVal == "true") {
        Serial.println("🔄 FULL RESET");
        gasLatched = false;
        notified = false;
        myServo.write(0);
        setRelay(false);
        firebaseSet("/controls/reset", "false");
        firebaseSet("/controls/fan", "false");   // Sync manual flags
        firebaseSet("/controls/servo", "false");
      } else {
        // 2. Allow independent manual control of Fan and Servo
        String fanManual = firebaseGet("/controls/fan");
        if (fanManual != "") fanActiveLatched = (fanManual == "true");
        
        String servoManual = firebaseGet("/controls/servo");
        if (servoManual != "") servoClosedLatched = (servoManual == "true");

        setRelay(fanActiveLatched);
        myServo.write(servoClosedLatched ? 90 : 0);
      }
    } else {
       // Maintain current latched state between syncs
       setRelay(fanActiveLatched);
       myServo.write(servoClosedLatched ? 90 : 0);
    }
  } 
  else {
    // SAFE: Normal Operation
    digitalWrite(BUZZER, LOW);
    myServo.write(0);
    setRelay(false);
    notified = false;
  }

  // 2. Periodic Cloud Update
  if (now - lastCloudSyncMs > SYNC_INTERVAL) {
    lastCloudSyncMs = now;
    
    // Push current sensor data
    firebaseSet("/sensor/gas", String(gasValue));
    
    // Report detailed status for the app
    StaticJsonDocument<256> statusDoc;
    statusDoc["gasValue"] = gasValue;
    statusDoc["threshold"] = gasThreshold;
    statusDoc["latched"] = gasLatched;
    statusDoc["danger"] = gasActive;
    statusDoc["fanOn"] = (gasActive || gasLatched);
    statusDoc["servoClosed"] = (gasActive || gasLatched);
    statusDoc["buzzerOn"] = gasActive;
    
    String statusStr = "NORMAL";
    if (gasActive) statusStr = "GAS_DETECTED";
    else if (gasLatched) statusStr = "POST_LEAK_LATCHED";
    statusDoc["status"] = statusStr;

    String payload;
    serializeJson(statusDoc, payload);
    firebaseSet("/sensor/state", payload);

    // Also fetch settings
    String t = firebaseGet("/settings/threshold");
    if (t != "" && t != "null") gasThreshold = t.toInt();
  }

  Serial.print("Gas: "); Serial.print(gasValue);
  Serial.print(" | Latched: "); Serial.println(gasLatched ? "YES" : "NO");
  delay(500);
}
