TH06 Arduino Library
=======
Arduino library for the HOPERF TH06 Temperature & Humidity Sensor breakout board. This sensor interfaces via I2C and runs at 3.3V.

"The TH06 I2C Humidity and Temperature Sensor is a monolithic CMOS IC integrating humidity and temperature sensor elements, an analog-to-digital converter, signal processing, calibration data, and an I2C Interface." - [HOPERF](https://www.hoperf.com/sensor/temperature_sensor/TH06.html "HOPERF page for TH06 sensor")

Currently developing on an arduino uno, not tested on any other platform.

Built off of aruduno library for the *very* similar [Si7006 arduino library](https://github.com/automote/Si7006 "Si7006 arduino library")

## Development History ##
- - - - - -
* V0 - Get basic functionality
  * V0.1 Get status of module, read temp/humidity <-- **Currently here**
* V1 - Full functionality
  * V1.0 - get and set heater and resolution.
  * V1.1 - Quality of life improvement for responses to get and set heater and resolution (no idea if implementable).
  * V1.2 - Make documentation shiny and clear, improve exammple .ino etc.
* Fix issues

## Known issues ##
- - - - - -
* `th06.setTempControl(resolution,heaterStatus)` does not seem to be working as expected.
* `th06.getTempControl(resolution,voltage,heaterStatus)` should not require `voltage`, as this cannot be changed and is superfluous, should be removed.

## Repository Contents ##
- - - - - - - -
* **/examples** - Example sketch for the library (.ino). Run it from the Arduino IDE. 
* **/src** - Source files for the library (.cpp, .h).
* **library.properties** - General library properties for the Arduino package manager.
* **library.json** - General library properties for the Arduino package manager in JSON format
* **keywords.txt** - Contains the keywords for Arduino IDE.
* **license** - GNU General License v3.0
* **TH06 Datasheet** - Datasheet for TH06 - [Direct Download](https://www.hoperf.com/data/upload/portal/20190307/TH06%20Datasheet.pdf "Direct link to datasheet")
