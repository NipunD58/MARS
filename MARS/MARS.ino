#include "arduino_secrets.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include "thingProperties.h"

// Define the pins for the sensors and actuators
#define SOIL_MOISTURE_PIN A1
#define WATER_PUMP_PIN 7
#define BUZZER_PIN 8
#define DHT_PIN 2 
#define RAIN_SENSOR_PIN 3 // Digital pin for the HW-028 rain sensor

// Initialize the I2C LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Initialize the DHT sensor
#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);

// Thresholds
const int soilMoistureLowThreshold = 50; // Adjust this value based on your sensor calibration
const int soilMoistureHighThreshold = 80; // Adjust this value based on your sensor calibration
const float temperatureThreshold = 30.0; // Adjust this value as needed
const float humidityThreshold = 60.0; // Adjust this value as needed

bool isWaterPumpOn = false;
bool isBuzzerOn = false;
bool pumpStatus = false;

void setup() {
  // Initialize serial and wait for the `port to open
  Serial.begin(9600);
  delay(1500);

  // Initialize the LCD
  lcd.init();
  lcd.backlight();

  // Display startup message
  lcd.setCursor(0, 0);
  lcd.print("MARS");
  delay(3000); // Display for 3 seconds

  // Initialize the DHT sensor
  dht.begin();

  // Initialize the rain sensor pin as input
  pinMode(RAIN_SENSOR_PIN, INPUT);

  // Initialize the water pump and buzzer pins as outputs
  pinMode(WATER_PUMP_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Turn off the water pump and buzzer initially
  digitalWrite(WATER_PUMP_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  // Initialize cloud properties
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  // Set debug message level
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
}

void loop() {
  ArduinoCloud.update();

  // Read the soil moisture value
  int sensor_analog = analogRead(SOIL_MOISTURE_PIN);
  soilM = (100 - ((sensor_analog / 1023.0) * 100));
  Serial.print("Moisture Percentage = ");
  Serial.print(soilM);
  Serial.println("%");

  // Read the temperature and humidity values
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  // Check if any reads failed and exit early (to try again).
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Temperature: ");
  Serial.println(temperature);
  Serial.print("Humidity: ");
  Serial.println(humidity);

  // Check for rain
  rain = digitalRead(RAIN_SENSOR_PIN) == LOW; // LOW means rain is detected
  if (rain) {
    Serial.println("Rain detected!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Rain detected!");
    digitalWrite(BUZZER_PIN, HIGH); // Turn on the buzzer to alert the farmer
    delay(2000); // Keep the message and buzzer on for 2 seconds
    digitalWrite(BUZZER_PIN, LOW); // Turn off the buzzer
  }

  // Determine the overall status
  status = !(soilM < soilMoistureLowThreshold || soilM > soilMoistureHighThreshold ||
             temperature > temperatureThreshold || humidity > humidityThreshold || rain);

  Serial.print("Status: ");
  if (status) {
    Serial.println("OK");
  } else {
    Serial.println("ALERT");
  }

  // Display temperature and humidity on the LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.setCursor(6, 0);
  lcd.print(temperature);
  lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print("Hum: ");
  lcd.setCursor(6, 1);
  lcd.print(humidity);
  lcd.print("%");
  delay(2000);

  // Check the soil moisture level
  if (soilM < soilMoistureLowThreshold) {
    // Turn on the water pump
    digitalWrite(WATER_PUMP_PIN, HIGH);
    if (!isWaterPumpOn) {
      Serial.println("Water Pump: ON");
      pumpStatus = true;
      isWaterPumpOn = true;
    }
  } else if (soilM > soilMoistureHighThreshold) {
    // Turn off the water pump if soil is too wet
    digitalWrite(WATER_PUMP_PIN, LOW);
    if (isWaterPumpOn) {
      Serial.println("Water Pump: OFF");
      pumpStatus = false;
      isWaterPumpOn = false;
    }
  }

  // Check the temperature and humidity
  if (soilM < soilMoistureLowThreshold || soilM > soilMoistureHighThreshold ||
      temperature > temperatureThreshold || humidity > humidityThreshold) {
    // Turn on the buzzer to alert the farmer
    digitalWrite(BUZZER_PIN, HIGH);
    if (!isBuzzerOn) {
      Serial.println("Buzzer: ON");
      isBuzzerOn = true;
    }
  } else {
    // Turn off the buzzer
    digitalWrite(BUZZER_PIN, LOW);
    if (isBuzzerOn) {
      Serial.println("Buzzer: OFF");
      isBuzzerOn = false;
    }
  }

  // Display soil moisture and pump status on the LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("soilM: ");
  lcd.setCursor(6, 0);
  lcd.print(soilM);
  lcd.print("%");
  lcd.setCursor(0, 1);
  lcd.print("Pump: ");
  lcd.setCursor(6, 1);
  if (isWaterPumpOn) {
    lcd.print("ON");
  } else {
    lcd.print("OFF");
  }
  delay(2000);

  // Wait for a short period before the next loop
  delay(2000);
}
