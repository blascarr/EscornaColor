// ColorSensor.h
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

ZGZMakerSpace - Blascarr Contribution
*/

//#include "Enums.h"
#include "EventManager.h"
#include "ColorSensor.h"


#define SERIALDEBUG Serial

//Pattern for DEBUG detection
#ifndef DEBUG_STARTCMD 
	#define  DEBUG_STARTCMD "{" 
#endif
#ifndef DEBUG_ENDCMD 
	#define  DEBUG_ENDCMD "}"
#endif
#ifndef DEBUG_SEPCM 
	#define  DEBUG_SEPCMD ","
#endif

#if defined(SERIAL_DEBUG)
	#define  DUMP(s, v)  { SERIALDEBUG.print(F(s)); SERIALDEBUG.print(v); }
	#define  DUMP_CMD(s, v)  { SERIALDEBUG.print(DEBUG_STARTCMD); SERIALDEBUG.print(F(s)); SERIALDEBUG.print(v); SERIALDEBUG.print(DEBUG_ENDCMD); }
	#define  DUMPS(s)    {  SERIALDEBUG.print(F(s)); }
	#define  DUMPS_CMD(s)    { SERIALDEBUG.print(DEBUG_STARTCMD); SERIALDEBUG.print(F(s)); SERIALDEBUG.print(DEBUG_ENDCMD);}
	#define  DUMPPRINTLN() { SERIALDEBUG.println();}

	#define  DUMPCAL(s, v)  { DUMP(s, v); }
	#define  DUMPSCAL(s)    { DUMPS(s); }
	#define  DUMPDEBUGCAL(s, v)  { DUMPCAL(s, v); }
	#define  DUMPSDEBUGCAL(s)    { DUMPSCAL(s); }
	#define  DUMPSAVAILABLE()    SERIALDEBUG.available()
	#define  DUMPCOLOR(code) { SERIALDEBUG.print(COLOR_STARTCMD);SERIALDEBUG.print(code);SERIALDEBUG.print(COLOR_ENDCMD);}
	#define  DUMPREADSTRING() {SERIALDEBUG.readString();}
	#define  DUMPREAD(str) { str = SERIALDEBUG.read();}
#else
	#define  DUMP(s, v)
	#define  DUMP_CMD(s, v) 
	#define  DUMPS(s)
	#define  DUMPS_CMD(s)
	#define  DUMPPRINTLN() 
	#define  DUMPCAL(s, v)
	#define  DUMPSCAL(s)
	#define  DUMPDEBUGCAL(s, v)
	#define  DUMPSDEBUGCAL(s)
	#define  DUMPSAVAILABLE() false
	#define  DUMPCOLOR(code)
	#define  DUMPREADSTRING()
	#define  DUMPREAD(str)
#endif
//////////////////////////////////////////////////////////////////////

extern EventManager* EVENTS;

//////////////////////////////////////////////////////////////////////

ColorSensor::ColorSensor(const Config* config)
{
    this->_config = config;

    _isPause = false;
}

void ColorSensor::init()
{	
	EVENTS->add(this);
	ColorSensor::setPins( _config->pin_S2 , _config->pin_S3, _config->pin_OUT, _config->pin_led );
	ColorSensor::nSamples( _config->nSamples  );
	ColorSensor::setRefreshTime( _config->refreshTime );
	ColorSensor::setFrequency( TCS3200_FREQ_HI );
	ColorSensor::LEDON( true );
}

//////////////////////////////////////////////////////////////////////

void ColorSensor::tick(uint32_t micros)
{
    _current_millis = micros / 1000;

	if (onChangeColor()){
		Serial.print( ColorSensor::readColor() );
	};
}

//////////////////////////////////////////////////////////////////////

void ColorSensor::setPins(uint8_t S2, uint8_t S3, uint8_t OUT, uint8_t LED ){ 
	ColorSensor::_S2 = S2; ColorSensor::_S3 = S3; ColorSensor::_OUT = OUT; ColorSensor::_LED = LED; 
	pinMode( ColorSensor::_S2, OUTPUT);
	pinMode( ColorSensor::_S3, OUTPUT);
	pinMode( ColorSensor::_LED, OUTPUT);
	pinMode( ColorSensor::_OUT, INPUT);
};

void  ColorSensor::setRefreshTime(unsigned long refreshTime){
	ColorSensor::refreshTime = refreshTime;
}

void  ColorSensor::setEEPROMaddress( uint8_t nEEPROM ){
	ColorSensor::_nEEPROM = nEEPROM;
}

void  ColorSensor::LEDON(bool ledON){
	digitalWrite(_LED, (ledON) ? HIGH : LOW );
	ColorSensor::_isPause = !ledON;
}

void  ColorSensor::setID(String ID){
	ID.toCharArray(ColorSensor::_ID, TCS_SIZENAME);
}


void ColorSensor::setFrequency( uint8_t f ){
	ColorSensor::_freqSet = f;
	/* set the sensor Frequency 
		|	Freq 	|	S0 		|	S1 		|
		|	OFF 	|	LOW 	|	LOW 	|
		|	LOW 	|	LOW 	|	HIGH 	|
		|	MEDIUM 	|	HIGH 	|	LOW 	|
		|	HIGH 	|	HIGH 	|	HIGH 	|
	*/
	switch (f){
		case TCS3200_FREQ_HI:  /*DUMPS("HI");*/  digitalWrite(_S0, HIGH);  digitalWrite(_S1, HIGH);  break;
		case TCS3200_FREQ_MID: /*DUMPS("MID");*/ digitalWrite(_S0, HIGH);  digitalWrite(_S1, LOW);   break;
		case TCS3200_FREQ_LO:  /*DUMPS("LO");*/  digitalWrite(_S0, LOW);   digitalWrite(_S1, HIGH);  break;
		case TCS3200_FREQ_OFF: /*DUMPS("OFF");*/ digitalWrite(_S0, LOW);   digitalWrite(_S1, LOW);   break;
		//default:  DUMP("Unknown freq option", f); break;
	}
}

void ColorSensor::setFilter(uint8_t f){
	/* set the sensor color filter 
		|	RGB 	|	S2 		|	S3 		|
		|	R 		|	LOW 	|	LOW 	|
		|	G 		|	HIGH 	|	HIGH 	|
		|	B 		|	LOW 	|	HIGH 	|
		|	X 		|	HIGH 	|	LOW 	|
	*/

	switch (f){
		case TCS3200_RGB_R:   digitalWrite(_S2, LOW);   digitalWrite(_S3, LOW);   break;
		case TCS3200_RGB_G:   digitalWrite(_S2, HIGH);  digitalWrite(_S3, HIGH);  break;
		case TCS3200_RGB_B:   digitalWrite(_S2, LOW);   digitalWrite(_S3, HIGH);  break;
		case TCS3200_RGB_X:   digitalWrite(_S2, HIGH);  digitalWrite(_S3, LOW);   break;
		//default:  DUMP("Unknown filter option", f); break;
	}
}


void ColorSensor::read(bool RGB) {
	ColorSensor::currentMillis = millis();
	if(ColorSensor::currentMillis-ColorSensor::oldMillis >= ColorSensor::refreshTime){
		
		ColorSensor::readRGB();
		ColorSensor::oldMillis  = ColorSensor::currentMillis;

	}
}

/*
	-------------------------------------------------------
	------------Non - Blocking Functions-------------------
	-------------------------------------------------------
*/
bool ColorSensor::onChangeColor(){
	//if ( !_isPause  ){
		//ColorSensor::readRGB();
		ColorSensor::read();
		int cli= ColorSensor::checkColor( &_rgb );

		if(cli != ColorSensor::_lastColor){

			ColorSensor::_lastColor = cli;
			#if SENDCOLOR 
				DUMPCOLOR( readColor() );
			#endif

			return true;
		}else{
			return false;
		}
	//}else{
	//	return false;
	//}

}

String ColorSensor::readColor(){
	return ColorSensor::_ct[ _lastColor ].name;
}

uint8_t ColorSensor::readColorID(){
	return ColorSensor::_lastColor;
}

sensorData ColorSensor::color(){
	sensorData sensorcolor;   

	for (int i = 0; i < RGB_SIZE; ++i){

		float value = 0;
		float fvalue = 0;
		int n = 0;

			for (int j = 0; j < _nSamples; ++j){
				ColorSensor::setFilter(i); 
				float color = pulseIn(_OUT, digitalRead(_OUT) == HIGH ? LOW : HIGH) ;

				// 1.5 value of threshold
				if (color/ColorSensor::_nSamples > fvalue / 1.5) {

					fvalue = (fvalue + color)/_nSamples;
					value = value + color;
					n++;
				}

				sensorcolor.value[i] = value/n;  

			}

		}

	/*DUMP(" Red: ", color.value[0]);
	DUMP(" Green: ", color.value[1]);
	DUMP(" Blue: ", color.value[2]);  */
	return sensorcolor;
}

void ColorSensor::voidRAW( sensorData *d ) {
	for (int i = 0; i < RGB_SIZE; ++i){
		d->value[i] = 0;
	}
}

colorData ColorSensor::readRGB() {
	ColorSensor::readRAW();
	colorData color = ColorSensor::raw2RGB();

	//DUMP(" Red : ",_rgb.value[TCS3200_RGB_R]);
	//DUMP(" Green : ",_rgb.value[TCS3200_RGB_G]);
	//DUMP(" Blue : ",_rgb.value[TCS3200_RGB_B]);
	return color;
}

sensorData ColorSensor::readRAW() {
	sensorData cl;
	sensorData rawcl;
	ColorSensor::voidRAW(&rawcl);

	rawcl = ColorSensor::color();
	_raw.value[TCS3200_RGB_R] = rawcl.value[0];
	_raw.value[TCS3200_RGB_G] = rawcl.value[1];
	_raw.value[TCS3200_RGB_B] = rawcl.value[2];
	_raw.value[TCS3200_RGB_X] = rawcl.value[3];

	//DUMP(" Red : ",_raw.value[TCS3200_RGB_R]);
	//DUMP(" Green : ",_raw.value[TCS3200_RGB_G]);
	//DUMP(" Blue : ",_raw.value[TCS3200_RGB_B]);
	//DUMP(" Clear : ",_raw.value[TCS3200_RGB_X]);
	//DUMPPRINTLN();

	return rawcl;
}

sensorData  ColorSensor::relativeColor(bool RGB){
	if (RGB){
		uint32_t sumcolor = _rgb.value[0]+_rgb.value[1]+_rgb.value[2];
		_relrgb.value[TCS3200_RGB_R] = _rgb.value[TCS3200_RGB_R]/sumcolor;
		_relrgb.value[TCS3200_RGB_G] = _rgb.value[TCS3200_RGB_G]/sumcolor;
		_relrgb.value[TCS3200_RGB_B] = _rgb.value[TCS3200_RGB_B]/sumcolor;
		//_relraw.value[TCS3200_RGB_X] = _raw.value[TCS3200_RGB_X]/sumcolor;

		//DUMP(" RelRed : ",_relrgb.value[TCS3200_RGB_R]);
		//DUMP(" RelGreen : ",_relrgb.value[TCS3200_RGB_G]);
		//DUMP(" RelBlue : ",_relrgb.value[TCS3200_RGB_B]);
		//DUMP(" RelClear : ",_relraw.value[TCS3200_RGB_X]);
		return _relrgb;
	}else{
		uint32_t sumcolor = _raw.value[0]+_raw.value[1]+_raw.value[2];
		_relraw.value[TCS3200_RGB_R] = _raw.value[TCS3200_RGB_R]/sumcolor;
		_relraw.value[TCS3200_RGB_G] = _raw.value[TCS3200_RGB_G]/sumcolor;
		_relraw.value[TCS3200_RGB_B] = _raw.value[TCS3200_RGB_B]/sumcolor;
		//_relraw.value[TCS3200_RGB_X] = _raw.value[TCS3200_RGB_X]/sumcolor;

		//DUMP(" RelRed : ",_relraw.value[TCS3200_RGB_R]);
		//DUMP(" RelGreen : ",_relraw.value[TCS3200_RGB_G]);
		//DUMP(" RelBlue : ",_relraw.value[TCS3200_RGB_B]);
		//DUMP(" RelClear : ",_relraw.value[TCS3200_RGB_X]);
		return _relraw;
	}
}

void ColorSensor::getRGB(colorData *rgb){
	if (rgb == NULL)
	return;

	//DUMPS("\ngetRGB");
	for (uint8_t i=0; i<RGB_SIZE; i++){
		rgb->value[i] = _rgb.value[i];
		//DUMP(" ", rgb->value[i]);
	}
}

void ColorSensor::getRaw(sensorData *d){
	// get the raw data of the current reading
	// useful to set dark and white calibration data
	if (d == NULL) return;

	//DUMPS("\ngetRAW");
	for (uint8_t i=0; i<RGB_SIZE; i++){
		d->value[i] = _raw.value[i];
		//DUMP(" ", d->value[i]);
	}
}

colorData ColorSensor::raw2RGB(void){
	// Exploiting linear relationship to remap the range 
	int32_t x;
	colorData color;
	for (uint8_t i=0; i<RGB_SIZE; i++){
		x = (_raw.value[i] - _darkraw.value[i]) * 255;
		x /= (_whiteraw.value[i] - _darkraw.value[i]);

		// copy results back into the global structures
		if (x < 0) color.value[i] = 0; 
		else if (x > 255) color.value[i] = 255;
		else color.value[i] = x;
	}
	_rgb = color;
	return color;
}

uint8_t ColorSensor::checkColor(colorData *rgb){
	int32_t   d;
	uint32_t  v, minV = 999999L;
	uint8_t   minI;

	for (uint8_t i=0; i< SIZECOLORS; i++){
		v = 0;
		for (uint8_t j=0; j<RGB_SIZE; j++){
			d = _ct[i].rgb.value[j] - rgb->value[j];
			v += (d * d);
		}
		if (v < minV) {
			minV = v;
			minI = i;
		}
		if (v == 0) break;   // perfect match, no need to search more
	}
	//DUMP("Color Checked : ", minI);
	//DUMPPRINTLN();
	return(minI);
}

/*
	-------------------------------------------------------
	---------------------CALIBRATION MODE--------------------------
	-------------------------------------------------------
*/


void ColorSensor::calibration(uint8_t nEEPROM){
	ColorSensor::setDarkCal();
	ColorSensor::setWhiteCal();
	DUMPREADSTRING();

	DUMPSDEBUGCAL("\nDo you want to save Calibration in EEPROM?");
	while(!DUMPSAVAILABLE()){
	}

	char readY;
	DUMPREAD(readY);
	DUMPDEBUGCAL("Char Read : ",readY);
	if (readY == 'Y'){
		DUMPDEBUGCAL("\nBlack and White Calibration saved in EEPROM Address:  ",ColorSensor::_nEEPROM);
		DUMPPRINTLN();
		ColorSensor::saveCal(nEEPROM);
	}

	DUMPREADSTRING();DUMPSDEBUGCAL("\nDo you want to set Color values (Y) or Load EEPROM Values (N)?");
	while(!DUMPSAVAILABLE()){
	}

	DUMPREAD(readY);
	DUMPDEBUGCAL("Char Read : ",readY);
	if (readY == 'Y'){
		DUMPSDEBUGCAL("\nSetting RGB Values");
		ColorSensor::setColorCal();
		DUMPREADSTRING();DUMPSDEBUGCAL("\nDo you want to save Calibration Colors in EEPROM?\n");
		while(!DUMPSAVAILABLE()){
		}

		DUMPREAD(readY);
		DUMPDEBUGCAL("Char Read : ",readY);
		if (readY == 'Y'){
	  		DUMPDEBUGCAL("\nColour table saved in EEPROM in EEPROM Address:  ", ColorSensor::_nEEPROM);
	  		ColorSensor::saveCT(ColorSensor::_nEEPROM);
		}else{
	  		DUMPSDEBUGCAL("\nNot saved in EEPROM");
		}
	}else if (readY == 'N'){
		DUMPSDEBUGCAL("\nEEPROM RGB Values");
		ColorSensor::loadCT( nEEPROM );
	}

}

void ColorSensor::setDarkCal(){
	sensorData darkcl;
	DUMPSDEBUGCAL(" BLACK Calibration ");
	DUMPPRINTLN();
	ColorSensor::voidRAW(&darkcl);
	bool sure= false;
	while (sure == false){

		while(!DUMPSAVAILABLE()){

		}
		DUMPREADSTRING();

		darkcl = ColorSensor::readRAW();
		DUMPSDEBUGCAL("RGB BLACK Values"); 

		String dataRGB = "";
		for (int i = 0; i < RGB_SIZE; ++i){
			dataRGB += darkcl.value[i];
			dataRGB += DEBUG_SEPCMD;
		}
		DUMPCAL("",dataRGB ); 
		DUMPPRINTLN();

		DUMPDEBUGCAL("",dataRGB);
		DUMPPRINTLN();
		DUMPSDEBUGCAL(" Are you sure this values are correct for BLACK Calibration? (Y/N)");
		while(!DUMPSAVAILABLE()){

		}
		DUMPPRINTLN();
		char chr;
		DUMPREAD(chr);
		DUMPDEBUGCAL("Char Read : ",chr);
		if (chr == 'Y'){
			_darkraw = darkcl;
			sure = true;
		}
	}
	DUMPPRINTLN();
	DUMPSDEBUGCAL(" End BLACK Calibration");
}


void ColorSensor::setWhiteCal(){
	sensorData whitecl;
	DUMPPRINTLN();
	DUMPSDEBUGCAL(" WHITE Calibration ");
	ColorSensor::voidRAW(&whitecl);
	bool sure= false;
	while (sure == false){

		while(!DUMPSAVAILABLE()){

		}
		DUMPREADSTRING();

		whitecl = ColorSensor::readRAW();
		DUMPSDEBUGCAL("RGB WHITE Values"); 

		String dataRGB = "";
		for (int i = 0; i < RGB_SIZE; ++i){
			dataRGB += whitecl.value[i];
			dataRGB += DEBUG_SEPCMD;
		}
		DUMPCAL("",dataRGB ); 
		DUMPDEBUGCAL("",dataRGB);
		DUMPPRINTLN();
		DUMPSDEBUGCAL(" Are you sure this values are correct for WHITE Calibration? (Y/N)");
		while(!DUMPSAVAILABLE()){

		}
		DUMPPRINTLN();
		char chr;
		DUMPREAD(chr);
		DUMPDEBUGCAL("Char Read : ",chr);
		if (chr == 'Y'){
			_whiteraw = whitecl;
			sure = true;
		}
	}

	DUMPSDEBUGCAL(" End WHITE Calibration");
}

void ColorSensor::setColorCal(){

	DUMPSDEBUGCAL(" Color Calibration ");

	while(!DUMPSAVAILABLE()){ }

	DUMPREADSTRING();
	for (int i = 0; i < SIZECOLORS; ++i){
		bool sure= false;
		while (sure == false){
			DUMPPRINTLN();
			DUMPDEBUGCAL("Calibration for color - ", _ct[i].name);
			DUMPPRINTLN();
			colorData colorcl = ColorSensor::readRGB();

			String dataRGB = "";
			for (int i = 0; i < RGB_SIZE; ++i){
				dataRGB += colorcl.value[i];
				dataRGB += DEBUG_SEPCMD;
			}
			DUMPCAL("",dataRGB );
			DUMPPRINTLN();
			DUMPSDEBUGCAL("Is Correct? ");
			while(!DUMPSAVAILABLE()){
			}

			char readY;
			DUMPREAD(readY);
			DUMPDEBUGCAL("Char Read : ",readY);
			DUMPPRINTLN();

			if (readY == 'Y'){
				_ct[i].rgb = colorcl;
				sure = true;
			}else if(readY == 'N'){
				//sure = true;
			}
			DUMPREADSTRING();
		}
	}
}

void  ColorSensor::saveCal(uint8_t nEEPROM){
	_nEEPROM = nEEPROM;
	int calWhiteAddress = _nEEPROM;
	int calDarkAddress = calWhiteAddress + sizeof(sensorData);
	EEPROM.put(calWhiteAddress, _whiteraw);
	EEPROM.put(calDarkAddress, _darkraw);

	DUMPS("\t White Calibration: ");
	for (uint8_t i=0; i<4; i++){
		DUMP(" RGB[ ", i);
		DUMP(" ] - ", _whiteraw.value[i]);
		DUMPS("\t");
	}
	DUMPS("\n \t Black Calibration: ");
	for (uint8_t i=0; i<4; i++){
		DUMP(" RGB[ ", i);
		DUMP(" ] - ", _darkraw.value[i]);
		DUMPS("\t");
	}

	DUMPS("\nSaved Calibration");
}

void  ColorSensor::loadCal(uint8_t nEEPROM){
	ColorSensor::loadBW(nEEPROM);
	ColorSensor::loadCT(nEEPROM);
}

void  ColorSensor::loadBW(uint8_t nEEPROM){
	_nEEPROM = nEEPROM;
	int calWhiteAddress = _nEEPROM;
	int calDarkAddress = calWhiteAddress + sizeof(sensorData);

	EEPROM.get(calWhiteAddress, _whiteraw);
	EEPROM.get(calDarkAddress, _darkraw);

	DUMPS("\n \t White Calibration: ");
	for (uint8_t i=0; i<4; i++){
		DUMP("RGB [",i);
		DUMP("] - ", _whiteraw.value[i]);
		DUMPS("\t");
	}
	DUMPS("\n \tBlack Calibration: ");
	for (uint8_t i=0; i<4; i++){
		DUMP("RGB [",i);
		DUMP("] - ", _darkraw.value[i]);
		DUMPS("\t");
	}
	DUMPS("\nCalibration Loaded ");
}

void  ColorSensor::saveCT(uint8_t nEEPROM ){
	int address = nEEPROM + 2*sizeof(sensorData);

	for (int i = 0; i < SIZECOLORS; ++i){
		EEPROM.put(address, _ct[i]);
		address += sizeof(colorTable);
	}
	ColorSensor::readCT();

	DUMPS("\n Color Table Calibration Saved ");
	DUMPPRINTLN();
}

void  ColorSensor::loadCT(uint8_t nEEPROM ){
	int address = nEEPROM + 2*sizeof(sensorData);

	for (int i = 0; i < SIZECOLORS; ++i){
		EEPROM.get(address, _ct[i]);
		address += sizeof(colorTable);
	}
	ColorSensor::readCT();

	DUMPS("\n Color Table Calibration Loaded ");
}

void  ColorSensor::readCT(){
	for (int i = 0; i < SIZECOLORS; ++i){
		DUMP("\nColor: ",_ct[i].name);
		for (uint8_t j=0; j<3; j++){
			DUMP("  -  ", _ct[i].rgb.value[j]);
		}
		DUMPS("\n");
	}
}
