# System Architecture & Design

## 1. Overall System Architecture

### Block Diagram

```
┌────────────────────────────────────────────────────────────────────┐
│                        SMART GAS DETECTION SYSTEM                   │
├────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  SENSING LAYER                  PROCESSING LAYER   CONTROL LAYER   │
│  ───────────────────────        ──────────────────  ─────────────  │
│                                                                     │
│  ┌──────────────────┐           ┌─────────────────────────────┐   │
│  │  Gas Sensor      │           │   Arduino/ESP32             │   │
│  │  (MQ-2/MQ-5)     ├──────────→│  - ADC Conversion           │   │
│  │  Analog: 0-1023  │           │  - State Machine            │   │
│  └──────────────────┘           │  - Logic Processing         │   │
│                                  │  - Timer Management         │   │
│  ┌──────────────────┐           │  - EEPROM Control           │───┐
│  │  Reset Button    │           │  - Serial Communication    │   │
│  │  Manual Switches │───────────→│  - Display Rendering        │   │
│  └──────────────────┘           └─────────────────────────────┘   │
│                                           ↓↓↓                      │
│                                                                     │
│                           ┌────────────────────────────────┐       │
│                           │   OUTPUT DEVICES               │       │
│                           ├────────────────────────────────┤       │
│                           │ ┌──────────────────────────┐    │       │
│                           │ │ Exhaust Fan (12V)        │◄───┤ Relay│
│                           │ └──────────────────────────┘    │       │
│                           │ ┌──────────────────────────┐    │       │
│                           │ │ Gas Valve (NC Solenoid)  │◄───┤ Relay│
│                           │ └──────────────────────────┘    │       │
│                           │ ┌──────────────────────────┐    │       │
│                           │ │ Buzzer (5V)              │◄───┤ GPIO │
│                           │ └──────────────────────────┘    │       │
│                           │ ┌──────────────────────────┐    │       │
│                           │ │ LCD Display (I2C)        │◄───┤ I2C  │
│                           │ └──────────────────────────┘    │       │
│                           │ ┌──────────────────────────┐    │       │
│                           │ │ WiFi/GSM Module (Opt)    │◄───┤ UART │
│                           │ └──────────────────────────┘    │       │
│                           └────────────────────────────────┘       │
│                                                                     │
│  POWER DISTRIBUTION                                                │
│  ─────────────────────────────                                     │
│  12V Supply ──→ Fan, Valve                                         │
│  5V Supply  ──→ Buzzer, Relays, Arduino, LCD, Sensors            │
│                                                                     │
└────────────────────────────────────────────────────────────────────┘
```

## 2. Hardware Architecture

### Microcontroller: Arduino Uno/Mega or ESP32

- **Role**: Central decision-making unit
- **Processing**: Monitors sensor, executes logic, controls outputs
- **Memory**: SRAM for variables, EEPROM for persistent flags
- **Communication**: Serial (debug), I2C (LCD), GPIO (relays/buttons)

### Sensing Subsystem

```
Gas Sensor (MQ-2)
├── Output Type: Analog voltage (0-5V)
├── Resolution: 10-bit ADC (0-1023)
├── Response Time: ~1-30 seconds
├── Detection Range: 300-10,000 PPM (depends on gas)
└── Calibration: ~15 seconds warm-up required
```

### Control Subsystem

```
Relay Modules (2-Channel 5V)
├── Fan Relay
│   ├── Normally Open / Closed: NO (to stop gas leakage)
│   ├── Control Logic: LOW=ON, HIGH=OFF
│   └── Load: 12V DC Exhaust Fan
├── Valve Relay
│   ├── Normally Closed for safety: NC
│   ├── Control Logic: LOW=OPEN, HIGH=CLOSED
│   └── Load: 12V Solenoid Gas Valve
└── Buzzer (Direct GPIO)
    ├── Normally Open: NO
    ├── Control Logic: HIGH=ON, LOW=OFF
    └── Pulsing for lower power consumption
```

### Display Subsystem

```
LCD 16x2 Display (I2C)
├── Address: 0x27 (typical) or 0x3F
├── Communication: I2C (SDA/SCL)
├── Update Rate: 500ms (prevent flicker)
└── Content: Real-time status and gas levels
```

## 3. Electrical Schematic

### Schematic Description

```
┌─────────────────────────────────────────────────────┐
│            POWER SUPPLY SECTION                     │
├─────────────────────────────────────────────────────┤
│                                                     │
│  12V Power Source                                   │
│       │                                             │
│       ├──→ 12V Fan                                  │
│       │                                             │
│       ├──→ 12V Solenoid Valve                       │
│       │                                             │
│       └──→ 5V Voltage Regulator (LM7805)            │
│            │                                        │
│            ├──→ Arduino 5V Pin                      │
│            ├──→ Relay Module VCC                    │
│            ├──→ LCD Display VCC                     │
│            ├──→ Buzzer (via transistor)             │
│            └──→ Sensor VCC                          │
│                                                     │
└─────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────┐
│            CONTROL CIRCUIT SECTION                  │
├─────────────────────────────────────────────────────┤
│                                                     │
│  MQ-2 Sensor:                                       │
│  ─────────────────                                  │
│  VCC → 5V                                           │
│  GND → Ground                                       │
│  Out → A0 (ADC)                                     │
│  Aout → Not used                                    │
│                                                     │
│  Reset Button:                                      │
│  ──────────────                                     │
│  Pin 2 → Button → Ground (with pullup)             │
│                                                     │
│  Relay Module:                                      │
│  ──────────────                                     │
│  VCC → 5V                                           │
│  GND → Ground                                       │
│  IN1 → Pin 3 (Fan Control)                          │
│  IN2 → Pin 4 (Valve Control)                        │
│  Fan Terminal: COM ↔ NO (Normally Open)             │
│  Valve Terminal: COM ↔ NC (Normally Closed)         │
│                                                     │
│  Buzzer:                                            │
│  ────────                                           │
│  Positive → Pin 9 (via 1kΩ resistor)                │
│  Negative → Ground                                  │
│                                                     │
│  LCD Display (I2C):                                 │
│  ────────────────────                               │
│  VCC → 5V                                           │
│  GND → Ground                                       │
│  SDA → A4 (SDA)                                     │
│  SCL → A5 (SCL)                                     │
│  Backlight control → 5V (always on)                 │
│                                                     │
└─────────────────────────────────────────────────────┘
```

## 4. Software Architecture

### State Machine Diagram

```
                    ┌──────────────┐
                    │   STARTUP    │
                    └──────┬───────┘
                           ↓
                   ╔──────────────╗
                   ║   NORMAL     ║  ← Idle state
                   ╚──────┬───────╝
                          │
                   Gas Level > Threshold
                          ↓
                   ╔──────────────╗
                   ║ GAS_DETECTED ║  ← Alarm active
                   ╚──────┬───────╝
                          │
                   Gas Level < Threshold
                          ↓
                   ╔──────────────╗
                   ║   WARNING    ║  ← Requires reset
                   ╚──────┬───────╝
                          │
                   Reset Button Pressed
                          ↓
                  ┌──────────────────┐
                  │  RESET SEQUENCE  │
                  │ (Clear Flag)     │
                  └──────┬───────────┘
                         ↓
                   ╔──────────────╗
                   │   NORMAL     ║
                   ╚──────────────╝
```

### Main Loop Flow

```
┌─────────────────────────────────────┐
│          LOOP START                 │
└────────────────┬────────────────────┘
                 ↓
    ┌────────────────────────────┐
    │ Read User Inputs           │
    │ • Check reset button       │
    │ • Check manual switches    │
    └────────────┬───────────────┘
                 ↓
    ┌────────────────────────────┐
    │ Non-blocking Sensor Read   │
    │ (Every 1000ms)             │
    │ • Read analog value        │
    │ • Calculate average        │
    │ • Smooth with filter       │
    └────────────┬───────────────┘
                 ↓
    ┌────────────────────────────┐
    │ Execute State Machine      │
    │ • Check thresholds         │
    │ • Transition states        │
    │ • Update flags             │
    │ • Save to EEPROM           │
    └────────────┬───────────────┘
                 ↓
    ┌────────────────────────────┐
    │ Update Outputs             │
    │ • Control buzzer           │
    │ • Control fan              │
    │ • Control valve            │
    │ • Apply manual overrides    │
    └────────────┬───────────────┘
                 ↓
    ┌────────────────────────────┐
    │ Update Display             │
    │ • Show status message      │
    │ • Show gas level           │
    │ • Show valve position      │
    └────────────┬───────────────┘
                 ↓
    ┌────────────────────────────┐
    │ Log System Status (5sec)   │
    │ • Serial output            │
    │ • EEPROM logging           │
    │ • Data storage             │
    └────────────┬───────────────┘
                 ↓
             (LOOP)
```

## 5. Data Flow Diagram

```
Input Data → Sensor Reading → Filtering → Threshold Check
     ↓             ↓                ↓           ↓
   Raw ADC      Smoothing      Moving Avg    Detection?
   (0-1023)     (5-sample)     (0-1023)        YES
                                               ↓
                                         State Change
                                         ↓ ↓ ↓ ↓
                                    ┌────────────────┐
                                    │ Gas Detected   │
                                    │ Set Flag       │
                                    │ Activate Buzzer│
                                    │ Turn Fan ON    │
                                    │ Close Valve    │
                                    │ Update Display │
                                    │ Log Event EEPROM
                                    └────────────────┘
```

## 6. Module Dependencies

```
main.ino (GasLeakDetection.ino)
    ├── config.h
    │   └── Pin definitions
    │   └── Thresholds
    │   └── Timers
    │
    ├── Wire.h (I2C for LCD)
    │   └── LiquidCrystal_I2C.h
    │       └── LCD display control
    │
    ├── EEPROM.h
    │   └── Persistent storage
    │
    ├── Serial
    │   └── Debug output
    │
    └── GPIO/ADC
        ├── Sensor reading
        ├── Button inputs
        └── Relay outputs
```

## 7. Safety Architecture

### Multi-Level Safety

```
Level 1: Hardware Failsafe
  └─ Solenoid valve set to NC (Normally Closed)
     Fails safe if power is lost

Level 2: State Memory
  └─ EEPROM stores leakage flag
     Prevents false positive resets

Level 3: Manual Override
  └─ Users can manually control Fan/Valve
     Emergency control bypass

Level 4: Timeout Protection
  └─ Fan auto-off timer (3 minutes)
     Prevents indefinite operation

Level 5: Real-time Monitoring
  └─ Continuous sensor reading
     Quick response to gas changes
```

## 8. Scalability & Future Expansion

### Multi-Node Architecture (For Advanced Systems)

```
Primary Unit (Head)
  ├── Master Microcontroller
  ├── Main Sensor
  └── WiFi/Network Interface
       └──→ Cloud (Firebase/AWS)
                 ↑
            Data Upstream

Remote Nodes (Field Units)
  ├── Secondary Microcontroller
  ├── Local Sensor
  └── Wireless Communication
       └──→ Primary Unit ←→ Cloud
       └──→ Cloud Direct (with WiFi)
```

### Component Addition Points

1. **Additional Sensors**

   - Secondary MQ sensors at different locations
   - Temperature/humidity monitoring
   - Flame detection

2. **Communication Expansion**

   - WiFi module for remote access
   - GSM for SMS alerts
   - LoRa for long-range mesh networks

3. **Data Enhancement**
   - Cloud database for analytics
   - Mobile app integration
   - Predictive algorithms

## 9. Power Budget Analysis

| Component          | Power (mA)   | Voltage | Notes               |
| ------------------ | ------------ | ------- | ------------------- |
| Arduino Uno        | 50           | 5V      | Microcontroller     |
| MQ-2 Sensor        | 150          | 5V      | Sensor heater       |
| LCD Display        | 30           | 5V      | Display & backlight |
| Buzzer             | 30           | 5V      | Alarm (avg)         |
| Relay Coils        | 80           | 5V      | Both coils          |
| **Total @ 5V**     | **~340mA**   | **5V**  | Logic side          |
| **Fan Motor**      | **500-1000** | **12V** | External load       |
| **Solenoid Valve** | **300-500**  | **12V** | External load       |

**Recommendation**: 12V/10A supply for robust operation

## 10. Communication Protocols

### I2C Protocol (LCD)

- Address: 0x27 (default)
- Clock: 100kHz standard
- Data: SDA/SCL pins

### Serial Protocol (Debug)

- Baud Rate: 9600
- Format: 8N1 (8 bits, No parity, 1 stop bit)
- Port: USB/Serial monitor

### Relay Control (GPIO)

- Logic: 5V digital HIGH/LOW
- Pin mode: OUTPUT
- Control: digitalWrite()

## File Organization

```
gas_detection_system/
├── arduino_code/
│   ├── GasLeakDetection.ino      (Main code)
│   ├── config.h                   (Configuration)
│   └── [future modules]
├── documentation/
│   ├── ARCHITECTURE.md            (This file)
│   ├── FLOWCHART.md
│   ├── SAFETY.md
│   └── CALIBRATION.md
└── diagrams/
    ├── circuit_diagram.txt
    ├── state_machine.md
    └── signal_timing.txt
```

---

Next: See [FLOWCHART.md](FLOWCHART.md) for detailed algorithmic flow diagrams.
