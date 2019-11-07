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
int lives = 4;


void leftforward(){
  ESCORNABOT.move( MOVE_FORWARD );
  ESCORNABOT.move( MOVE_LEFT );
  Serial.println("LEFT - FORWARD");
}

void rightforward(){
  ESCORNABOT.move( MOVE_BACKWARD );
  ESCORNABOT.move( MOVE_RIGHT );
  Serial.println("LEFT - RIGHT");
}

void forward(){
  ESCORNABOT.move( MOVE_FORWARD );
  Serial.println("FORWARD");
}

void winner(){
  ESCORNABOT.win();
  Serial.println(" YOU WIN ");
}

void randomMove(){
    ESCORNABOT.move( random( 8 ) );
    Serial.println(" RANDOM MOVE ");
}

void warning(){
    damage();
    randomMove();
    
}

void damage(){
  lives -= 1;
  Serial.print("Lives: ");Serial.println(lives);
  if (lives <= 0 ){
    ESCORNABOT.game_over();
    Serial.println("GAME OVER");
  }
}
void yellow(){
 
}


//////////////////////////////////////////////////////////////////////

void setup()
{
    ESCORNABOT.init();
    ESCORNABOT.load_colortable();

    //Color Actions Define
    ESCORNABOT.color_action( WHITE , forward );
    ESCORNABOT.color_action( BLACK , rightforward);
    ESCORNABOT.color_action( RED , damage );
    
    //GREEN change non consecutively
    ESCORNABOT.color_action( GREEN , winner );
    //YELLOW need to change from consecutive yellow colors
    ESCORNABOT.color_action( YELLOW , yellow );
    
    randomSeed(analogRead(A5));
    ESCORNABOT.color_action( BLUE , leftforward );

    ESCORNABOT.color_action( BROWN , randomMove);
    ESCORNABOT.color_action( ORANGE , warning);
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
