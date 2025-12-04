# Weather Clock

  This is an Arduino Based Weather Clock using NeoPixels. Displays the weather (temperature and precipitation), National Weather Service alerts, and sends discord messages.

## Versions

 - V11 -> Only one weather location
 - V12 -> Two weather locations, mac address info
 - V13 -> Discord Bot to send message

## Weather display:
```
Temperature: Using color coding on the clock
  red -> warmer, gold -> cold, blue -> stay the same

Percipitation: Using flashing of the temperature LEDS on the clock
  flashing -> high precipitation, solid -> low precipitation

National Weather Service alerts: Using the background of the clock to indicate the message level
  red -> warning, yellow -> advisory, orange -> watch
    order of priority -> warning, advisory, watch
```

## Discord messages
V11 and V12 use [Usini Discord WebHook](https://github.com/usini/usini_discord_webhook) to send messages.\
V13 uses built in code to send via a Bot.
```
Weather:
  Temperature: current and forcast
  Wind Chill/Heat Index: sent based on if it is over or under a value
  Percipitation
National Weather Service alerts:
  Headline and Location
    when multiple will add a new line with same data
```

## Clock
Uses NeoPixels to create a clock. Currently set up for a LED count of 60.

## Device 
Currently for and ESP32. Needs wifi connection.

# Code Setup
## WiFi
  ssid is your WiFi name\
  password is your WiFi password, if no password just leave blank

## Time
  Need to set gmtOffset_sec, this is the offset from GMT(timezone) in seconds\
  Example: Central Standard Time(CST) is -6 hours so it is -21600 seconds. <br/>

  Need to set daylightOffset_sec, this is the Daylight Saving Time offset\
  So 1 hour is 3600 seconds, this only take effect if Daylight Saving Time is in efect.

## Discord
### Version 11 and 12
  A weblink for the server channel will be needed. The first slash of numbers in the CHANNEL_ID, and the last section is the TOKEN. For more information and examples go to https://github.com/usini/usini_discord_webhook.
### Version 13 and later
  First you will need to create a Discord Bot. To create the bot you will need to go to your developer page, then applications. Now create a new applications, go to bot and then reset the token, after reseting this will need to be save for your code and added to BOT_TOKEN in the code.\
  To add the bot to your discord server go to OAuth2 page on the developers applications page. To create the link you need to select bot in the OAuth2 URL Generator, then the permitions that you want easiest is to just fo with Administrator. Finaly copy the Generated URL into your web browers and follow the steps.\
  One final thing is you will need to get the channel id from what ever channel you are adding the clock to. This can done by right clicking on the channel and clicking Copy Channel ID.

## Weather and Alerts

### Forecast (Variable = website)
  1. Go to the National Weather Service [website](https://www.weather.gov)
  2. Get local forecast by puting the zip code or city in
  3. On the forcast website take the longitude(N) and latitude(W)
     - Example: Houghton, MI -> lat(N) = 47.17 Lon(W) = 88.48
  4. Add longitude(N) and latitude(E) to https://api.weather.gov/points/{lat(N)},{lon(E)}
     - making sure to make latitude negative due to output as West but input needing East
     - Example: https://api.weather.gov/points/47.17,-88.57
  5. On the website from above locate "forecastHourly", that is the forcast website
     - Example: https://api.weather.gov/gridpoints/MQT/114,98/forecast/hourly

### Current (Variable = website2)
  1. Go to the National Weather Service [website](https://www.weather.gov)
  2. Get local forecast by puting the zip code or city in
  3. On the forcast website take the stationIdentifier
     - Example: Colorado Springs, CO -> KCOS
     - Example: Seward, AK -> PAWD
  4. Add the code from above to https://api.weather.gov/stations/{stationIdentifier}/observations/latest?require_qc=true
     - Example: https://api.weather.gov/stations/PAWD/observations/latest?require_qc=true

### Alerts (Variable = website3)
  1. Locate the Federal Information Processing System (FIPS) Code for the wanted countys [Helpful Link](https://transition.fcc.gov/oet/info/maps/census/fips/fips.txt)
  2. Take the state abbreviation add C and then add the last 3 numbers of the FIPS code
     - Example: Douglas County, WI -> FIPS = 55031 -> WIC031
  3. Add the code from above to https://api.weather.gov/alerts/active/zone/{FIPScode}
     - Example: https://api.weather.gov/alerts/active/zone/WIC031
    

# Changes to Code/Function
## Change/Addition in new version
This is what has changed in this version
 - Added a built in Discord messaging system using a bot

## Old Change/Addition in new version
V12
 - Added a second locations to send notifications for
 - Added mac address infomation

## Next Changes/Additions
This could happen at any point in time


