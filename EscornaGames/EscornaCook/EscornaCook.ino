// EscornaCook.ino
#include "Bot.h"

/*REGLAS
Sacamos una carta y nos ha tocado un tipo de comida.  

Por ejemplo, la Pizza requiere de los siguientes ingredientes:
  - 0 Bandeja Horno - Negra
  - 1 Harina - Naranja
  - 2 Agua - Azul
  - 3 Verdura - Verde
  - 4 Queso - Amarillo

El objetivo es buscar cada uno de los ingredientes asociados a los colores en orden para hacer nuestra comida para poder cocinarlo. 
Para ejecutar los movimientos, debemos tirar un dado y solo podremos introducir una secuencia de movimientos con el número del dado hasta acercarnos en orden a los colores.

  Cuando se hayan conseguido todos los ingredientes, nos podremos dirigir a la cocina para preparar nuestra comida en el horno.
  
  - END Cocinar - Rojo

  El primero que llegue gana
*/

Bot ESCORNABOT;

bool ingredients[5];

void getIngredient(int n ){
  if( !ingredients[n] ){
    ingredients[n] = true;
    ESCORNABOT.tick_tone();
  }
}

void horno(){
  getIngredient(0);
  if(ingredients[0] == true ){
    Serial.println("Horno");
  }
}

void harina(){
  if(ingredients[0] == true ){
    getIngredient(1);
    Serial.println("Harina");
  }
  
}

void agua(){
  if(ingredients[1] == true ){
    getIngredient(2);
     Serial.println("Agua");
  }
 
}

void verdura(){
  if(ingredients[2] == true ){
    getIngredient(3);
    Serial.println("Verdura");
  }
  
}

void queso(){
  if(ingredients[3] == true ){
    getIngredient(4);
    Serial.println("Queso");
  }
  
}

void cocinar(){
  bool checkingredients = false;
  for(int i = 0; i <= 5 ; i++){
    checkingredients &= ingredients[i];
    if( checkingredients == false ){
      break;
    }
  }
  if( checkingredients ){
    ESCORNABOT.win();
    Serial.println("WIN");
  }
}

//////////////////////////////////////////////////////////////////////

void setup()
{
    ESCORNABOT.init();
    ESCORNABOT.load_colortable();

    //Color Actions Define
    
    ESCORNABOT.color_action( BLACK , horno );
    ESCORNABOT.color_action( RED , cocinar );
    ESCORNABOT.color_action( ORANGE , harina );
    ESCORNABOT.color_action( GREEN , verdura );
    ESCORNABOT.color_action( YELLOW , queso );
    ESCORNABOT.color_action( BLUE , agua );

    //Brown and White not working
    ESCORNABOT.color_action( WHITE , aux_void );
    ESCORNABOT.color_action( BROWN , aux_void );
    
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
