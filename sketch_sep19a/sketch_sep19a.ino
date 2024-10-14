// Defining blynk template id and name
#define BLYNK_TEMPLATE_ID "BLYNK TEMPLATE ID"
#define BLYNK_TEMPLATE_NAME "BLYNK TEMPLATE NAME"

#include <Arduino.h> 
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// Defining pins for the components
#define SOIL_MOISTURE_SENSOR 34
#define RELAY_PIN 25
#define Led_pin 16
#define Light_sensor_pin 35


// Defining threshold values for sensors (in percentage)
const int drySoil = 30;  // 30% or below means soil is dry
const int wetSoil = 65;  // 65% or above means soil is wet

// Defining light sensor raw value ranges
const int MIN_LIGHT_RAW = 0;    // Assume the lowest light level gives a raw value close to 0
const int MAX_LIGHT_RAW = 4095; // Maximum raw value from the sensor

// Light sensor threshold for brightness
const int LOW_LIGHT = 200;
const int MID_LOW_LIGHT = 400;
const int MID_LIGHT = 600;
const int MID_HIGH_LIGHT = 800;
const int HIGH_LIGHT = 1000;

// PWM channel and settings for the LED
const int pwmChannel = 0;
const int pwmFreq = 5000;  // PWM frequency
const int pwmResolution = 8;  // 8-bit resolution (0-255)

// Creating arrays to store ssid, wifi password and blynk token
char ssid[] = "WIFI NAME";
char pass[] = "PASSWORD OF WIFI";
char token[] = "BLYNK TOKEN"; 

void setup() {
  Serial.begin(115200);
  //connect to the BLYNK IOT
  Blynk.begin(token, ssid, pass);

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(SOIL_MOISTURE_SENSOR, INPUT);
  pinMode(Light_sensor_pin, INPUT);

  // Set relay off initially
  digitalWrite(RELAY_PIN, LOW);

  // Configure PWM channel for the LED pin
  ledcSetup(pwmChannel, pwmFreq, pwmResolution);
  ledcAttachPin(Led_pin, pwmChannel);
}

void loop() {
  Blynk.run();
  // Reading soil moisture sensor value
  int rawSoilMoistureLevel = analogRead(SOIL_MOISTURE_SENSOR);

  // Map the raw value (1800-4095) to a percentage (0-100)
  int soilMoistureLevel = map(rawSoilMoistureLevel, 1800, 4095, 100, 0);

  // Reading ambient light sensor value
  int rawLightLevel = analogRead(Light_sensor_pin);

  // Map the raw light value (0-4095) to a percentage (0-100)
  int lightLevelPercentage = map(rawLightLevel, MIN_LIGHT_RAW, MAX_LIGHT_RAW, 0, 100);

  // Print raw and mapped soil moisture level
  Serial.print("Raw Soil Moisture Level: ");
  Serial.println(rawSoilMoistureLevel);
  Serial.print("Mapped Soil Moisture Level (%): ");
  Serial.println(soilMoistureLevel);
  Blynk.virtualWrite(V0, soilMoistureLevel);

  // Print raw and mapped light level
  Serial.print("Raw Light Level: ");
  Serial.println(rawLightLevel);
  Serial.print("Mapped Light Level (%): ");
  Serial.println(lightLevelPercentage);
  Blynk.virtualWrite(V1, lightLevelPercentage);

  // Control water pump based on soil moisture percentage

  if (soilMoistureLevel <= drySoil) {
    // If soil moisture is below or equal to dry threshold (30% or less), turn on water pump
    Blynk.logEvent("dry_soil", "WARNING!! Soil is dry we are adding water.");
    digitalWrite(RELAY_PIN, LOW);

  } else if (soilMoistureLevel >= wetSoil) {
    // If soil moisture is above or equal to wet threshold (70% or more), turn off water pump
    Blynk.logEvent("wet_soil", "Soil is in Good Condition :)");
    digitalWrite(RELAY_PIN, HIGH);
  }

  int virtualRelayButton = digitalRead(RELAY_PIN) == LOW ? 1 : 0;
  Blynk.virtualWrite(V2, virtualRelayButton);

  // LED brightness control based on ambient light level
  int ledBrightness;
  
  if (lightLevelPercentage <= 20) {
    ledBrightness = 255;  // Maximum brightness for very low light
  } 
  else if (lightLevelPercentage <= 40) {
    ledBrightness = 200;  // Brightness level for low light
  } 
  else if (lightLevelPercentage <= 60) {
    ledBrightness = 150;  // Moderate brightness
  } 
  else if (lightLevelPercentage <= 80) {
    ledBrightness = 100;  // Lower brightness
  } 
  else if (lightLevelPercentage <= 90) {
    ledBrightness = 50;  // Very low brightness
  } 
  else {
    ledBrightness = 0;  // Minimum brightness when the light is too high
  }

  // Write PWM signal to LED
  ledcWrite(pwmChannel, ledBrightness);
  Blynk.virtualWrite(V4, ledBrightness);

  if(ledBrightness > 0){
    Blynk.virtualWrite(V3, ledBrightness);
  }

  delay(1000);  // Wait for 1 second
}
