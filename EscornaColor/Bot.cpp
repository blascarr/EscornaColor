// Bot.cpp
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

#include "Bot.h"
#include "Escornabot.h"

//////////////////////////////////////////////////////////////////////

void Bot::init()
{
    // init engine
    ENGINE->init();

    // init button set
    #if USE_BUTTONS
    BUTTONS->init();
    #endif

    // early console
    Serial.begin(9600);
    Serial.println(FIRMWARE_VERSION);
    Serial.setTimeout(300);
    delay(100);

    // init bluetooth
    #if USE_BLUETOOTH
    BLUETOOTH->init();
    #endif

    // init buzzer
    #if USE_BUZZER
    BUZZER.init();
    BUZZER.beep();
    #endif

    #if USE_SIMPLE_LED
    SIMPLE_LED.init();
    #endif

    #if USE_KEYPAD_LEDS
    KEYPAD_LEDS.init();
    #endif
    
    //Init ColorSensor
    #if USE_COLORSENSOR
        TCS.init();
    #endif
    
    // restore last program
    #if USE_PERSISTENT_MEMORY
    PROGRAM->load();
    #endif

    EVENTS->add(this);
}

//////////////////////////////////////////////////////////////////////

void Bot::loop()
{
    EVENTS->indicateTick(micros());

    if (!ENGINE->isExecuting()) delay(10);

}

//////////////////////////////////////////////////////////////////////

void Bot::buttonPressed(BUTTON button)
{
    #if USE_SIMPLE_LED
    SIMPLE_LED.setStatus(true);
    #endif
}

//////////////////////////////////////////////////////////////////////

void Bot::buttonReleased(BUTTON button)
{
    #if USE_SIMPLE_LED
    SIMPLE_LED.setStatus(false);
    #endif

    if (ENGINE->isExecuting())
    {
        ENGINE->cancelExecution();
        return;
    }

    switch (button)
    {
        case BUTTON_UP:
            _storeMove(MOVE_FORWARD);
            break;

        case BUTTON_RIGHT:
            _storeMove(MOVE_RIGHT);
            break;

        case BUTTON_DOWN:
            _storeMove(MOVE_BACKWARD);
            break;

        case BUTTON_LEFT:
            _storeMove(MOVE_LEFT);
            break;

        case BUTTON_GO:
            _go();
            break;

        case BUTTON_RESET:
            PROGRAM->clear();
            break;
    }
}

//////////////////////////////////////////////////////////////////////

void Bot::buttonLongReleased(BUTTON button)
{
    #if USE_SIMPLE_LED
    SIMPLE_LED.setStatus(false);
    #endif

    if (ENGINE->isExecuting())
    {
        ENGINE->cancelExecution();
        return;
    }

    switch (button)
    {
        case BUTTON_RIGHT:
            _storeMove(MOVE_ALT_RIGHT);
            break;

        case BUTTON_DOWN:
            _storeMove(MOVE_PAUSE);
            break;

        case BUTTON_LEFT:
            _storeMove(MOVE_ALT_LEFT);
            break;

        case BUTTON_GO:
            _next_game_mode();
            break;
    }
}

//////////////////////////////////////////////////////////////////////

void Bot::programFinished()
{
    #if PROGRAM_RESET_ALWAYS
    PROGRAM->clear();
    #endif

    #if USE_BUZZER
    BUZZER.playRttl(PROGRAM_FINISHED_RTTL);
    #endif

    if (_game_mode == GAME_MODE_GRID_90 && !ENGINE->isAligned(90))
    {
        // advise next advance is expected to be a diagonal:
        #if USE_SIMPLE_LED
        SIMPLE_LED.setStatus(true);
        #elif USE_KEYPAD_LEDS
        KEYPAD_LEDS.setLed(BUTTON_RIGHT, true);
        KEYPAD_LEDS.setLed(BUTTON_LEFT, true);
        #endif
    }
}

//////////////////////////////////////////////////////////////////////

void Bot::programAborted(uint8_t executed, uint8_t total)
{
    #if PROGRAM_RESET_ALWAYS
    PROGRAM->clear();
    #endif
}

//////////////////////////////////////////////////////////////////////

void Bot::_go()
{
    // preloaded program
    if (PROGRAM->getMoveCount() == 0 && _total_programs == 0)
    {
        PROGRAM->addProgram(PROGRAM_ESCORNA_GREETING);
    }

    // only with movements
    if (PROGRAM->getMoveCount() > 0)
    {
        #if USE_PERSISTENT_MEMORY
        // save the movement program in EEPROM
        PROGRAM->save();
        #endif

        // let user to release the 'go' button before the action
        delay(DELAY_BEFORE_GO);

        ENGINE->execute(PROGRAM, AFTER_MOVEMENT_PAUSE, POV_ESCORNABOT);
        _total_programs++;
    }
}

//////////////////////////////////////////////////////////////////////

void Bot::_storeMove(MOVE move)
{
    if (PROGRAM->getMoveCount() < MOVE_LIMIT)
    {
        PROGRAM->addMove(move);
    }
    else
    {
        // memory is full
        _go();
    }
}
//////////////////////////////////////////////////////////////////////

void Bot::move(MOVE move)
{
    _storeMove( move );
    //_go();
}

void Bot::endGame( bool success ){
    if( success ){
        KEYPAD_LEDS.setAllLed( true );
        BUZZER.playRttl( ":d=8,o=6,b=400:c,e,g,e,c,g,e,g,c,g,c,e,c,g,e,g,e,c," );
    }else{
        BUZZER.playRttl( ":d=4,o=7,b=125:e,d#,e,d#,e,b#,d,c,8a," );
    }
    _game_mode = GAME_OVER;
}

//////////////////////////////////////////////////////////////////////

void Bot::_next_game_mode()
{
    // select the next game mode (3 modes currently available)
    ++_game_mode %= 3;

    switch (_game_mode)
    {
        case GAME_MODE_GRID_90:
            PROGRAM->setTurnDegrees(90);
            PROGRAM->setAltTurnDegrees(45);
            ENGINE->setSquareDiagonals(true);
            break;

        case GAME_MODE_GRID_60:
            PROGRAM->setTurnDegrees(60);
            PROGRAM->setAltTurnDegrees(120);
            ENGINE->setSquareDiagonals(false);
            break;
        case GAME_MODE_COLOR:
            
            
            break;
    }

    EVENTS->indicateGameModeSelected(_game_mode);
}

void Bot::color_action ( COLORS color, void *f(), bool on_Change ) {
    TCS.COLORACTION[ color ].action = f;
    TCS.COLORACTION[ color ].onChange = on_Change;
}

void Bot::load_colortable ( uint16_t nEEPROM  ){
    TCS.loadCal( nEEPROM );
}

void Bot::color_calibration( uint16_t nEEPROM  ){
    TCS.calibration( nEEPROM );
}

void Bot::set_win_tone( char win_tone ){
    _win_tone = &win_tone;
}

void Bot::set_over_tone( char over_tone ){
    _over_tone = &over_tone;
}

//////////////////////////////////////////////////////////////////////

// EOF
