#include <WiFi.h>
#include <HTTPClient.h>
#include "time.h"
#include <Adafruit_NeoPixel.h>
#include <WiFiClientSecure.h>
#include <esp_wifi.h>

WiFiServer server(80);
const char* ssid = "";  //Wifi name
const char* password = ""; //Wifi password

//_1 = 0, _2 = 1
const char* forecast_1 = ""; //forcast website
const char* current_1 = ""; //current website
const char* alerts_1 = ""; //alerts website

const char* forecast_2 = ""; //forcast website
const char* current_2 = ""; //current website
const char* alerts_2 = ""; //alerts website

const char* BOT_TOKEN = ""; //discord bot token
String DISCORD_CHANNEL_ID_1 = ""; //channel ID for _1
String DISCORD_CHANNEL_ID_2 = ""; //channel ID for _2

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0; //timezone offset
const int daylightOffset_sec = 0; //daylight savings time offset

#define LED_PIN 32
#define LED_COUNT 60
Adafruit_NeoPixel pixels(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

int hourt;
int hourt2;

float dimFactor;

int clockDif;
int clockPer;

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
int curTempF_1;
int curTempF_2;

String curBaro = "";
double curBaroPa;
double curBarohPa_1;
double curBarohPa_2;

String curChill = "";
double curChillC;
int curChillF_1;
int curChillF_2;

String curHeat = "";
double curHeatC;
int curHeatF_1;
int curHeatF_2;

String htmlSourceCode2 = "";
String temp = "";
int temps[13];
int preTempF_1;
int dif_1;
int preTempF_2;
int dif_2;

String precipitation = "";
int pop[13];
int twoPOP_1;
int twoPOP_2;

String htmlSourceCode3 = "";
String event = "";
String headline = "";
String location = "";
String allevent = "";

String allevent_1 = "";
String oldevent_1 = "";
String allevent_2 = "";
String oldevent_2 = "";

int callLimit = 0;

void setup() {
  Serial.begin(115200);
  WiFi.setHostname("Weather_Clock");

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

  WiFi.mode(WIFI_STA);
  WiFi.STA.begin();
  Serial.print("[DEFAULT] ESP32 Board MAC Address: ");
  readMacAddress();

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();


  sendDiscordMessage("Clock up and running", 0);
  currentDATA(current_1, 0);
  futureDATA(forecast_1, 0);
  weatherDATA(0);
  alerts(alerts_1, 0);

  sendDiscordMessage("Clock up and running", 1);
  currentDATA(current_2, 1);
  futureDATA(forecast_2, 1);
  weatherDATA(1);
  alerts(alerts_2, 1);
}

void loop() {
  delay(1000);
  printLocalTime();
}

/*
  Getting and showing the time in the serial monitor and on the neoPixel ring
  calls the other weather methods at set times
*/
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

  hourt2 = hour - 1;
  if (hour == 1) hourt2 = 24;

  if (hour > 12) {
    hour = hour - 12;
  }
  double hournew = hour;
  hournew = (hournew / 12) * 60;
  hour = (int)hournew;
  if (hour == 60) {
    hour = 0;
  }

  if (hourt < 7 || hourt >= 21) {
    dimFactor = 0.05;
  } else {
    dimFactor = 1;
  }

  clockDif = dif_2;
  clockPer = twoPOP_2;

  if (hourt < 6 || hourt >= 22) {
    // Night time
    if (gasr == 100 && gasg == 0) {
      pixels.fill(pixels.Color(1, 0, 0));
    } else {
      pixels.fill(pixels.Color(0, 0, 0));
    }
  } else {
    // Day time
    pixels.setPixelColor(bgd1, dimColor(gasr, gasg, gasb, dimFactor));
    pixels.setPixelColor(bgd2, dimColor(gasr, gasg, gasb, dimFactor));
    pixels.setPixelColor(bgd3, dimColor(gasr, gasg, gasb, dimFactor));

    if (clockDif > 2) {
      if (clockPer > 50) {
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
    } else if (clockDif < -2) {
      if (clockPer > 50) {
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
      if (clockPer > 50) {
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

  if (hourt % 2 == 0 && min == 1 && sec == 0) {
    weatherDATA(1);
  }
  if (hourt2 % 2 == 0 && min == 1 && sec == 0) {
    weatherDATA(0);
  }

  if (min % 5 == 0 && sec == 1) {
    if (WiFi.status() == WL_CONNECTION_LOST || WiFi.status() == WL_DISCONNECTED) {
      wifirestart();
    }
    currentDATA(current_1, 0);
    futureDATA(forecast_1, 0);
    alerts(alerts_1, 0);

    currentDATA(current_2, 1);
    futureDATA(forecast_2, 1);
    alerts(alerts_2, 1);
  }

  Serial.print("MKE " + String(hourt2) + " " + String(min) + " " + String(sec) + " C[" + String(curTempF_1) + "] F[" + String(preTempF_1) + "] D[" + String(dif_1) + "] P[" + String(twoPOP_1) + "] WC[" + String(curChillF_1) + "] HI[" + String(curHeatF_1) + "] BA[" + String(curBarohPa_1) + "]   ");
  Serial.println("MTU " + String(hourt) + " " + String(min) + " " + String(sec) + " C[" + String(curTempF_2) + "] F[" + String(preTempF_2) + "] D[" + String(dif_2) + "] P[" + String(twoPOP_2) + "] WC[" + String(curChillF_2) + "] HI[" + String(curHeatF_2) + "] BA[" + String(curBarohPa_2) + "]   ");
}

/*
  Send the data to discord
  Changes the channel based on 0 or 1
*/
void weatherDATA(int loc) {
  if (loc == 0) {
    sendDiscordMessage("Current temp: " + String(curTempF_1) + "F, Forecast temp: " + String(preTempF_1) + "F", 0);
    if (curChillF_1 <= 15) {
      sendDiscordMessage("Wind Chill: " + String(curChillF_1) + "F", 0);
    }
    if (curHeatF_1 >= 95) {
      sendDiscordMessage("Heat Index: " + String(curHeatF_1) + "F", 0);
    }
    sendDiscordMessage("Precipitation: " + String(twoPOP_1) + "%, Barometric Pressure: " + String(curBarohPa_1) + "hPa", 0);
  }
  if (loc == 1) {
    sendDiscordMessage("Current temp: " + String(curTempF_2) + "F, Forecast temp: " + String(preTempF_2) + "F", 1);
    if (curChillF_2 <= 15) {
      sendDiscordMessage("Wind Chill: " + String(curChillF_2) + "F", 1);
    }
    if (curHeatF_2 >= 95) {
      sendDiscordMessage("Heat Index: " + String(curHeatF_2) + "F", 1);
    }
    sendDiscordMessage("Precipitation: " + String(twoPOP_2) + "%, Barometric Pressure: " + String(curBarohPa_2) + "hPa", 1);
  }
}

/*
  Sets current temperature, wind chill, and heat index
  link is for the weblink to make interchangeable
  Changes based save location on 0 or 1
*/
void currentDATA(String link, int loc) {
  HTTPClient http;
  http.begin(link);
  int httpResponseCode = http.GET();
  if (httpResponseCode > 0) {
    htmlSourceCode = http.getString();
  } else {
    Serial.print("Error code for currentDATA: ");
    Serial.println(httpResponseCode);
    if (callLimit >= 4) {
      callLimit = 0;
      http.end();
      return;
    } else {
      callLimit++;
      currentDATA(link, loc);
      http.end();
      return;
    }
  }
  http.end();

  Serial.println(String(htmlSourceCode.length()) + " Current API");

  if (htmlSourceCode.indexOf("Errors") > 0 || htmlSourceCode == 0) {
    if (callLimit >= 4) {
      callLimit = 0;
      return;
    } else {
      callLimit++;
      currentDATA(link, loc);
      return;
    }
  }

  htmlSourceCode = htmlSourceCode.substring(htmlSourceCode.indexOf("properties"));
  curTemp = htmlSourceCode.substring(htmlSourceCode.indexOf("temperature"), htmlSourceCode.indexOf("dewpoint"));
  curTemp = curTemp.substring(curTemp.indexOf("value"), curTemp.indexOf("qualityControl"));
  curTemp = curTemp.substring(curTemp.indexOf(":") + 2, curTemp.indexOf(","));
  curTempC = curTemp.toDouble();
  if (loc == 0) curTempF_1 = (curTempC * 1.8) + 32;
  if (loc == 1) curTempF_2 = (curTempC * 1.8) + 32;
  Serial.println(curTempF_1);


  curBaro = htmlSourceCode.substring(htmlSourceCode.indexOf("barometricPressure"), htmlSourceCode.indexOf("seaLevelPressure"));
  curBaro = curBaro.substring(curBaro.indexOf("value"), curBaro.indexOf("qualityControl"));
  curBaro = curBaro.substring(curBaro.indexOf(":") + 2, curBaro.indexOf(","));
  curBaroPa = curBaro.toDouble();
  if (loc == 0) curBarohPa_1 = curBaroPa * 0.01;
  if (loc == 1) curBarohPa_2 = curBaroPa * 0.01;
  Serial.println(curBarohPa_1);


  curChill = htmlSourceCode.substring(htmlSourceCode.indexOf("windChill"), htmlSourceCode.indexOf("heatIndex"));
  curChill = curChill.substring(curChill.indexOf("value"), curChill.indexOf("qualityControl"));
  curChill = curChill.substring(curChill.indexOf(":") + 2, curChill.indexOf(","));
  if (!curChill.equals("null")) {
    curChillC = curChill.toDouble();
    if (loc == 0) curChillF_1 = (curChillC * 1.8) + 32;
    if (loc == 1) curChillF_2 = (curChillC * 1.8) + 32;
    Serial.println(curChillF_1);
  } else {
    if (loc == 0) curChillF_1 = 1000;
    if (loc == 1) curChillF_2 = 1000;
    Serial.println("No Wind Chill");
  }


  curHeat = htmlSourceCode.substring(htmlSourceCode.indexOf("heatIndex"), htmlSourceCode.indexOf("cloudLayers"));
  curHeat = curHeat.substring(curHeat.indexOf("value"), curHeat.indexOf("qualityControl"));
  curHeat = curHeat.substring(curHeat.indexOf(":") + 2, curHeat.indexOf(","));
  if (!curHeat.equals("null")) {
    curHeatC = curHeat.toDouble();
    if (loc == 0) curHeatF_1 = (curHeatC * 1.8) + 32;
    if (loc == 1) curHeatF_2 = (curHeatC * 1.8) + 32;
    Serial.println(curHeatF_1);
  } else {
    if (loc == 0) curHeatF_1 = -1000;
    if (loc == 1) curHeatF_2 = -1000;
    Serial.println("No Heat Index");
  }
}

/*
  Sets current temperature and precipitattion based on the api
  link is for the weblink to make interchangeable
  Changes based save location on 0 or 1
*/
void futureDATA(String link, int loc) {
  HTTPClient http2;
  http2.begin(link);
  int httpResponseCode2 = http2.GET();
  if (httpResponseCode2 > 0) {
    htmlSourceCode2 = http2.getString();
  } else {
    Serial.print("Error code for futureDATA: ");
    Serial.println(httpResponseCode2);
    if (callLimit >= 4) {
      callLimit = 0;
      http2.end();
      return;
    } else {
      callLimit++;
      futureDATA(link, loc);
      http2.end();
      return;
    }
  }
  http2.end();

  Serial.println(String(htmlSourceCode2.length()) + " Forecast API");

  if (htmlSourceCode2.indexOf("Errors") > 0 || htmlSourceCode2 == 0) {
    if (callLimit >= 4) {
      callLimit = 0;
      return;
    } else {
      callLimit++;
      futureDATA(link, loc);
      return;
    }
  }

  htmlSourceCode2 = htmlSourceCode2.substring(htmlSourceCode2.indexOf("properties"), htmlSourceCode2.indexOf("properties") + 13000);
  for (int i = 1; i < 13; i++) {
    htmlSourceCode2 = htmlSourceCode2.substring(htmlSourceCode2.indexOf("number") + 6, 13000);
    temp = htmlSourceCode2.substring(htmlSourceCode2.indexOf("temperature"), htmlSourceCode2.indexOf("temperatureUnit"));
    temp = temp.substring(temp.indexOf(":") + 2, temp.indexOf(","));
    temps[i] = temp.toInt();
    precipitation = htmlSourceCode2.substring(htmlSourceCode2.indexOf("probabilityOfPrecipitation"), htmlSourceCode2.indexOf("dewpoint") + 10);
    precipitation = precipitation.substring(precipitation.indexOf("value"), precipitation.indexOf("dewpoint") + 10);
    precipitation = precipitation.substring(precipitation.indexOf(":") + 2, precipitation.indexOf("dewpoint") - 37);
    pop[i] = precipitation.toInt();
  }
  if (loc == 0) {
    preTempF_1 = temps[1] + temps[2];
    preTempF_1 = preTempF_1 / 2;
    dif_1 = preTempF_1 - curTempF_1;
    twoPOP_1 = pop[1] + pop[2] - ((pop[1] * pop[2]) / 100);
    Serial.println(String(preTempF_1) + ", " + String(dif_1) + ", " + String(twoPOP_1));
  }
  if (loc == 1) {
    preTempF_2 = temps[1] + temps[2];
    preTempF_2 = preTempF_2 / 2;
    dif_2 = preTempF_2 - curTempF_2;
    twoPOP_2 = pop[1] + pop[2] - ((pop[1] * pop[2]) / 100);
    Serial.println(String(preTempF_2) + ", " + String(dif_2) + ", " + String(twoPOP_2));
  }
}

/*
  send discord notificaton for the alerts
  sets the background color of the clock based on alerts severity
  link is for the weblink to make interchangeable
  Changes based save location on 0 or 1
*/
void alerts(String link, int loc) {
  HTTPClient http3;
  http3.begin(link);
  int httpResponseCod3 = http3.GET();
  if (httpResponseCod3 > 0) {
    htmlSourceCode3 = http3.getString();
  } else {
    Serial.print("Error code for Alerts: ");
    Serial.println(httpResponseCod3);
    if (callLimit >= 4) {
      callLimit = 0;
      http3.end();
      return;
    } else {
      callLimit++;
      alerts(link, loc);
      http3.end();
      return;
    }
  }
  http3.end();

  Serial.println(String(htmlSourceCode3.length()) + " Alerts API");


  if (htmlSourceCode3.indexOf("Errors") > 0 || htmlSourceCode3.length() == 0) {
    if (callLimit >= 4) {
      callLimit = 0;
      return;
    } else {
      callLimit++;
      alerts(link, loc);
      return;
    }
  }

  allevent = "";
  while (htmlSourceCode3.indexOf("properties") > 0) {
    htmlSourceCode3 = htmlSourceCode3.substring(htmlSourceCode3.indexOf("properties") + 10);
    location = htmlSourceCode3.substring(htmlSourceCode3.indexOf("areaDesc") + 12, htmlSourceCode3.indexOf("geocode") - 20);
    //event = htmlSourceCode3.substring(htmlSourceCode3.indexOf("event") + 9, htmlSourceCode3.indexOf("sender") - 20);
    headline = htmlSourceCode3.substring(htmlSourceCode3.indexOf("headline") + 12, htmlSourceCode3.indexOf("description") - 20);
    allevent = allevent + headline + " -> " + location + "\\n";

    if (loc == 0) allevent_1 = allevent;
    if (loc == 1) allevent_2 = allevent;
    //Serial.println(allevent);
  }

  if (loc == 0) {
    //allevent_1 = allevent;
    if (!oldevent_1.equals(allevent_1)) {
      if (allevent_1.length() == 0) {
        sendDiscordMessage("No more Alerts", 0);
      } else {
        sendDiscordMessage(allevent_1, 0);
      }
      oldevent_1 = allevent_1;
    }
  }

  if (loc == 1) {
    //allevent_2 = allevent;
    if (!oldevent_2.equals(allevent_2)) {
      if (allevent_2.length() == 0) {
        sendDiscordMessage("No more Alerts", 1);
      } else {
        sendDiscordMessage(allevent_2, 1);
      }
      oldevent_2 = allevent_2;
    }
  }

  if (oldevent_2.indexOf("Warning") > 0) {
    gasr = 100;
    gasg = 0;
    gasb = 0;
  } else if (oldevent_2.indexOf("Advisory") > 0) {
    gasr = 100;
    gasg = 100;
    gasb = 0;
  } else if (oldevent_2.indexOf("Watch") > 0) {
    gasr = 100;
    gasg = 65;
    gasb = 0;
  } else {
    gasr = 0;
    gasg = 0;
    gasb = 0;
  }

  if (loc == 0) {
    if (allevent_1.length() > 10) {
      Serial.println(allevent_1);
    } else {
      Serial.println("No Alerts");
    }
  }
  if (loc == 1) {
    if (allevent_2.length() > 10) {
      Serial.println(allevent_2);
    } else {
      Serial.println("No Alerts");
    }
  }
}

//restarts the wifi
void wifirestart() {
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
  sendDiscordMessage("WiFi restarted", 0);
  sendDiscordMessage("WiFi restarted", 1);
}

/*
  Param: red, green, and blue value
  Param: factor (the amount to change the RGB values by)
  Return: the color for neoPixel
*/
uint32_t dimColor(uint8_t r, uint8_t g, uint8_t b, float factor) {
  return pixels.Color(r * factor, g * factor, b * factor);
}

/*
  Sends the message to discord via a discord bots api
  Changes the channel it is send to based on 0 or 1
*/
void sendDiscordMessage(String text, int loc) {
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient httpS;
  String apiUrl = "https://discord.com/api/v10/channels/";
  if (loc == 0) {
    apiUrl += DISCORD_CHANNEL_ID_1;
  }
  if (loc == 1) {
    apiUrl += DISCORD_CHANNEL_ID_2;
  }
  if (loc != 0 && loc != 1) {
    httpS.end();
    return;
  }
  apiUrl += "/messages";
  httpS.begin(client, apiUrl);

  String authHeader = "Bot " + String(BOT_TOKEN);
  httpS.addHeader("Authorization", authHeader);
  httpS.addHeader("Content-Type", "application/json");
  String payload = String("{\"content\":\"") + text + "\"}";
  int httpResponseCode = httpS.POST(payload);
  if (httpResponseCode > 0) {
    String response = httpS.getString();
    if (response.indexOf("error") > 0) {
      Serial.printf("POST sent. Response: %d\n", httpResponseCode);
      Serial.println(response);
    }
  } else {
    Serial.printf("Error sending POST: %s\n", httpS.errorToString(httpResponseCode).c_str());
  }
  httpS.end();
}

//mac address stuff
void readMacAddress() {
  uint8_t baseMac[6];
  esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);
  if (ret == ESP_OK) {
    Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
                  baseMac[0], baseMac[1], baseMac[2],
                  baseMac[3], baseMac[4], baseMac[5]);
  } else {
    Serial.println("Failed to read MAC address");
  }
}