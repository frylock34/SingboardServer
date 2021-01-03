#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
#include <WidgetRTC.h>
#include <secrets.h>
#include <definitions.h>
#include <ArduinoOTA.h>
#include <FastLED.h>
#include <animations.h>

#define LED_COUNT 12 * 32
#define LED_PIN 3
CRGB leds[LED_COUNT];

uint16_t frame = 0;
#define ANIMATE_SPEED 100;

// nighttime dimming constants
// brightness based on time of day- could try warmer colors at night?
#define DAYBRIGHTNESS 64
#define NIGHTBRIGHTNESS 20

// cutoff times for day / night brightness. feel free to modify.
#define MORNINGCUTOFF 7 // when does daybrightness begin?   7am
#define NIGHTCUTOFF 22  // when does nightbrightness begin? 10pm

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
  int hours = hour();
  int minutes = minute();
  int seconds = second();

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
    //set brightness
    if (seconds == 0)
    {
      if (hours < MORNINGCUTOFF || hours >= NIGHTCUTOFF)
      {
        FastLED.setBrightness(NIGHTBRIGHTNESS);
      }
      else
      {
        FastLED.setBrightness(DAYBRIGHTNESS);
      }
    }

    hours = hours % 12;
    byte pixelColorRed, pixelColorGreen, pixelColorBlue;

    for (uint8_t i = 0; i < LED_COUNT; i++)
    {

      if (i <= seconds)
      {
        // calculates a faded arc from low to maximum brightness
        pixelColorBlue = (i + 1) * (255 / (seconds + 1));
        //pixelColorBlue = 255;
      }
      else
      {
        pixelColorBlue = 0;
      }

      if (i <= minutes)
      {
        pixelColorGreen = (i + 1) * (255 / (minutes + 1));
        //pixelColorGreen = 255;
      }
      else
      {
        pixelColorGreen = 0;
      }

      if (i <= hours)
      {
        pixelColorRed = (i + 1) * (255 / (hours + 1));
        //pixelColorRed = 255;
      }
      else
      {
        pixelColorRed = 0;
      }

      leds[i % LED_COUNT] = CRGB(pixelColorRed, pixelColorGreen, pixelColorBlue);
    }

    //display
    FastLED.show();
  }
}

void handleAnimation()
{
  if (mode != MODE_ANIMATION)
    return;

  switch (animation)
  {
  case 1:
    RingPair(leds, frame);
    break;
  case 2:
    DoubleChaser(leds, frame);
    break;
  case 3:
    TripleBounce(leds, frame);
    break;
  case 4:
    WaveInt(leds, frame, 180);
    break;
  case 5:
    Wave(leds, frame, 180);
    break;
  case 6:                         //Blue spark (Slow)
    Spark(leds, frame, 255, 188); //Overloaded version of "Spark" with Hue value, 255 for fade is the slowest fade possible. 256 is on/off
    delay(2);                     //Slow things down a bit more for Slow Spark
    break;
  case 7:                         //Blue spark (fast)
    Spark(leds, frame, 246, 188); //Overloaded version of "Spark" with Hue value, 246 fade is faster which makes for a sharper dropoff
    break;
  case 8:                    //White spark (Slow)
    Spark(leds, frame, 255); //"Spark" function without hue make a white spark, 255 for fade is the slowest fade possible.
    delay(2);                //Slow things down a bit more for Slow Spark
    break;
  case 9: //White spark (fast)			//"Spark" function without hue make a white spark, 246 fade is faster which makes for a sharper dropoff
    Spark(leds, frame, 245);
    break;
  case 10:
    RainbowSpark(leds, frame, 240); //240 for dropoff is a pretty sharp fade, good for this animation
    break;
  default:
    delay(100); //Animation OFF
  }

  FastLED.show(); //All animations are applied!..send the results to the strip(s)
  frame += ANIMATE_SPEED;
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
  int animValue = param.asInt();
  if (animValue > 10)
  {
    animValue = 10; //we have only 10 animations
  }
  animation = animValue;
}

void setup()
{
  // Debug console
  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass);

  FastLED.addLeds<WS2811, LED_PIN>(leds, LED_COUNT);
  FastLED.setBrightness(DAYBRIGHTNESS);
  FastLED.clear();

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
  handleAnimation();
}
