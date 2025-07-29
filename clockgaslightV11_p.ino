#include <dummy.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "time.h"
#include <Adafruit_NeoPixel.h>
#include <WiFiClientSecure.h>
#include <Discord_WebHook.h>

WiFiServer server(80);
const char* ssid = ""; //WiFi name
const char* password = ""; //WiFi password

const char* website = ; //forcast website
const char* website2 = ; //current website
const char* website3 = ; //arert website

Discord_Webhook discord; //discord weblink info
String DISCORD_CHANNEL_ID = "";
String DISCORD_TOKEN = ""; 

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = ; //timezone offset
const int daylightOffset_sec = ; //daylight savings time offset

#define LED_PIN 32
#define LED_COUNT 60
Adafruit_NeoPixel pixels(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

int hourt;

float dimFactor;

int bgd1 = 0;
int bgd2 = 0;
int bgd3 = 0;

int gasr = 0;
int gasg = 0;
int gasb = 0;

double bgdDim = 1;
double mainDim = 1;

String htmlSourceCode = "";
String curTemp = "";
double curTempC;
int curTempF;

String curChill = "";
double curChillC;
int curChillF;

String curHeat = "";
double curHeatC;
int curHeatF;

String htmlSourceCode2 = "";
String temp = "";
int temps[13];
int preTempF;
int dif;

String precipitation = "";
int pop[13];
int twoPOP;

String htmlSourceCode3 = "";
String event = "";
String headline = "";
String location = "";
String allevent = "";
String oldevent = "";

void setup() {
  Serial.begin(115200);

  pixels.begin();
  pixels.show();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    pixels.setPixelColor(6, 5, 0, 0);
    pixels.show();
    delay(100);
    pixels.setPixelColor(6, 0, 0, 0);
    pixels.show();
    delay(400);
  }
  Serial.println();
  pixels.setPixelColor(6, 0, 5, 0);
  pixels.show();
  delay(100);
  pixels.setPixelColor(6, 0, 0, 0);
  pixels.show();

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  discord.begin(DISCORD_CHANNEL_ID, DISCORD_TOKEN);
  discord.addWiFi(ssid, password);
  discord.connectWiFi();
  discord.send("Clock up and running");

  currentDATA();
  futureDATA();
  alerts();
  weatherDATA();
}

void loop() {
  delay(1000);
  printLocalTime();
}

void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return;
  }
  char timeHour[3];
  strftime(timeHour, 3, "%H", &timeinfo);
  char timeMin[3];
  strftime(timeMin, 3, "%M", &timeinfo);
  char timeSec[3];
  strftime(timeSec, 3, "%S", &timeinfo);
  int hour = atoi(timeHour);
  int min = atoi(timeMin);
  int sec = atoi(timeSec);
  hourt = hour;
  if (hour > 12) {
    hour = hour - 12;
  }
  double hournew = hour;
  hournew = (hournew / 12) * 60;
  hour = (int)hournew;
  if (hour == 60) {
    hour = 0;
  }

  if(hourt < 7){
    dimFactor = 0.05;
  }else{
    dimFactor = 1;
  }

  if (hourt < 6 || hourt >= 21) {
    // Night time
    if(gasr == 100 && gasg == 0){
      pixels.fill(pixels.Color(1, 0, 0));
    }else{
      pixels.fill(pixels.Color(0, 0, 0));
    }
  } else {
    // Day time
    pixels.setPixelColor(bgd1, dimColor(gasr, gasg, gasb, dimFactor));
    pixels.setPixelColor(bgd2, dimColor(gasr, gasg, gasb, dimFactor));
    pixels.setPixelColor(bgd3, dimColor(gasr, gasg, gasb, dimFactor));

    if (dif > 2) {
      if (twoPOP > 50) {
        if (sec % 2 == 0) {
          for (int x = 0; x < 61; x += 15) {
            pixels.setPixelColor(x - 1, dimColor(127, 0, 0, dimFactor));
            pixels.fill(dimColor(127, 0, 0, dimFactor), x, 2);
          }
        } else {
          for (int x = 0; x < 61; x += 15) { 
            pixels.setPixelColor(x - 1, dimColor(63, 0, 0, dimFactor));
            pixels.fill(dimColor(63, 0, 0, dimFactor), x, 2);
          }
        }
      } else {
        for (int x = 0; x < 61; x += 15) {
          pixels.setPixelColor(x - 1, dimColor(127, 0, 0, dimFactor));
          pixels.fill(dimColor(127, 0, 0, dimFactor), x, 2);
        }
      }
    } else if (dif < -2) {
      if (twoPOP > 50) {
        if (sec % 2 == 0) {
          for (int x = 0; x < 61; x += 15) {
            pixels.setPixelColor(x - 1, dimColor(127, 107, 0, dimFactor));
            pixels.fill(dimColor(127, 107, 0, dimFactor), x, 2);
          }
        } else {
          for (int x = 0; x < 61; x += 15) {
            pixels.setPixelColor(x - 1, dimColor(63, 53, 0, dimFactor));
            pixels.fill(dimColor(63, 53, 0, dimFactor), x, 2);
          }
        }
      } else {
        for (int x = 0; x < 61; x += 15) {
          pixels.setPixelColor(x - 1, dimColor(127, 107, 0, dimFactor));
          pixels.fill(dimColor(127, 107, 0, dimFactor), x, 2);
        }
      }
    } else {
      if (twoPOP > 50) {
        if (sec % 2 == 0) {
          for (int x = 0; x < 61; x += 15) {
            pixels.setPixelColor(x - 1, dimColor(0, 0, 127, dimFactor));
            pixels.fill(dimColor(0, 0, 127, dimFactor), x, 2);
          }
        } else {
          for (int x = 0; x < 61; x += 15) {
            pixels.setPixelColor(x - 1, dimColor(0, 0, 63, dimFactor));
            pixels.fill(dimColor(0, 0, 63, dimFactor), x, 2);
          }
        }
      } else {
        for (int x = 0; x < 61; x += 15) {
          pixels.setPixelColor(x - 1, dimColor(0, 0, 127, dimFactor));
          pixels.fill(dimColor(0, 0, 127, dimFactor), x, 2);
        }
      }
    }

    pixels.setPixelColor(sec, dimColor(0, 0, 255, dimFactor));
    if (sec == min) {
      pixels.setPixelColor(min, dimColor(0, 255, 255, dimFactor));
    } else {
      pixels.setPixelColor(min, dimColor(0, 255, 0, dimFactor));
    }
    if (sec == hour && min != hour) {
      pixels.setPixelColor(hour, dimColor(158, 0, 255, dimFactor));
    } else if (min == hour && sec != hour) {
      pixels.setPixelColor(hour, dimColor(158, 255, 0, dimFactor));
    } else if (sec == hour && min == hour) {
      pixels.setPixelColor(hour, dimColor(255, 255, 255, dimFactor));
    } else {
      pixels.setPixelColor(hour, dimColor(158, 8, 148, dimFactor));
    }
  }
  pixels.show();

  bgd1 = sec;
  bgd2 = min;
  bgd3 = hour;

  if (hour % 2 == 0 && min == 1 && sec == 0) {
    weatherDATA();
  }

  if(min % 5 == 0 && sec == 1){
    if(WiFi.status() == WL_CONNECTION_LOST || WiFi.status() == WL_DISCONNECTED){
      wifirestart();
    }
    currentDATA();
    futureDATA();
    alerts();
  }

  Serial.print(hourt);
  Serial.print(", ");
  Serial.print(min);
  Serial.print(", ");
  Serial.print(sec);
  Serial.print(", ");
  Serial.print(curTempF);
  Serial.print(", ");
  Serial.print(preTempF);
  Serial.print(", ");
  Serial.print(dif);
  Serial.print(", ");
  Serial.print(twoPOP);
  Serial.print(", ");
  Serial.print(curChillF);
  Serial.print(", ");
  Serial.println(curHeatF);
}

void weatherDATA() {
  discord.send("Current temp: " + String(curTempF) + "F, Forcast temp: " + String(preTempF) + "F");
  if(curChillF <= 0){
    discord.send("Wind Chill: " + String(curChillF) + "F");
  }
  if(curHeatF >= 95){
    discord.send("Heat Index: " + String(curHeatF) + "F");
  }
  discord.send("Precipitation: " + String(twoPOP) + "%");
}

void currentDATA(){
  HTTPClient http;
  http.begin(website2);
  int httpResponseCode = http.GET();
  if (httpResponseCode > 0) {
    htmlSourceCode = http.getString();
  } else {
    Serial.print("Error code for currentDATA: ");
    Serial.println(httpResponseCode);
    currentDATA();
    return;
  }
  http.end();

  if(htmlSourceCode.indexOf("Errors") > 0){
    currentDATA();
    return;
  }

  htmlSourceCode = htmlSourceCode.substring(htmlSourceCode.indexOf("properties"));
  curTemp = htmlSourceCode.substring(htmlSourceCode.indexOf("temperature"), htmlSourceCode.indexOf("dewpoint"));
  curTemp = curTemp.substring(curTemp.indexOf("value"), curTemp.indexOf("qualityControl"));
  curTemp = curTemp.substring(curTemp.indexOf(":") + 2, curTemp.indexOf(","));
  curTempC = curTemp.toDouble();
  curTempF = (curTempC * 1.8) + 32;
  Serial.println(curTempF);


  curChill = htmlSourceCode.substring(htmlSourceCode.indexOf("windChill"), htmlSourceCode.indexOf("heatIndex"));
  curChill = curChill.substring(curChill.indexOf("value"), curChill.indexOf("qualityControl"));
  curChill = curChill.substring(curChill.indexOf(":") + 2, curChill.indexOf(","));
  if(!curChill.equals("null")){
    curChillC = curChill.toDouble();
    curChillF = (curChillC * 1.8) + 32;
    Serial.println(curChillF);
  }else {
    curChillF = 1000;
    Serial.println("No Wind Chill");
  }


  curHeat = htmlSourceCode.substring(htmlSourceCode.indexOf("heatIndex"), htmlSourceCode.indexOf("cloudLayers"));
  curHeat = curHeat.substring(curHeat.indexOf("value"), curHeat.indexOf("qualityControl"));
  curHeat = curHeat.substring(curHeat.indexOf(":") + 2, curHeat.indexOf(","));
  if(!curHeat.equals("null")){
    curHeatC = curHeat.toDouble();
    curHeatF = (curHeatC * 1.8) + 32;
    Serial.println(curHeatF);
  }else {
    curHeatF = -1000;
    Serial.println("No Heat Index");
  }
}

void futureDATA(){
  HTTPClient http2;
  http2.begin(website);
  int httpResponseCode2 = http2.GET();
  if (httpResponseCode2 > 0) {
    htmlSourceCode2 = http2.getString();
  } else {
    Serial.print("Error code for futureDATA: ");
    Serial.println(httpResponseCode2);
    futureDATA();
    return;
  }
  http2.end();

  if(htmlSourceCode2.indexOf("Errors") > 0){
    futureDATA();
    return;
  }

  htmlSourceCode2 = htmlSourceCode2.substring(htmlSourceCode2.indexOf("properties"), htmlSourceCode2.indexOf("properties") + 13000);
  for(int i = 1; i < 13; i++){
    htmlSourceCode2 = htmlSourceCode2.substring(htmlSourceCode2.indexOf("number") + 6, 13000);
    temp = htmlSourceCode2.substring(htmlSourceCode2.indexOf("temperature"), htmlSourceCode2.indexOf("temperatureUnit"));
    temp = temp.substring(temp.indexOf(":") + 2, temp.indexOf(","));
    temps[i] = temp.toInt();
    precipitation = htmlSourceCode2.substring(htmlSourceCode2.indexOf("probabilityOfPrecipitation"), htmlSourceCode2.indexOf("dewpoint") + 10);
    precipitation = precipitation.substring(precipitation.indexOf("value"), precipitation.indexOf("dewpoint") + 10);
    precipitation = precipitation.substring(precipitation.indexOf(":") + 2, precipitation.indexOf("dewpoint") - 37);
    pop[i] = precipitation.toInt();
  }
  preTempF = temps[1] + temps[2];
  preTempF = preTempF/2;
  dif = preTempF - curTempF;
  twoPOP = pop[1] + pop[2] - ((pop[1] * pop[2]) / 100);
  Serial.println(String(preTempF) + ", " + String(dif) + ", " + String(twoPOP));
}

void alerts(){
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http3;
    http3.begin(website3);
    int httpResponseCod3 = http3.GET();
    if (httpResponseCod3 > 0) {
      htmlSourceCode3 = http3.getString();
    } else {
      Serial.print("Error code for Alerts: ");
      Serial.println(httpResponseCod3);
      alerts();
      return;
    }
    http3.end();
  } else {
    Serial.println("WiFi disconnected");
  }
  if(htmlSourceCode3.indexOf("Errors") > 0){
    alerts();
    return;
  }


  allevent = "";
  while(htmlSourceCode3.indexOf("properties") > 0){
    htmlSourceCode3 = htmlSourceCode3.substring(htmlSourceCode3.indexOf("properties") + 10);
    location = htmlSourceCode3.substring(htmlSourceCode3.indexOf("areaDesc") + 12, htmlSourceCode3.indexOf("geocode") - 20);
    event = htmlSourceCode3.substring(htmlSourceCode3.indexOf("event") + 9, htmlSourceCode3.indexOf("sender") - 20);
    headline = htmlSourceCode3.substring(htmlSourceCode3.indexOf("headline") + 12, htmlSourceCode3.indexOf("description") - 20);
    allevent = allevent + headline + " -> " + location + "\\n";
  }

  if(!oldevent.equals(allevent)){
    if(allevent.length() == 0){
      discord.send("No more Alerts");
    }else{
      discord.send(allevent);
    }
    oldevent = allevent;
  }

  if(oldevent.indexOf("Warning") > 0){
    gasr = 100;
    gasg = 0;
    gasb = 0;
  }else if(oldevent.indexOf("Advisory") > 0){
    gasr = 100;
    gasg = 100;
    gasb = 0;
  }else if(oldevent.indexOf("Watch") > 0){
    gasr = 100;
    gasg = 65;
    gasb = 0;
  }else{
    gasr = 0;
    gasg = 0;
    gasb = 0;
  }
  if(allevent.length() > 10){
    Serial.println(allevent);
  }else{
    Serial.println("No Alerts");
  }
}

void wifirestart(){
  WiFi.disconnect();
  pixels.setPixelColor(6, 5, 5, 5);
  pixels.show();
  delay(100);
  pixels.setPixelColor(6, 0, 0, 0);
  pixels.show();
  delay(100);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    pixels.setPixelColor(6, 5, 0, 0);
    pixels.show();
    delay(100);
    pixels.setPixelColor(6, 0, 0, 0);
    pixels.show();
    delay(400);
  }
  pixels.setPixelColor(6, 0, 5, 0);
  pixels.show();
  delay(100);
  pixels.setPixelColor(6, 0, 0, 0);
  pixels.show();
  discord.send("WiFi restarted");
}

uint32_t dimColor(uint8_t r, uint8_t g, uint8_t b, float factor) {
  return pixels.Color(r * factor, g * factor, b * factor);
}