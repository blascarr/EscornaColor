// Escornabot.ino
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

//////////////////////////////////////////////////////////////////////
// instance

Bot ESCORNABOT;
int lives = 2;


void white(){
  ESCORNABOT.move( MOVE_BACKWARD );
  Serial.println("COLOR White");
}

void black(){
  ESCORNABOT.move( MOVE_FORWARD );
  Serial.println("COLOR Black");
}

void red(){
  ESCORNABOT.win();
}

void blue(){
    ESCORNABOT.move( random( 8 ) );
}

void green(){
  lives -= 1;
  Serial.print("Lives: ");Serial.println(lives);
  if (lives <= 0 ){
    ESCORNABOT.game_over();
    Serial.println("GAME OVER");
  }
}
void yellow(){
  lives -= 1;
  Serial.print("Lives: ");Serial.println(lives);
  if (lives <= 0 ){
    ESCORNABOT.game_over();
    Serial.println("GAME OVER");
  }
}


//////////////////////////////////////////////////////////////////////

void setup()
{
    ESCORNABOT.init();
    //ESCORNABOT.load_colortable();

    //Color Actions Define
    ESCORNABOT.color_action( WHITE , white );
    ESCORNABOT.color_action( BLACK , black );
    ESCORNABOT.color_action( RED , red );
    
    //GREEN change non consecutively
    ESCORNABOT.color_action( GREEN , green, true );
    //YELLOW need to change from consecutive yellow colors
    ESCORNABOT.color_action( YELLOW , yellow );
    
    //random movements for Blue color
    randomSeed(analogRead(5));
    ESCORNABOT.color_action( BLUE , blue );

    //Brown and Orange not working
    ESCORNABOT.color_action( BROWN , aux_void );
    ESCORNABOT.color_action( ORANGE , aux_void );
}

//////////////////////////////////////////////////////////////////////

void loop()
{
    ESCORNABOT.loop();
}


void aux_void(){
  
}
//////////////////////////////////////////////////////////////////////


// EOF
