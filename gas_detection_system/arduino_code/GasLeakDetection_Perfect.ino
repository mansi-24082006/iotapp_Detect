#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>

// ---------- WiFi & Firebase ----------
const char* WIFI_SSID = "Mital";
const char* WIFI_PASSWORD = "12346789";
String FIREBASE_DB_URL = "https://smartgas-c6396-default-rtdb.firebaseio.com/";
String FIREBASE_AUTH = "fJV8LMi0I28bvVdYShi8iROpz9nFPXjM8CYNHINs";

// ---------- Pins ----------
#define MQ6_PIN 34
#define BUZZER 25
#define RELAY 26
#define SERVO_PIN 14
#define RELAY_ACTIVE_LOW true

// ---------- Variables ----------
Servo myServo;
int gasThreshold = 1800;
bool gasLatched = false;
bool resetCommand = false;
bool notified = false;
unsigned long lastSync = 0;

// ---------- WiFi ----------
void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 20) {
    delay(500); Serial.print("."); retry++;
  }
}

// ---------- Firebase ----------
String makeUrl(const String& path) {
  return FIREBASE_DB_URL + path + ".json?auth=" + FIREBASE_AUTH;
}

String firebaseGet(const String& path) {
  WiFiClientSecure client; client.setInsecure();
  HTTPClient http; http.begin(client, makeUrl(path));
  int code = http.GET();
  String body = (code == 200) ? http.getString() : "";
  http.end(); return body;
}

void firebaseSet(const String& path, String value) {
  WiFiClientSecure client; client.setInsecure();
  HTTPClient http; http.begin(client, makeUrl(path));
  http.addHeader("Content-Type", "application/json");
  http.PUT(value); http.end();
}

// ---------- Relay ----------
void setRelay(bool state) {
  digitalWrite(RELAY, RELAY_ACTIVE_LOW ? (state ? LOW : HIGH) : (state ? HIGH : LOW));
}

// ---------- Sync ----------
void fetchFirebase() {
  String t = firebaseGet("/settings/threshold");
  if (t != "" && t != "null") gasThreshold = t.toInt();

  String resetVal = firebaseGet("/controls/reset");
  resetCommand = (resetVal == "true");
}

// ---------- Setup ----------
void setup() {
  Serial.begin(115200);
  pinMode(BUZZER, OUTPUT); pinMode(RELAY, OUTPUT); pinMode(MQ6_PIN, INPUT);
  digitalWrite(BUZZER, LOW); setRelay(false);
  myServo.attach(SERVO_PIN); myServo.write(0);
  connectWiFi();
  delay(1000);
}

// ---------- LIVE LOOP ----------
void loop() {
  connectWiFi();
  if (WiFi.status() != WL_CONNECTED) return;

  unsigned long now = millis();
  int gasValue = analogRead(MQ6_PIN);
  bool gasActive = gasValue >= gasThreshold;

  // 1. FAST SENSOR PUSH
  firebaseSet("/sensor/gas", String(gasValue));

  if (gasActive) {
    gasLatched = true;
    digitalWrite(BUZZER, HIGH); myServo.write(90); setRelay(true);
    
    // Update live indicators for App
    firebaseSet("/device/fan", "true");
    firebaseSet("/device/servo", "true");
    firebaseSet("/device/buzzer", "true");
    firebaseSet("/device/status", "\"DANGER\"");
    
    if (!notified) {
      firebaseSet("/alerts/lastAlert", "\"🚨 GAS LEAK DETECTED\"");
      notified = true;
    }
  } 
  else if (gasLatched) {
    digitalWrite(BUZZER, LOW); 

    if (now - lastSync > 1500) {
      lastSync = now;
      fetchFirebase();

      // Read manual overrides from App
      String fanCmd = firebaseGet("/controls/fan");
      if (fanCmd != "") setRelay(fanCmd == "true");

      String servoCmd = firebaseGet("/controls/servo");
      if (servoCmd != "") myServo.write(servoCmd == "true" ? 90 : 0);

      if (resetCommand) {
        gasLatched = false; notified = false;
        myServo.write(0); setRelay(false);
        firebaseSet("/controls/reset", "false");
        firebaseSet("/controls/fan", "false");
        firebaseSet("/controls/servo", "false");
        firebaseSet("/device/status", "\"SAFE\"");
      }
    }
  } 
  else {
    digitalWrite(BUZZER, LOW); myServo.write(0); setRelay(false);
    if (now - lastSync > 3000) {
      lastSync = now;
      fetchFirebase();
      firebaseSet("/device/fan", "false");
      firebaseSet("/device/servo", "false");
      firebaseSet("/device/buzzer", "false");
      firebaseSet("/device/status", "\"SAFE\"");
    }
  }
  delay(50);
}
