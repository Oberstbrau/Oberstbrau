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
SimpleTimer timer;

// ****** General PID settings ******
int WindowSize = 5000;
unsigned long windowStartTime;

// ****** HLT PID settings ******
int hltSsr;
double hltSetpoint, hltInput, hltOutput;
double hltKp = 150, hltKi = 15, hltKd = 10;
PID hltPID(&hltInput, &hltOutput, &hltSetpoint, hltKp, hltKi, hltKd, DIRECT);
BLYNK_WRITE(V0)
{
  hltSetpoint = param.asInt();
}

// ****** HERMS PID settings ******
int hermsSsr;
double hermsSetpoint, hermsInput, hermsOutput;
double hermsKp = 150, hermsKi = 15, hermsKd = 10;
PID hermsPID(&hermsInput, &hermsOutput, &hermsSetpoint, hermsKp, hermsKi, hermsKd, DIRECT);
BLYNK_WRITE(V1)
{
  hermsSetpoint = param.asInt();
}

void setup()
{
  Serial.begin(115200);
  Blynk.begin(auth, "ssid", "pwd");
  windowStartTime = millis();
  hltSetpoint = 0;
  hermsSetpoint = 0;
  hltPID.SetOutputLimits(0, WindowSize);
  hltPID.SetMode(AUTOMATIC);
  hermsPID.SetOutputLimits(0, WindowSize);
  hermsPID.SetMode(AUTOMATIC);
  timer.setInterval(3000, readTemp);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
}
void readTemp()
{
  hltInput = DS18B20_Sensors.getTempCByIndex(0);
  hermsInput = DS18B20_Sensors.getTempCByIndex(1);
  hltPID.Compute();
  hermsPID.Compute();
  Blynk.virtualWrite(4, hltInput);      // HLT Temp display
  Blynk.virtualWrite(6, hermsInput);    // HERMS Temp display
  DS18B20_Sensors.requestTemperatures();// Request temp readings for next time
}

void pidPins()
{
  // Turn the output pins on/off based on pid output
  if (millis() - windowStartTime > WindowSize)
  { //time to shift the Relay Window
    windowStartTime += WindowSize;
  }
  if (hltOutput < millis() - windowStartTime)
  {
    if (hltSsr == LOW)
    {
      digitalWrite(D6, HIGH);
      Blynk.virtualWrite(5, 0);
      hltSsr=HIGH;
    }
  }
  else
  {
    if (hltSsr == HIGH)
    {
      digitalWrite(D6, LOW);
      Blynk.virtualWrite(5, 1023);
      hltSsr=LOW;
    }
  }
  if (hermsOutput < millis() - windowStartTime) 
  {
    if (hermsSsr == LOW)
    {
      digitalWrite(D7, HIGH);
      Blynk.virtualWrite(7, 0);
      hermsSsr=HIGH;
    }
  }
  else 
  {
    if (hermsSsr == HIGH)
    {
      digitalWrite(D7, LOW);
      Blynk.virtualWrite(7, 1023);
      hermsSsr=LOW;
    }
  }
}

void loop()
{
  Blynk.run();
  timer.run();
  pidPins();
}
