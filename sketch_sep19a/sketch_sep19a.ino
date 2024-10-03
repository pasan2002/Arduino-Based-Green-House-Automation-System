// Defining pins for the components
#define SOIL_MOISTURE_SENSOR 34
#define RELAY_PIN 25
#define LED_PIN 23
#define LIGHT_SENSOR_PIN 35

// Defining threshold values for sensors
const int drySoil = 800;
const int wetSoil = 400;

const int lowLight = 200;
const int highLight = 800;

// PWM channel for LED
const int pwmChannel = 0;
const int pwmFreq = 5000;  // PWM frequency
const int pwmResolution = 8;  // 8-bit resolution (0-255)

void setup() {
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(SOIL_MOISTURE_SENSOR, INPUT);
  pinMode(LIGHT_SENSOR_PIN, INPUT);

  // Set relay off initially
  digitalWrite(RELAY_PIN, LOW);

  // Configure PWM channel for the LED pin
  ledcSetup(pwmChannel, pwmFreq, pwmResolution);
  ledcAttachPin(LED_PIN, pwmChannel);
}

void loop() {
  // Reading soil moisture sensor value
  int soilMoistureLevel = analogRead(SOIL_MOISTURE_SENSOR);

  // Reading ambient light sensor value
  int lightLevel = analogRead(LIGHT_SENSOR_PIN);

  Serial.println("Soil Moisture Level: ");
  Serial.println(soilMoistureLevel);

  Serial.println("Light Level: ");
  Serial.println(lightLevel);

  // Control water pump based on soil moisture
  if (soilMoistureLevel > drySoil) {
    // If soil moisture is above the dry threshold, turn on water pump
    digitalWrite(RELAY_PIN, HIGH);
  } else if (soilMoistureLevel < wetSoil) {
    // If soil moisture is below the wet threshold, turn off water pump
    digitalWrite(RELAY_PIN, LOW);
  }

  // Adjust LED brightness based on light level
  if (lightLevel <= lowLight) {
    // If light is lower than the required level, increase LED brightness
    ledcWrite(pwmChannel, 255);  // Full brightness
  } else if (lightLevel >= highLight) {
    // If light is higher than the required level, decrease LED brightness
    ledcWrite(pwmChannel, 25);  // Very dim light
  } else {
    // Map light level to LED brightness
    int ledBrightness = map(lightLevel, lowLight, highLight, 255, 25);
    ledcWrite(pwmChannel, ledBrightness);
  }

  delay(1000);  // Wait for 1 second
}
