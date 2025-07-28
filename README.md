# Weather_Clock

  This is an Arduino Based Weather Clock using NeoPixels. Displays the weather (temperature and precipitation), NWS alerts, and sends discord messages.

## Weather display:
```
Temperature: Using color coding on the clock
  red -> warmer, gold -> cold, blue -> stay the same

Percipitation: Using flashing of the Temp LEDS on the clock
  flashing -> high precipitation, solid -> low precipitation

NWS alerts: Using the background of the clock to indicate the message level
  red -> warning, yellow -> advisory, orange -> watch
    order of priority -> warning, advisory, watch
```

        
