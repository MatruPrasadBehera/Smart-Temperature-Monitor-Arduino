/*
  ===========================================================
   SMART TEMPERATURE MONITOR PRO
   Embedded Systems Task-1  |  Maincrafts Technology
   Board   : Arduino Uno
   Sensor  : DS18B20 (1-Wire Digital Temperature Sensor)
  ===========================================================
*/

#include <OneWire.h>            // 1-Wire communication protocol
#include <DallasTemperature.h>  // Driver for the DS18B20 sensor

#define ONE_WIRE_BUS 2   // DS18B20 data (DQ) pin -> Arduino D2

#define GREEN_LED  5     // SAFE status indicator
#define YELLOW_LED 6     // WARNING status indicator
#define RED_LED    7     // DANGER status indicator
#define BUZZER     8     // Active buzzer for the DANGER alarm

OneWire oneWire(ONE_WIRE_BUS);         // 1-Wire bus on pin D2
DallasTemperature sensors(&oneWire);   // DS18B20 sensor on that bus

float currentTemp;            // Latest temperature reading (deg C)
float previousTemp = -100;    // Previous reading, used for trend; -100 = "no reading yet"
float maxTemp = -100;         // Highest temperature recorded since power-on

void setup() {
  Serial.begin(9600);      // Start Serial Monitor communication
  sensors.begin();         // Initialize the DS18B20 sensor

  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  Serial.println("=================================");
  Serial.println(" SMART TEMPERATURE MONITOR PRO");
  Serial.println("=================================");
}

void loop() {
  sensors.requestTemperatures();             // Ask the sensor for a fresh reading
  currentTemp = sensors.getTempCByIndex(0);  // Read temperature in Celsius

  // Handle a disconnected or faulty sensor gracefully
  if (currentTemp == DEVICE_DISCONNECTED_C) {
    Serial.println("Sensor Error!");
    delay(1000);
    return;   // Skip this cycle and try again
  }

  // Track the highest temperature seen so far
  if (currentTemp > maxTemp)
    maxTemp = currentTemp;

  Serial.print("Current Temperature : ");
  Serial.print(currentTemp);
  Serial.println(" C");

  Serial.print("Maximum Temperature : ");
  Serial.print(maxTemp);
  Serial.println(" C");

  // Determine trend by comparing with the previous reading
  // (skipped on the very first loop, since there is no previous reading yet)
  if (previousTemp != -100) {
    if (currentTemp > previousTemp)
      Serial.println("Trend : Rising");
    else if (currentTemp < previousTemp)
      Serial.println("Trend : Falling");
    else
      Serial.println("Trend : Stable");
  }

  // Three-tier status logic: SAFE -> WARNING -> DANGER
  if (currentTemp < 25) {
    Serial.println("Status : SAFE");
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, LOW);
    noTone(BUZZER);
  }
  else if (currentTemp < 35) {
    Serial.println("Status : WARNING");
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    noTone(BUZZER);
  }
  else {
    Serial.println("Status : DANGER");
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    tone(BUZZER, 1000);   // Sound the buzzer at 1000 Hz
  }

  previousTemp = currentTemp;   // Save this reading for next loop's trend check
  Serial.println("----------------------------");
  delay(1000);   // Wait 1 second before the next reading
}
