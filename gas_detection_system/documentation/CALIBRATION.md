# Gas Sensor Calibration Guide

## 1. Understanding MQ-2 Gas Sensor

### Sensor Basics

```
MQ-2 Sensor Specifications:
┌─────────────────────────────────────┐
│ Detection Gases: LPG, Butane, CH₄   │
│ Recommended Load: 5kΩ resistor      │
│ Target Gas: LPG                     │
│ Concentration: 300-10,000 PPM       │
│ Response Time: 10-40 seconds        │
│ Heat-up Time: 15-20 seconds         │
│ Lifespan: 5 years (typical)         │
│ Operating Voltage: 5V               │
│ Heater Current: 150mA (avg)         │
│ Preheat Power: Stable after 15s     │
└─────────────────────────────────────┘

SENSOR PIN CONFIGURATION:
VCC ──┐
      │ (Internal heater)
GND ──┤
      │
Aout──┴─ (Analog output, 0-5V)
```

### How the Sensor Works

```
RESISTANCE VARIATION:
┌─────────────────────────────────────┐
│ Clean Air (Baseline):               │
│ Sensor internal resistance = R₀     │
│ Output voltage = VCC / (Rs + Load) │
│ Typically: 3.3-4.5V (low output)    │
│                                     │
│ When Gas Present:                   │
│ Sensor resistance decreases         │
│ Output voltage increases            │
│ Voltage proportional to gas level   │
│                                     │
│ High Gas Concentration:             │
│ Very low resistance                 │
│ Output voltage → near VCC           │
│ (1.5-2.5V for danger levels)       │
└─────────────────────────────────────┘
```

## 2. Pre-Calibration Setup

### Hardware Requirements

```
REQUIRED COMPONENTS:
├─ Arduino with analog input (A0-A5)
├─ MQ-2 gas sensor module
├─ 5V power supply (stable)
├─ USB cable for serial monitoring
├─ Common clean environment
└─ Optional: Known gas source for testing

RECOMMENDED EQUIPMENT:
├─ Multimeter for voltage checking
├─ Certified gas calibration kit
├─ Temperature/humidity sensor
└─ Gas flow meter (advanced)
```

### Preparation Checklist

- [ ] Arduino powered for 30 minutes (stability)
- [ ] All connections verified and solid
- [ ] Serial monitor open at 9600 baud
- [ ] No active gas sources nearby
- [ ] Windows open for ventilation
- [ ] Temperature stable (20-25°C ideal)
- [ ] Humidity in normal range (30-70%)
- [ ] Room has not had recent gas exposure
- [ ] Sensor never exposed to extreme conditions before

## 3. Initial Calibration Procedure

### Step 1: Warm-Up Phase (15-20 seconds)

```
PROCEDURE:
1. Power up Arduino with MQ-2 sensor
2. Observe serial monitor output:

   Time(s) │ ADC Raw │ Voltage │ Status
   ───────────────────────────────────
   0       │ 850     │ 4.15V   │ Heating
   5       │ 820     │ 4.00V   │ Heating
   10      │ 790     │ 3.86V   │ Stabilizing
   15      │ 780     │ 3.81V   │ STABLE ✓

3. Wait until values stabilize (change < 20/second)
4. Once stable, baseline established

WHAT YOU'RE LOOKING FOR:
├─ Voltage decreases initially (normal)
├─ Stabilizes after 15-20 seconds
├─ No sudden spikes or drops
└─ Consistent readings (±10 ADC from mean)
```

### Step 2: Establish Baseline

```
CODE TO ADD FOR CALIBRATION:
(Replace in GasLeakDetection.ino)

#define CALIBRATION_MODE true

void setup() {
  // ... existing code ...

  if (CALIBRATION_MODE) {
    Serial.println("CALIBRATION MODE - BASELINE ESTABLISHMENT");
    Serial.println("DO NOT expose sensor to gas");
    Serial.println("Waiting for stability...\n");

    // Take 50 baseline readings
    int baselineSum = 0;
    for (int i = 0; i < 50; i++) {
      int value = analogRead(GAS_SENSOR_PIN);
      baselineSum += value;

      Serial.print("Reading ");
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(value);
      Serial.print(" ADC (");
      Serial.print((value * 5.0) / 1023.0);
      Serial.println(" V)");

      delay(100);
    }

    int baseline = baselineSum / 50;
    Serial.print("\nBASELINE ESTABLISHED: ");
    Serial.println(baseline);
    Serial.println("This is your R₀ reference value");
    Serial.println("Store this value in code");
  }
}

#define BASELINE_ADC 780  // Update with your value
```

### Step 3: Record Values

Create a calibration log:

```
CALIBRATION LOG TEMPLATE:
─────────────────────────────────────
Date: ____________
Location: ___________________________
Temperature: ______°C
Humidity: ______%

BASELINE READINGS (Clean Air):
Reading 1:  ___ ADC (___.__ V)
Reading 2:  ___ ADC (___.__ V)
Reading 3:  ___ ADC (___.__ V)
Reading 10: ___ ADC (___.__ V)
Reading 25: ___ ADC (___.__ V)
Reading 50: ___ ADC (___.__ V)

AVERAGE R₀: ___ ADC (___.__ V)
MIN: ___ MAX: ___ RANGE: ___
Standard Deviation: ___

NOTES:
- Sensor behavior: _________________
- Any anomalies: ___________________
- Environmental factors: ____________
```

## 4. Threshold Calibration

### Method 1: Empirical Testing (Recommended)

```
PROCEDURE:
1. Establish baseline (from Step 3 above)
2. Get known gas source (optional, recommended)
3. Expose sensor to gas gradually
4. Record ADC values at different concentrations

EXAMPLE DATA COLLECTION:

Gas Level (PPM) │ ADC Value │ Voltage │ Status
────────────────│───────────│─────────│────────
0 (Clean)       │ 780       │ 3.81V   │ Baseline
50 PPM          │ 700       │ 3.42V   │ Slight gas
100 PPM         │ 650       │ 3.18V   │ Detectable
200 PPM         │ 600       │ 2.93V   │ Noticeable
300 PPM         │ 550       │ 2.69V   │ Warning
400 PPM         │ 480       │ 2.35V   │ Moderate
500 PPM         │ 420       │ 2.05V   │ ALARM ✓
600 PPM         │ 380       │ 1.86V   │ High
1000 PPM        │ 300       │ 1.47V   │ Danger

RESULT: Set threshold = 500 ADC (or adjust to suit environment)
```

### Method 2: Calculation-Based (Mathematical)

```
FORMULA APPROACH:

Rs/R₀ = (VCC - VOUT) / VOUT

Where:
- Rs = Sensor resistance at gas level
- R₀ = Baseline sensor resistance
- VCC = Supply voltage (5V)
- VOUT = Sensor output voltage

Example with MQ-2 typical values:
If THRESHOLD = 500 PPM LPG
Then Rs/R₀ ≈ 1.5 (from datasheet curve)

Calculate required VOUT:
VOUT = VCC / (1 + (5kΩ / Rs))
VOUT ≈ 2.0V (example)

Convert to ADC: ADC = (VOUT × 1023) / 5 ≈ 410

THRESHOLD ADC VALUE: 410-500 (adjust for other gases)
```

## 5. Temperature & Humidity Compensation

### Why Compensation Matters

```
SENSOR SENSITIVITY VARIATION:
┌─────────────────────────────────────┐
│ Temperature Effects:                │
│ ├─ Every ±10°C ≈ ±5-10% shift      │
│ ├─ Cold (0°C): Less responsive     │
│ └─ Hot (50°C): More responsive     │
│                                     │
│ Humidity Effects:                   │
│ ├─ 20% RH: Lower sensitivity       │
│ ├─ 50% RH: Nominal sensitivity     │
│ └─ 80% RH: Variable accuracy       │
│                                     │
│ SOLUTION: Apply correction factor  │
└─────────────────────────────────────┘
```

### Simple Compensation Algorithm

```cpp
// Add to code for compensation
#include <DHT.h>  // Temperature/Humidity sensor

#define DHTPIN A1
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

float compensatedThreshold(float baseThreshold) {
  float temp = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Temperature compensation (-10 to +50°C range)
  float tempFactor = 1.0;
  if (temp < 20) {
    tempFactor = 1.0 + (20 - temp) * 0.005;  // Reduce sensitivity
  } else if (temp > 20) {
    tempFactor = 1.0 - (temp - 20) * 0.003;  // Increase sensitivity
  }

  // Humidity compensation (assume 50% optimal)
  float humidityFactor = 1.0;
  if (humidity < 40 || humidity > 80) {
    humidityFactor = 0.95;  // Reduce reliability
  }

  return baseThreshold * tempFactor * humidityFactor;
}
```

## 6. Verification and Testing

### Verification Checklist

- [ ] Baseline readings stable (±5%)
- [ ] No sensor drift over 1 hour
- [ ] Response time < 30 seconds
- [ ] Recovery time reasonable
- [ ] Threshold appropriate for application
- [ ] Alarm triggers consistently above threshold
- [ ] No false alarms in 24-hour test
- [ ] Long-term stability verified over 1 week

### Long-Term Drift Monitoring

```
MAINTENANCE LOG:

Date     │ Time │ ADC Value │ Drift │ Status
─────────┼──────┼───────────┼───────┼────────
04/07/26 │ 09:00│ 780       │ ---   │ New
04/07/26 │ 21:00│ 778       │ -0.3% │ Normal
04/08/26 │ 09:00│ 775       │ -0.6% │ Normal
04/14/26 │ 09:00│ 765       │ -1.9% │ Minor
04/21/26 │ 09:00│ 745       │ -4.5% │ Monitor
04/28/26 │ 09:00│ 720       │ -7.7% │ Recalibrate

ACTION: If drift > 10%, recalibrate or replace sensor
```

## 7. Troubleshooting Calibration Issues

### Problem 1: Values Not Stabilizing

```
SYMPTOMS:
├─ ADC values constantly changing (±50 or more)
├─ No baseline established after 30 seconds
└─ Erratic readings in serial monitor

POSSIBLE CAUSES:
├─ Poor power supply (unstable voltage)
├─ Loose connection at sensor
├─ Electromagnetic interference
├─ Sensor damaged
└─ Arduino ADC noise

SOLUTIONS:
1. Check power supply voltage with multimeter
   │ Should be stable 5.0 ± 0.1V
2. Inspect all connections (reseat if loose)
3. Keep wires away from EM noise sources
4. Test different Arduino pins
5. Add 100µF capacitor across power
6. Try different sensor (if available)
```

### Problem 2: Baseline Too High/Low

```
SYMPTOMS:
├─ Baseline voltage > 4.5V (too high)
├─ Baseline voltage < 3.0V (too low)
└─ Can't set proper threshold

POSSIBLE CAUSES:
├─ Sensor not fully warm
├─ Wrong load resistor value
├─ Damaged sensor
├─ Wrong calibration
└─ Environmental contamination

SOLUTIONS:
1. Extend warm-up time to 30 seconds
2. Verify load resistor is 5kΩ
3. Check sensor connections
4. Use fresh sensor from same batch
5. Move to cleaner environment
6. Check for residual gas in area
```

### Problem 3: Inconsistent Threshold Response

```
SYMPTOMS:
├─ Alarm triggers inconsistently
├─ Same gas level gives different readings
├─ Threshold drift over time
└─ Hysteresis behavior (response varies)

POSSIBLE CAUSES:
├─ Temperature fluctuations
├─ Humidity changes
├─ Sensor aging
├─ Contamination
└─ Insufficient averaging

SOLUTIONS:
1. Implement moving average (5-10 samples)
2. Add hysteresis (e.g., 480-520 ADC band)
3. Apply temperature/humidity compensation
4. Extend sensor preheat time
5. Clean sensor gently with air
6. Replace if > 2 years old
```

## 8. Advanced Calibration Techniques

### Multi-Point Calibration Curve

```
For more accuracy, create 3+ point calibration:

Point 1: Baseline (Clean Air)
Point 2: 300 PPM (low detection)
Point 3: 500 PPM (alarm threshold)
Point 4: 1000 PPM (high danger)

CODE IMPLEMENTATION:
const int CAL_POINTS = 4;
int calADC[CAL_POINTS] = {780, 550, 420, 300};
int calPPM[CAL_POINTS] = {0, 300, 500, 1000};

float getPPM(int adcValue) {
  // Linear interpolation between calibration points
  for (int i = 0; i < CAL_POINTS - 1; i++) {
    if (adcValue >= calADC[i+1] &&
        adcValue <= calADC[i]) {
      // Interpolate
      float ratio = (float)(adcValue - calADC[i]) /
                    (calADC[i+1] - calADC[i]);
      return calPPM[i] + ratio * (calPPM[i+1] - calPPM[i]);
    }
  }
  return -1;  // Out of range
}
```

### Adaptive Re-calibration

```
Periodically re-check baseline:

#define RECAL_CHECK_HOURS 24
uint32_t lastRecalTime = 0;

void checkRecalibration() {
  if (millis() - lastRecalTime >
      RECAL_CHECK_HOURS * 3600000) {

    // Measure current baseline in clean air
    int currentBaseline = readSensorAverage();

    // Compare with stored baseline
    if (abs(currentBaseline - BASELINE_ADC) > 50) {
      Serial.println("WARNING: Baseline drift detected!");
      Serial.println("Manual recalibration recommended");
      // Optionally trigger service alert
    }

    lastRecalTime = millis();
  }
}
```

## 9. Calibration Documentation Template

### Required Documentation to Keep

```
SENSOR CALIBRATION RECORD

SENSOR INFORMATION:
├─ Model: MQ-2
├─ Serial Number: ______________
├─ Manufacturing Date: __________
├─ First Installation: __________
└─ Expected Replacement: ________

CALIBRATION 1 - INITIAL
├─ Date: ________________
├─ Technician: __________
├─ Baseline ADC: ________
├─ Baseline Voltage: ________
├─ Threshold ADC: ________
├─ Test Results: PASS / FAIL
├─ Notes: _______________
└─ Signature: ___________

CALIBRATION 2 - 6 MONTHS
├─ Date: ________________
├─ Current Baseline ADC: ________
├─ Drift: ±_____%
├─ Recalibration Needed: YES / NO
└─ Notes: _______________

CALIBRATION 3 - 12 MONTHS
├─ Date: ________________
├─ Current Baseline ADC: ________
├─ Replacement Date: ____________
└─ Notes: _______________
```

---

**Next**: See [SAFETY.md](SAFETY.md) for critical safety procedures and [ARCHITECTURE.md](ARCHITECTURE.md) for system design.
