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

const unsigned long SensorReadInterval = 120000;
const unsigned long WateringDuration = 10000;
const unsigned long PostWateringCheckDelay = 20000;

unsigned long lastSensorRead = 0;

bool watering = false;
unsigned long wateringStartTime = 0;
bool postWateringCheckPending = false;

void updateBatteryLeds()
{
    int batteryValue = analogRead(BatteryPotenciometerPin);

    int batteryLevel = map(
        batteryValue,
        0,
        1023,
        0,
        100);

    batteryLevel = constrain(batteryLevel, 0, 100);

    if (batteryLevel >= 70)
    {
        digitalWrite(HighBatteryLedPin, HIGH);
        digitalWrite(MediumBatteryLedPin, LOW);
        digitalWrite(LowBatteryLedPin, LOW);
    }
    else if (batteryLevel >= 30)
    {
        digitalWrite(HighBatteryLedPin, LOW);
        digitalWrite(MediumBatteryLedPin, HIGH);
        digitalWrite(LowBatteryLedPin, LOW);
    }
    else
    {
        digitalWrite(HighBatteryLedPin, LOW);
        digitalWrite(MediumBatteryLedPin, LOW);
        digitalWrite(LowBatteryLedPin, HIGH);
    }
}

void setup()
{
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

void loop()
{

    unsigned long currentTime = millis();

    // Atualiza os LEDs da bateria mesmo durante os intervalos de espera.
    updateBatteryLeds();

    // --------------------------------------------------
    // Se o motor estiver regando, verifica se já passaram
    // 10 segundos.
    // --------------------------------------------------

    if (watering)
    {

        if (currentTime - wateringStartTime >= WateringDuration)
        {

            digitalWrite(MotorPin, LOW);
            digitalWrite(MotorLedPin, LOW);

            watering = false;
            postWateringCheckPending = true;
            lastSensorRead = currentTime - SensorReadInterval + PostWateringCheckDelay;

            Serial.println("Motor: DESLIGADO");
            Serial.println("Regada concluida. Nova verificacao em 20 segundos.");
            Serial.println("-----------------------------");
        }

        return;
    }

    // --------------------------------------------------
    // Checagem normal ou verificacao apos a rega
    // --------------------------------------------------

    if (currentTime - lastSensorRead >= SensorReadInterval)
    {

        lastSensorRead = currentTime;

        bool isPostWateringCheck = postWateringCheckPending;
        postWateringCheckPending = false;

        if (isPostWateringCheck)
        {
            Serial.println("Verificacao apos rega iniciada.");
        }

        // -------------------------
        // Bateria
        // -------------------------

        int batteryValue = analogRead(BatteryPotenciometerPin);

        int batteryLevel = map(
            batteryValue,
            0,
            1023,
            0,
            100);

        batteryLevel = constrain(batteryLevel, 0, 100);

        if (batteryLevel >= 70)
        {

            digitalWrite(HighBatteryLedPin, HIGH);
            digitalWrite(MediumBatteryLedPin, LOW);
            digitalWrite(LowBatteryLedPin, LOW);
        }
        else if (batteryLevel >= 30)
        {

            digitalWrite(HighBatteryLedPin, LOW);
            digitalWrite(MediumBatteryLedPin, HIGH);
            digitalWrite(LowBatteryLedPin, LOW);
        }
        else
        {

            digitalWrite(HighBatteryLedPin, LOW);
            digitalWrite(MediumBatteryLedPin, LOW);
            digitalWrite(LowBatteryLedPin, HIGH);
        }

        // -------------------------
        // Umidade do solo
        // -------------------------

        int humiditySensorValue = analogRead(HumiditySensorPin);

        int humidityLevel = map(
            humiditySensorValue,
            0,
            1023,
            0,
            100);

        humidityLevel = constrain(humidityLevel, 0, 100);

        // -------------------------
        // Umidade desejada
        // -------------------------

        int maxHumidityValue = analogRead(MaxHumidityPotenciometerPin);

        int maxHumidity = map(
            maxHumidityValue,
            0,
            1023,
            0,
            100);

        maxHumidity = constrain(maxHumidity, 0, 100);

        // -------------------------
        // Reservatório
        // -------------------------

        int reservatoryValue = analogRead(ReservatoryPotenciometerPin);

        int reservatoryLevel = map(
            reservatoryValue,
            0,
            1023,
            0,
            100);

        reservatoryLevel = constrain(reservatoryLevel, 0, 100);

        // -------------------------
        // Verifica se o solo está seco
        // -------------------------

        bool soilIsDry = humidityLevel < maxHumidity;

        if (soilIsDry)
        {
            digitalWrite(LowHumidityLedPin, HIGH);
        }
        else
        {
            digitalWrite(LowHumidityLedPin, LOW);
        }

        // -------------------------
        // Verifica água e bateria
        // -------------------------

        bool enoughWater = reservatoryLevel > 0;
        bool enoughBattery = batteryLevel > 0;

        // -------------------------
        // Decide se deve regar
        // -------------------------

        bool motorOn = soilIsDry && enoughWater && enoughBattery;

        if (motorOn)
        {

            digitalWrite(MotorPin, HIGH);
            digitalWrite(MotorLedPin, HIGH);

            watering = true;
            wateringStartTime = currentTime;

            if (isPostWateringCheck)
            {
                Serial.println("Umidade ainda baixa. Repetindo ciclo de rega.");
            }

            Serial.println("Motor: LIGADO");
            Serial.println("Regando por 10 segundos...");
        }
        else
        {

            digitalWrite(MotorPin, LOW);
            digitalWrite(MotorLedPin, LOW);

            Serial.println("Motor: DESLIGADO");

            if (isPostWateringCheck)
            {
                if (!soilIsDry)
                {
                    Serial.println("Umidade desejada atingida. Aguardando 2 minutos para a proxima verificacao.");
                }
                else if (!enoughWater)
                {
                    Serial.println("Regada interrompida: reservatorio sem agua suficiente.");
                }
                else if (!enoughBattery)
                {
                    Serial.println("Regada interrompida: bateria sem carga suficiente.");
                }
            }
        }

        // -------------------------
        // Serial
        // -------------------------

        Serial.println("-----------------------------");

        Serial.print("Bateria: ");
        Serial.print(batteryLevel);
        Serial.println("%");

        Serial.print("Umidade do solo: ");
        Serial.print(humidityLevel);
        Serial.println("%");

        Serial.print("Umidade maxima: ");
        Serial.print(maxHumidity);
        Serial.println("%");

        Serial.print("Reservatorio: ");
        Serial.print(reservatoryLevel);
        Serial.println("%");

        Serial.print("Solo seco: ");
        Serial.println(soilIsDry ? "SIM" : "NAO");

        Serial.print("Motor: ");
        Serial.println(motorOn ? "LIGADO" : "DESLIGADO");

        Serial.println("-----------------------------");
    }
}
