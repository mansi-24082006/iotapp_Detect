# Smart Gas Leakage Detection and Automated Safety System

## Overview

A comprehensive IoT-based smart gas detection and safety automation system that monitors hazardous gas concentrations and automatically triggers safety mechanisms while allowing manual control and user override.

## System Features

### Core Functionality
- **Continuous Gas Monitoring**: Real-time monitoring using MQ-2/MQ-5 gas sensors
- **Automatic Safety Activation**: Immediate response to gas detection
- **State Memory**: Remembers leakage events even after gas levels normalize
- **Manual Reset**: User-controlled system recovery mechanism
- **Manual Override**: Independent control of fan and valve
- **Real-time Display**: LCD/OLED status messages and alerts

### Safety Features
- Dual-trigger activation (buzzer + exhaust fan)
- Automatic gas valve shutoff
- Warning state with persistent valve closure
- Emergency manual controls
- Visual and audio alerts

## System Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                   SMART GAS DETECTION SYSTEM                 │
├─────────────────────────────────────────────────────────────┤
│                                                               │
│  INPUT DEVICES          PROCESSING UNIT      OUTPUT DEVICES  │
│  ─────────────────      ───────────────────  ──────────────  │
│  • Gas Sensor            • Arduino/ESP32      • Exhaust Fan   │
│    (MQ-2/MQ-5)           • Logic Processing   • Buzzer Alarm  │
│  • Reset Button           • State Management  • Gas Valve     │
│  • Manual Controls        • Memory (EEPROM)   • LCD/OLED      │
│  • Fan Switch             • Timer Management  • Status LEDs    │
│  • Valve Switch           • Alert Generation  • WiFi/GSM      │
│                                                              │
│  COMMUNICATION LAYER (Optional)                             │
│  ────────────────────────────────────────────             │
│  • WiFi (ESP8266/ESP32) → Mobile App                       │
│  • GSM Module → SMS/Call Alerts                            │
│  • Firebase → Remote Monitoring                            │
│                                                            │
└─────────────────────────────────────────────────────────────┘
```

## Hardware Components

### Essential Components
| Component | Specification | Quantity | Purpose |
|-----------|---------------|----------|---------|
| Microcontroller | Arduino Uno/Mega or ESP32 | 1 | Central processing |
| Gas Sensor | MQ-2 or MQ-5 | 1 | Gas detection |
| Buzzer | 5V Active/Passive | 1 | Alarm alert |
| Relay Module | 2-Channel 5V | 2 | Fan & Valve control |
| Exhaust Fan | 12V DC Motor | 1 | Ventilation |
| Gas Solenoid Valve | 12V NC (Normally Closed) | 1 | Gas shutoff |
| Display | 16x2 LCD or OLED | 1 | Status display |
| Push Buttons | 5V | 3 | Reset & Controls |
| Power Supply | 12V & 5V | 1 | System power |
| Resistors/Capacitors | Various | Multiple | Circuit stabilization |

### Optional IoT Components
- ESP8266/ESP32 WiFi Module
- GSM Module (SIM800/SIM900)
- Temperature/Humidity Sensor (DHT11)
- Smoke Detector (MQ-135)
- Water Detection Sensor

## Software Requirements

### Development Tools
- Arduino IDE or PlatformIO
- USB CH340 Driver (for Arduino clones)
- Serial Monitor for debugging
- Firebase Console (optional)

### Libraries Required
```
#include <LiquidCrystal_I2C.h>    // LCD control
#include <EEPROM.h>               // Persistent storage
#include <WiFi.h>                 // WiFi (ESP32)
#include <FirebaseESP32.h>        // Firebase (optional)
```

## System States and Logic Flow

### State Transitions
```
1. NORMAL STATE
   ├─ Gas Level < Threshold
   ├─ Buzzer: OFF
   ├─ Fan: OFF
   ├─ Valve: ON
   └─ Display: "System Normal"

2. DETECTION STATE
   ├─ Gas Level > Threshold
   ├─ Buzzer: ON (continuous)
   ├─ Fan: ON (ventilation)
   ├─ Valve: OFF (gas shutoff)
   └─ Display: "Gas Leakage Detected" → "System Activated"

3. WARNING STATE
   ├─ Gas Level < Threshold but Flag Set
   ├─ Buzzer: OFF
   ├─ Fan: OFF
   ├─ Valve: OFF (remains closed)
   └─ Display: "Gas Leakage Not Stopped - Manual Reset Required"

4. MANUAL OVERRIDE
   ├─ User controls Fan/Valve
   └─ Acts independently of automatic system
```

## Safety Thresholds

| Level | PPM Range | Status | Action |
|-------|-----------|--------|--------|
| Safe | 0-300 | Green | Normal operation |
| Warning | 300-500 | Yellow | Pre-alert (optional) |
| Danger | 500-1000+ | Red | Immediate activation |

**Note**: Thresholds vary by gas type. MQ-2 typical alarm: 300-500 PPM

## Configuration

All system parameters can be configured in `config.h`:

```cpp
// Gas Sensor Threshold
#define GAS_THRESHOLD        500

// Pin Definitions
#define GAS_SENSOR_PIN       A0
#define BUZZER_PIN           9
#define RESET_BUTTON_PIN     2
#define FAN_RELAY_PIN        3
#define VALVE_RELAY_PIN      4

// Timer Settings
#define FAN_VENTILATION_TIME 180000  // 3 minutes
#define SENSOR_READ_DELAY    1000    // 1 second
```

## Getting Started

### 1. Hardware Assembly
- Connect gas sensor to analog input pin
- Connect relays for fan and valve control
- Connect buzzer to digital output pin
- Connect LCD via I2C
- Connect push buttons with resistors

### 2. Software Setup
```bash
# Clone repository
git clone [repo-url]

# Open Arduino IDE
# Select Board: Arduino Uno/Mega or ESP32
# Select COM Port
# Upload code from arduino_code/GasLeakDetection.ino
```

### 3. Calibration
- Expose sensor to clean air for 15-20 seconds (baseline)
- Sensor auto-calibrates on power-up
- Adjust GAS_THRESHOLD if needed

### 4. Testing
- Test manual override controls
- Test reset button functionality
- Verify alarm triggers above threshold
- Confirm all display messages

## File Structure

```
gas_detection_system/
├── README.md (this file)
├── arduino_code/
│   ├── GasLeakDetection.ino          (Main code)
│   ├── config.h                       (Configuration)
│   ├── GasSensorModule.cpp            (Sensor handler)
│   ├── AlarmModule.cpp                (Buzzer control)
│   ├── SafetyModule.cpp               (Fan/Valve control)
│   ├── DisplayModule.cpp              (LCD control)
│   └── StateManager.cpp               (State machine)
├── documentation/
│   ├── ARCHITECTURE.md                (System design)
│   ├── FLOWCHART.md                   (Logic diagrams)
│   ├── SAFETY.md                      (Safety guide)
│   ├── CALIBRATION.md                 (Sensor setup)
│   └── TROUBLESHOOTING.md             (Common issues)
├── diagrams/
│   ├── circuit_diagram.txt            (ASCII diagram)
│   ├── state_machine.svg              (State transitions)
│   └── timing_diagram.txt             (Signal timing)
├── config/
│   ├── config.h                       (Hardware config)
│   ├── thresholds.h                   (Safety thresholds)
│   └── wifi_config.h                  (IoT settings)
└── examples/
    ├── basic_demo.ino                 (Simple version)
    └── iot_integration.ino            (Advanced version)
```

## Documentation Files

- **[ARCHITECTURE.md](documentation/ARCHITECTURE.md)** - Detailed system design
- **[FLOWCHART.md](documentation/FLOWCHART.md)** - Flowcharts and diagrams
- **[SAFETY.md](documentation/SAFETY.md)** - Safety considerations
- **[CALIBRATION.md](documentation/CALIBRATION.md)** - Sensor calibration guide
- **[TROUBLESHOOTING.md](documentation/TROUBLESHOOTING.md)** - Problem diagnosis

## Implementation Status

- ✅ System Architecture
- ✅ Complete Arduino Code
- ✅ Configuration System
- ✅ Flowcharts & Diagrams
- ✅ Safety Documentation
- ✅ Implementation Guide
- 🔄 IoT Integration (WiFi/Firebase)
- 🔄 Mobile App Integration

## Future Enhancements

### Phase 2
- WiFi integration (ESP32) for remote monitoring
- Mobile app notifications
- Data logging and analytics
- Multiple sensor support

### Phase 3
- Machine learning for pattern detection
- GSM module for SMS alerts
- Temperature and humidity monitoring
- Integration with smart home systems

### Phase 4
- Cloud storage with AWS/Google Cloud
- Advanced predictive analytics
- Integration with SCADA systems
- Multi-node network deployment

## Safety Considerations

⚠️ **CRITICAL SAFETY NOTES**:
1. Always test in a safe environment first
2. Use certified gas sensors with proper calibration
3. Valve must be NO (Normally Open) or NC (Normally Closed) appropriately
4. Implement redundant safety mechanisms
5. Regular maintenance and sensor replacement required
6. Follow local safety regulations and standards

See [SAFETY.md](documentation/SAFETY.md) for detailed guidelines.

## Support & Contribution

For issues, feature requests, or contributions:
- Review [TROUBLESHOOTING.md](documentation/TROUBLESHOOTING.md)
- Check circuit connections
- Verify sensor calibration
- Test in isolated environment

## License

This project is designed for educational and real-world safety applications.

## Version

**Current Version**: 1.0.0  
**Last Updated**: April 2026  
**Status**: Production Ready

---

**Author**: Gas Detection System Team  
**Contact**: [Your Information]  
**Support**: [Support Details]
