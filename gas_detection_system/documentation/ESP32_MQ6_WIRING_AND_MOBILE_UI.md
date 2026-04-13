# ESP32 + MQ6 Wiring and Mobile UI Guide

## 1) Required Hardware

- ESP32 Dev Board
- MQ6 gas sensor module (A0 analog output used)
- 5V active buzzer module (or transistor-driven buzzer)
- SG90/MG90 (or similar) servo motor
- 1-channel 5V relay module for exhaust fan
- 5V external power supply (recommended for servo + relay + MQ6)
- Common ground wiring between ESP32 and external 5V supply

## 2) ESP32 Pin Mapping

- MQ6 analog out (A0) -> GPIO34 (input only)
- Buzzer signal -> GPIO25
- Relay IN (fan control) -> GPIO26
- Servo signal -> GPIO14

Power lines:

- MQ6 VCC -> 5V
- Servo VCC -> 5V external supply
- Relay VCC -> 5V
- ESP32 powered by USB or 5V VIN
- All GNDs must be connected together (ESP32 GND, MQ6 GND, relay GND, servo GND, PSU GND)

## 3) Important Power and Logic Notes

- ESP32 GPIO is 3.3V logic.
- Do not power the servo from an ESP32 GPIO pin.
- Use an external 5V source for servo + relay + MQ6 to avoid ESP32 brownout resets.
- If your relay board is active LOW (common), set RELAY_ACTIVE_LOW = true in code.

## 4) System Logic Table

| Component  | Normal          | Gas Detected    | Gas Cleared (Waiting Manual Reset) |
| ---------- | --------------- | --------------- | ---------------------------------- |
| MQ6 Sensor | Below threshold | Above threshold | Below threshold                    |
| Buzzer     | OFF             | ON              | OFF                                |
| Servo      | 0 deg           | 90 deg          | 90 deg (latched)                   |
| Fan Relay  | OFF             | ON              | ON (latched)                       |

## 5) Threshold Calibration

- Local code variable: gasThreshold (default 1600 in sketch).
- Remote calibration path: /settings/threshold in Firebase RTDB.
- Let MQ6 warm up 20-60 seconds after power-on before trusting readings.

Recommended quick process:

1. Observe stable no-gas baseline from /device/gasLevel.
2. Set threshold above baseline with safety margin.
3. Validate by introducing controlled test gas in a safe environment.

## 6) Firebase Data Contract (Mobile Dashboard)

### Telemetry published by ESP32

Path: /device

- gasLevel (int)
- threshold (int)
- gasDetected (bool)
- sensorWarmedUp (bool)
- buzzerOn (bool)
- servoAt90 (bool)
- fanOn (bool)
- eventLatched (bool)
- status ("WARMING_UP" | "NORMAL" | "GAS_DETECTED" | "WAITING_MANUAL_RESET")
- wifiConnected (bool)
- tsMs (unsigned long)

### Controls written by mobile app

Path: /controls

- resetServo (bool one-shot)
- resetFan (bool one-shot)
- resetAll (bool one-shot, optional)

Behavior:

- When gas is currently detected, reset commands are ignored for safety.
- After gas is safe, resetServo returns servo to 0 deg.
- After gas is safe, resetFan turns fan OFF.
- When both are reset, eventLatched clears and system returns to NORMAL.

### Alerts

- /alerts/latest contains latest alert JSON payload.
- /alerts/pushTrigger can be watched by app/cloud function for push notifications.

## 7) Mobile UI Suggestions

Dashboard widgets:

- Real-time gas value gauge
- Threshold numeric field/slider (writes /settings/threshold)
- Status pill (NORMAL / GAS_DETECTED / WAITING_MANUAL_RESET)
- Fan status indicator
- Servo status indicator
- Buttons:
  - Reset Servo (writes /controls/resetServo=true)
  - Reset Fan (writes /controls/resetFan=true)
  - Optional Reset All (writes /controls/resetAll=true)

Recommended UX behavior:

- Disable reset buttons while gasDetected=true.
- Show warm-up banner until sensorWarmedUp=true.
- Show critical red alert banner on GAS_DETECTED.

## 8) Safety Checklist

- Test alarm, relay, and servo movement before connecting real gas line.
- Use proper ventilation and certified hardware for real deployment.
- Add fuse/protection on fan and power rails.
- Keep high-voltage fan wiring isolated from low-voltage ESP32 section.
- Consider watchdog and failsafe defaults for production use.
