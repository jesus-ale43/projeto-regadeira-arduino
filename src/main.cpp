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

const unsigned long WateringDuration = 5000;

bool watering = false;
unsigned long wateringStartTime = 0;

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

    unsigned long currentTime = millis();

    if (watering) {

        if (currentTime - wateringStartTime >= WateringDuration) {

            digitalWrite(MotorPin, LOW);
            digitalWrite(MotorLedPin, LOW);

            watering = false;

            Serial.println("Motor: DESLIGADO");
            Serial.println("Regada concluida. Aguardando proxima verificacao.");
            Serial.println("-----------------------------");
        }

        return;
    }

    int batteryValue = analogRead(BatteryPotenciometerPin);

    int batteryLevel = map(
        batteryValue,
        0,
        1023,
        0,
        100
    );

    batteryLevel = constrain(batteryLevel, 0, 100);

    if (batteryLevel >= 70) {

        digitalWrite(HighBatteryLedPin, HIGH);
        digitalWrite(MediumBatteryLedPin, LOW);
        digitalWrite(LowBatteryLedPin, LOW);

    }
    else if (batteryLevel >= 30) {

        digitalWrite(HighBatteryLedPin, LOW);
        digitalWrite(MediumBatteryLedPin, HIGH);
        digitalWrite(LowBatteryLedPin, LOW);

    }
    else {

        digitalWrite(HighBatteryLedPin, LOW);
        digitalWrite(MediumBatteryLedPin, LOW);
        digitalWrite(LowBatteryLedPin, HIGH);
    }

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

    bool soilIsDry = humidityLevel < maxHumidity;

    if (soilIsDry) {
        digitalWrite(LowHumidityLedPin, HIGH);
    }
    else {
        digitalWrite(LowHumidityLedPin, LOW);
    }

    bool enoughWater = reservatoryLevel > 0;
    bool enoughBattery = batteryLevel > 0;

    bool motorOn = soilIsDry && enoughWater && enoughBattery;

    if (motorOn) {

        digitalWrite(MotorPin, HIGH);
        digitalWrite(MotorLedPin, HIGH);

        watering = true;
        wateringStartTime = currentTime;

        Serial.println("Motor: LIGADO");
        Serial.println("Regando por 5 segundos...");

    }
    else {

        digitalWrite(MotorPin, LOW);
        digitalWrite(MotorLedPin, LOW);

        Serial.println("Motor: DESLIGADO");
    }
}
