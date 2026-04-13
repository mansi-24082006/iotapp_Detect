# Implementation & Deployment Guide

## 1. System Requirements Checklist

### Hardware Requirements

```
ESSENTIAL COMPONENTS:
☐ Arduino Uno or Mega (or compatible clone)
☐ MQ-2 Gas Sensor Module (pre-assembled, with I2C or analog)
☐ 2-Channel 5V Relay Module
☐ 16x2 LCD Display with I2C adapter (I2C address 0x27 or 0x3F)
☐ 5V Active Piezo Buzzer
☐ 12V DC Exhaust Fan (500-1000mA)
☐ 12V Solenoid Gas Valve (NC - Normally Closed)
☐ Push buttons (x3: Reset, Manual Fan, Manual Valve)
☐ Resistors: 1kΩ (x4), 10kΩ (x3), pull-up for buttons
☐ Diodes: 1N4007 (x2) for relay protection
☐ Capacitors: 1000µF, 100µF, 100nF ceramic (multiple)
☐ 12V Power Supply (10A minimum)
☐ 5V Power Supply or Voltage Regulator (LM7805)
☐ 10A Fuse and holder
☐ Wiring (18-24 AWG, depending on current)
☐ Solder and soldering iron
☐ Heatshrink tubing
☐ PCB or breadboard for prototyping
```

### Software Requirements

```
☐ Arduino IDE (latest version)
☐ USB CH340 Driver (if using Arduino clone)
☐ Libraries:
  ☐ Wire.h (I2C - built-in)
  ☐ LiquidCrystal_I2C.h (LCD control)
  ☐ EEPROM.h (Persistent storage - built-in)

OPTIONAL LIBRARIES:
☐ DHT.h (for temperature/humidity sensor)
☐ WiFi.h and FirebaseESP32.h (for IoT integration)
☐ SoftwareSerial.h (for external serial devices)
```

### Tools & Equipment

```
☐ Multimeter (voltage, continuity checking)
☐ Soldering iron & solder
☐ Wire strippers
☐ Crimpers (for terminal connections)
☐ Wrench set (for solenoid installation)
☐ Compressed air (for sensor cleaning)
☐ Safety glasses & gloves
☐ Hot glue gun (for component mounting)
☐ Cable ties / zip ties
```

## 2. Step-by-Step Assembly Instructions

### Phase 1: Prototype & Testing (Breadboard)

#### Step 1: Basic Arduino Setup

```
1. Install Arduino IDE on your computer
2. Connect Arduino via USB
3. Open Tools → Board → Select "Arduino Uno"
4. Open Tools → Port → Select appropriate COM port
5. Install required library:
   - Sketch → Include Library → Manage Libraries
   - Search for "LiquidCrystal_I2C"
   - Click Install
6. Load example sketch to verify:
   - File → Examples → Basics → Blink
   - Upload and verify LED blinks
```

#### Step 2: Wire on Breadboard

```
CONNECTIONS IN ORDER:

1. Power Distribution:
   - 5V supply to row 1 (VCC rail)
   - GND supply to row 2 (GND rail)
   - 12V supply separate

2. Arduino Placement:
   - Place Arduino on breadboard
   - Connect Arduino GND to GND rail
   - Connect Arduino 5V to VCC rail

3. Gas Sensor:
   - Connect VCC pin to 5V rail
   - Connect GND to GND rail
   - Connect AOUT to Arduino A0 via wire

4. LCD Display:
   - Connect VCC to 5V rail
   - Connect GND to GND rail
   - Connect SDA to Arduino A4
   - Connect SCL to Arduino A5

5. Relay Module:
   - Connect VCC to 5V rail
   - Connect GND to GND rail
   - Connect IN1 to Arduino D3 (via 1kΩ resistor)
   - Connect IN2 to Arduino D4 (via 1kΩ resistor)

6. Buzzer:
   - Connect positive pin to Arduino D9 (via 1kΩ resistor)
   - Connect negative pin to GND rail

7. Push Buttons:
   - Reset button: D2 to GND (normal high with internal pullup)
   - Fan button: D6 to GND
   - Valve button: D7 to GND
```

#### Step 3: Test Basic Functions

```
UPLOAD THIS TEST CODE:

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(9, OUTPUT);  // Buzzer
  Serial.println("System test starting...");
}

void loop() {
  // Test ADC reading
  int gasLevel = analogRead(A0);
  Serial.print("Gas Level: ");
  Serial.println(gasLevel);

  // Test buzzer
  digitalWrite(9, HIGH);
  delay(100);
  digitalWrite(9, LOW);
  delay(900);
}

VERIFY:
- Serial monitor shows gas readings
- Buzzer produces sound every 1 second
- No error messages
```

### Phase 2: Sensor Calibration

#### Step 4: Sensor Warm-up & Baseline

```
1. Power up system fresh
2. Monitor serial output for 20+ seconds
3. Record baseline ADC value when stable
4. Update CONFIG: #define BASELINE_ADC [your_value]
5. Note baseline voltage for records

EXPECTED BASELINE:
- Voltage: 3.5-4.5V (typically 4.0V)
- ADC: 720-920 (typically 820)
```

#### Step 5: Threshold Setting

```
1. Modify GAS_THRESHOLD in config.h
   - Start with 500 (factory default)

2. Optional: Test with gas source
   - Use controlled gas exposure
   - Record ADC at different levels
   - Verify threshold appropriate

3. Document baseline for future reference
```

### Phase 3: Integration Testing

#### Step 6: Upload Full Application Code

```
1. Copy GasLeakDetection.ino to Arduino IDE
2. Compile: Verify → Compile (Ctrl+Shift+R)
3. Upload: Upload (Ctrl+U)
4. Open Serial Monitor (Tools → Serial Monitor)
5. Observe startup sequence:
   - "System Init..." message
   - Pin configuration complete
   - EEPROM load
   - Sensor warming (15 seconds)
```

#### Step 7: Test All Inputs

```
TEST RESET BUTTON:
├─ Press reset button
├─ Observe serial: "RESET BUTTON PRESSED"
├─ System should return to NORMAL state
└─ LCD should show "System Normal"

TEST MANUAL CONTROLS:
├─ Press fan switch
├─ Observe: Fan relay activates
├─ Press valve switch
├─ Observe: Valve relay activates

TEST GAS DETECTION:
├─ Expose sensor to gas (lighter fume works)
├─ Observe ADC jumps above threshold
├─ Buzzer should activate
├─ Fan relay should turn ON
├─ Valve relay should turn OFF
└─ LCD should display "LEAKAGE DETECTED!"

TEST DISPLAY:
├─ Verify all text visible on LCD
├─ Check contrast if needed (adjust potentiometer)
```

## 3. Permanent Installation

### Phase 4: Mounting & Enclosure

#### Step 8: Prepare Enclosure

```
MATERIALS NEEDED:
- Plastic or metal project box (10x15x8 cm minimum)
- Through-hole connectors for external signals
- DIN-rail or adhesive for component mounting
- Thermal pads for components
- Mounting brackets

LAYOUT PLANNING:
1. Mark positions for:
   - Arduino board
   - Relay module
   - Power supply/regulator
   - Terminal blocks

2. Cut holes for:
   - Button access (reset at front)
   - Gas sensor connection
   - LCD display window
   - Power inlet
   - Output connectors (fan, valve)

3. Install:
   - Arduino DIN-rail mounting
   - Relay module on shelf
   - Terminal blocks on backplane
   - LCD display on front panel
```

#### Step 9: Solder Permanent Connections

```
CONVERT FROM BREADBOARD TO SOLDERED:

1. Photo-document breadboard layout
2. De-solder one connection at a time
3. Solder directly to component pads or headers
4. Use heatshrink over all solder joints
5. Double-check polarity before moving on

SOLDER JOINTS SHOULD BE:
- Shiny and smooth (not dull)
- Cone-shaped (not globular)
- No cold joints (dull appearance) - RESOLDER
- No bridging between connections

USE FLUX:
- Helps solder flow
- Better joint quality
- Clean flux residue after with IPA
```

#### Step 10: Install External Connections

```
GAS SENSOR:
- Mount near potential leak source
- 15-20cm minimum from walls
- Protect from dust with mesh screen
- Use twisted-pair shielded cable (if long runs)

EXHAUST FAN:
- Wall or duct mounted for outdoor discharge
- Flexible duct connection (vibration isolation)
- One-way damper on intake (prevents backflow)
- Accessible for cleaning

GAS SOLENOID VALVE:
- Inline with main gas supply line
- Thread-seal tape on all connections
- Pressure relief valve upstream (safety)
- Manual shutoff valve nearby (emergency)
- Check for leaks with soapy water

DISPLAY:
- Mount on wall at eye level
- Protected from direct water spray
- Not in direct sunlight
- Visible from key locations
```

## 4. Software Configuration

### Step 11: Configure System Parameters

```
EDIT CONFIG.H:

#define GAS_THRESHOLD        500    // Your calibrated value
#define GAS_SENSOR_PIN       A0     // Adjust if needed
#define BUZZER_PIN           9      // Change if needed
#define LCD_ADDRESS          0x27   // Verify your I2C address

// Thresholds based on environment:
// Home/Kitchen:     500-600 PPM
// Industrial:       300-400 PPM
// High-traffic:     600-800 PPM

#define FAN_VENTILATION_TIME 180000 // 3 minutes
#define SENSOR_READ_DELAY    1000   // 1 second intervals

OPTIONAL SETTINGS:

#define DEBUG_LEVEL          3      // Info-level logging
#define ENABLE_EEPROM_LOGGING true  // Save events
#define ENABLE_SERIAL_DEBUG   true  // Serial output
```

### Step 12: Test Configuration

```
1. Upload configured code
2. Verify serial output shows correct values:
   Serial.println(String("Gas Level: ") + gaslevel);
3. Check LCD displays correct information
4. Perform full system test again
5. Adjust as needed
```

## 5. Verification & Commissioning

### Step 13: Pre-Operation Verification

```
ELECTRICAL SAFETY:
☐ Mulitmeter: Check 5V supply (4.8-5.2V)
☐ Multimeter: Check 12V supply (11.5-12.5V)
☐ Continuity: Test ground connections
☐ Insulation: No exposed wires
☐ Thermal: Use thermal camera, check for hot spots

FUNCTIONAL VERIFICATION:
☐ Sensor reads valid values
☐ Baseline established and saved
☐ All buttons respond to press
☐ All relays click when activated
☐ Buzzer produces audible sound
☐ LCD displays all characters
☐ Reset clears system state
☐ Manual override works

GAS LEAKAGE SIMULATION:
☐ Expose sensor to light gas (lighter fume)
☐ Verify alarm activation within 5 seconds
☐ Check all outputs activate
☐ Verify state transition to WARNING
☐ Verify manual reset works
```

## 6. Documentation & Records

### Step 14: Create System Documentation

```
CREATE FILE: SYSTEM_LOG.txt

SYSTEM INSTALLATION RECORD:
Date Installed: ____________
Location: ___________________
Technician: _________________
Contact: ____________________

HARDWARE RECORD:
Arduino Model: _____________
Sensor Serial: _____________
Sensor Baseline ADC: _______
Threshold Setting: _________
Calibration Date: __________

FIRMWARE RECORD:
Version: 1.0.0
Upload Date: _______________
Last Update: _______________
Configuration Hash: ________

MAINTENANCE SCHEDULE:
Next Inspection: ___________
Next Sensor Replacement: ____
Annual Service: ____________

EMERGENCY CONTACTS:
Gas Utility: _______________
Fire Department: ___________
Technician Service: ________
```

## 7. User Training

### Step 15: Operator Training

```
TRAINING CHECKLIST:

Basic Operation:
☐ Show reset button location and purpose
☐ Explain daily visual checks
☐ Show how to read display status
☐ Train on emergency procedures

Display Status Codes:
☐ "System Normal" - OK to proceed
☐ "LEAKAGE DETECTED" - Evacuate
☐ "Manual Reset Required" - Press reset
☐ "System Error" - Call technician

Emergency Procedures:
☐ Evacuation route during alarm
☐ Manual gas shutoff location
☐ Phone emergency services number
☐ When NOT to use electronic devices
☐ Assembly point outside

Maintenance:
☐ Show sensor location
☐ Quarterly cleaning procedure
☐ When to call for service
☐ Record keeping expectations
```

## 8. Ongoing Maintenance

### Monthly Checks

```
- Visual inspection for damage
- Test all buttons
- Verify gas sensor baseline
- Check for any error messages
```

### Quarterly Checks

```
- Functional test of all components
- Gas level accuracy verification
- Relay response verification
- Buzzer volume test
```

### Annual Tasks

```
- Professional electrical inspection
- Sensor replacement (recommended)
- Full system recalibration
- Safety certification if required
- Update documentation
```

## 9. Warranty & Support

```
WARRANTY COVERAGE:
- Component failures: Check vendors
- Software issues: Free updates
- Installation defects: 30 days

SUPPORT OPTIONS:
- Technical documentation on-site
- Remote troubleshooting via serial monitor
- Component replacement guidance
- Professional service referrals

CREATE FILE: SUPPORT_CONTACTS.txt
Include:
- Sensor manufacturer support
- Solenoid valve seller
- Arduino community forums
- Local HVAC technician
- Gas utility contact
- Emergency services number
```

---

**Next**: See [TROUBLESHOOTING.md](TROUBLESHOOTING.md) for common issues and solutions.
