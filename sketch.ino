#include <WiFi.h>
#include <HTTPClient.h>

// ---------- Wi-Fi (Wokwi simulated network) ----------
const char* ssid     = "Wokwi-GUEST";
const char* password = "";

// ---------- ThingSpeak ----------
String apiKey = "YOUR_WRITE_API_KEY"   // paste your Write API Key
String server = "http://api.thingspeak.com/update";

// ---------- Pins ----------
#define SOIL_PIN   34   // Analog input (potentiometer = soil sensor)
#define RELAY_PIN  26   // Relay module control
#define PUMP_LED   27   // LED showing pump state

// ---------- Thresholds ----------
// Soil moisture % = mapped from ADC (0-4095)
// Below DRY_LIMIT  -> soil too dry  -> pump ON
// Above WET_LIMIT  -> soil wet enough -> pump OFF
// Gap between them (hysteresis) prevents rapid ON/OFF switching
const int DRY_LIMIT = 30;   // % moisture
const int WET_LIMIT = 60;   // % moisture

bool pumpState = false;
unsigned long lastUpload = 0;
const unsigned long uploadInterval = 20000; // ThingSpeak free tier: min 15 s

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(PUMP_LED, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(PUMP_LED, LOW);

  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected!");
}

int readMoisturePercent() {
  int raw = analogRead(SOIL_PIN);            // 0 - 4095
  // Pot at max = wet (100%), pot at min = dry (0%)
  int percent = map(raw, 0, 4095, 0, 100);
  return constrain(percent, 0, 100);
}

void setPump(bool on) {
  pumpState = on;
  digitalWrite(RELAY_PIN, on ? HIGH : LOW);
  digitalWrite(PUMP_LED, on ? HIGH : LOW);
}

void loop() {
  int moisture = readMoisturePercent();

  // ----- Control logic with hysteresis -----
  if (moisture < DRY_LIMIT && !pumpState) {
    setPump(true);
    Serial.println(">> Soil DRY: Pump ON");
  } else if (moisture > WET_LIMIT && pumpState) {
    setPump(false);
    Serial.println(">> Soil WET: Pump OFF");
  }

  Serial.print("Moisture: ");
  Serial.print(moisture);
  Serial.print("%  | Pump: ");
  Serial.println(pumpState ? "ON" : "OFF");

  // ----- Upload to ThingSpeak every 20 s -----
  if (millis() - lastUpload >= uploadInterval) {
    lastUpload = millis();
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      String url = server + "?api_key=" + apiKey +
                   "&field1=" + String(moisture) +
                   "&field2=" + String(pumpState ? 1 : 0);
      http.begin(url);
      int code = http.GET();
      Serial.print("ThingSpeak response: ");
      Serial.println(code);   // 200 = success
      http.end();
    }
  }
  delay(1000);
}
