#define BLYNK_PRINT Serial // Comment this out to disable prints and save space
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SimpleTimer.h>
#include <PID_v1.h>
char auth[] = "blynkauth";

// ****** DS18B20 and Temperature settings ******
#define ONE_WIRE_BUS D2                  // DS18B20 pin to D2 (4)
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20_Sensors(&oneWire);

// ****** HLT PID settings ******
double hltSetpoint, hltInput, hltOutput;
double hltKp=2, hltKi=5, hltKd=1;
PID hltPID(&hltInput, &hltOutput, &hltSetpoint, hltKp, hltKi, hltKd, DIRECT);
int WindowSize = 5000;
unsigned long windowStartTime;
SimpleTimer timer;
BLYNK_WRITE(V0)
{
    hltSetpoint=param.asInt();
}

void setup()
{
    Serial.begin(115200);
    Blynk.begin(auth, "ssid", "pwd");
    windowStartTime = millis();
    hltSetpoint = 50;
    hltPID.SetOutputLimits(0, WindowSize);
    hltPID.SetMode(AUTOMATIC);
    timer.setInterval(3000, readTemp);
    //timer.setInterval(1000, hltPid);
    pinMode(D6, OUTPUT);
}
void readTemp()
{
    DS18B20_Sensors.requestTemperatures();
    delay(100);
    float hltTemp;
    hltInput = DS18B20_Sensors.getTempCByIndex(0);
    Blynk.virtualWrite(1,hltTemp);
    Blynk.virtualWrite(2,hltSetpoint);
    Blynk.virtualWrite(3,hltOutput);
}

void loop()
{
    Blynk.run();
    timer.run();
    hltPID.Compute();
     /************************************************
     * turn the output pin on/off based on pid output
     ************************************************/
    if (millis() - windowStartTime > WindowSize)
    { //time to shift the Relay Window
        windowStartTime += WindowSize;
    }
    if (hltOutput < millis() - windowStartTime) digitalWrite(D6, HIGH);
    else digitalWrite(D6, LOW);
}
