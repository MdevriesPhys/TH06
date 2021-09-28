/*
	TH06 Temperature and humidity sensor library for Arduino
	Lovelesh, thingTronics
	
The MIT License (MIT)

Copyright (c) 2015 thingTronics Limited

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

version 0.1
*/


#include <TH06.h>
#include <Wire.h>

TH06::TH06(void) {
	// TH06 object
}
			
boolean TH06::begin(void) {
	// Initialize TH06 library with default address (0x40)
	// Always returns true
	
	_i2c_address = TH06_ADDR;
	Wire.begin();
	return(true);
}
			
		
boolean TH06::reset(void) {
	// SW Reset the sensor
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)
	
	// In reset we write default value to USER Register
	byte res = 0x00;
	boolean heater = false;
	if(setTempControl(res, heater)) {
		delay(15);
		return(true);
	}
	
	return(false);
}
			
boolean TH06::getTempControl(byte &res, boolean voltage, boolean heater) {
	// Gets the contents RH/Temp User Register of the sensor
	// res uses D7 and D0 bit
	// If res = 0, RH is set to 12 bit & temp 14 bit resolution (default)
	// If res = 1, RH is set to 8 bit & temp 12 bit resolution
	// If res = 2, RH is set to 10 bit & temp 13 bit resolution
	// If res = 4, RH is set to 11 bit & temp 11 bit resolution
	//----------------------------------------------------------
	// If voltage = false(0), VDD OK (default)
	// If voltage = true(1), VDD LOW
	//----------------------------------------------------------
	// If heater = false(0), On-chip Heater is disabled (default)
	// If heater = true(1), On-chip Heater is disabled
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)
			
	byte control, humidity;
	
	// Reading the control byte
	if(readByte(TH06_READ_HUMIDITY_TEMP_CONTR,control)) {
		// Extract resolution
		// extracting D7 control bit into D1 res; D0 control bit into D0 res 
		res = ((control & 0x80) >> 6) | (control & 0x01);
		
		// Extract voltage
		voltage = (control & 0x40) ? true : false; 
		
		// Extract heater
		heater = (control & 0x04) ? true : false;
		
		// return if successful
		return(true);
	}
	return(false);		
}

boolean TH06::setTempControl(byte res, boolean heater) {
	// Sets the contents RH/Temp User Register of the sensor
	// Gets the contents RH/Temp User Register of the sensor
	// res uses D7 and D0 bit
	// If res = 0, RH is set to 12 bit & temp 14 bit resolution (default)
	// If res = 1, RH is set to 8 bit & temp 12 bit resolution
	// If res = 2, RH is set to 10 bit & temp 13 bit resolution
	// If res = 4, RH is set to 11 bit & temp 11 bit resolution
	//----------------------------------------------------------
	// If heater = false(0), On-chip Heater is disabled (default)
	// If heater = true(1), On-chip Heater is disabled
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)
	
	byte control = 0x00;
	
	// sanity check for gain
	if (res > 4) {
		res = 0x00;
	}
	
	// control byte logic
	control |= (res & 0x02) << 6 | (res & 0x01);
	
	if(heater) {
		control |= 0x04;
	}
	
	return(writeByte(TH06_WRITE_HUMIDITY_TEMP_CONTR,control));
}			
			
boolean TH06::getDeviceID(char (&deviceID)[8]) {
	// Gets the Device ID of the chip
	// Default value of MSB 0x06
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)
	
	char tempDeviceID[4];
	// read first 4 bytes
	if(read4ByteData(TH06_READ_ID_LOW_0,TH06_READ_ID_LOW_1,tempDeviceID)) {
		strcpy(deviceID, tempDeviceID);
		//deviceID <<= 32;
		
		// read the next 4 bytes
		if(read4ByteData(TH06_READ_ID_HIGH_0,TH06_READ_ID_HIGH_1,tempDeviceID)) {
			strcat(deviceID, tempDeviceID);
			//deviceID += double(tempDeviceID) * pow(2,32);
			// return if successful
			return(true);
		}
	}
	return(false);
}

boolean TH06::getFirmwareVer(byte &firmware) {
	// Gets the Firmware Version of the chip
	// Default value is 0xFF for version 1.0
	// or 0x20 for version 2.0
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)
	
	return(read1ByteData(TH06_FIRMWARE_0, TH06_FIRMWARE_1, firmware));
}
			
boolean TH06::getTemperature(float &temperature, boolean mode) {
	// Gets the Temperature data from the sensor
	// If mode = true(1), Hold Master Mode is used
	// If mode = false(0), No Hold Master Mode is used
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)
	
	boolean success = false;
	unsigned int tempTemperature;
	
	if(mode) {
		if(!readUInt(TH06_MEAS_TEMP_MASTER_MODE,tempTemperature))
			return(false);
	}
	else {
		if(!readUInt(TH06_MEAS_TEMP_NO_MASTER_MODE,tempTemperature))
			return(false);
	}
	
	// Check if temperature is correct by ANDing with 0xFFFC
	if(tempTemperature & 0xFFFC) {
		temperature = (175.72 * (float)tempTemperature)/65536 - 46.85;
		return(true);
	}
	
	return(false);
}		
		
boolean TH06::getOldTemperature(float &temperature) {
	// Gets the Old Temperature data from the sensor
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)
	
	unsigned int tempTemperature;
	if(readUInt(TH06_READ_OLD_TEMP,tempTemperature)) {
		// Check if temperature is correct by ANDing with 0xFFFC
		if(tempTemperature & 0xFFFC) {
			temperature = (175.72 * (float)tempTemperature)/65536 - 46.85;
			return(true);
		}
	}
	
	return(false);
}
		
boolean TH06::getHumidity(float &humidity, boolean mode) {
	// Gets the Humidity data from the sensor
	// If mode = true(1), Hold Master Mode is used
	// If mode = false(0), No Hold Master Mode is used
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() below)
	
	boolean success = false;
	unsigned int tempHumidity;
	
	if(mode) {
		if(!readUInt(TH06_MEAS_REL_HUMIDITY_MASTER_MODE,tempHumidity))
			return(false);
	}
	else {
		if(!readUInt(TH06_MEAS_REL_HUMIDITY_NO_MASTER_MODE,tempHumidity))
			return(false);
	}
	
	// Check if humidity is correct by ANDing with 0xFFFE
	if(tempHumidity & 0xFFFE) {
		humidity = (125 * (float)tempHumidity)/65536 - 6;
		return(true);
	}
	
	return(false);
}
			
byte TH06::crc8(const uint8_t *data, int len) {
	// Returns the CRC byte generated from the data
	/*
	 *
	 * CRC-8 formula from page 14 of SHT spec pdf
	 *
	 * Test data 0xBE, 0xEF should yield 0x92
	 *
	 * Initialization data 0xFF
	 * Polynomial 0x31 (x8 + x5 +x4 +1)
	 * Final XOR 0x00
	*/

	const uint8_t POLYNOMIAL(0x31);
	uint8_t crc(0xFF);

	for ( int j = len; j; --j ) {
	  crc ^= *data++;

	  for ( int i = 8; i; --i ) {
	crc = ( crc & 0x80 )
	  ? (crc << 1) ^ POLYNOMIAL
	  : (crc << 1);
	  }
	}
	return crc;
}
			
byte TH06::getError(void) {
	// If any library command fails, you can retrieve an extended
	// error code using this command. Errors are from the wire library: 
	// 0 = Success
	// 1 = Data too long to fit in transmit buffer
	// 2 = Received NACK on transmit of address
	// 3 = Received NACK on transmit of data
	// 4 = Other error
	
	return(_error);
}
		
// Private Functions:
		
boolean TH06::readByte(byte address, byte &value) {
	// Reads a byte from a TH06 address
	// Address: LTR303 address (0 to 15)
	// Value will be set to stored byte
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() above)
	
	// Check if sensor present for read
	Wire.beginTransmission(_i2c_address);
	Wire.write(address);
	_error = Wire.endTransmission();

	// Read requested byte
	if (_error == 0)
	{
		Wire.requestFrom(_i2c_address,1);
		if (Wire.available() == 1)
		{
			value = Wire.read();
			return(true);
		}
	}
	return(false);
}

boolean TH06::writeByte(byte address, byte value) {
	// Write a byte to a TH06 address
	// Address: LTR303 address (0 to 15)
	// Value: byte to write to address
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() above)
	
	Wire.beginTransmission(_i2c_address);
	// Write byte
	Wire.write(address);
	Wire.write(value);
	_error = Wire.endTransmission();
	if (_error == 0)
		return(true);

	return(false);
}

boolean TH06::readUInt(byte address, unsigned int &value) {
	// Reads an unsigned integer (16 bits) from a TH06 address (high byte first)
	// Address: TH06 address (0 to 15), high byte first
	// Value will be set to stored unsigned integer
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() above)
	
	byte high, low;
	
	// Check if sensor present for read
	Wire.beginTransmission(_i2c_address);
	Wire.write(address);
	_error = Wire.endTransmission();

	// Read two bytes (low and high)
	if (_error == 0)
	{
		Wire.requestFrom(_i2c_address,2);
		if (Wire.available() == 2)
		{
			high = Wire.read();
			low = Wire.read();
			// Combine bytes into unsigned int
			value = word(high,low);
			return(true);
		}
	}	
	return(false);
}

boolean TH06::read1ByteData(byte address1, byte address2, byte &value) {
	// Reads a byte from a TH06 sensor when provided with 2 addresses
	// Address: TH06 address (0 to 15)
	// Value will be set to stored byte
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() above)
	
	// Check if sensor present for read
	Wire.beginTransmission(_i2c_address);
	Wire.write(address1);
	Wire.write(address2);
	_error = Wire.endTransmission();

	// Read requested byte
	if (_error == 0)
	{
		Wire.requestFrom(_i2c_address,1);
		if (Wire.available() == 1)
		{
			value = Wire.read();
			return(true);
		}
	}
	return(false);
}

boolean TH06::read4ByteData(byte address1, byte address2, char (&value)[4]) {
	// Reads an unsigned long (32 bits) from a TH06 address (high byte first)
	// Address: TH06 register addresses (0 to 15), high byte first
	// Value will be set to stored 4 byte character array
	// Returns true (1) if successful, false (0) if there was an I2C error
	// (Also see getError() above)
	
	// Check if sensor present for read
	Wire.beginTransmission(_i2c_address);
	Wire.write(address1);
	Wire.write(address2);
	_error = Wire.endTransmission();

	// Read requested byte
	if (_error == 0)
	{
		Wire.requestFrom(_i2c_address,4);
		if (Wire.available() == 4)
		{
			value[3] |= Wire.read();
			value[2] |= Wire.read();
			value[1] |= Wire.read();
			value[0] |= Wire.read();
 			return(true);
		}
	}
	return(false);
}
