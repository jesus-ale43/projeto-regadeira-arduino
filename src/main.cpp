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

    pinMode(MotorLedPin, OUTPUT);
    pinMode(LowHumidityLedPin, OUTPUT);

    pinMode(HighBatteryLedPin, OUTPUT);
    pinMode(MediumBatteryLedPin, OUTPUT);
    pinMode(LowBatteryLedPin, OUTPUT);

    pinMode(MotorPin, OUTPUT);

    digitalWrite(MotorLedPin, LOW);
    digitalWrite(LowHumidityLedPin, LOW);

    digitalWrite(HighBatteryLedPin, LOW);
    digitalWrite(MediumBatteryLedPin, LOW);
    digitalWrite(LowBatteryLedPin, LOW);

    digitalWrite(MotorPin, LOW);
}

void loop() {
    int humiditySensorValue = analogRead(HumiditySensorPin);

    int humidityLevel = map(
        humiditySensorValue,
        0,
        1023,
        0,
        100
    );

    humidityLevel = constrain(humidityLevel, 0, 100);

    int maxHumidityValue = analogRead(MaxHumidityPotenciometerPin);

    int maxHumidity = map(
        maxHumidityValue,
        0,
        1023,
        0,
        100
    );

    maxHumidity = constrain(maxHumidity, 0, 100);

    int reservatoryValue = analogRead(ReservatoryPotenciometerPin);

    int reservatoryLevel = map(
        reservatoryValue,
        0,
        1023,
        0,
        100
    );

    reservatoryLevel = constrain(reservatoryLevel, 0, 100);
}
