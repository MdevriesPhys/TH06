/* 
	TH06-A20 library example sketch
	Lovelesh, thingTronics

This sketch shows how to use the TH06 library to read the TH06 temperature and humidity sensor.

Hardware connections:

3V3 to 3.3V
GND to GND

(WARNING: do not connect 3V3 to 5V or the sensor will be damaged!)

You will also need to connect the I2C pins (SCL and SDA) to your Arduino.
The pins are different on different Arduinos:

                    SDA    SCL
Any Arduino        "SDA"  "SCL"
Uno, Pro            A4     A5
Mega2560, Due       20     21
Leonardo            2      3
ESP8266				5      4

*/
 
#include <Wire.h>
#include <TH06.h>

TH06 th06; 

void setup() {
	
	// Initialize the Serial port:  
	Serial.begin(9600);
	Serial.println("TH06 example sketch");

	// Initialize the TH06 library
	// You can pass nothing to light.begin() for the default I2C address (0x40)
	th06.begin();
	
	if (!th06.begin()) {
		Serial.println("Couldn't find TH06");
		while (1)
			delay(1); // Do Nothing
	}
		
	// Most library commands will return true if communications was successful,
	// and false if there was a problem. You can ignore this returned value,
	// or check whether a command worked correctly and retrieve an error code:
	else {
		byte error = th06.getError();
		printError(error);
	}

  byte firmwareVer;
  if (th06.getFirmwareVer(firmwareVer)){
    Serial.print("Got firmware version: 0x");
    Serial.println(firmwareVer,HEX);
  }
  else{
    byte error = th06.getError();
    printError(error);
  }
  
	// Gets the contents RH/Temp User Register of the sensor
  // If resolution = 0 (00), RH is set to 12 bit & temp 14 bit resolution (default)
  // If resolution = 1 (01), RH is set to 8 bit & temp 12 bit resolution
  // If resolution = 2 (10), RH is set to 10 bit & temp 13 bit resolution
  // If resolution = 3 (11), RH is set to 11 bit & temp 11 bit resolution
	byte resolution;
  boolean voltage, heaterStatus;
	th06.getTempControl(resolution, voltage, heaterStatus);
	Serial.print("Resolution code is: ");
	Serial.println(resolution);
  Serial.print("Voltage is: ");
  if (!voltage){ //voltage=0 is voltage good, voltage=1 is low voltage
    Serial.println("Good");
  }
  else{
    Serial.println("Low");
  }
  Serial.print("Heater is: ");
  if (heaterStatus){
    Serial.println("On");
  }
  else{
    Serial.println("Off");
  }

  //---------Doesn't seem to be working right, need to fix!----------------
	// Setting the resolution and heater disable
	byte setresolution = 0x00;
	heaterStatus = false;
	th06.setTempControl(setresolution, heaterStatus);
}
void loop() {
  float temp;
  float humidity;
  boolean mode=true;//false; //Hold master mode = true, no hold master mode = false
  //--------------measurement doesn't work in no hold mode yet----------------//

  // Read temperature
  if(th06.getTemperature(temp, mode)) {
    Serial.print("Temp *C = ");
    Serial.println(temp);
  }
  else{
    Serial.println("Failed to read temperature");
    byte error = th06.getError();
    printError(error);
  }
  // Read humidity
  if (th06.getHumidity(humidity,mode)){
    Serial.print("Rel humidity %: ");
    Serial.println(humidity);
  }
  else{
    Serial.println("Failed to read humidity");
    byte error = th06.getError();
    printError(error);
  }
  delay(1000);
}

void printError(byte error) {
  // If there's an I2C error, this function will
  // print out an explanation.

  Serial.print("I2C error: ");
  Serial.print(error,DEC);
  Serial.print(", ");
  
  switch(error) {
    case 0:
      Serial.println("success");
      break;
    case 1:
      Serial.println("data too long for transmit buffer");
      break;
    case 2:
      Serial.println("received NACK on address (disconnected?)");
      break;
    case 3:
      Serial.println("received NACK on data");
      break;
    case 4:
      Serial.println("other error");
      break;
    default:
      Serial.println("unknown error");
  }
}
