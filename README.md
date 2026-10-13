# AquaSense: Smart Irrigation Controller

An ESP32-based automatic irrigation controller that reads soil moisture, switches a water pump through a relay, and uploads real-time data to a ThingSpeak dashboard. Built and tested in the Wokwi simulator.

## Problem Statement

Manual irrigation often wastes water by over-watering, or harms crops by under-watering. This project automates the decision: water is applied only when the soil is actually dry, and the data can be monitored remotely from anywhere.

## How It Works

1. The ESP32 reads the soil moisture value on an analog pin and converts it to a percentage.
2. If moisture falls **below 30%**, the pump turns **ON**.
3. If moisture rises **above 60%**, the pump turns **OFF**.
4. Between 30% and 60%, the pump keeps its previous state. This gap is called **hysteresis**. It stops the pump from rapidly switching ON and OFF around a single threshold, which protects the relay and pump.
5. Every 20 seconds, the moisture percentage and pump status are sent to ThingSpeak over Wi-Fi.

## Components

| Component | Purpose |
|---|---|
| ESP32 DevKit | Controller and Wi-Fi connectivity |
| Potentiometer | Simulates the soil moisture sensor |
| Relay module | Switches the water pump |
| LED + 220 Ω resistor | Represents the pump (ON/OFF indicator) |
| Wokwi | Circuit and code simulation |
| ThingSpeak | Cloud storage and live graphs |

> **Note:** Wokwi has no soil moisture sensor, so a potentiometer is used instead. Turning the knob left simulates dry soil, and turning it right simulates wet soil.

## Pin Connections

| Component | Pin | ESP32 Pin |
|---|---|---|
| Potentiometer | VCC | 3V3 |
| Potentiometer | SIG | GPIO 34 |
| Potentiometer | GND | GND |
| Relay module | VCC | 5V (VIN) |
| Relay module | GND | GND |
| Relay module | IN | GPIO 26 |
| Pump LED | Anode (via 220 Ω) | GPIO 27 |
| Pump LED | Cathode | GND |

In the simulation the LED represents the pump. On real hardware, the pump would connect to the relay's COM and NO terminals with its own power supply.

## Thresholds

| Setting | Value | Meaning |
|---|---|---|
| `DRY_LIMIT` | 30% | Below this, the pump turns ON |
| `WET_LIMIT` | 60% | Above this, the pump turns OFF |
| `uploadInterval` | 20 s | ThingSpeak free tier allows one update per 15 s |

## Getting Started

1. Create a free account at [ThingSpeak](https://thingspeak.com) and make a channel with two fields:
   - Field 1: Soil Moisture (%)
   - Field 2: Pump Status (0/1)
2. Copy your channel's **Write API Key**.
3. Go to [Wokwi](https://wokwi.com), create a new ESP32 project, and paste in the contents of `sketch.ino` and `diagram.json`.
4. In `sketch.ino`, replace the placeholder:
   ```cpp
   String apiKey = "YOUR_WRITE_API_KEY";
   ```
5. Click **Play**, wait for "Wi-Fi connected!", then rotate the potentiometer.

> **Security:** never commit your real API key to GitHub. Keep the placeholder in the repository.

## Test Results

| Moisture | Pump State | Result |
|---|---|---|
| 15% | ON | Pass |
| 19% | ON | Pass |
| 45% (pump was ON) | Stays ON (hysteresis) | Pass |
| 64% | OFF | Pass |
| 75% | OFF | Pass |

## Repository Structure

```
AquaSense-Smart-Irrigation/
├── sketch.ino
├── diagram.json
└── README.md
```

## Applications

- Farms and agricultural fields
- Greenhouses and nurseries
- Home gardens and terrace plants
- Parks, lawns and sports grounds

## Future Improvements

- Use a real capacitive soil moisture sensor (its output is inverted: high reading means dry)
- Add a rain sensor or weather API to skip watering before rain
- Add temperature and humidity sensing (DHT22)
- Add mobile alerts and manual pump control through Blynk
- Average multiple sensor readings and add Wi-Fi auto-reconnect
- Solar power for remote fields

