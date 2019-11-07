// Escornabot.ino

#include "Bot.h"

//////////////////////////////////////////////////////////////////////
/*
  Reglas del juego.

  Nuestro robot debe ir desde la casilla de color naranja, hasta la casilla de color Verde.
  Pero el tablero aún no está construido completamente. 
  Solo hay unas cuantas casillas que son las trampas que deberemos evitar.
  El resto de colores tienen programados diferentes movimientos.
  Nuestra misión es descubrir que movimiento se corresponde con cada color para llegar desde la salida hasta la meta de forma directa.

  Os dejamos un ejemplo de tablero 

  ////////////////////////////////////////////////////
  |       |       |       |       |       |       |
  |       |       |       |       |       |       |
  |       |       |       |       |       |       |
  ////////////////////////////////////////////////////
  |       |       |       |       |       |       |
  |       |       |   R   |       |   R   |   V   | (META)
  |       |       |       |       |       |       |
  ////////////////////////////////////////////////////  
  |       |       |       |       |       |       |
  |       |   R   |       |       |   R   |       |
  |       |       |       |       |       |       |
  ////////////////////////////////////////////////////
  |       |       |       |       |       |       |
  |SALIDA |       |       |       |       |       |
  |       |       |       |       |       |       |
  ////////////////////////////////////////////////////
  /
*/

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
    ESCORNABOT.color_action( ORANGE , aux_void);
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
