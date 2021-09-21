// https://www.tinkercad.com/things/31rp6czbZtc-dynamic-traffic-lights-4-way/editel
#include <Arduino.h>

#if defined(ESP32)
#include <WiFi.h>
#include <FirebaseESP32.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#endif

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
// #include <SPI.h>

#include "RTClib.h"

///////////////////////////
//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

///////////////////////////
// Your WiFi credentials
#define WIFI_SSID "sugarDady"
#define WIFI_PASSWORD "spasiAja"

///////////////////////////
#define FIREBASE_HOST "fir-cloudmessage-bb89a.firebaseio.com"
#define FIREBASE_AUTH "0rK6akg5dvL1bYsG47iLj0MexMHVbR0x0BIEqnDk"

// orange light wait time
int orangeDelay = 2;
// time that both directions are red, before green starts
int redDelay = 500;

int greenDelay = 0;

int green1Delay = 5;
int green2Delay = 5;
int green3Delay = 5;
int green4Delay = 5;

int cd = 0;
// name pins for lights
//{Red, Orange, Green};
int signal1[] = {13, 12, 14};
int signal2[] = {33, 32, 26};
int signal3[] = {23, 19, 5};
int signal4[] = {2, 4, 15};

// other mode pin
// int r1Pin = 13;
// int o1Pin = 12;
// int g1Pin = 14;

// int r2Pin = 33;
// int o2Pin = 32;
// int g2Pin = 26;

// int r3Pin = 23;
// int o3Pin = 19;
// int g3Pin = 5;

// int r4Pin = 2;
// int o4Pin = 4;
// int g4Pin = 15;

// set initial times for lights
unsigned long greenMillis = 0;
unsigned long orangeMillis = 0;
unsigned long redMillis = 0;
// set current direction
int currentDirection = 1;
// set the starting status - we set it green
bool greenStatus = true;
bool orangeStatus = false;
bool redStatus = false;

///////////////////////////
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define CLK_PIN 18  // 18 or 25
#define DATA_PIN 27 // 16 or 27
#define CS_PIN 25   // 17 or 26
#define MAX_DEVICES 4

MD_Parola DotMatrix = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
// MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

uint8_t scrollSpeed = 20; // default frame delay value
textEffect_t scrollEffect = PA_SCROLL_LEFT;
textPosition_t scrollAlign = PA_LEFT;
uint16_t scrollPause = 1000; // in milliseconds

String msg, message, text = "Hati-Hati";

String status = "0";

uint8_t curString = 0;

// Sprite Definition
const uint8_t F_ROCKET = 2;
const uint8_t W_ROCKET = 11;
const uint8_t PROGMEM rocket[F_ROCKET * W_ROCKET] = // rocket
    {
        0x18,
        0x24,
        0x42,
        0x81,
        0x99,
        0x18,
        0x99,
        0x18,
        0xa5,
        0x5a,
        0x81,
        0x18,
        0x24,
        0x42,
        0x81,
        0x18,
        0x99,
        0x18,
        0x99,
        0x24,
        0x42,
        0x99,
};

const uint8_t F_PMAN1 = 6;
const uint8_t W_PMAN1 = 8;
const uint8_t PROGMEM pacman1[F_PMAN1 * W_PMAN1] = // gobbling pacman animation
    {
        0x00,
        0x81,
        0xc3,
        0xe7,
        0xff,
        0x7e,
        0x7e,
        0x3c,
        0x00,
        0x42,
        0xe7,
        0xe7,
        0xff,
        0xff,
        0x7e,
        0x3c,
        0x24,
        0x66,
        0xe7,
        0xff,
        0xff,
        0xff,
        0x7e,
        0x3c,
        0x3c,
        0x7e,
        0xff,
        0xff,
        0xff,
        0xff,
        0x7e,
        0x3c,
        0x24,
        0x66,
        0xe7,
        0xff,
        0xff,
        0xff,
        0x7e,
        0x3c,
        0x00,
        0x42,
        0xe7,
        0xe7,
        0xff,
        0xff,
        0x7e,
        0x3c,
};

const uint8_t F_PMAN2 = 6;
const uint8_t W_PMAN2 = 18;
const uint8_t PROGMEM pacman2[F_PMAN2 * W_PMAN2] = // ghost pursued by a pacman
    {
        0x00,
        0x81,
        0xc3,
        0xe7,
        0xff,
        0x7e,
        0x7e,
        0x3c,
        0x00,
        0x00,
        0x00,
        0xfe,
        0x7b,
        0xf3,
        0x7f,
        0xfb,
        0x73,
        0xfe,
        0x00,
        0x42,
        0xe7,
        0xe7,
        0xff,
        0xff,
        0x7e,
        0x3c,
        0x00,
        0x00,
        0x00,
        0xfe,
        0x7b,
        0xf3,
        0x7f,
        0xfb,
        0x73,
        0xfe,
        0x24,
        0x66,
        0xe7,
        0xff,
        0xff,
        0xff,
        0x7e,
        0x3c,
        0x00,
        0x00,
        0x00,
        0xfe,
        0x7b,
        0xf3,
        0x7f,
        0xfb,
        0x73,
        0xfe,
        0x3c,
        0x7e,
        0xff,
        0xff,
        0xff,
        0xff,
        0x7e,
        0x3c,
        0x00,
        0x00,
        0x00,
        0xfe,
        0x7b,
        0xf3,
        0x7f,
        0xfb,
        0x73,
        0xfe,
        0x24,
        0x66,
        0xe7,
        0xff,
        0xff,
        0xff,
        0x7e,
        0x3c,
        0x00,
        0x00,
        0x00,
        0xfe,
        0x7b,
        0xf3,
        0x7f,
        0xfb,
        0x73,
        0xfe,
        0x00,
        0x42,
        0xe7,
        0xe7,
        0xff,
        0xff,
        0x7e,
        0x3c,
        0x00,
        0x00,
        0x00,
        0xfe,
        0x7b,
        0xf3,
        0x7f,
        0xfb,
        0x73,
        0xfe,
};
///////////////////////////
RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Ahad", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu"};

///////////////////////////
// Firebase Realtime Database Object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
String databasePath = "";
String textTemp = "";
String statusTemp = "";

// Stores the elapsed time from device start up
unsigned long elapsedMillis = 0;
// The frequency of sensor updates to firebase, set to 10seconds
unsigned long update_interval = 5000;
// Dummy counter to test initial firebase updates
int count = 0;
// Store device authentication status

void Wifi_Init()
{
  pinMode(2, OUTPUT);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    // Serial.print(".");
    digitalWrite(2, HIGH);
    delay(300);
    digitalWrite(2, LOW);
    delay(300);
  }
  Serial.println();
  digitalWrite(2, LOW);
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}

void firebase_init()
{
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;

  Firebase.reconnectWiFi(true);
  Firebase.begin(&config, &auth);

  // untuk mereset pengaturan ke semula alat
  Firebase.setInt(fbdo, "/monitoring/status", 1);
  delay(250);
}

void matrix_init()
{
  DotMatrix.begin();
  DotMatrix.setIntensity(0); //Intensity 0-15

  DotMatrix.displayText("Stanby", PA_CENTER, scrollSpeed, scrollPause, PA_SCROLL_DOWN, PA_SCROLL_DOWN);

  // DotMatrix.setTextEffect(PA_SCROLL_DOWN, PA_SCROLL_DOWN);
  // DotMatrix.getSpeedIn(10);
  // DotMatrix.setPause(1000);
}

void rtc_init()
{
#ifndef ESP8266
  while (!Serial)
    ; // wait for serial port to connect. Needed for native USB
#endif

  if (!rtc.begin())
  {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

  if (rtc.lostPower())
  {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void trafficLight_init()
{
  // initialise pins for leds as outputs
  for (int i = 0; i < 3; i++)
  {
    pinMode(signal1[i], OUTPUT);
    pinMode(signal2[i], OUTPUT);
    pinMode(signal3[i], OUTPUT);
    pinMode(signal4[i], OUTPUT);
  }
  // turn on green light direction 1
  digitalWrite(signal1[2], HIGH);
  // turn on red light direction 2
  digitalWrite(signal2[0], HIGH);
  // turn on red light direction 3
  digitalWrite(signal3[0], HIGH);
  // turn on red light direction 4
  digitalWrite(signal4[0], HIGH);

  // // initialise pins for leds as outputs
  // pinMode(r1Pin, OUTPUT);
  // pinMode(o1Pin, OUTPUT);
  // pinMode(g1Pin, OUTPUT);
  // pinMode(r2Pin, OUTPUT);
  // pinMode(o2Pin, OUTPUT);
  // pinMode(g2Pin, OUTPUT);
  // pinMode(r3Pin, OUTPUT);
  // pinMode(o3Pin, OUTPUT);
  // pinMode(g3Pin, OUTPUT);
  // pinMode(r4Pin, OUTPUT);
  // pinMode(o4Pin, OUTPUT);
  // pinMode(g4Pin, OUTPUT);

  // // turn on green light direction 1
  // digitalWrite(g1Pin, HIGH);
  // // turn on red light direction 2
  // digitalWrite(r2Pin, HIGH);
  // // turn on red light direction 3
  // digitalWrite(r3Pin, HIGH);
  // // turn on red light direction 4
  // digitalWrite(r4Pin, HIGH);
}

// start the setup
void setup()
{
  Serial.begin(115200);
  cd = green1Delay;
  greenDelay = 5;
  if (!rtc.begin())
  {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

  if (rtc.lostPower())
  {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  Wifi_Init();
  firebase_init();
  matrix_init();
  trafficLight_init();
}

// function to change to orange just needs current direction
void goOrange(int currentDirection)
{
  if (currentDirection == 1)
  {
    // Serial.print(" goOrange 1 : nyala");
    digitalWrite(signal1[2], LOW);
    digitalWrite(signal1[1], HIGH);
  }
  else if (currentDirection == 2)
  {
    // Serial.print(" goOrange 2 : nyala");
    digitalWrite(signal2[2], LOW);
    digitalWrite(signal2[1], HIGH);
  }
  else if (currentDirection == 3)
  {
    // Serial.print(" goOrange 3 : nyala");
    digitalWrite(signal3[2], LOW);
    digitalWrite(signal3[1], HIGH);
  }
  else
  {
    // Serial.print(" goOrange 4 : nyala");
    digitalWrite(signal4[2], LOW);
    digitalWrite(signal4[1], HIGH);
  }
}

// function to change to red just needs current direction
void goRed(int currentDirection)
{
  if (currentDirection == 1)
  {
    // Serial.print(" goRed 1 : nyala");
    digitalWrite(signal1[1], LOW);
    digitalWrite(signal1[0], HIGH);
  }
  else if (currentDirection == 2)
  {
    // Serial.print(" goRed 2 : nyala");
    digitalWrite(signal2[1], LOW);
    digitalWrite(signal2[0], HIGH);
  }
  else if (currentDirection == 3)
  {
    // Serial.print(" goRed 3 : nyala");
    digitalWrite(signal3[1], LOW);
    digitalWrite(signal3[0], HIGH);
  }
  else
  {
    // Serial.print(" goRed 4 : nyala");
    digitalWrite(signal4[1], LOW);
    digitalWrite(signal4[0], HIGH);
  }
}

// function to change to green just needs current direction
void goGreen(int currentDirection)
{
  if (DotMatrix.displayAnimate())
  {

    DotMatrix.setTextAlignment(PA_CENTER);
    DotMatrix.setTextEffect(PA_SCROLL_DOWN, PA_NO_EFFECT);
    // DotMatrix.getSpeedIn(100);
    // DotMatrix.setPause(1000);
    if (currentDirection == 1)
    {
      msg = "maju 2";
      // Serial.print(" goGreen 2 : nyala");
      digitalWrite(signal2[0], LOW);
      digitalWrite(signal2[2], HIGH);
    }
    else if (currentDirection == 2)
    {
      msg = "maju 3";
      // Serial.print(" goGreen 3 : nyala");
      digitalWrite(signal3[0], LOW);
      digitalWrite(signal3[2], HIGH);
    }
    else if (currentDirection == 3)
    {
      msg = "maju 4";
      // Serial.print(" goGreen 4 : nyala");
      digitalWrite(signal4[0], LOW);
      digitalWrite(signal4[2], HIGH);
    }
    else
    {
      msg = "maju 1";
      // Serial.print(" goGreen 1 : nyala");
      digitalWrite(signal1[0], LOW);
      digitalWrite(signal1[2], HIGH);
    }
    DotMatrix.setTextBuffer(msg.c_str());
    DotMatrix.displayReset();
  }
}

void codeDotMatrix()
{
  DateTime now = rtc.now();
  const uint8_t h = now.hour();
  //   DotMatrix.displayText(msg.c_str(), scrollAlign, scrollSpeed, scrollPause, PA_SPRITE, PA_SPRITE);

  if (DotMatrix.displayAnimate())
  {
    switch (curString)
    {
    case 0:
      msg = String(rtc.getTemperature()) + "c";
      DotMatrix.setTextEffect(PA_SPRITE, PA_SCROLL_DOWN);
      DotMatrix.getSpeed(10);
      curString++;
      break;
    case 1:
      msg = String((h > 12) ? h - 12 : ((h == 0) ? 12 : h), DEC) + ':' + String(now.minute(), DEC) + String((h < 12) ? ".A" : ".P");
      DotMatrix.setTextEffect(PA_SCROLL_DOWN, PA_SCROLL_DOWN);
      DotMatrix.getSpeedIn(50);
      curString++;
      break;
    case 2:
      msg = String(daysOfTheWeek[now.dayOfTheWeek()]);
      DotMatrix.setTextEffect(PA_SPRITE, PA_SCROLL_LEFT);
      DotMatrix.getSpeed(10);
      curString++;
      break;
    case 3:
      msg = String(now.day(), DEC) + "-" + String(now.month(), DEC);
      DotMatrix.setTextEffect(PA_SCROLL_LEFT, PA_SCROLL_RIGHT);
      DotMatrix.getSpeedOut(50);
      curString++;
      break;
    case 4:
      msg = String(now.year(), DEC);
      DotMatrix.setTextEffect(PA_SCROLL_RIGHT, PA_SPRITE);
      curString = 0;
      break;
    }

    DotMatrix.setTextBuffer(msg.c_str());
    DotMatrix.displayReset();
  }
}

// start the loop
void loop()
{
  unsigned long currentMillis = millis();

  if (DotMatrix.displayAnimate())
  {
    // if green time is reached and green is the current mode
    if (currentMillis - greenMillis >= (greenDelay * 1000) && greenStatus == true)
    {
      if (cd == 0)
      {
        msg = cd;
        DotMatrix.setTextAlignment(PA_CENTER);
        DotMatrix.setTextBuffer(msg.c_str());
        DotMatrix.displayReset();

        cd = greenDelay;
        // set orange millis start time to current time
        orangeMillis = millis();
        // change to orange using function
        goOrange(currentDirection);
        // delay 4 milliseconds because of millis bug
        delay(4);
        // un set the green mode
        greenStatus = false;
        // set the current mode to orange
        orangeStatus = true;
      }
      else if (cd <= 20)
      {
        msg = cd;
        DotMatrix.setTextAlignment(PA_CENTER);
        DotMatrix.setTextEffect(PA_SCROLL_DOWN, PA_SCROLL_DOWN);
        DotMatrix.setTextBuffer(msg.c_str());
        DotMatrix.displayReset();
        cd--;
      }
      else
      {
        msg = text;
        DotMatrix.setTextAlignment(PA_LEFT);
        DotMatrix.setTextEffect(PA_SCROLL_LEFT, PA_SCROLL_LEFT);
        DotMatrix.setTextBuffer(msg.c_str());
        DotMatrix.displayReset();
        cd--;
      }
    }

    // if orange time is reached and orange is the current mode
    if (currentMillis - orangeMillis >= (orangeDelay * 1000) && orangeStatus == true)
    {
      // set red millis start time to current time
      redMillis = millis();
      // change to orange using function
      goRed(currentDirection);
      // delay 4 milliseconds because of millis bug
      delay(4);
      // un set the orange mode
      orangeStatus = false;
      // set the current mode to red
      redStatus = true;
    }

    // if red is on (both ways) then check the time and change to green other direction
    if (currentMillis - redMillis >= redDelay && redStatus == true)
    {
      // set green millis start time to current time
      greenMillis = millis();
      // change to green using function
      goGreen(currentDirection);
      // delay 4 milliseconds because of millis bug
      delay(4);
      // un set the red mode
      redStatus = false;
      // unset the red mode
      greenStatus = true;

      if (currentDirection == 2)
      {
        currentDirection = 3;
        // Firebase.getInt(fbdo, "/control/green3Delay") ? String(fbdo.intData()).c_str() : fbdo.errorReason().c_str();
        // delay(100);
        // text =  Firebase.getString(fbdo, "/control/text") ? String(fbdo.stringData()).c_str() : fbdo.errorReason().c_str();
        // greenDelay = fbdo.intData();
      }
      else if (currentDirection == 3)
      {
        currentDirection = 4;
        // Firebase.getInt(fbdo, "/control/green4Delay") ? String(fbdo.intData()).c_str() : fbdo.errorReason().c_str();
        // delay(100);
        // text =  Firebase.getString(fbdo, "/control/text") ? String(fbdo.stringData()).c_str() : fbdo.errorReason().c_str();
        // greenDelay = fbdo.intData();
      }
      else if (currentDirection == 4)
      {
        currentDirection = 1;
        // Firebase.getInt(fbdo, "/control/green1Delay") ? String(fbdo.intData()).c_str() : fbdo.errorReason().c_str();
        // delay(100);
        // text =  Firebase.getString(fbdo, "/control/text") ? String(fbdo.stringData()).c_str() : fbdo.errorReason().c_str();
        // greenDelay = fbdo.intData();
      }
      else
      {
        currentDirection = 2;
        // Firebase.getInt(fbdo, "/control/green2Delay") ? String(fbdo.intData()).c_str() : fbdo.errorReason().c_str();
        // delay(100);
        // text =  Firebase.getString(fbdo, "/control/text") ? String(fbdo.stringData()).c_str() : fbdo.errorReason().c_str();
        // greenDelay = fbdo.intData();
      }
    }
  }
}
