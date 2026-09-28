#include <Arduino.h>

const int MotorLedPin = 3;
const int LowHumidityLedPin = 4;

const int HighBatteryLedPin = 13;
const int MediumBatteryLedPin = 12;
const int LowBatteryLedPin = 11;

const int MaxHumidityPotenciometerPin = A3;
const int ReservatoryPotenciometerPin = A2;
const int BatteryPotenciometerPin = A1;

const int HumiditySensorPin = A0;

const int MotorPin = 2;

void setup() {
    Serial.begin(9600);
}

void loop() {
}
