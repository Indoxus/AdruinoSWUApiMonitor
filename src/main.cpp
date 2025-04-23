#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// put function declarations here:
int myFunction(int, int);
String parsetimeleft(int t);
String tolength(String str, int l);

// WLAN creds
const char *ssid = "";
const char *pwd = "";
String Stop = "";
String Req = "https://api.swu.de/mobility/v1/stop/passage/Departures?StopNumber=" + Stop + "&Limit=" + "3";
// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 4;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

void setup()
{
  // initialize LCD
  lcd.init();
  // turn on LCD backlight
  lcd.backlight();
  // WLAN Setup
  if (pwd == ""){WiFi.begin(ssid);}
  else{WiFi.begin(ssid,pwd);}
  int i = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    lcd.setCursor(i++, 0);
    lcd.print(".");
    delay(500);
  }
  Serial.begin(9600);
}

// put function definitions here:

String parsetimeleft(int t) //formats the Seconds till Departure
{
  String time = "";
  String z = "";
  int minutes;
  int seconds;
  z = (t<0)? "-":" ";
  t *= (t<0)? -1:1;
  minutes = t / 60; // / is an integer divide
  seconds = t % 60;
  time += (minutes < 10)?"0" + String(minutes):String(minutes);
  time += ":";
  time += (seconds<10)?"0" + String(seconds):String(seconds);
  return time;
}

String tolength(String str, int l) // shortens or fills Strings
{
  int lone = str.length();
  if (lone >= l)
  {
    return str.substring(0, l);
  }
  else
  {
    for (int i = lone; i < l; i++)
    {
      str += " ";
    }
  }
  return str;
}


HTTPClient http;
int httpResponseCode;
JsonDocument json;
void loop()
{

  if (WiFi.status() == WL_CONNECTED) //keine Ahnug was passiert wenn man aus dem WLAN rein und rausläuft
  {

    // API Req
    http.begin(Req.c_str());

    // Send HTTP GET request
    httpResponseCode = http.GET(); //currently don't undersand this library and trying to filter out "null"
    delay(500);
    if (httpResponseCode > 0)
    {
      const char *payload = http.getString().c_str();
      deserializeJson(json, payload);
      if (json == NULL){return;}
      String time = json["StopPassage"]["CurrentTimestamp"];
      if (time ==NULL){return;}
      lcd.setCursor(0, 0);
      lcd.print(time.substring(11, 19));// gets the time stamp we need
      lcd.print(" ");
      String stop = json["StopPassage"]["StopName"];
      lcd.print(stop); // Auto cuts off as the next code writes over it, i was to lazy to count
      for (int i = 0; i < 3; i++)
      {
        String stopname = json["StopPassage"]["DepartureData"][i]["DepartureDirectionText"];
        if (stopname == NULL){return;} // escapes if req is corrupted 
        lcd.setCursor(0, i + 1);
        lcd.print(tolength(stopname, 14));
        lcd.print(" ");
        int t = json["StopPassage"]["DepartureData"][i]["DepartureCountdown"];
        if (t == NULL){return;}
        lcd.print(parsetimeleft(t));
      }
    }
    else
    {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }
  else
  {
    Serial.println("WiFi Disconnected");
    lcd.clear();
    lcd.print("WiFi Disconnected");
  }
}
