# Smart Gas Detection System - Circuit Diagram & Wiring Guide

## 1. Complete System Circuit Diagram

```
╔════════════════════════════════════════════════════════════════════════════╗
║                    SMART GAS LEAKAGE DETECTION SYSTEM                      ║
║                            CIRCUIT DIAGRAM (v1.0)                          ║
╚════════════════════════════════════════════════════════════════════════════╝

┌─────────────────────────────────────────────────────────────────────────────┐
│                          POWER SUPPLY SECTION                               │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌─────────────┐                                                           │
│  │             │ 12V                                                       │
│  │  12V Power  ├────┬────────────────────────────────────────────────┐    │
│  │   Supply    │    │                                                │    │
│  │  (10A min)  │    │  ┌──────────────────────────────────────────┐ │    │
│  │             │    └──┤ Fuse (10A)                               │ │    │
│  │ GND ────┬───┘       └──────┬────────────────────────────────────┘ │    │
│  └─────────┼──────────────────┼────────────────────────────────────────┘   │
│            │                  │                                            │
│            │ GND          12V  │ (Fused)                                   │
│            │                  │                                            │
│            ├──────────────────┼─────────────────────────────────────────┐  │
│            │                  │                                         │  │
│            │                  └──→ [Relay Coil 1] ───→ Exhaust Fan     │  │
│            │                                                             │  │
│            │                  ┌──────────────────→ [Relay Coil 2] ───→ │  │
│            │                  │                   Gas Solenoid Valve   │  │
│            │                  │                                         │  │
│            │            ┌─────┴──────┐                                 │  │
│            │        ┌──→│ 5V Reg     │                                 │  │
│            │        │   │ (LM7805)   │                                 │  │
│            └────────┤   └───────┬────┘                                 │  │
│                     │           │ 5V                                   │  │
│                     │           │                                      │  │
│                     │      ┌────┴────┐                                 │  │
│                     └──────┤ GND     │                                 │  │
│                            └─────────┘                                 │  │
│                                                                         │  │
└─────────────────────────────────────────────────────────────────────────┘  │
                                                                              │
│ ┌──────────────────────────────────────────────────────────────────────┐  │
│ │                         ARDUINO SECTION                              │  │
│ ├──────────────────────────────────────────────────────────────────────┤  │
│ │                                                                      │  │
│ │  ┌────────────────────────────────┐                                 │  │
│ │  │        Arduino Uno/Mega        │                                 │  │
│ │  │                                │                                 │  │
│ │  │  GND  ○────────────────GND     │                                 │  │
│ │  │  5V   ○────────────────5V      │                                 │  │
│ │  │       │                         │                                 │  │
│ │  │ A0    ○───[Gas Sensor]          │  (ADC Input)                  │  │
│ │  │ A4    ○───[LCD SDA]             │  (I2C)                        │  │
│ │  │ A5    ○───[LCD SCL]             │  (I2C)                        │  │
│ │  │       │                         │                                 │  │
│ │  │ D2    ○───[Reset Button]        │  (Digital Input, PULLUP)      │  │
│ │  │ D3    ○───[1kΩ Resistor]        │  (to Relay1)                  │  │
│ │  │ D4    ○───[1kΩ Resistor]        │  (to Relay2)                  │  │
│ │  │ D6    ○───[Manual Fan Switch]   │  (Digital Input, PULLUP)      │  │
│ │  │ D7    ○───[Manual Valve Switch] │  (Digital Input, PULLUP)      │  │
│ │  │ D9    ○───[1kΩ Resistor]        │  (to Buzzer)                  │  │
│ │  │                                │                                 │  │
│ │  └────────────────────────────────┘                                 │  │
│ │                                                                      │  │
│ └──────────────────────────────────────────────────────────────────────┘  │
│                                                                              │
│ ┌──────────────────────────────────────────────────────────────────────┐  │
│ │                      GAS SENSOR SECTION                              │  │
│ ├──────────────────────────────────────────────────────────────────────┤  │
│ │                                                                      │  │
│ │  ┌──────────────────────────┐                                        │  │
│ │  │    MQ-2 Gas Sensor       │                                        │  │
│ │  │  ┌──────────────────┐    │                                        │  │
│ │  │  │  Heating Element │    │                                        │  │
│ │  │  │    (~150mA @5V)  │    │                                        │  │
│ │  │  └──────────────────┘    │                                        │  │
│ │  │                          │                                        │  │
│ │  │  VCC ──────────→ (+5V)   │                                        │  │
│ │  │  GND ──────────→ (GND)   │                                        │  │
│ │  │                          │                                        │  │
│ │  │  AOUT ──→ (To Arduino A0)│                                        │  │
│ │  │           (Analog 0-5V)  │                                        │  │
│ │  │                          │                                        │  │
│ │  └──────────────────────────┘                                        │  │
│ │                                                                      │  │
│ └──────────────────────────────────────────────────────────────────────┘  │
│                                                                              │
│ ┌──────────────────────────────────────────────────────────────────────┐  │
│ │                      DISPLAY SECTION (I2C)                           │  │
│ ├──────────────────────────────────────────────────────────────────────┤  │
│ │                                                                      │  │
│ │  ┌────────────────────────────────┐                                  │  │
│ │  │  16x2 LCD (I2C Module)         │                                  │  │
│ │  │                                │                                  │  │
│ │  │  VCC ──→ (+5V)                 │                                  │  │
│ │  │  GND ──→ (GND)                 │                                  │  │
│ │  │  SDA ──→ (Arduino A4)          │                                  │  │
│ │  │  SCL ──→ (Arduino A5)          │                                  │  │
│ │  │                                │                                  │  │
│ │  │ ┌──────────────────────────┐   │                                  │  │
│ │  │ │ Display Output Example:  │   │                                  │  │
│ │  │ │ [System Normal       ]   │   │                                  │  │
│ │  │ │ [Gas: 420  V: ON    ]   │   │                                  │  │
│ │  │ └──────────────────────────┘   │                                  │  │
│ │  │                                │                                  │  │
│ │  └────────────────────────────────┘                                  │  │
│ │                                                                      │  │
│ └──────────────────────────────────────────────────────────────────────┘  │
│                                                                              │
│ ┌──────────────────────────────────────────────────────────────────────┐  │
│ │                    INPUT CONTROLS SECTION                            │  │
│ ├──────────────────────────────────────────────────────────────────────┤  │
│ │                                                                      │  │
│ │  Reset Button:                                                      │  │
│ │  ┌────────────────────┐                                              │  │
│ │  │   Pushbutton       │                                              │  │
│ │  │   ┌─────[10kΩ]──→ +5V         (Pull-up Resistor)                 │  │
│ │  │   │                │                                              │  │
│ │  │   └─→ Arduino D2 ─┴─ GND      (When pressed)                     │  │
│ │  │                                                                   │  │
│ │  └────────────────────┘                                              │  │
│ │                                                                      │  │
│ │  Manual Fan Control Button:                                          │  │
│ │  Similar configuration: D6 pin                                       │  │
│ │                                                                      │  │
│ │  Manual Valve Control Button:                                        │  │
│ │  Similar configuration: D7 pin                                       │  │
│ │                                                                      │  │
│ └──────────────────────────────────────────────────────────────────────┘  │
│                                                                              │
│ ┌──────────────────────────────────────────────────────────────────────┐  │
│ │                      RELAY CONTROL SECTION                           │  │
│ ├──────────────────────────────────────────────────────────────────────┤  │
│ │                                                                      │  │
│ │  2-Channel 5V Relay Module:                                          │  │
│ │  ┌──────────────────────────────────┐                                │  │
│ │  │ Relay Module (5V)                │                                │  │
│ │  │                                  │                                │  │
│ │  │ VCC → +5V                        │                                │  │
│ │  │ GND → GND                        │                                │  │
│ │  │                                  │                                │  │
│ │  │ IN1 ←─ Arduino D3 (via 1kΩ)      │  [Relay 1 for Fan]            │  │
│ │  │    │                             │                                │  │
│ │  │    └─ 1N4007 Diode ──→ GND       │  (Protection)                 │  │
│ │  │                                  │                                │  │
│ │  │ IN2 ←─ Arduino D4 (via 1kΩ)      │  [Relay 2 for Valve]          │  │
│ │  │    │                             │                                │  │
│ │  │    └─ 1N4007 Diode ──→ GND       │  (Protection)                 │  │
│ │  │                                  │                                │  │
│ │  │ COM1 ─→ Runs to 12V load         │  (Normally Open)              │  │
│ │  │ NO1  ─→ Exhaust Fan              │                                │  │
│ │  │                                  │                                │  │
│ │  │ COM2 ─→ Runs to 12V load         │  (Normally Closed)            │  │
│ │  │ NC2  ─→ Gas Solenoid Valve       │                                │  │
│ │  │                                  │                                │  │
│ │  └──────────────────────────────────┘                                │  │
│ │                                                                      │  │
│ └──────────────────────────────────────────────────────────────────────┘  │
│                                                                              │
│ ┌──────────────────────────────────────────────────────────────────────┐  │
│ │                      BUZZER SECTION                                  │  │
│ ├──────────────────────────────────────────────────────────────────────┤  │
│ │                                                                      │  │
│ │  Buzzer Configuration:                                              │  │
│ │  ┌───────────────────────────────┐                                   │  │
│ │  │  5V Active Buzzer             │                                   │  │
│ │  │                               │                                   │  │
│ │  │  (+) ─→ Arduino D9            │  ─────┐                           │  │
│ │  │         (via 1kΩ Resistor)    │       │                           │  │
│ │  │                               │    Current Limiting              │  │
│ │  │  (-) ─→ GND                   │       │                           │  │
│ │  │                               │ ─────┘                            │  │
│ │  │                               │                                   │  │
│ │  └───────────────────────────────┘                                   │  │
│ │                                                                      │  │
│ └──────────────────────────────────────────────────────────────────────┘  │
│                                                                              │
└──────────────────────────────────────────────────────────────────────────────┘
```

## 2. Detailed Pin Connection Table

```
PIN ASSIGNMENTS - ARDUINO UNO/MEGA:

ANALOG INPUTS:
┌────────────┬──────────────┬───────────────────────────────────┐
│ Arduino    │ Component    │ Purpose                           │
├────────────┼──────────────┼───────────────────────────────────┤
│ A0         │ Gas Sensor   │ Analog reading (0-1023)           │
│ A1         │ Temp Sensor  │ Optional temperature (DHT11)      │
│ A2         │ Reserved     │ Future expansion                  │
│ A4/SDA     │ LCD Display  │ I2C Serial Data                   │
│ A5/SCL     │ LCD Display  │ I2C Serial Clock                  │
└────────────┴──────────────┴───────────────────────────────────┘

DIGITAL OUTPUTS:
┌────────────┬──────────────┬───────────────────────────────────┐
│ Arduino    │ Component    │ Purpose                           │
├────────────┼──────────────┼───────────────────────────────────┤
│ D3 (PWM)   │ Relay Module │ Fan Relay Control (IN1)          │
│ D4         │ Relay Module │ Valve Relay Control (IN2)        │
│ D9 (PWM)   │ Buzzer       │ Alarm Control (via 1kΩ)          │
│ D10 (PWM)  │ Reserved     │ Future use                       │
└────────────┴──────────────┴───────────────────────────────────┘

DIGITAL INPUTS:
┌────────────┬──────────────┬───────────────────────────────────┐
│ Arduino    │ Component    │ Purpose                           │
├────────────┼──────────────┼───────────────────────────────────┤
│ D2 (INT0)  │ Reset Button │ System Reset (Internal PULLUP)   │
│ D6         │ Manual Switch│ Manual Fan Control (Internal PU)  │
│ D7         │ Manual Switch│ Manual Valve Control (Internal PU)│
│ D8         │ Reserved     │ Future expansion                  │
└────────────┴──────────────┴───────────────────────────────────┘

POWER PINS:
┌────────────┬──────────────┬───────────────────────────────────┐
│ GND        │ Ground       │ All components via star point     │
│ +5V        │ 5V Supply    │ Logic circuits, sensors, LCD      │
│ Vin/Raw    │ 12V Supply   │ Power input (optional)            │
└────────────┴──────────────┴───────────────────────────────────┘
```

## 3. Component Wiring Details

### 3.1 Gas Sensor Wiring

```
MQ-2 Sensor Module (Typically pre-assembled):

    ┌──────────────────┐
    │   MQ-2 Module    │
    │                  │
    │  GND ────→ (0V)  │ (Black wire)
    │  VCC ────→ (+5V) │ (Red wire)
    │  AOUT ───→ (A0)  │ (Yellow wire)
    │  DOUT ────→ X    │ (Not used: Digital output)
    │                  │
    └──────────────────┘

Connection to Arduino:
├─ VCC → Arduino 5V (or external 5V)
├─ GND → Arduino GND
└─ AOUT → Arduino A0 (Analog read)

Notes:
- Sensor should warm up 15-20 seconds before use
- Keep wires short to reduce noise (< 1m)
- Twisted pair recommended if wires > 50cm
- Provide heater current at startup
```

### 3.2 LCD Display Wiring (I2C 16x2)

```
LCD 16x2 Display with I2C Adapter:

    ┌─────────────────────────┐
    │  16x2 LCD I2C Adapter   │
    │                         │
    │  GND  ────→ (0V)        │ (Black)
    │  VCC  ────→ (+5V)       │ (Red)
    │  SDA  ────→ (A4/SDA)    │ (Green)
    │  SCL  ────→ (A5/SCL)    │ (Yellow)
    │                         │
    │ I2C Address: 0x27       │
    │             (default)   │
    │                         │
    └─────────────────────────┘

Arduino Pin Connections:
├─ VCC → Arduino 5V
├─ GND → Arduino GND
├─ SDA → Arduino A4 (SDA pin)
└─ SCL → Arduino A5 (SCL pin)

Notes:
- I2C uses open-drain outputs
- Pull-up resistors usually on module
- Address can be 0x27 or 0x3F (check jumper)
- No need for separate power regulator
- Keep cable length < 1m
```

### 3.3 Relay Module Wiring

```
2-Channel 5V Relay Module:

    ┌──────────────────────────┐
    │  Relay Module (5V)       │
    │                          │
    │  Pin 1:  VCC → (+5V)     │
    │  Pin 2:  GND → (GND)     │
    │                          │
    │  Pin 3:  IN1 → Arduino D3│  (via 1kΩ resistor)
    │  Pin 4:  IN2 → Arduino D4│  (via 1kΩ resistor)
    │                          │
    │  ┌────────────────────┐  │
    │  │  RELAY 1 (Fan)     │  │
    │  │  COM ─→ 12V supply │  │
    │  │  NO  ─→ Fan motor  │  │
    │  └────────────────────┘  │
    │                          │
    │  ┌────────────────────┐  │
    │  │ RELAY 2 (Valve)    │  │
    │  │ COM ─→ 12V supply  │  │
    │  │ NC  ─→ Solenoid    │  │ (Normally Closed)
    │  └────────────────────┘  │
    │                          │
    └──────────────────────────┘

Connection Diagram:

    Arduino D3 ──[1kΩ]──→ IN1 ─→ Relay 1 coil
                                      ↓
    Arduino D4 ──[1kΩ]──→ IN2 ─→ Relay 2 coil

Protection Diodes:

    ┌─→ IN1 ──→ Relay 1
    │
    ├─ 1N4007 ──→ To GND (Cathode marked)
    │
    └─→ IN2 ──→ Relay 2

    Reverse biased: Protects Arduino from back-EMF

Current Protection:

    Arduino D3 ──┬─[1kΩ]──→ Relay IN1
                 │
                 └─ Limits current to relay to ~5mA

    This protects Arduino GPIO which can source ~40mA max
```

### 3.4 Push Button Wiring

```
Reset Button Configuration (All buttons similar):

    ┌──────────────────────────┐
    │   Pushbutton Switch      │
    │                          │
    │   [ ]──────────[ ]       │
    │    │           │         │
    │    │        [10kΩ]      │
    │    │           │         │
    │    └──→ Arduino D2      │
    │         ├─→ GND (when pressed)
    │         └─→ +5V (through resistor)
    │                          │
    └──────────────────────────┘

Wiring Method A - External Pull-up:

    +5V ─[10kΩ]─┬─→ Arduino D2
                │
            [Button]
                │
               GND

Wiring Method B - Internal Pull-up (Recommended):

    Arduino D2 (Set INPUT_PULLUP in code)
              ├─→ +5V internally via 20-50kΩ
              │
            [Button]
              │
             GND

Notes:
- Internal pull-up built into Arduino
- Use pinMode(D2, INPUT_PULLUP)
- Button reads LOW when pressed
- External 10kΩ resistor needed for manual switches
```

### 3.5 Buzzer Wiring

```
Active 5V Buzzer Configuration:

    ┌──────────────────────────┐
    │   5V Active Buzzer       │
    │                          │
    │  (+) ────→ PWM output    │ (Positive terminal)
    │  (-) ────→ GND           │ (Negative terminal)
    │                          │
    │ Built-in oscillator      │
    │ Pulsing 500Hz - 4kHz     │
    │                          │
    └──────────────────────────┘

Connection Diagram:

    Arduino D9 ──[1kΩ]──(+) Buzzer
                           │
                    (Pulsing signal)

    GND ────────────────(-) Buzzer

Current Limiting Resistor:
    Arduino GPIO max: 40mA
    Buzzer typical: 30mA avg
    Resistor: 1kΩ limits to ~5mA drive
    Overcurrent protection

Alternative: Using NPN Transistor (for high current):

    Arduino D9 ──[2.2kΩ]──ǀB
                           ├E─→ GND
                           └C─→ (+) Buzzer

    5V ─────────────────(-) Buzzer

    Allows higher current draw without Arduino stress
```

## 4. Power Distribution

```
POWER TREE:

12V Main Supply (10A fuse recommended)
    │
    ├─[10A Fuse]─→ Split to:
    │
    ├─1→ 12V Exhaust Fan Motor
    │
    ├─2→ 12V Solenoid Gas Valve
    │
    └─3→ 5V Voltage Regulator (LM7805)
           │
           ├─ Arduino Vin
           ├─ Relay Module VCC
           ├─ Sensor VCC
           ├─ LCD VCC
           ├─ Buzzer VCC
           └─ Button pull-up resistors
```

## 5. Capacitor Placement (Decoupling)

```
For stable operation, place capacitors:

POWER SUPPLY SIDE:
├─ 100µF electrolytic across 5V regulator output
├─ 1000µF electrolytic on 12V supply (before fuse)
└─ 100nF ceramic near Arduino power pins

SIGNAL SIDE:
├─ 100nF ceramic across sensor power
├─ 100nF ceramic across relay VCC
└─ 100nF ceramic across LCD VCC

Purpose:
- Reduce voltage ripple
- Absorb transient spikes
- Improve signal integrity
- Protect against EMI
```

## 6. Ground Connections (Critical)

```
STAR GROUNDING POINT:

All circuits should return to a common ground point:

    Arduino GND ──┐
    5V Dev GND ──┤
    12V Dev GND──┼─→ [STAR POINT] ──→ Power Supply GND Return
    Sensor GND ──┤
    LCD GND ─────┤
    Relay GND ───┤
    Buzzer GND ──┘

Benefits:
- Prevents ground loops
- Reduces noise coupling
- Improves system stability
- Prevents latch-up in relays

Do NOT use separate ground returns!
```

## 7. Cable Specifications

```
RECOMMENDED WIRE GAUGES:

5V Supply Lines (Logic):
├─ Arduino 5V: 22-24 AWG (up to 1m)
├─ Sensor VCC: 24 AWG (short runs < 50cm)
├─ LCD VCC: 24 AWG
└─ Buzzer VCC: 24 AWG

12V Supply Lines (Loads):
├─ Fused 12V main: 18 AWG
├─ Fan motor: 18 AWG
├─ Valve motor: 18 AWG
└─ Relay input: 20 AWG

Ground Lines:
├─ Star return: 18 AWG (same as 12V)
└─ Logic return: 22 AWG (same as 5V)

High-Current Connections:
├─ Relay contacts: Use appropriate terminals (crimp)
├─ Motor connections: Solder or quick-connect
└─ Fuse connections: Proper fuse holder (no twist-on)
```

## 8. Connector Recommendations

```
PREFERRED CONNECTORS:

Logic Signals (< 2A):
├─ Dupont connectors (0.1" pitch)
├─ Crimp housings for reliability
└─ Strain relief boots

Power Connections (> 2A):
├─ Terminal blocks (5-10A rated)
├─ Or properly crimped connectors
└─ Insulated terminals with boots

Motor Loads:
├─ Weatherproof connectors (if exposed)
├─ Proper 12V rated connectors
└─ Locking connectors to prevent disconnection

Optional: Use labeled shrink tubing for identification
```

---

## 9. Assembly Checklist

Before powering on system:

- [ ] All connections visually inspected
- [ ] Polarity verified (+5V and +12V correct)
- [ ] No exposed conductors
- [ ] All solder joints solid and shiny
- [ ] Heatshrink tubing in place
- [ ] Diodes oriented correctly (cathode to positive)
- [ ] Capacitors oriented correctly (positive to positive)
- [ ] Fuse installed before applying 12V
- [ ] Ground star point verified
- [ ] No shorts detected with multimeter
- [ ] Each supply voltage verified (5V ± 0.5V, 12V ± 0.5V)

---

**Next**: See [ARCHITECTURE.md](ARCHITECTURE.md) for system design details.
