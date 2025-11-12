


#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>
#include <WiFiS3.h>
#include <OneWire.h>
#include <DallasTemperature.h>

//Wi-Fi
const char SSID[] = "WIFINAME";
const char PASS[] = "PASSWORD";

//Sensors
#define TEMP_SENSOR_PIN 13
#define WATER_LEVEL_PIN A0
#define PH_SENSOR_PIN A1
#define WATER_FLOW_PIN 8

OneWire oneWire(TEMP_SENSOR_PIN);
DallasTemperature sensors(&oneWire);

volatile long pulse = 0;

void increase() {
  pulse++;
}

// Arduino Cloud Variables
float temperature;
int waterLevel;
float ph;
int waterFlow;

//Connection Handler
WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Initialize sensors
  sensors.begin();
  attachInterrupt(digitalPinToInterrupt(WATER_FLOW_PIN), increase, RISING);

  // Connect to Arduino Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  // Bind variables to Thing properties
  ArduinoCloud.addProperty(temperature, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(waterLevel, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(ph, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(waterFlow, READ, ON_CHANGE, NULL);

  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  // Connect to Wi-Fi
  WiFi.begin(SSID, PASS);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi!");
}

void loop() {
  // Required for Arduino Cloud
  ArduinoCloud.update();

  //Read Sensors
  sensors.requestTemperatures();
  temperature = sensors.getTempFByIndex(0);

  waterLevel = analogRead(WATER_LEVEL_PIN);

  float phVoltage = (analogRead(PH_SENSOR_PIN) / 1023.0) * 5.0;
  ph = (7 - ((2.5 - phVoltage) / 0.05916)) * -1;

  noInterrupts();
  waterFlow = pulse;
  pulse = 0;
  interrupts();

  //Serial Debug
  Serial.print("Temperature: "); Serial.println(temperature);
  Serial.print("Water Level: "); Serial.println(waterLevel);
  Serial.print("pH: "); Serial.println(ph);
  Serial.print("Water Flow: "); Serial.println(waterFlow);
  Serial.println("---------------------------");

  delay(1000); // update interval
}
