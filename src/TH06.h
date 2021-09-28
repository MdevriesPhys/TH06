/*
	TH06 Temperature and humidity sensor library for Arduino
	
	Copyright (C) 2021  Mitchell de Vries

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.

	version 0.1
*/

#ifndef TH06_h
#define TH06_h

#include "Arduino.h"

#define TH06_ADDR   0x40 // default address

// TH06 register addresses
#define TH06_MEAS_REL_HUMIDITY_MASTER_MODE		0xE5
#define TH06_MEAS_REL_HUMIDITY_NO_MASTER_MODE	0xF5
#define TH06_MEAS_TEMP_MASTER_MODE				0xE3
#define TH06_MEAS_TEMP_NO_MASTER_MODE			0xF3
#define TH06_READ_OLD_TEMP						0xE0
#define TH06_RESET 								0xFE
#define TH06_WRITE_HUMIDITY_TEMP_CONTR			0xE6
#define TH06_READ_HUMIDITY_TEMP_CONTR 			0xE7
#define TH06_FIRMWARE_0							0x84
#define TH06_FIRMWARE_1							0x8b


class TH06 {
	public:
		TH06(void);
			// TH06 object
			
		boolean begin();
			// Initialize TH06 library with default address (0x40)
			// Always returns true
			
		boolean reset(void);
			// SW Reset the sensor
			// Returns true (1) if successful, false (0) if there was an I2C error
			// (Also see getError() below)
			
		boolean getTempControl(byte &res, boolean voltage, boolean heater);
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
			
		boolean setTempControl(byte res, boolean heater);
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
			
		boolean getFirmwareVer(byte &firmware);
			// Gets the Firmware Version of the chip
			// Returns true (1) if successful, false (0) if there was an I2C error
			// (Also see getError() below)
			
		boolean getTemperature(float &temperature, boolean mode);
			// Gets the Temperature data from the sensor
			// If mode = true(1), Hold Master Mode is used
			// If mode = false(0), No Hold Master Mode is used
			// Returns true (1) if successful, false (0) if there was an I2C error
			// (Also see getError() below)
			
		boolean getHumidity(float &humidity, boolean mode);
			// Gets the Humidity data from the sensor
			// If mode = true(1), Hold Master Mode is used
			// If mode = false(0), No Hold Master Mode is used
			// Returns true (1) if successful, false (0) if there was an I2C error
			// (Also see getError() below)
			
		boolean getOldTemperature(float &temperature);
			// Gets the Old Temperature data from the sensor
			// Returns true (1) if successful, false (0) if there was an I2C error
			// (Also see getError() below)
			
		uint8_t crc8(const uint8_t *data, int len);
			// Returns the CRC byte generated from the data
			
		byte getError(void);
			// If any library command fails, you can retrieve an extended
			// error code using this command. Errors are from the wire library: 
			// 0 = Success
			// 1 = Data too long to fit in transmit buffer
			// 2 = Received NACK on transmit of address
			// 3 = Received NACK on transmit of data
			// 4 = Other error
		
	private:
		
		boolean readByte(byte address, byte &value);
			// Reads a byte from a LTR303 address
			// Address: LTR303 address (0 to 15)
			// Value will be set to stored byte
			// Returns true (1) if successful, false (0) if there was an I2C error
			// (Also see getError() above)
	
		boolean writeByte(byte address, byte value);
			// Write a byte to a LTR303 address
			// Address: LTR303 address (0 to 15)
			// Value: byte to write to address
			// Returns true (1) if successful, false (0) if there was an I2C error
			// (Also see getError() above)

		boolean readUInt(byte address, unsigned int &value);
			// Reads an unsigned integer (16 bits) from a LTR303 address (low byte first)
			// Address: LTR303 address (0 to 15), low byte first
			// Value will be set to stored unsigned integer
			// Returns true (1) if successful, false (0) if there was an I2C error
			// (Also see getError() above)

		boolean read1ByteData(byte address1, byte address2, byte &value);
			// Reads a byte from a TH06 sensor when provided with 2 addresses
			// Address: TH06 address (0 to 15)
			// Value will be set to stored byte
			// Returns true (1) if successful, false (0) if there was an I2C error
			// (Also see getError() above)
			
		boolean read4ByteData(byte address1, byte address2, char (&value)[4]);
			// Reads an unsigned long (32 bits) from a TH06 address (high byte first)
			// Address: TH06 register address (0 to 15), high byte first
			// Value will be set to stored unsigned long
			// Returns true (1) if successful, false (0) if there was an I2C error
			// (Also see getError() above)
		
		byte _i2c_address;
		byte _error;
};

#endif