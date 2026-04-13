/*
 * SMART GAS LEAKAGE DETECTION AND AUTOMATED SAFETY SYSTEM
 * 
 * Purpose: Detects hazardous gas leakage and automatically triggers safety mechanisms
 *          including exhaust fan, buzzer alarm, and gas valve shutoff with manual reset
 * 
 * Author: Gas Detection System Team
 * Version: 1.0.0
 * Date: April 2026
 * 
 * Hardware:
 * - Arduino Uno/Mega or ESP32
 * - MQ-2/MQ-5 Gas Sensor
 * - 2-Channel 5V Relay Module
 * - 5V Active Buzzer
 * - 16x2 LCD (I2C)
 * - Push Buttons (Reset, Manual Controls)
 * 
 * PIN CONFIGURATION:
 * A0  -> Gas Sensor Analog Input
 * 2   -> Reset Button
 * 3   -> Fan Relay Control (via relay)
 * 4   -> Valve Relay Control (via relay)
 * 9   -> Buzzer
 * 20  -> SDA (LCD I2C)
 * 21  -> SCL (LCD I2C)
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

// ============================================================================
// CONFIGURATION SECTION
// ============================================================================

#define GAS_THRESHOLD          500    // PPM threshold for gas detection
#define SENSOR_READ_DELAY      1000   // Delay between sensor readings (ms)
#define FAN_VENTILATION_TIME   180000 // Fan auto-off time (3 minutes)
#define BUZZER_BLINK_INTERVAL  500    // Buzzer blink interval (ms)

// Pin Definitions
#define GAS_SENSOR_PIN         A0     // Analog input for MQ-2/MQ-5
#define BUZZER_PIN             9      // Digital output for buzzer
#define RESET_BUTTON_PIN       2      // Digital input for reset button
#define FAN_RELAY_PIN          3      // Digital output for fan relay
#define VALVE_RELAY_PIN        4      // Digital output for valve relay
#define MANUAL_FAN_SWITCH      6      // Digital input for manual fan control
#define MANUAL_VALVE_SWITCH    7      // Digital input for manual valve control

// EEPROM Addresses for persistent storage
#define EEPROM_LEAKAGE_FLAG    0      // Leakage detection flag address
#define EEPROM_LEAKAGE_COUNT   1      // Leakage event count address
#define EEPROM_LAST_ALERT_TIME 5      // Last alert time address (4 bytes)

// LCD Configuration (16x2 I2C)
#define LCD_ADDRESS            0x27   // I2C address (0x27 for 20x4, 0x3F for 16x2)
#define LCD_COLS               16
#define LCD_ROWS               2

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================

// LCD Object
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS);

// System State Variables
enum SystemState {
  STATE_NORMAL,           // Normal operation, all systems ready
  STATE_GAS_DETECTED,     // Gas detected above threshold
  STATE_WARNING,          // Gas was detected but now below threshold (requires manual reset)
  STATE_MANUAL_OVERRIDE   // User manually controlling fan/valve
};

SystemState currentState = STATE_NORMAL;
bool leakageDetected = false;        // Flag: Has gas been detected?
bool buzzerActive = false;           // Flag: Is buzzer currently on?
bool fanActive = false;              // Flag: Is fan currently on?
bool valveOpen = true;               // Flag: Is gas valve open?
bool systemReset = false;            // Flag: System was just reset

unsigned long gasDetectionTime = 0;  // Time when gas was first detected
unsigned long fanStartTime = 0;      // Time when fan was started
unsigned long lastSensorReadTime = 0;// Time of last sensor reading
unsigned long lastBuzzerToggle = 0;  // Time of last buzzer state change

int currentGasLevel = 0;             // Current gas level (0-1023)
int maxGasLevelDetected = 0;         // Peak gas level detected in current event
int averageGasLevel = 0;             // Average gas level for smoothing

// Manual Control Variables
bool manualFanControl = false;       // User controlling fan manually
bool manualValveControl = false;     // User controlling valve manually
bool lastFanButtonState = LOW;
bool lastValveButtonState = LOW;

// ============================================================================
// SETUP FUNCTION
// ============================================================================

void setup() {
  // Serial communication for debugging
  Serial.begin(9600);
  Serial.println("\n\n=== SMART GAS DETECTION SYSTEM ===");
  Serial.println("Initializing System...");
  
  // Initialize LCD
  Wire.begin();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("System Init...");
  delay(500);
  
  // Initialize Pin Modes
  pinMode(GAS_SENSOR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
  pinMode(FAN_RELAY_PIN, OUTPUT);
  pinMode(VALVE_RELAY_PIN, OUTPUT);
  pinMode(MANUAL_FAN_SWITCH, INPUT_PULLUP);
  pinMode(MANUAL_VALVE_SWITCH, INPUT_PULLUP);
  
  // Set initial states (relays typically use HIGH = OFF, LOW = ON for NC relay)
  digitalWrite(BUZZER_PIN, LOW);       // Buzzer OFF
  digitalWrite(FAN_RELAY_PIN, HIGH);   // Fan OFF
  digitalWrite(VALVE_RELAY_PIN, LOW);  // Valve ON (normally open)
  
  // Load system state from EEPROM
  leakageDetected = EEPROM.read(EEPROM_LEAKAGE_FLAG);
  
  Serial.println("Pin Configuration Complete");
  Serial.println("EEPROM Load Complete");
  Serial.println("System Ready!");
  
  // Display startup message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Gas Detection");
  lcd.setCursor(0, 1);
  lcd.print("System Ready");
  delay(2000);
  
  // Sensor warm-up period
  Serial.println("Sensor warming up... (15 seconds)");
  displayMessage("Sensor Warm-up", "Please wait...");
  for (int i = 0; i < 15; i++) {
    readGasSensor();
    delay(1000);
  }
  
  Serial.println("System Initialization Complete!");
  leakageDetected ? displayMessage("System Status:", "!WARNING!") : displayMessage("System Status:", "Normal");
}

// ============================================================================
// MAIN LOOP
// ============================================================================

void loop() {
  // Read button inputs
  handleResetButton();
  handleManualControls();
  
  // Non-blocking sensor reading
  if (millis() - lastSensorReadTime >= SENSOR_READ_DELAY) {
    lastSensorReadTime = millis();
    readGasSensor();
  }
  
  // State machine processing
  updateSystemState();
  
  // Update outputs based on current state
  updateOutputs();
  
  // Update display
  updateDisplay();
  
  // Log system status periodically
  static unsigned long lastLogTime = 0;
  if (millis() - lastLogTime >= 5000) {  // Log every 5 seconds
    lastLogTime = millis();
    logSystemStatus();
  }
}

// ============================================================================
// GAS SENSOR READING
// ============================================================================

void readGasSensor() {
  // Read analog value from gas sensor
  int rawValue = analogRead(GAS_SENSOR_PIN);
  
  // Convert to PPM (calibration dependent on sensor type)
  // MQ-2: Rough conversion for LPG/Butane/Methane
  // Real PPM = (Vrl / Vc - 1) * B / A where:
  // Vrl = voltage with pollutant (analog read * 5/1023)
  // Vc = voltage in clean air
  // A and B are sensor-specific coefficients
  
  // Simplified: direct threshold on analog value
  currentGasLevel = rawValue;
  
  // Smooth the reading using simple averaging
  static int sensorHistory[5] = {0};
  static int historyIndex = 0;
  sensorHistory[historyIndex] = rawValue;
  historyIndex = (historyIndex + 1) % 5;
  
  // Calculate average
  int sum = 0;
  for (int i = 0; i < 5; i++) {
    sum += sensorHistory[i];
  }
  averageGasLevel = sum / 5;
  
  // Serial output for monitoring
  Serial.print("Gas Level: ");
  Serial.print(averageGasLevel);
  Serial.print(" | State: ");
  Serial.println(getStateString());
}

// ============================================================================
// STATE MACHINE UPDATE
// ============================================================================

void updateSystemState() {
  switch (currentState) {
    
    case STATE_NORMAL:
      // Normal state: waiting for gas detection
      if (averageGasLevel >= GAS_THRESHOLD) {
        // Gas detected! Transition to detection state
        Serial.println("*** GAS DETECTED ***");
        currentState = STATE_GAS_DETECTED;
        gasDetectionTime = millis();
        leakageDetected = true;
        maxGasLevelDetected = averageGasLevel;
        
        // Save leakage flag to EEPROM
        EEPROM.write(EEPROM_LEAKAGE_FLAG, 1);
        EEPROM.commit();
        
        Serial.println("Leakage flag saved. Activating emergency response.");
      }
      break;
      
    case STATE_GAS_DETECTED:
      // Gas detected state: active alarm and safety mechanisms
      if (averageGasLevel < GAS_THRESHOLD) {
        // Gas level returned to normal
        Serial.println("*** GAS LEVEL NORMALIZED ***");
        Serial.println("Transitioning to WARNING state...");
        currentState = STATE_WARNING;
        fanStartTime = millis();  // Start fan continuation timer
      } else {
        // Update peak detection
        if (averageGasLevel > maxGasLevelDetected) {
          maxGasLevelDetected = averageGasLevel;
        }
      }
      break;
      
    case STATE_WARNING:
      // Warning state: gas normalized but system requires manual reset
      // Check if fan auto-off timer has expired
      if (millis() - fanStartTime >= FAN_VENTILATION_TIME) {
        Serial.println("Fan ventilation time expired. Turning off fan.");
        fanActive = false;
      }
      
      // Stay in this state until manual reset
      break;
      
    case STATE_MANUAL_OVERRIDE:
      // Manual override state handled separately
      break;
  }
}

// ============================================================================
// OUTPUT CONTROL
// ============================================================================

void updateOutputs() {
  // Buzzer control
  if (currentState == STATE_GAS_DETECTED) {
    // Continuous buzzer in detection state
    activateBuzzer();
  } else {
    // Buzzer off in other states
    deactivateBuzzer();
  }
  
  // Fan control
  if (manualFanControl) {
    // Manual override for fan
    // Fan state controlled by manual switch
  } else {
    // Automatic fan control
    if (currentState == STATE_GAS_DETECTED) {
      fanActive = true;
    }
    // Fan stays on during warning state until timer expires
  }
  
  // Apply fan relay control
  digitalWrite(FAN_RELAY_PIN, fanActive ? LOW : HIGH);  // LOW = ON, HIGH = OFF
  
  // Valve control
  if (manualValveControl) {
    // Manual override for valve
    // Valve state controlled by manual switch
  } else {
    // Automatic valve control
    if (currentState == STATE_GAS_DETECTED) {
      valveOpen = false;  // Close valve in gas detection
    }
    // Valve stays closed during warning state
  }
  
  // Apply valve relay control
  digitalWrite(VALVE_RELAY_PIN, valveOpen ? LOW : HIGH);  // LOW = ON (open), HIGH = OFF (closed)
}

// ============================================================================
// BUTTON HANDLERS
// ============================================================================

void handleResetButton() {
  static unsigned long debounceTime = 0;
  static bool buttonPressed = false;
  
  if (digitalRead(RESET_BUTTON_PIN) == LOW) {  // Button pressed (active low)
    if (!buttonPressed && (millis() - debounceTime) > 50) {
      buttonPressed = true;
      debounceTime = millis();
      
      Serial.println("*** RESET BUTTON PRESSED ***");
      resetSystem();
    }
  } else {
    buttonPressed = false;
  }
}

void handleManualControls() {
  // Manual fan switch
  bool currentFanButtonState = digitalRead(MANUAL_FAN_SWITCH);
  if (currentFanButtonState != lastFanButtonState && currentFanButtonState == LOW) {
    manualFanControl = !manualFanControl;
    fanActive = !fanActive;
    Serial.print("Manual Fan: ");
    Serial.println(fanActive ? "ON" : "OFF");
    delay(20);  // Debounce
  }
  lastFanButtonState = currentFanButtonState;
  
  // Manual valve switch
  bool currentValveButtonState = digitalRead(MANUAL_VALVE_SWITCH);
  if (currentValveButtonState != lastValveButtonState && currentValveButtonState == LOW) {
    manualValveControl = !manualValveControl;
    valveOpen = !valveOpen;
    Serial.print("Manual Valve: ");
    Serial.println(valveOpen ? "OPEN" : "CLOSED");
    delay(20);  // Debounce
  }
  lastValveButtonState = currentValveButtonState;
}

// ============================================================================
// RESET SYSTEM
// ============================================================================

void resetSystem() {
  Serial.println("Resetting System to Normal Operation...");
  
  // Clear leakage flag
  leakageDetected = false;
  EEPROM.write(EEPROM_LEAKAGE_FLAG, 0);
  EEPROM.commit();
  
  // Reset all components
  currentState = STATE_NORMAL;
  buzzerActive = false;
  fanActive = false;
  valveOpen = true;
  manualFanControl = false;
  manualValveControl = false;
  maxGasLevelDetected = 0;
  
  // Updates outputs
  digitalWrite(BUZZER_PIN, LOW);        // Buzzer OFF
  digitalWrite(FAN_RELAY_PIN, HIGH);    // Fan OFF
  digitalWrite(VALVE_RELAY_PIN, LOW);   // Valve ON
  
  Serial.println("System Reset Complete - Back to Normal Operation");
  displayMessage("System Reset", "Normal Operation");
  delay(2000);
}

// ============================================================================
// ALARM CONTROL
// ============================================================================

void activateBuzzer() {
  // Buzzer pulse pattern (can be modified for different alarm patterns)
  static unsigned long lastToggle = 0;
  unsigned long currentTime = millis();
  
  if (currentTime - lastToggle >= BUZZER_BLINK_INTERVAL) {
    lastToggle = currentTime;
    buzzerActive = !buzzerActive;
  }
  
  digitalWrite(BUZZER_PIN, buzzerActive ? HIGH : LOW);  // HIGH = ON
}

void deactivateBuzzer() {
  buzzerActive = false;
  digitalWrite(BUZZER_PIN, LOW);  // Buzzer OFF
}

// ============================================================================
// DISPLAY FUNCTIONS
// ============================================================================

void updateDisplay() {
  static unsigned long lastDisplayUpdate = 0;
  
  if (millis() - lastDisplayUpdate < 500) {
    return;  // Update display every 500ms to avoid flicker
  }
  lastDisplayUpdate = millis();
  
  lcd.clear();
  
  // Line 1: System Status
  lcd.setCursor(0, 0);
  switch (currentState) {
    case STATE_NORMAL:
      lcd.print("System Normal");
      break;
    case STATE_GAS_DETECTED:
      lcd.print("LEAKAGE DETECTED!");
      break;
    case STATE_WARNING:
      lcd.print("Manual Reset Req");
      break;
    case STATE_MANUAL_OVERRIDE:
      lcd.print("Manual Override");
      break;
  }
  
  // Line 2: Gas Level and Valve Status
  lcd.setCursor(0, 1);
  lcd.print("Gas:");
  lcd.print(averageGasLevel);
  lcd.print(" V:");
  lcd.print(valveOpen ? "ON" : "OFF");
}

void displayMessage(const char* line1, const char* line2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

// ============================================================================
// LOGGING AND DIAGNOSTICS
// ============================================================================

void logSystemStatus() {
  Serial.println("\n--- System Status Report ---");
  Serial.print("State: ");
  Serial.println(getStateString());
  Serial.print("Gas Level: ");
  Serial.println(averageGasLevel);
  Serial.print("Leakage Detected: ");
  Serial.println(leakageDetected ? "YES" : "NO");
  Serial.print("Buzzer: ");
  Serial.println(buzzerActive ? "ON" : "OFF");
  Serial.print("Fan: ");
  Serial.println(fanActive ? "ON" : "OFF");
  Serial.print("Valve: ");
  Serial.println(valveOpen ? "OPEN" : "CLOSED");
  Serial.print("Manual Override: ");
  Serial.println((manualFanControl || manualValveControl) ? "YES" : "NO");
  Serial.println("----------------------------\n");
}

const char* getStateString() {
  switch (currentState) {
    case STATE_NORMAL:
      return "NORMAL";
    case STATE_GAS_DETECTED:
      return "GAS_DETECTED";
    case STATE_WARNING:
      return "WARNING";
    case STATE_MANUAL_OVERRIDE:
      return "MANUAL_OVERRIDE";
    default:
      return "UNKNOWN";
  }
}

// ============================================================================
// END OF CODE
// ============================================================================
