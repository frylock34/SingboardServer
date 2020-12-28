#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
#include <WidgetRTC.h>
#include <secrets.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = SSID;
char pass[] = PASS;

BlynkTimer timer;

WidgetRTC rtc;

void clockDisplay()
{
  // You can call hour(), minute(), ... at any time
  // Please see Time library examples for details

  String currentTime = String(hour()) + ":" + minute() + ":" + second();
  String currentDate = String(day()) + " " + month() + " " + year();
  Serial.print("Current time: ");
  Serial.print(currentTime);
  Serial.print(" ");
  Serial.print(currentDate);
  Serial.println();

  // Send time to the App
  Blynk.virtualWrite(V1, currentTime);
  // Send date to the App
  Blynk.virtualWrite(V2, currentDate);
}

BLYNK_CONNECTED()
{
  // Synchronize time on connection
  rtc.begin();
}

BLYNK_WRITE(V0) // zeRGBa assigned to V0
{
  // get a RED channel value
  int r = param[0].asInt();
  // get a GREEN channel value
  int g = param[1].asInt();
  // get a BLUE channel value
  int b = param[2].asInt();

  Serial.print("Color: ");
  Serial.print(r);
  Serial.print(" ,");
  Serial.print(g);
  Serial.print(" ,");
  Serial.print(b);
  Serial.println();
}

void setup()
{
  // Debug console
  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass);

  setSyncInterval(10 * 60); // Sync interval in seconds (10 minutes)

  // Display digital clock every 10 seconds
  timer.setInterval(10000L, clockDisplay);
}

void loop()
{
  Blynk.run();
  timer.run();
}
