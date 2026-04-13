/*
 * CONFIGURATION HEADER FILE
 * Smart Gas Leakage Detection System
 * 
 * This file contains all configurable parameters for the system.
 * Modify these values to customize system behavior without changing main code.
 */

#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// SENSOR CONFIGURATION
// ============================================================================

// Gas Threshold Settings (in Analog Value 0-1023)
// For MQ-2 Sensor:
// - 0-300 PPM: Safe
// - 300-500 PPM: Warning
// - 500+ PPM: Danger
#define GAS_THRESHOLD              500    // Threshold to trigger alarm
#define GAS_PRE_ALERT_THRESHOLD    300    // Optional pre-alert threshold

// Sensor Reading Configuration
#define SENSOR_READ_DELAY          1000   // Delay between readings (milliseconds)
#define SENSOR_WARMUP_TIME         15000  // Warmup time on startup (milliseconds)
#define SENSOR_CALIBRATION_SAMPLES 100    // Samples for calibration

// Sensor Type (uncomment the one you're using)
#define SENSOR_TYPE_MQ2                   // LPG, Butane, Methane
// #define SENSOR_TYPE_MQ5                // LPG, LNG, Methane
// #define SENSOR_TYPE_MQ135              // Air Quality

// ============================================================================
// PIN CONFIGURATION
// ============================================================================

// Analog Input Pins
#define GAS_SENSOR_PIN             A0    // Analog pin for gas sensor

// Digital Output Pins
#define BUZZER_PIN                 9     // Buzzer control pin
#define FAN_RELAY_PIN              3     // Exhaust fan relay pin
#define VALVE_RELAY_PIN            4     // Gas valve relay pin

// Digital Input Pins
#define RESET_BUTTON_PIN           2     // System reset button
#define MANUAL_FAN_BUTTON          6     // Manual fan control button
#define MANUAL_VALVE_BUTTON        7     // Manual valve control button

// Optional Pins
#define TEMP_SENSOR_PIN            A1    // DHT11/DHT22 temperature sensor (optional)
#define HUMIDITY_SENSOR_PIN        A2    // Humidity sensor (optional)

// ============================================================================
// RELAY CONFIGURATION
// ============================================================================

// Relay Logic Configuration
// Some relays use inverted logic:
// TRUE = ON,  FALSE = OFF  (STANDARD RELAY)
// TRUE = OFF, FALSE = ON   (INVERTED RELAY)

#define RELAY_LOGIC_STANDARD       true   // Set to false if using inverted relay

// Fan Relay Settings
#define FAN_RELAY_ACTIVE_HIGH      false  // TRUE if fan is ON when pin is HIGH
#define FAN_RELAY_ACTIVE_LOW       true   // TRUE if fan is ON when pin is LOW

// Valve Relay Settings
#define VALVE_RELAY_ACTIVE_HIGH    true   // TRUE if valve is OPEN when pin is HIGH
#define VALVE_RELAY_ACTIVE_LOW     false  // TRUE if valve is OPEN when pin is LOW

// ============================================================================
// SAFETY TIMER CONFIGURATION
// ============================================================================

// Fan Auto-Off Timer
#define FAN_VENTILATION_TIME       180000 // How long to run fan after gas detected (milliseconds)
#define FAN_AUTO_OFF_ENABLED       true   // Enable auto-off after timeout
#define FAN_MIN_RUN_TIME           30000  // Minimum fan run time (30 seconds)

// Buzzer Pattern
#define BUZZER_PATTERN_CONTINUOUS true   // TRUE for continuous, FALSE for pulsing
#define BUZZER_PULSE_INTERVAL      500    // Pulse interval in milliseconds (if pulsing)
#define BUZZER_PULSE_WIDTH         250    // Pulse width in milliseconds (if pulsing)

// Button Debounce Time
#define BUTTON_DEBOUNCE_TIME       50     // Debounce delay (milliseconds)
#define BUTTON_HOLD_TIME           2000   // Time to hold for long-press detection (milliseconds)

// ============================================================================
// DISPLAY CONFIGURATION
// ============================================================================

// LCD Display I2C Settings
#define LCD_ADDRESS                0x27   // I2C address (0x27 for typical 16x2, 0x3F for some)
#define LCD_COLS                   16     // Number of columns
#define LCD_ROWS                   2      // Number of rows
#define LCD_UPDATE_INTERVAL        500    // Update display every X milliseconds

// Display Messages (customize as needed)
#define MSG_STARTUP_LINE1           "Gas Detection"
#define MSG_STARTUP_LINE2           "System Ready"
#define MSG_NORMAL_LINE1            "System Normal"
#define MSG_NORMAL_LINE2            "No Gas Detected"
#define MSG_DETECTED_LINE1          "LEAKAGE ALERT!"
#define MSG_DETECTED_LINE2          "Activating Safety"
#define MSG_WARNING_LINE1           "Manual Reset Req"
#define MSG_WARNING_LINE2           "Valve: OFF"
#define MSG_RESET_LINE1             "System Reset"
#define MSG_RESET_LINE2             "Returning Normal"

// ============================================================================
// EEPROM STORAGE CONFIGURATION
// ============================================================================

// EEPROM Memory Addresses for persistent storage
#define EEPROM_LEAKAGE_FLAG        0      // Leakage detection flag (1 byte)
#define EEPROM_LEAKAGE_COUNT       1      // Total leakage events (1 byte)
#define EEPROM_LAST_ALERT_TIME     5      // Timestamp of last alert (4 bytes)
#define EEPROM_TOTAL_FAN_TIME      9      // Total fan runtime (4 bytes)
#define EEPROM_SYSTEM_VERSION      13     // EEPROM data version (1 byte)

// Enable/Disable EEPROM Logging
#define ENABLE_EEPROM_LOGGING      true   // Enable persistent event logging

// ============================================================================
// COMMUNICATION CONFIGURATION
// ============================================================================

// Serial Communication
#define SERIAL_BAUD_RATE           9600   // Serial monitor baud rate
#define ENABLE_SERIAL_DEBUG        true   // Enable serial debugging output

// WiFi Settings (for ESP32 only)
#define ENABLE_WIFI                false  // Enable WiFi connectivity
#define WIFI_SSID                  "Your_SSID"
#define WIFI_PASSWORD              "Your_Password"
#define WIFI_RECONNECT_INTERVAL    30000  // Attempt WiFi reconnect every X ms

// Firebase Configuration (optional)
#define ENABLE_FIREBASE            false
#define FIREBASE_HOST              "your-project.firebaseio.com"
#define FIREBASE_AUTH              "your-auth-key"

// GSM Module Configuration (optional)
#define ENABLE_GSM                 false  // Enable GSM SMS/Call alerts
#define GSM_BAUD_RATE              9600   // GSM module baud rate
#define ALERT_PHONE_NUMBER         "+1234567890"  // Phone number for alerts

// ============================================================================
// ADVANCED FEATURES
// ============================================================================

// Data Logging
#define ENABLE_DATA_LOGGING        true   // Log sensor data over time
#define LOG_INTERVAL               5000   // Log every X milliseconds
#define LOG_MAX_ENTRIES            100    // Maximum log entries to store

// Sensor Filtering
#define ENABLE_SENSOR_FILTER       true   // Enable moving average filter
#define FILTER_WINDOW_SIZE         5      // Number of samples for averaging

// Adaptive Thresholds
#define ENABLE_ADAPTIVE_THRESHOLD  false  // Automatically adjust threshold based on environment
#define ADAPTION_RATE              0.1    // Rate of threshold adaptation

// Multi-Sensor Support
#define ENABLE_MULTI_SENSOR        false  // Support multiple gas sensors
#define NUM_OF_SENSORS             1      // Number of sensors connected

// ============================================================================
// SAFETY FEATURES
// ============================================================================

// Safety Lock
#define ENABLE_SAFETY_LOCK         true   // Prevent valve reopening immediately after detection

// Watchdog Timer (for Arduino boards that support it)
#define ENABLE_WATCHDOG            true   // Enable watchdog timer
#define WATCHDOG_TIMEOUT           8000   // Watchdog timeout (milliseconds)

// System Health Checks
#define ENABLE_HEALTH_CHECK        true   // Enable periodic health checks
#define HEALTH_CHECK_INTERVAL      60000  // Health check every X milliseconds

// ============================================================================
// TESTING AND CALIBRATION
// ============================================================================

// Calibration Mode
#define CALIBRATION_MODE           false  // Set to true for sensor calibration
#define BASELINE_VOLTAGE           0      // Baseline voltage in clean air (auto-calculated)

// Test Mode
#define TEST_MODE                  false  // Set to true to simulate gas detection
#define TEST_GAS_LEVEL             600    // Simulated gas level when in test mode

// ============================================================================
// DEVELOPMENT & DEBUG SETTINGS
// ============================================================================

// Verbose Logging
#define DEBUG_LEVEL                2      // 0=None, 1=Error, 2=Warning, 3=Info, 4=Verbose
#define VERBOSE_SENSOR_READINGS    false  // Print every sensor reading
#define VERBOSE_STATE_CHANGES      true   // Print state machine transitions

// Performance Profiling
#define ENABLE_PROFILING           false  // Enable performance timing measurements
#define PROFILE_INTERVAL           10000  // Profile every X milliseconds

#endif // CONFIG_H
