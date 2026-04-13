# Troubleshooting Guide

## Common Issues & Solutions

### Issue 1: LCD Not Displaying Anything

**Symptoms:**

- LCD screen blank/no text
- Backlight on but no characters
- Code compiles/uploads fine

**Solutions:**

```
1. Check I2C Address:
   ├─ Open Tools → Serial Monitor (9600 baud)
   └─ Upload example I2C scanner sketch

2. Verify Connection:
   ├─ SDA → GPIO 21 (or your configured pin)
   ├─ SCL → GPIO 22 (or your configured pin)
   ├─ GND → Ground
   └─ VCC → 5V

3. Update Address in Code:
   ├─ If scanner shows 0x3F, change:
   │  #define LCD_ADDRESS 0x3F
   │
   └─ If scanner shows 0x27, keep as is

4. Test Display Independently:
   ├─ Upload LCD test sketch only
   ├─ Verify display initializes
   └─ Check for error messages
```

### Issue 2: Gas Sensor Not Reading Values

**Symptoms:**

- Serial shows gas level = 0
- ADC values don't change
- Always reads same value

**Solutions:**

```
1. Verify Sensor Power:
   ├─ Multimeter: Check VCC pin (should be 5V)
   ├─ Check GND pin (should be 0V)
   └─ Wait 20 seconds (sensor warm-up required)

2. Check Analog Connection:
   ├─ Confirm AOUT → GPIO 34 (A6)
   ├─ Solder connection secure
   └─ No loose wires

3. Test ADC Pin:
   ├─ Read from different analog pin
   ├─ Try GPIO 35 (A7) or GPIO 36 (A0)
   └─ Verify pin is functioning

4. Verify Sensor Module:
   ├─ Multiple LED on board? Should be on
   ├─ Potentiometer adjustable? Should be
   └─ Try another MQ6 if available

5. Check Code Configuration:
   ├─ Verify #define MQ6_PIN 34
   ├─ Confirm pinMode(MQ6_PIN, INPUT)
   └─ Check analogRead() returns 0-4095
```

### Issue 3: Reset Button Not Responding

**Symptoms:**

- Button press doesn't trigger reset
- No serial message when pressed
- System doesn't return to NORMAL state

**Solutions:**

```
1. Verify Button Wiring:
   ├─ One pin → GPIO 32
   ├─ Other pin → GND
   └─ No resistor needed (internal pullup used)

2. Debug Button State:
   ├─ Add debug code:
   │  Serial.println(digitalRead(RESET_BUTTON_PIN));
   │
   ├─ Serial monitor should show:
   │  HIGH (button released)
   │  LOW  (button pressed)
   │
   └─ If inverted, swap GPIO pin

3. Check Button Hardware:
   ├─ Physical buttons sometimes defective
   ├─ Test with another button
   ├─ Try different GPIO pin
   └─ Verify no solder bridges

4. Test Reset Function:
   ├─ Add manual call in setup():
   │  resetSystem();  // Force reset on startup
   │
   └─ Verify output changes (buzzer, fan, valve)
```

### Issue 4: Relay Not Activating

**Symptoms:**

- Relay doesn't click when triggered
- Fan/Valve doesn't turn on
- No LED on relay module when activated

**Solutions:**

```
1. Verify Relay Module Power:
   ├─ Multimeter check relay VCC (should be 5V)
   ├─ Check relay GND (should be 0V)
   └─ Module LED should light up

2. Test GPIO Control:
   ├─ Add debug code:
   │  digitalWrite(RELAY_FAN, HIGH);   // Relay ON
   │  delay(2000);
   │  digitalWrite(RELAY_FAN, LOW);    // Relay OFF
   │
   └─ Serial: Relay should click audibly twice

3. Verify Pin Connections:
   ├─ RELAY_FAN → GPIO 26 (IN1)
   ├─ RELAY_VALVE → GPIO 27 (IN2)
   ├─ Protective resistor (1kΩ) on each
   └─ Diode (1N4007) for back-EMF protection

4. Check Relay Contacts:
   ├─ Relay should have:
   │  COM (common)
   │  NO (normally open for fan)
   │  NC (normally closed for valve)
   │
   └─ Wiring to actual loads correct?

5. Test with External Power:
   ├─ Power relay separately (12V supply)
   ├─ Use external signal (not Arduino pin)
   └─ Verify relay clicking (proves hardware OK)
```

### Issue 5: System Stays in WARNING State

**Symptoms:**

- After gas detection, can't exit WARNING
- Reset button doesn't work
- Valve won't open

**Solutions:**

```
1. Verify Reset Button Physically:
   ├─ Press and hold reset button
   ├─ Watch serial monitor for "RESET BUTTON PRESSED"
   ├─ If nothing appears, button isn't working
   └─ Test button continuity with multimeter

2. Check System State:
   ├─ Serial shows state: WARNING
   ├─ This is intentional pending reset
   └─ Not a bug, expected behavior

3. Force Software Reset:
   ├─ Add timeout to WARNING state (optional):
   │  if (millis() - warningStartTime > 3600000) {
   │    // Auto-reset after 1 hour
   │    resetSystem();
   │  }
   │
   └─ Or manually send reset via Firebase

4. Manual Valve Override:
   ├─ Manual valve switch (GPIO 35) should work
   ├─ This overrides safety lockout
   └─ Test: Press manual valve switch → valve opens
```

### Issue 6: WiFi Keeps Disconnecting

**Symptoms:**

- "WiFi: FAIL" in serial monitor
- Firebase shows "Firebase: FAIL"
- Connection drops periodically

**Solutions:**

```
1. Check WiFi Credentials:
   ├─ WiFi SSID: "Mital" (correct?)
   ├─ Password: "12346789" (correct?)
   ├─ Special chars in password? Could cause issues
   └─ Try connecting phone to verify

2. Improve Signal:
   ├─ Move ESP32 closer to router
   ├─ Check for interference (microwave, etc)
   ├─ Restart router if needed
   └─ Check router supports 2.4GHz (not 5GHz)

3. Verify Credentials in Code:
   ├─ Double-check spelling
   ├─ Exact match required (case-sensitive)
   ├─ No leading/trailing spaces
   └─ Recompile and upload after changes

4. Check Firebase Connectivity:
   ├─ Optional: Update Firebase URL if changed
   ├─ Auth token might have expired
   ├─ Check Firebase rules allow client access
   └─ Regenerate token if needed

5. Power Issues:
   ├─ Weak power supply can cause drops
   ├─ Use 2A+ supply for ESP32
   ├─ Capacitor on power input helps
   └─ Add 100µF capacitor to stabilize
```

### Issue 7: Sensor Trigger Too Sensitive (Too Many False Alarms)

**Symptoms:**

- Gas alarm triggers without actual gas
- Random spikes in sensor readings
- Inconsistent behavior

**Solutions:**

```
1. Increase Threshold Value:
   ├─ Current: gasThreshold = 1600
   ├─ Increase to: gasThreshold = 1800 or 2000
   ├─ Higher number = less sensitive
   └─ Test in your environment

2. Improve Sensor Readings:
   ├─ Already using 5-sample average
   ├─ Could increase to 10-sample average:
   │  Change: static int sensorHistory[5] → [10]
   │  Change: sum / 5 → sum / 10
   │
   └─ More averaging = slower response

3. Add Hysteresis:
   ├─ Require 2 consecutive readings above threshold
   ├─ Prevents single noise spike triggering alarm
   └─ Example code:
   │
   │  static int detectionCount = 0;
   │  if (averageGasLevel >= gasThreshold) {
   │    detectionCount++;
   │  } else {
   │    detectionCount = 0;
   │  }
   │
   │  if (detectionCount >= 2) {  // 2 seconds of gas
   │    // Trigger alarm
   │  }

4. Check Environment:
   ├─ Dust, particles can affect sensor
   ├─ Humidity extremes cause drift
   ├─ Temperature changes affect reading
   └─ Calibrate in actual environment
```

### Issue 8: Sensor Trigger Too Insensitive (Won't Detect Gas)

**Symptoms:**

- Gas present but alarm doesn't trigger
- Sensor readings below threshold always
- No detection in danger situations

**Solutions:**

```
1. Decrease Threshold Value:
   ├─ Current: gasThreshold = 1600
   ├─ Decrease to: gasThreshold = 1200 or 1000
   ├─ Lower number = more sensitive
   └─ Test carefully (may cause false alarms)

2. Verify Sensor Calibration:
   ├─ Sensor needs 20-second warm-up
   ├─ Must be exposed to clean air for baseline
   ├─ Try relocating sensor location
   └─ Check manufacturer specs for MQ6

3. Test Sensor Independently:
   ├─ Use simpler test sketch:
   │  void loop() {
   │    Serial.println(analogRead(34));
   │    delay(100);
   │  }
   │
   ├─ Expose to lighter fume
   └─ Check if values change

4. Check Sensor Age:
   ├─ MQ sensors have ~5 year lifespan
   ├─ Sensitivity decreases over time
   ├─ If sensor is old, replacement may be needed
   └─ Try with fresh sensor to verify
```

### Issue 9: Firebase Updates Too Slow

**Symptoms:**

- Data appears in Firebase after 10+ seconds
- Responsiveness feels delayed
- Commands take time to process

**Solutions:**

```
1. Reduce Update Interval:
   ├─ Current: delay(1000) between updates
   ├─ Change to: delay(500) for faster updates
   ├─ Must balance with data usage
   └─ Test with your WiFi bandwidth

2. Optimize JSON Payloads:
   ├─ Smaller payloads upload faster
   ├─ Remove unnecessary fields
   ├─ Compress string data
   └─ Currently optimized, should be fast

3. Check WiFi Signal:
   ├─ Weak signal causes upload delays
   ├─ Move router closer
   ├─ Check for interference
   └─ Signal strength shows in serial

4. Firebase Performance:
   ├─ Firebase project location matters
   ├─ Check Firebase console for issues
   ├─ Region closest to you = faster
   └─ May require Firebase upgrade for limits
```

### Issue 10: Serial Monitor Shows Garbled Text

**Symptoms:**

- Serial monitor shows random characters
- Text appears corrupted
- Partial messages display

**Solutions:**

```
1. Check Baud Rate:
   ├─ Code uses: Serial.begin(115200)
   ├─ Serial Monitor should be: 115200
   ├─ Found in serial monitor dropdown
   └─ Must match exactly

2. Verify USB Cable:
   ├─ Try different USB cable
   ├─ Check USB port on ESP32
   ├─ Try different USB port on computer
   └─ Good quality cable reduces issues

3. Check Driver Installation:
   ├─ CH340 driver needed for some boards
   ├─ Windows: Check Device Manager for "CH340"
   ├─ Mac: May need VCP driver
   └─ Linux: Usually works without driver

4. Reset Serial Connection:
   ├─ Close Serial Monitor
   ├─ Unplug ESP32
   ├─ Wait 5 seconds
   ├─ Plug back in
   └─ Open Serial Monitor
```

### Issue 11: Buzzer Doesn't Sound

**Symptoms:**

- No sound when alarm triggers
- LED on relay lights but no beep
- No audible alert during danger

**Solutions:**

```
1. Verify Buzzer Power:
   ├─ Multimeter check + pin (should be 5V when on)
   ├─ Check - pin (should be 0V)
   └─ Should hear click when powered

2. Check Pin Connection:
   ├─ Buzzer + → GPIO 25 (via 1kΩ resistor)
   ├─ Buzzer - → GND
   └─ Polarity matters for electrolytic buzzers

3. Test Buzzer Independently:
   ├─ Connect directly to 5V
   ├─ Should produce sound immediately
   └─ If silent, buzzer may be defective

4. Verify Code:
   ├─ Check digitalWrite(BUZZER, HIGH) triggers
   ├─ Serial monitor should show buzzer events
   └─ Timing: Should activate in STATE_GAS_DETECTED

5. Audio Output Test:
   ├─ Try different sound frequency:
   │  tone(BUZZER, 1000, 1000);  // 1kHz, 1 second
   │
   └─ Some buzzers have frequency preferences
```

## Hardware Testing Checklist

Before troubleshooting code, verify hardware:

```
☐ 5V Power Supply: 4.8-5.2V (measure with multimeter)
☐ 12V Power Supply: 11.5-12.5V (measure with multimeter)
☐ GND Connections: All grounds connected to star point
☐ Gas Sensor: Shows ~4V output in clean air
☐ LCD: Powers on (backlight lights)
☐ Relay Module: LED lights when pin goes HIGH
☐ Buzzer: Produces sound when 5V applied
☐ Buttons: Continuity when pressed
☐ Servo: Moves when command sent
☐ WiFi: Connects to your network
```

## Quick Reference: Serial Debug Codes

Add these to serial output to identify issues:

```cpp
Serial.print("ADC Raw: "); Serial.println(analogRead(34));
Serial.print("Button State: "); Serial.println(digitalRead(32));
Serial.print("Relay State: "); Serial.println(digitalRead(26));
Serial.print("WiFi Status: "); Serial.println(WiFi.status());
// Status: 0=Idle, 1=ScanStarted, 2=ScanCompleted, 3=Connected
Serial.print("Free Memory: "); Serial.println(esp_get_free_heap_size());
Serial.print("WiFi RSSI: "); Serial.println(WiFi.RSSI());  // Signal strength
```

## Getting Help

If you're still stuck:

1. **Collect information:**

   - Serial monitor output (full startup to issue)
   - What hardware you changed
   - What exactly happens vs what should happen
   - Photos of wiring

2. **Test methodology:**

   - Isolate the problem (test components individually)
   - Use simpler test sketches first
   - Verify hardware before blaming code

3. **Documentation:**
   - Check [ENHANCED_CODE_GUIDE.md](ENHANCED_CODE_GUIDE.md)
   - Review [SAFETY.md](SAFETY.md) for warnings
   - Check [ARCHITECTURE.md](ARCHITECTURE.md) for system design

---

**Remember**: Most issues are wiring-related, not code-related. Verify hardware connections first!
