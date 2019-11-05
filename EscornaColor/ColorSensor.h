// ColorSensor.h
//ZGZMakerSpace - Blascarr Contribution
/*
/*

Copyright (C) 2014-2019 Escornabot - http://escornabot.com

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
USA.

See LICENSE.txt for details

*/

#ifndef _COLOR_SENSOR_H
#define _COLOR_SENSOR_H
	
	#include "EventListener.h"
	#include "Arduino.h"
	#include <EEPROM.h>

	#define RGB_SIZE  4 // array index counter limit
	#define TCS_SIZENAME 10
	#define SIZECOLORS 8

	// Frequency setting defines
	#define TCS3200_FREQ_HI  0 // 100% prescaler
	#define TCS3200_FREQ_MID 1 // 20%
	#define TCS3200_FREQ_LO  2 // 2%
	#define TCS3200_FREQ_OFF 3 // off

	// Indices for any RGB data/ filter selection
	#define TCS3200_RGB_R  0
	#define TCS3200_RGB_G  1
	#define TCS3200_RGB_B  2
	#define TCS3200_RGB_X  3 // 'Clear' filter selection

	typedef struct{
		float value[RGB_SIZE];  // Raw data from the sensor
	} sensorData;

	typedef struct{
		uint8_t value[RGB_SIZE]; // the evaluated colour data (RGB value 0-255 or other)
	} colorData;
	typedef struct	{
		char    name[TCS_SIZENAME];  // color name 
		colorData rgb;    // RGB value
	} colorTable;
	
	typedef void ( *_f_COLOR )( );

	typedef struct{
		_f_COLOR action = NULL ;
		bool onChange = false;
	} colorAction;

	/**
	* \Color Sensor TCS3200 for color detection.
	* \author @blascarr
	*/
	class ColorSensor : public EventListener {
		public:
			typedef struct {
				uint8_t pin_led;
				uint8_t pin_S2;
				uint8_t pin_S3;
				uint8_t pin_OUT;
				uint8_t nSamples;
				uint8_t refreshTime;
				uint16_t EEPROM_DIR;
			} Config;
			
			colorTable _ct[SIZECOLORS]={
				colorTable {"WHITE", {255, 255, 255} },
				colorTable {"BLACK", {0, 0, 0} },
				colorTable {"RED", {250, 175, 190} },
				colorTable {"GREEN", {180, 220, 195} },
				colorTable {"BLUE", {150, 190, 220} },
				colorTable {"YELLOW", {250, 250, 225} },
				colorTable {"ORANGE", {240, 200, 180} },
				colorTable {"BROWN", {190, 170, 150} }
			};

			colorAction COLORACTION [SIZECOLORS];
			
			ColorSensor(const Config* config);

			virtual void init();
			
			/**
			* Sensor Color Constructor
			*/
	    	//ColorSensor();
	    	ColorSensor(uint8_t S2, uint8_t S3, uint8_t OUT, uint8_t LED );
	    	void  setPins(uint8_t S2, uint8_t S3, uint8_t OUT, uint8_t LED );
			void  setID(String ID);
			
			void  LEDON(bool ledON);
			void  nSamples(int nSamples){ _nSamples = nSamples;}
			void  setEEPROMaddress( uint16_t nEEPROM );
			
			void  voidRAW(sensorData *d);

			void  setRefreshTime(unsigned long refreshTime);
			void  setInterrupt(bool ON);

			void  setFilter(uint8_t f); // set the photodiode filter
			void  setFrequency(uint8_t f);// set frequency prescaler - default 100%	
			void  setRGBMode(bool _RGBMode);// set RGB Mode (true) or RAW Mode (false) in readings	

			void read();      // synchronously non-blocking reading value
			void readColor();
			bool onChangeColor();
			sensorData  color();	//Single Reading

			sensorData readRAW();      // Read RAW Values
			colorData readRGB();      // Read RGB Values
			
			void sendColor();      
			String getColor();
			uint8_t getColorID();

			//Events for Calibration
			void  BWCal( bool Black );
			void  setDarkCal();
			void  setWhiteCal();
			void  calibration(uint8_t nEEPROM = 0);
			void  setColorCal();

			colorData  raw2RGB(void);  // convert raw data to RGB
			uint8_t checkColor(colorData *rgb);

			//EEPROM Saving Values
			void  saveCal(uint8_t nEEPROM = 0);
			void  loadCal(uint8_t nEEPROM = 0);
			void  loadBW(uint8_t nEEPROM = 0);
			void  saveCT(uint8_t nEEPROM = 0);
			void  loadCT(uint8_t nEEPROM = 0);
			void  readCT();
		protected:
			void moveExecuted ( MOVE move );
			//void buttonPressed(BUTTON button);
			void buttonLongReleased(BUTTON button);

			/**
			* Runs the execution thread. This mode pause each step movement. Reserved for Developers and improvement.
			*/
			//virtual void tick(uint32_t micros);

		private:
			const Config* _config;

			uint8_t   _OUT;    // output enable pin
			uint8_t   _S0, _S1; // frequency scaler
			uint8_t   _S2, _S3; // photodiode filter selection
			uint8_t   _LED;
			uint8_t   _freqSet; 
			uint8_t   _nEEPROM=0;
			int 	  _lastColor = 0;
			int	      _nSamples = 40;
			char	  _ID[TCS_SIZENAME];

			bool _isON = true;

			unsigned long currentMillis,oldMillis, _current_millis;
			unsigned long refreshTime= 200; 

			sensorData  _raw;    // current raw sensor reading
			
			//Calibration white and Black
			sensorData  _relraw;    // current relative raw sensor reading
			sensorData  _darkraw;    // Dark Calibration values
			sensorData  _whiteraw;    // White Calibration values

			colorData _rgb;   // colour based data for current reading
			sensorData  _relrgb;    // current relative raw sensor reading
	};

#endif
