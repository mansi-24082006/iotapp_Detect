# Enhanced Gas Detection Code - Modifications Guide

## Overview

Your existing code has been enhanced with the Smart Gas Leakage Detection System features while maintaining full WiFi and Firebase functionality.

## Key Enhancements Added

### 1. **State Machine Logic**

```
NORMAL → GAS_DETECTED → WARNING
   ↑                        ↓
   └────[Reset Button]──────┘
```

- **NORMAL**: System idle, all operating normally
- **GAS_DETECTED**: Gas detected above threshold, all alarms active
- **WARNING**: Gas normalized but system requires manual reset, valve stays closed for safety

### 2. **New Hardware Pins Defined**

```cpp
#define RELAY_FAN 26          // Separate relay for fan
#define RELAY_VALVE 27        // Separate relay for solenoid valve
#define RESET_BUTTON_PIN 32   // Manual reset button
#define MANUAL_FAN_SWITCH 33  // Manual fan control
#define MANUAL_VALVE_SWITCH 35 // Manual valve control
```

### 3. **LCD Display Integration**

- 16x2 I2C LCD display added
- Shows system status and gas levels
- Auto-updates every 500ms

### 4. **EEPROM Persistent Storage**

- Leakage flag saved to EEPROM
- Remembers detection even after power loss
- Prevents false positive resets

### 5. **Sensor Smoothing**

- Moving average filter (5-sample window)
- Reduces noise from sensor readings
- More stable threshold detection

### 6. **Gas Sensor Reading**

```cpp
void readGasSensor() {
  // Smoothing with moving average
  // Filters out noise for reliable detection
}
```

### 7. **Automatic Safety Response**

When gas is detected:

- Buzzer turns ON immediately
- Fan relay activates (RELAY_FAN)
- Valve relay closes (RELAY_VALVE)
- Servo moves to 0° (valve closed position)
- LCD shows "LEAKAGE DETECT!"

When gas normalizes:

- Buzzer turns OFF
- Fan continues running for 3 minutes (ventilation)
- **Valve stays OFF** (safety lock)
- LCD shows "Manual Reset Req"
- System requires button press to recover

### 8. **Manual Controls**

- Manual fan switch (GPIO 33): Toggle fan ON/OFF
- Manual valve switch (GPIO 35): Toggle valve OPEN/CLOSED
- Override automatic controls during emergencies

### 9. **Firebase Integration (Preserved)**

All your original WiFi and Firebase features remain:

- Updates `/device` with real-time status
- Reads remote commands from `/device`
- Updates threshold settings
- Status includes new state information

## Hardware Connections for ESP32

```
SENSOR & CONTROLS:
GPIO 34 (A6) ── MQ6 Gas Sensor
GPIO 32      ── Reset Button + GND
GPIO 33      ── Manual Fan Switch + GND
GPIO 35      ── Manual Valve Switch + GND

OUTPUTS:
GPIO 25      ── Buzzer (HIGH = ON)
GPIO 26      ── Fan Relay (HIGH = ON)
GPIO 27      ── Valve Relay (HIGH = ON)
GPIO 14      ── Servo Control (90° = Open, 0° = Closed)

I2C (LCD Display):
GPIO 21 (SDA) ── LCD SDA
GPIO 22 (SCL) ── LCD SCL

Firebase: WiFi (unchanged)
```

## Installation Steps

### Step 1: Update Hardware

If adding new pins:

```
- Connect relay module second channel (pin 27) for solenoid valve
- Add second relay if not using same module
- Install push buttons on GPIO 32, 33, 35
- Connect LCD I2C (SDA-21, SCL-22)
```

### Step 2: Install Required Libraries

In Arduino IDE:

1. Sketch → Include Library → Manage Libraries
2. Search and install:
   - `LiquidCrystal_I2C` (Frank de Brabander)
   - `EEPROM` (already built-in)

### Step 3: Upload Code

```cpp
// Replace the old code completely OR
// Use the new GasLeakDetection_Enhanced.ino file
```

### Step 4: Configure LCD Address

If LCD doesn't display:

```cpp
// Change this line to match your I2C address
#define LCD_ADDRESS 0x27  // Try 0x3F if this doesn't work
```

To find LCD address:

- Use I2C scanner sketch
- Serial monitor will show address (usually 0x27 or 0x3F)

### Step 5: Adjust Threshold

The MQ6 threshold remains at 1600 but you can change via:

- Firebase: Send new value to `/settings/threshold`
- Code: Update `gasThreshold = 1600;` value
- WiFi remote commands still work

## Feature Comparison

| Feature              | Original | Enhanced          |
| -------------------- | -------- | ----------------- |
| WiFi Control         | ✓        | ✓ (Preserved)     |
| Firebase Integration | ✓        | ✓ (Enhanced)      |
| MQ6 Gas Sensing      | ✓        | ✓ (+ smoothing)   |
| Buzzer Control       | ✓        | ✓ (State-aware)   |
| Fan Relay            | ✓        | ✓ (Enhanced)      |
| Servo Control        | ✓        | ✓ (Valve control) |
| Valve Relay          | ✗        | ✓ (NEW)           |
| State Machine        | ✗        | ✓ (NEW)           |
| LCD Display          | ✗        | ✓ (NEW)           |
| EEPROM Persistence   | ✗        | ✓ (NEW)           |
| Manual Reset         | ✗        | ✓ (NEW)           |
| Safety Lockout       | ✗        | ✓ (NEW)           |
| Sensor Smoothing     | ✗        | ✓ (NEW)           |

## Firebase Data Structure

Your Firebase now receives:

```json
{
  "device": {
    "gasLevel": 1450,
    "status": "SAFE",
    "state": "NORMAL",
    "fan": false,
    "buzzer": false,
    "valve": true,
    "leakageDetected": false,
    "lastUpdated": 1234567890
  },
  "settings": {
    "threshold": 1600
  }
}
```

## Serial Output

Enhanced debug information:

```
Gas: 1450 | Threshold: 1600 | Status: SAFE | State: NORMAL | WiFi: OK | Firebase: OK | Write: OK
Gas: 1800 | Threshold: 1600 | Status: DANGER | State: GAS_DETECTED | WiFi: OK | Firebase: OK | Write: OK
Gas: 1400 | Threshold: 1600 | Status: WARNING | State: WARNING | WiFi: OK | Firebase: OK | Write: OK
```

## Testing Checklist

- [ ] Upload code successfully
- [ ] Serial monitor shows boot messages
- [ ] LCD displays "System Normal"
- [ ] WiFi connects to your network
- [ ] Firebase receives updates every second
- [ ] MQ6 sensor readings display
- [ ] Reset button works (returns to NORMAL)
- [ ] Manual switches control fan/valve
- [ ] Gas detection triggers alarm
- [ ] System enters WARNING state
- [ ] Valve stays closed in WARNING
- [ ] Fan auto-stops after 3 minutes

## Troubleshooting

### LCD Not Displaying

```cpp
// Try different I2C address
#define LCD_ADDRESS 0x3F  // Instead of 0x27
```

### Sensor Not Reading

```
Check GPIO 34 connection to MQ6 AOUT pin
Verify sensor is powered (VCC/GND connected)
```

### Reset Button Not Working

```
Test with Serial.println("Button press") code
Check GPIO 32 connected to pin with pull-up
```

### Relay Not Activating

```
Verify GPIO 26/27 are connected properly
Check relay module works independently
Test with digitalWrite(RELAY_FAN, HIGH);
```

### Firebase Not Updating

```
Check WiFi connection (Serial shows "WiFi: OK")
Verify Firebase URL and AUTH token
Check Firebase rules allow write access
```

## Customization

### Change Sensor Threshold

In Firebase or code:

```cpp
gasThreshold = 1200;  // Lower = more sensitive
gasThreshold = 2000;  // Higher = less sensitive
```

### Change Fan Auto-Off Timer

```cpp
// Default: 180 seconds (3 minutes)
// In updateSystemState():
if (millis() - fanStartTime >= 300000) {  // 5 minutes
```

### Change Display Update Rate

```cpp
// Default: 500ms
if (millis() - lastDisplayUpdate < 1000) {  // Every 1 second
```

### Change Sensor Read Rate

```cpp
// Default: 1000ms (1 second)
if (millis() - lastSensorReadTime >= 2000) {  // Every 2 seconds
```

## Migration from Original Code

### Option 1: Direct Replacement (Recommended)

1. Delete old code
2. Load `GasLeakDetection_Enhanced.ino`
3. Update pins if different
4. Compile and upload

### Option 2: Merge Features (Advanced)

1. Copy state machine functions
2. Copy sensor smoothing function
3. Copy button handlers
4. Copy display functions
5. Integrate into your existing structure

## Safety Notes

⚠️ **Important**:

- Solenoid valve should be **Normally Closed (NC)** type
- Fails safe if power is lost
- Always test in safe environment first
- Real gas testing requires proper ventilation

## Support

If you encounter issues:

1. Check serial monitor for error messages
2. Verify all physical connections
3. Review Firebase logs for communication status
4. Test hardware components independently
5. Use I2C scanner for LCD address verification

## Version History

- **v1.0 (Original)**: WiFi-controlled gas detection
- **v2.0 (Enhanced)**: Added state machine, safety lockout, LCD, EEPROM persistence

---

**Keep your existing WiFi and Firebase setup - it's fully integrated!** ✓
