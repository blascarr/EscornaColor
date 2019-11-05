// ColorSensor.h
//ZGZMakerSpace - Blascarr Contribution
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

#include "EventManager.h"
#include "ColorSensor.h"

#define SERIAL_DEBUG
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

//Pattern for CALIBRATION detection
#ifndef CAL_STARTCMD 
	#define  CAL_STARTCMD "<"
#endif
#ifndef CAL_ENDCMD
	#define  CAL_ENDCMD ">"
#endif
#ifndef CAL_SEPCMD
	#define  CAL_SEPCMD "|"
#endif

//Pattern for COLOR detection
#ifndef COLOR_STARTCMD 
	#define  COLOR_STARTCMD "#"
#endif
#ifndef COLOR_ENDCMD 
	#define  COLOR_ENDCMD "*"
#endif

#if defined(SERIAL_DEBUG)
	#define  DUMP(s, v)  { SERIALDEBUG.print(F(s)); SERIALDEBUG.print(v); }
	#define  DUMP_CMD(s, v)  { SERIALDEBUG.print(DEBUG_STARTCMD); SERIALDEBUG.print(F(s)); SERIALDEBUG.print(v); SERIALDEBUG.print(DEBUG_ENDCMD); }
	#define  DUMPS(s)    {  SERIALDEBUG.print(F(s)); }
	#define  DUMPS_CMD(s)    { SERIALDEBUG.print(DEBUG_STARTCMD); SERIALDEBUG.print(F(s)); SERIALDEBUG.print(DEBUG_ENDCMD);}
	#define  DUMPPRINTLN() { SERIALDEBUG.println();}

	#define  DUMPCAL(s, v)  { SERIALDEBUG.print(CAL_STARTCMD); SERIALDEBUG.print(F(s)); SERIALDEBUG.print(v);SERIALDEBUG.print(CAL_ENDCMD); }
	#define  DUMPSCAL(s)    { SERIALDEBUG.print(CAL_STARTCMD); SERIALDEBUG.print(F(s)); SERIALDEBUG.print(CAL_ENDCMD); }
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

/**
 * \Color Sensor TCS3200 for color detection.
 * \author @blascarr
 */

ColorSensor::ColorSensor(const Config* config)
{
    this->_config = config;
    _isON = true;
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

//////////////////////////// Event Habdling //////////////////////////////////////////

void  ColorSensor::moveExecuted ( MOVE move ){
	
	ColorSensor::read();
	int cli= ColorSensor::checkColor( &_rgb );

	if ( COLORACTION [ cli ].onChange ){
		//Accion cuando encuentra nuevo color 
		if(cli != ColorSensor::_lastColor){
			ColorSensor::_lastColor = cli;
			COLORACTION [ getColorID() ].action();
		};
		//if ( onChangeColor() ){
			
		//};
	}else{	
		//Accion ejecutada siempre que finaliza un movimiento
		ColorSensor::readColor();
		COLORACTION [ getColorID() ].action();
	}
	DUMPCOLOR( ColorSensor::getColor() );

}


void  ColorSensor::buttonLongReleased(BUTTON button){
	if ( button == BUTTON_DOWN){
		ColorSensor::calibration(0);
	}

	if ( button == BUTTON_UP){
		ColorSensor::loadCal(0);
	}

	if ( button == BUTTON_LEFT){
		ColorSensor::readColor();

		//Serial.print( ColorSensor::getColor() );
		DUMPCOLOR( ColorSensor::getColor() );
	}
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

void  ColorSensor::setEEPROMaddress( uint16_t nEEPROM ){
	ColorSensor::_nEEPROM = nEEPROM;
}

void  ColorSensor::LEDON(bool ledON){
	digitalWrite(_LED, (ledON) ? HIGH : LOW );
	ColorSensor::_isON = ledON;
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


/*
	-------------------------------------------------------
	------------Non - Blocking Functions-------------------
	-------------------------------------------------------
*/

void ColorSensor::read() {
	ColorSensor::currentMillis = millis();
	if(ColorSensor::currentMillis-ColorSensor::oldMillis >= ColorSensor::refreshTime){
		ColorSensor::LEDON(true);
		ColorSensor::readRGB();
		ColorSensor::LEDON(false);
		ColorSensor::oldMillis  = ColorSensor::currentMillis;

	}
}

bool ColorSensor::onChangeColor(){
	if ( ColorSensor::_isON  ){
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
	}else{
		return false;
	}

}

void ColorSensor::readColor(){
	ColorSensor::read();
	int cli= ColorSensor::checkColor( &_rgb );
	ColorSensor::_lastColor = cli;
}

String ColorSensor::getColor(){
	return ColorSensor::_ct[ _lastColor ].name;
}

uint8_t ColorSensor::getColorID(){
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
		for (uint8_t j=0; j< RGB_SIZE; j++){
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

	DUMPS_CMD("Do you want to save Calibration in EEPROM?");
	while(!DUMPSAVAILABLE()){
	}

	char readY;
	DUMPREAD(readY);
	DUMP("Char Read : ",readY);
	if (readY == 'Y'){
		DUMP_CMD("Black and White Calibration saved in EEPROM Address:  ",ColorSensor::_nEEPROM);
		DUMPPRINTLN();
		ColorSensor::saveCal(nEEPROM);
	}

	DUMPREADSTRING();DUMPS_CMD("Do you want to set Color values (Y) or Load EEPROM Values (N)?");
	while(!DUMPSAVAILABLE()){
	}

	DUMPREAD(readY);
	DUMP("Char Read : ",readY);
	DUMPPRINTLN();
	if (readY == 'Y'){
		DUMPS("Setting RGB Values");
		ColorSensor::setColorCal();
		DUMPREADSTRING();DUMPS_CMD(" Do you want to save Calibration Colors in EEPROM? ");
		while(!DUMPSAVAILABLE()){
		}

		DUMPREAD(readY);
		DUMP("Char Read : ",readY);//DUMPPRINTLN();
		if (readY == 'Y'){
	  		DUMP_CMD("Colour table saved in EEPROM in EEPROM Address:  ", ColorSensor::_nEEPROM);
	  		ColorSensor::saveCT(ColorSensor::_nEEPROM);
		}else{
	  		DUMPS_CMD("Not saved in EEPROM");
		}
		DUMPPRINTLN();
	}else if (readY == 'N'){
		DUMPS_CMD("EEPROM RGB Values");
		ColorSensor::loadCT( nEEPROM );
	}
	DUMPPRINTLN();
}

void ColorSensor::BWCal( bool Black ){
	sensorData BWcl;
	String BWLabel;
	if( Black ){
		BWLabel = "BLACK";
	}else{
		BWLabel = "WHITE";
	}
	DUMP("Calibration ", BWLabel);
	DUMPPRINTLN();
	ColorSensor::voidRAW(&BWcl);
	bool sure= false;
	while (sure == false){

		while(!DUMPSAVAILABLE()){

		}
		DUMPREADSTRING();

		BWcl = ColorSensor::readRAW();
		DUMP( "RGB ", BWLabel );
		DUMPS (" Values "); 

		String dataRGB = "";
		for (int i = 0; i < RGB_SIZE; ++i){
			dataRGB += BWcl.value[i];
			dataRGB += DEBUG_SEPCMD;
		}
		DUMPCAL("",dataRGB ); 
		DUMPPRINTLN();

		DUMP_CMD(" Are you sure this values are correct for Calibration? (Y/N) ",BWLabel);
		while(!DUMPSAVAILABLE()){

		}
		DUMPPRINTLN();
		char chr;
		DUMPREAD(chr);
		DUMP("Char Read : ",chr);DUMPPRINTLN();
		if (chr == 'Y'){
			if( Black ){
				_darkraw = BWcl;
			}else{
				_whiteraw = BWcl;
			}
			sure = true;
		}
	}
	DUMP( "End Calibration for ", BWLabel );
	//DUMP_CMD(" End BLACK Calibration");
	DUMPPRINTLN();
}

void ColorSensor::setDarkCal(){
	BWCal( true );
}


void ColorSensor::setWhiteCal(){
	BWCal( false );
}

void ColorSensor::setColorCal(){

	DUMPS_CMD(" Color Calibration ");

	while(!DUMPSAVAILABLE()){ }

	DUMPREADSTRING();
	for (int i = 0; i < SIZECOLORS; ++i){
		bool sure= false;
		while (sure == false){
			DUMPPRINTLN();
			DUMP_CMD("Calibration for color - ", _ct[i].name);
			DUMPPRINTLN();
			colorData colorcl = ColorSensor::readRGB();

			String dataRGB = "";
			for (int i = 0; i < RGB_SIZE; ++i){
				dataRGB += colorcl.value[i];
				dataRGB += DEBUG_SEPCMD;
			}
			DUMPCAL("",dataRGB );
			DUMPPRINTLN();
			DUMPS_CMD("Is Correct? ");
			while(!DUMPSAVAILABLE()){
			}

			char readY;
			DUMPREAD(readY);
			DUMP("Char Read : ",readY);
			//DUMPPRINTLN();

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

	DUMPS_CMD("Calibration Saved");DUMPPRINTLN();
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
	DUMPPRINTLN();
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
