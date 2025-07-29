# Code Explained

## printLocalTime
Gets time\
Converts hour from 24 to 12\
Gets what value is need for hour to show on the clock\
Checks if hour in under 7 -> to dim the brightness\
Checks if hours is less then 6 or greater then 21 -> turns off clock\
_ if a warning exists -> then the background is set to red\
Sets the clock background\
Sets the weather LEDs\
Sets the seconds, minutes, and hours\
Checks the time if hour is even and min is 1 -> calls weatherDATA\
if min is base 5 and sec is 1 -> checks wifi, calls currentDATA, futureDATA, and alerts\
prints all data in the serial monitor

## weatherDATA
Sends current temperature and forcasted temperature\
if under 0 -> Sends wind Chill\
if over 95 -> Sends heat index\
Sends precipitation

## currentDATA
Gets the NWS api response\
Locates the temperature\
Locates the windChill\
if null -> sets to 1000, to confirm it does not exist\
Locates the heatIndex\
if null -> sets to -1000, to confirm it does not exist

## futureDATA
Gets the NWS api response\
Locates the temperature and Precipitation for the next 12 hours\
Sets the next 2 hours for both temperature and Precipitation\
They get used as the data values for weather

## alerts
Gets the NWS api response\
Loops to get all the alerts for the area\
Checks if the list of alerts has changed\
_ if no  -> does nothing\
_ if yes -> Checks if the length is zero\
__ if yes -> Sends "No more Alerts"\
__ if no  -> Sends the list of alerts\
Checks what level of alert it is and then changes the background

## wifirestart
Disconnects wifi then connects wifi

## dimColor
Returns the color at a dimmed factor\
_This is currenty used in the lower the brightness_
