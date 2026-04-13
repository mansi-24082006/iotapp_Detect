# System Flowcharts and Logic Diagrams

## 1. Main Control Loop Flowchart

```
┌─────────────────────────┐
│   SYSTEM START          │
│   Initialize Hardware   │
│   Load EEPROM State     │
│   Display Startup Msg   │
│   Sensor Warm-up (15s)  │
└────────────┬────────────┘
             ↓
       ┌──────────────┐
       │   MAIN LOOP  │
       │  (Continuous)│
       └──────┬───────┘
              ↓
    ┌─────────────────────────┐
    │  Check Reset Button     │
    │ (Debounce: 50ms)       │
    └────────┬────────────────┘
             │
             ├─── Button Pressed?
             │         YES
             │         ↓
             │    ┌────────────────┐
             │    │  Reset System  │
             │    │ • Clear Flag   │
             │    │ • Buzzer OFF   │
             │    │ • Fan OFF      │
             │    │ • Valve ON     │
             │    │ • State=NORMAL │
             │    │ • Update EEPROM│
             │    └────────┬───────┘
             │             ↓
             ↓             ↓
    ┌─────────────────────────────┐
    │  Check Manual Controls      │
    │ • Fan Switch                │
    │ • Valve Switch              │
    │ (Toggle Settings)           │
    └────────┬────────────────────┘
             ↓
    ┌──────────────────────────────┐
    │  Time to Read Sensor?        │
    │ (Every 1000ms)               │
    └────┬──────────────────────┬──┘
         │ YES                  │ NO
         ↓                      │
    ┌────────────────┐          │
    │ Read ADC(A0)   │          │
    └────┬───────────┘          │
         ↓                      │
    ┌─────────────────────┐     │
    │ Moving Average      │     │
    │ Filter (5 samples)  │     │
    └────┬────────────────┘     │
         ↓                      │
    ┌──────────────────────┐    │
    │ averageGasLevel =    │    │
    │ smoothed value       │    │
    └────┬─────────────────┘    │
         ↓                      │
         └──────────┬───────────┘
                    ↓
        ┌─────────────────────┐
        │   STATE MACHINE     │
        │  (See Detailed Flow)│
        └────────┬────────────┘
                 ↓
        ┌──────────────────────┐
        │   Update Outputs     │
        │ • Buzzer             │
        │ • Fan Control        │
        │ • Valve Control      │
        └────────┬─────────────┘
                 ↓
        ┌──────────────────────┐
        │   Update Display     │
        │ • Status Message     │
        │ • Gas Level          │
        │ • Component Status   │
        └────────┬─────────────┘
                 ↓
        ┌──────────────────────┐
        │ Log Status (5sec)    │
        │ • Serial Output      │
        │ • EEPROM Logging     │
        └────────┬─────────────┘
                 ↓
          (REPEAT LOOP)
```

## 2. State Machine Detailed Flowchart

```
┌──────────────────────────────────┐
│   STATE MACHINE UPDATE           │
└────────────┬─────────────────────┘
             ↓
      ┌────────────────────┐
      │ Current State?     │
      └────┬───────────────┘
           │
    ┌──────┴──────┬──────────┬────────────┐
    │             │          │            │
   NORMAL     DETECTED     WARNING      OVERRIDE
    ↓             ↓          ↓            ↓


==============  STATE: NORMAL  ==============
(System Idle - No Gas Detected)
┌──────────────────────────────────┐
│ Check: Gas Level > Threshold?    │
└────┬─────────────────────────┬───┘
     │ NO                      │ YES
     │ (Stream < 500)          │ (Stream ≥ 500)
     ↓                         ↓
  Stay in          ┌─────────────────────────┐
  NORMAL           │ TRANSITION EVENT:       │
  State            │ Gas Detected!          │
                   │                         │
                   │ Actions:                │
                   │ 1. Set leakageDetected │
                   │ 2. Save to EEPROM      │
                   │ 3. Record detection    │
                   │    time & max level    │
                   │ 4. Log event           │
                   │                         │
                   │ Next State:             │
                   │ GAS_DETECTED            │
                   └──────────┬──────────────┘
                              ↓
                         (Continue Loop)


==============  STATE: GAS_DETECTED  ==============
(Active Alarm - Fan & Buzzer ON, Valve OFF)
┌──────────────────────────────────┐
│ Check: Gas Level < Threshold?    │
└────┬─────────────────────────┬───┘
     │ NO                      │ YES
     │ (Stream ≥ 500)          │ (Stream < 500)
     ↓                         ↓
  Stay in            ┌──────────────────────┐
  DETECTED           │ TRANSITION EVENT:    │
  State              │ Gas Normalized       │
                     │ (Below Threshold)    │
  If Gas > old peak: │                      │
  Update maxLevel    │ Actions:             │
                     │ 1. Start fan timer   │
                     │ 2. Keep valve OFF    │
                     │ 3. Turn buzzer OFF   │
                     │    (after settling)  │
                     │ 4. leakageDetected   │
                     │    remains TRUE      │
                     │                      │
                     │ Next State:          │
                     │ WARNING              │
                     └──────────┬───────────┘
                                ↓
                           (Continue Loop)


==============  STATE: WARNING  ==============
(Leakage Acknowledged - Waiting for Manual Reset)
┌──────────────────────────────┐
│ Buzzer: OFF                  │
│ Fan: Still Running (cooling) │
│ Valve: CLOSED (safety)       │
│ Display: Manual Reset Req    │
└────┬──────────────────────┬──┘
     │                      │
  ┌──┴──┐              ┌────┴────┐
  │     │              │         │
Check Reset  Check Fan  │    Manual
Button       Timer      │    Override?
  │             ↓       │         ↓
  │        ┌──────────┐ │    (See Separate
  │        │ Timer    │ │     Flowchart)
  │        │ Expired? │ │
  │        └─┬──────┬─┘ │
  │          │ YES  │   │
  │          │      │   │
  │          ↓      ↓   ↓
  │      Turn Fan   Stay in
  │      OFF        WARNING
  │                 State
  │
  ├─ YES: Reset Button Pressed
  │       ↓
  │   ┌────────────────────────┐
  │   │ RESET SEQUENCE:        │
  │   │ 1. Clear leakageFlag   │
  │   │ 2. Write to EEPROM(0)  │
  │   │ 3. Restore all outputs │
  │   │ 4. State = NORMAL      │
  │   │ 5. Display "Normal"    │
  │   │ 6. Log reset event     │
  │   └────────┬───────────────┘
  │            ↓
  │   Return to NORMAL
  │   (See above)
  │
  └─ NO: Stay in WARNING


==============  MANUAL OVERRIDE  ==============
(User Manually Controlling Fan/Valve)
┌──────────────────────────┐
│ Check Switch States      │
│ • Fan Override Switch    │
│ • Valve Override Switch  │
└────┬────────────────────┘
     ↓
  ┌────────────────────┐
  │ Fan Switch Pressed?│
  ├────┬──────────────┬┘
  │ ON │              │ OFF
  ↓    ↓              ↓
Toggle   Fan        Fan
State   Turns      Turns
       ON          OFF

  ┌────────────────────┐
  │ Valve Switch       │
  │ Pressed?           │
  ├────┬──────────────┬┘
  │ ON │              │ OFF
  ↓    ↓              ↓
Toggle   Valve      Valve
State   Opens      Closes

(Independent of System State)
```

## 3. Gas Detection & Response Flowchart

```
┌──────────────────────────────────┐
│     SENSOR READING CYCLE         │
│ (Every 1000ms / ~1 second)       │
└────────────┬─────────────────────┘
             ↓
    ┌─────────────────────┐
    │  Read ADC Value     │
    │  From Pin A0        │
    │  Range: 0-1023      │
    └────────┬────────────┘
             ↓
    ┌──────────────────────────┐
    │  Store in Array          │
    │  (Last 5 readings)       │
    └────────┬─────────────────┘
             ↓
    ┌──────────────────────────┐
    │  Calculate Moving        │
    │  Average of 5 samples    │
    │  (Smoothing filter)      │
    └────────┬─────────────────┘
             ↓
    ┌────────────────────────────┐
    │  Store as                  │
    │  averageGasLevel           │
    └────────┬───────────────────┘
             ↓
┌────────────────────────────────────────────┐
│        THRESHOLD COMPARISON                │
│   averageGasLevel vs GAS_THRESHOLD(500)    │
└─┬──────────────────────────────────────────┘
  │
  ├─── < 300 (Safe Range)
  │    └─→ Display: "System Normal"
  │        No action needed
  │
  ├─── 300-500 (Warning Range)
  │    └─→ Could trigger pre-alert (optional)
  │        Display: "Caution"
  │
  ├─── ≥ 500 (DANGER ZONE)
  │    └─→ IMMEDIATE ACTION REQUIRED
  │
  └─→ Continue to State Check
      (See State Machine Flowchart)
```

## 4. Emergency Response Timeline

```
Timeline: Gas Detection to Full System Reset

t=0      │ Gas Level exceeds 500 PPM
         │ ↓
t=0+50ms │ Next sensor read detects threshold
         │ Average calculation confirms danger
         │ ↓
t=0+100ms│ STATE TRANSITION: NORMAL → GAS_DETECTED
         │ leakageDetected flag = TRUE
         │ ↓
t=0+150ms│ OUTPUT ACTIVATION
         │ - Buzzer: Start pulsing (500ms interval)
         │ - Fan Relay: Activated (LOW signal)
         │ - Valve Relay: Closed (HIGH signal)
         │ ↓
t=0+200ms│ EEPROM: Flag saved (persistence)
         │ ↓
t=0+2.0s │ Gas level drops below 500 PPM
         │ (e.g., due to initial ventilation)
         │ ↓
t=0+2.1s │ STATE TRANSITION: GAS_DETECTED → WARNING
         │ leakageDetected flag remains TRUE
         │ ↓
t=0+2.2s │ OUTPUT ADJUSTMENT
         │ - Buzzer: OFF
         │ - Fan Relay: Remains ON (ventilation continues)
         │ - Valve Relay: Remains CLOSED (safety lock)
         │ ↓
t=0+3.0s │ DISPLAY UPDATE
         │ Shows: "Gas Leakage Not Stopped"
         │        "Manual Reset Required"
         │ ↓
t=0+180s │ FAN AUTO-OFF TIMER EXPIRES
         │ - Fan Relay: Deactivated
         │ - But valve remains CLOSED
         │ ↓
  ?      │ USER PRESSES RESET BUTTON
         │ ↓
  +X     │ SYSTEM RESET SEQUENCE
         │ - leakageDetected = FALSE
         │ - EEPROM Flag = 0
         │ - All outputs restored
         │ - STATE: WARNING → NORMAL
         │ - Valve opens
         │ - Display: "System Normal"
         │ ↓
         │ SYSTEM READY FOR NEXT EVENT
```

## 5. Sensor Reading & Filtering Process

```
┌─────────────────────────────────────────┐
│  RAW SENSOR READING SEQUENCE            │
└────────┬────────────────────────────────┘
         ↓
┌─────────────────────────────────────────┐
│ T=1000ms: analogRead(A0) → value1       │
│ Result: 420 PPM (example)               │
└────────┬────────────────────────────────┘
         ↓
┌─────────────────────────────────────────┐
│ T=2000ms: analogRead(A0) → value2       │
│ Result: 435 PPM                         │
└────────┬────────────────────────────────┘
         ↓
┌─────────────────────────────────────────┐
│ T=3000ms: analogRead(A0) → value3       │
│ Result: 410 PPM                         │
└────────┬────────────────────────────────┘
         ↓
┌─────────────────────────────────────────┐
│ T=4000ms: analogRead(A0) → value4       │
│ Result: 445 PPM                         │
└────────┬────────────────────────────────┘
         ↓
┌─────────────────────────────────────────┐
│ T=5000ms: analogRead(A0) → value5       │
│ Result: 430 PPM                         │
└────────┬────────────────────────────────┘
         ↓
┌─────────────────────────────────────────┐
│ MOVING AVERAGE CALCULATION              │
│ Average = (420+435+410+445+430) / 5     │
│         = 2140 / 5                      │
│         = 428 PPM (smoothed)            │
└────────┬────────────────────────────────┘
         ↓
┌─────────────────────────────────────────┐
│ COMPARISON WITH THRESHOLD               │
│ 428 PPM < 500 PPM (THRESHOLD)           │
│ RESULT: No alarm (in this reading)      │
└─────────────────────────────────────────┘

---

SECOND ITERATION (After Gas Spill):

┌─────────────────────────────────────────┐
│ T=6000ms: analogRead(A0) → value1       │
│ Result: 550 PPM ← SUDDEN INCREASE!      │
└────────┬────────────────────────────────┘
         ↓
┌─────────────────────────────────────────┐
│ History array: [435, 410, 445, 430, 550]│
└────────┬────────────────────────────────┘
         ↓
┌─────────────────────────────────────────┐
│ T=7000ms: analogRead(A0) → value2       │
│ Result: 620 PPM                         │
└────────┬────────────────────────────────┘
         ↓
┌─────────────────────────────────────────┐
│ T=8000ms: value3 = 580 PPM              │
│ History: [410, 445, 430, 550, 620]      │
└────────┬────────────────────────────────┘
         ↓
┌─────────────────────────────────────────┐
│ MOVING AVERAGE:                         │
│ = (410+445+430+550+620) / 5             │
│ = 2455 / 5                              │
│ = 491 PPM (still below but rising)      │
└────────┬────────────────────────────────┘
         ↓
┌─────────────────────────────────────────┐
│ Next reading cycles confirm trend...    │
│ Average reaches 520+ PPM                │
│ THRESHOLD EXCEEDED!                     │
│ ↓                                       │
│ ALERT TRIGGERED ✓                       │
└─────────────────────────────────────────┘
```

## 6. Decision Tree: What Action to Take

```
START
  │
  ├─ NORMAL State
  │  └─ Gas < 500? → Continue monitoring
  │  └─ Gas ≥ 500? → Activate Full Alert (DETECTED)
  │
  ├─ DETECTED State
  │  ├─ Still Gas ≥ 500? → Keep alert active
  │  └─ Gas < 500? → Transition to WARNING
  │
  ├─ WARNING State
  │  ├─ Reset button pressed?
  │  │  └─ YES → System Reset (back to NORMAL)
  │  │  └─ NO → Stay in WARNING
  │  │
  │  ├─ Fan timer expired (180s)?
  │  │  └─ YES → Turn off fan
  │  │            (Valve remains off for safety)
  │  │  └─ NO → Keep fan running
  │  │
  │  └─ Manual override active?
  │     └─ YES → Apply manual controls
  │     └─ NO → Keep automatic settings
  │
  └─ MANUAL_OVERRIDE State
     ├─ Fan switch toggled?
     │  └─ YES → Toggle fan state
     │  └─ NO → Keep fan in current state
     │
     └─ Valve switch toggled?
        └─ YES → Toggle valve state
        └─ NO → Keep valve in current state
```

---

See [ARCHITECTURE.md](ARCHITECTURE.md) for system design details.
