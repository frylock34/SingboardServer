#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
#include <WidgetRTC.h>
#include <secrets.h>
#include <definitions.h>
#include <ArduinoOTA.h>
#include <FastLED.h>

#define LED_COUNT 16 * 24
#define LED_PIN 3
CRGB leds[LED_COUNT];

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = SSID;
char pass[] = PASS;

BlynkTimer timer;

WidgetRTC rtc;

int mode = MODE_CLOCK;
int animation = ANIMATION_FADE;

int r = 255;
int g = 255;
int b = 255;

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

  if (mode == MODE_CLOCK)
  {
    //set clock
  }
}

BLYNK_CONNECTED()
{
  // Synchronize time on connection
  rtc.begin();
}

BLYNK_WRITE(V0) // zeRGBa assigned to V0
{
  // get a RED channel value
  r = param[0].asInt();
  // get a GREEN channel value
  g = param[1].asInt();
  // get a BLUE channel value
  b = param[2].asInt();

  Serial.print("Color: ");
  Serial.print(r);
  Serial.print(" ,");
  Serial.print(g);
  Serial.print(" ,");
  Serial.print(b);
  Serial.println();

  if (mode == MODE_CLOCK)
  {
    //set clock color
  }
}

BLYNK_WRITE(V3) //mode reading
{
  switch (param.asInt())
  {
  case 1:
  { // Item 1
    Serial.println("Clock mode enabled");
    mode = 1;
    break;
  }
  case 2:
  { // Item 2
    Serial.println("Animation enabled");
    mode = 2;
    break;
  }
  }
}

BLYNK_WRITE(V4)
{

  switch (param.asInt())
  {
  case 1:
  { // Item 1
    Serial.println("Fade animation");
    animation = 1;
    break;
  }
  case 2:
  { // Item 2
    Serial.println("Flash animation");
    animation = 2;
    break;
  }
  }
  if (mode == MODE_ANIMATION)
  {
    //set animation here
  }
}

void setup()
{
  // Debug console
  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass);

  FastLED.addLeds<WS2811, LED_PIN>(leds, LED_COUNT);

  // Port defaults to 8266
  ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  ArduinoOTA.setPassword("admin");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
    {
      type = "sketch";
    }
    else
    { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
    {
      Serial.println("Auth Failed");
    }
    else if (error == OTA_BEGIN_ERROR)
    {
      Serial.println("Begin Failed");
    }
    else if (error == OTA_CONNECT_ERROR)
    {
      Serial.println("Connect Failed");
    }
    else if (error == OTA_RECEIVE_ERROR)
    {
      Serial.println("Receive Failed");
    }
    else if (error == OTA_END_ERROR)
    {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();

  setSyncInterval(10 * 60); // Sync interval in seconds (10 minutes)

  // Display digital clock every 10 seconds
  timer.setInterval(10000L, clockDisplay);
}

void loop()
{
  Blynk.run();
  timer.run();
  ArduinoOTA.handle();
}
