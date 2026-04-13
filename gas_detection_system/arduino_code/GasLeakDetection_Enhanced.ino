#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>

// ------------------------------------------------------------
// Smart Gas Detection & Prevention System (ESP32 + MQ6)
// Logic:
// 1) Normal: buzzer OFF, servo 0 deg, fan relay OFF
// 2) Gas detected: buzzer ON, servo 90 deg, fan relay ON, notify mobile
// 3) Gas cleared: buzzer OFF automatically, servo/fan stay latched ON
//    until user manually resets them from the mobile UI
// ------------------------------------------------------------

// ---------- WiFi ----------
const char *WIFI_SSID = "Mital";
const char *WIFI_PASSWORD = "12346789";

// ---------- Firebase Realtime Database ----------
String FIREBASE_DB_URL = "https://smartgas-c6396-default-rtdb.firebaseio.com/";
String FIREBASE_AUTH = "fJV8LMi0I28bvVdYShi8iROpz9nFPXjM8CYNHINs";

// ---------- Hardware Pins ----------
static const int MQ6_PIN = 34; // ESP32 ADC input only pin
static const int BUZZER_PIN = 25;
static const int RELAY_FAN_PIN = 26;
static const int SERVO_PIN = 14;

// Most 1-channel relay modules are active LOW.
static const bool RELAY_ACTIVE_LOW = true;

// ---------- Tunables ----------
int gasThreshold = 1600; // Easy to calibrate in code or /settings/threshold
const unsigned long SENSOR_PERIOD_MS = 1000;
const unsigned long CLOUD_PERIOD_MS = 1000;
const unsigned long WIFI_RETRY_MS = 10000;
const unsigned long WARMUP_MS = 30000; // MQ6 warm-up

// ---------- Runtime State ----------
Servo valveServo;
bool sensorWarmedUp = false;
bool gasDetected = false;

// Latches for post-detection behavior
bool gasEventLatched = false;
bool servoLatchedClosed = false; // true -> servo at 90 deg
bool fanLatchedOn = false;       // true -> fan relay ON

bool alertSentForCurrentEvent = false;
int gasValue = 0;
unsigned long lastSensorReadMs = 0;
unsigned long lastCloudPushMs = 0;
unsigned long lastWifiTryMs = 0;

// ---------- Helpers ----------
String makeUrl(const String &path)
{
    String cleanPath = path;
    if (!cleanPath.startsWith("/"))
    {
        cleanPath = "/" + cleanPath;
    }

    String base = FIREBASE_DB_URL;
    if (!base.endsWith("/"))
    {
        base += "/";
    }

    cleanPath.remove(0, 1);
    return base + cleanPath + ".json?auth=" + FIREBASE_AUTH;
}

bool firebaseGet(const String &path, String &response)
{
    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient http;
    if (!http.begin(client, makeUrl(path)))
    {
        return false;
    }

    const int code = http.GET();
    if (code >= 200 && code < 300)
    {
        response = http.getString();
        http.end();
        return true;
    }

    http.end();
    return false;
}

bool firebaseSet(const String &path, const String &jsonValue)
{
    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient http;
    if (!http.begin(client, makeUrl(path)))
    {
        return false;
    }

    http.addHeader("Content-Type", "application/json");
    const int code = http.PUT(jsonValue);
    http.end();

    return code >= 200 && code < 300;
}

bool parseFlexibleBool(const String &value, bool defaultValue = false)
{
    if (value.length() == 0 || value == "null")
    {
        return defaultValue;
    }

    String s = value;
    s.trim();
    s.toLowerCase();

    if (s == "true" || s == "1")
    {
        return true;
    }
    if (s == "false" || s == "0")
    {
        return false;
    }

    return defaultValue;
}

void connectWiFiIfNeeded()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        return;
    }

    const unsigned long now = millis();
    if (now - lastWifiTryMs < WIFI_RETRY_MS)
    {
        return;
    }

    lastWifiTryMs = now;
    Serial.print("Connecting to WiFi");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    int retries = 0;
    while (WiFi.status() != WL_CONNECTED && retries < 20)
    {
        delay(250);
        Serial.print(".");
        retries++;
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.print("WiFi connected, IP: ");
        Serial.println(WiFi.localIP());
    }
    else
    {
        Serial.println("WiFi not connected");
    }
}

void setFanRelay(bool on)
{
    const int level = RELAY_ACTIVE_LOW ? (on ? LOW : HIGH) : (on ? HIGH : LOW);
    digitalWrite(RELAY_FAN_PIN, level);
}

void applyOutputs()
{
    // Buzzer follows live gas condition only (auto OFF when gas clears).
    digitalWrite(BUZZER_PIN, gasDetected ? HIGH : LOW);

    // Servo and fan follow latches.
    valveServo.write(servoLatchedClosed ? 90 : 0);
    setFanRelay(fanLatchedOn);
}

void sendGasAlertOnce()
{
    if (alertSentForCurrentEvent)
    {
        return;
    }

    StaticJsonDocument<256> alertDoc;
    alertDoc["message"] = "GAS DETECTED! Valve closed and fan activated.";
    alertDoc["gasLevel"] = gasValue;
    alertDoc["threshold"] = gasThreshold;
    alertDoc["tsMs"] = millis();

    String payload;
    serializeJson(alertDoc, payload);
    if (firebaseSet("/alerts/latest", payload))
    {
        // Optional trigger flag your mobile app can watch.
        firebaseSet("/alerts/pushTrigger", "true");
        alertSentForCurrentEvent = true;
    }
}

void fetchCloudSettingsAndCommands()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        return;
    }

    // Threshold calibration from mobile UI
    String thresholdBody;
    if (firebaseGet("/settings/threshold", thresholdBody))
    {
        if (thresholdBody != "null")
        {
            const int cloudThreshold = thresholdBody.toInt();
            if (cloudThreshold > 0)
            {
                gasThreshold = cloudThreshold;
            }
        }
    }

    // Manual reset toggles from mobile UI
    String resetServoBody;
    bool resetServo = false;
    if (firebaseGet("/controls/resetServo", resetServoBody))
    {
        resetServo = parseFlexibleBool(resetServoBody, false);
    }

    String resetFanBody;
    bool resetFan = false;
    if (firebaseGet("/controls/resetFan", resetFanBody))
    {
        resetFan = parseFlexibleBool(resetFanBody, false);
    }

    String resetAllBody;
    bool resetAll = false;
    if (firebaseGet("/controls/resetAll", resetAllBody))
    {
        resetAll = parseFlexibleBool(resetAllBody, false);
    }

    // Safety interlock: do not allow reset while gas is currently detected.
    if (!gasDetected)
    {
        if (resetAll)
        {
            servoLatchedClosed = false;
            fanLatchedOn = false;
        }
        else
        {
            if (resetServo)
            {
                servoLatchedClosed = false;
            }
            if (resetFan)
            {
                fanLatchedOn = false;
            }
        }

        // Event ends once both latches are released in a safe condition.
        if (!servoLatchedClosed && !fanLatchedOn)
        {
            gasEventLatched = false;
            alertSentForCurrentEvent = false;
        }
    }

    // Auto-clear one-shot control flags in cloud.
    if (resetServo)
    {
        firebaseSet("/controls/resetServo", "false");
    }
    if (resetFan)
    {
        firebaseSet("/controls/resetFan", "false");
    }
    if (resetAll)
    {
        firebaseSet("/controls/resetAll", "false");
    }
}

void publishTelemetry()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        return;
    }

    StaticJsonDocument<512> doc;
    doc["gasLevel"] = gasValue;
    doc["threshold"] = gasThreshold;
    doc["gasDetected"] = gasDetected;
    doc["sensorWarmedUp"] = sensorWarmedUp;
    doc["buzzerOn"] = gasDetected;
    doc["servoAt90"] = servoLatchedClosed;
    doc["fanOn"] = fanLatchedOn;
    doc["eventLatched"] = gasEventLatched;
    doc["status"] = gasDetected ? "GAS_DETECTED" : (gasEventLatched ? "WAITING_MANUAL_RESET" : "NORMAL");
    doc["wifiConnected"] = (WiFi.status() == WL_CONNECTED);
    doc["tsMs"] = millis();

    String payload;
    serializeJson(doc, payload);
    firebaseSet("/device", payload);
}

void setup()
{
    Serial.begin(115200);
    delay(500);
    Serial.println("Smart Gas Detection System starting...");

    pinMode(MQ6_PIN, INPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(RELAY_FAN_PIN, OUTPUT);

    digitalWrite(BUZZER_PIN, LOW);
    setFanRelay(false);

    valveServo.attach(SERVO_PIN);
    valveServo.write(0);

    connectWiFiIfNeeded();

    // Warm-up status to reduce false alarms right after power-on.
    firebaseSet("/device/status", "\"WARMING_UP\"");
    firebaseSet("/device/sensorWarmedUp", "false");

    Serial.print("MQ6 warming up for ");
    Serial.print(WARMUP_MS / 1000);
    Serial.println(" seconds...");
    delay(WARMUP_MS);

    sensorWarmedUp = true;
    firebaseSet("/device/sensorWarmedUp", "true");
    firebaseSet("/device/status", "\"NORMAL\"");
}

void loop()
{
    connectWiFiIfNeeded();

    const unsigned long now = millis();

    if (now - lastSensorReadMs >= SENSOR_PERIOD_MS)
    {
        lastSensorReadMs = now;

        gasValue = analogRead(MQ6_PIN);
        gasDetected = sensorWarmedUp && (gasValue >= gasThreshold);

        if (gasDetected)
        {
            gasEventLatched = true;
            servoLatchedClosed = true;
            fanLatchedOn = true;
            sendGasAlertOnce();
        }

        applyOutputs();

        Serial.print("Gas=");
        Serial.print(gasValue);
        Serial.print(" Threshold=");
        Serial.print(gasThreshold);
        Serial.print(" Detected=");
        Serial.print(gasDetected ? "YES" : "NO");
        Serial.print(" Servo90=");
        Serial.print(servoLatchedClosed ? "YES" : "NO");
        Serial.print(" Fan=");
        Serial.println(fanLatchedOn ? "ON" : "OFF");
    }

    // Poll cloud commands every cycle; they are lightweight requests.
    fetchCloudSettingsAndCommands();
    applyOutputs();

    if (now - lastCloudPushMs >= CLOUD_PERIOD_MS)
    {
        lastCloudPushMs = now;
        publishTelemetry();
    }

    delay(50);
}
