# EscornaColor
Version de Escronabot con sensor de color **TCS3200**.

El **sensor de color TCS3200** añade formatos de juego interesantes al famoso modelo de [**Escornabot**](https://escornabot.com/web/es).

Para usar el sensor de color, es necesario sustituir la pieza del alojamiento para la canica con el siguiente [modelo STL](https://www.thingiverse.com/thing:3934783) publicado en [Thingiverse](https://www.thingiverse.com/thing:3934783).

<p align="center">
	<img  src="src/TCS3200_BottomSide.png" height="250"/>
	<img  src="src/TCS3200_Bottom.png" height="250"/>
	<img  src="src/TCS3200_Side.png" height="250"/>
</p>

El sensor tiene distintos pines, de los cuales los más importantes son el pin S2, S3 y OUT. El pin de LED nos servirá solamente para encender y apagar el módulo.
- S2 --> D11
- S3 --> D12
- OUT --> D13
- LED --> A6
- S0 --> VCC
- S1 --> VCC

Este sensor de color viene preparado para reconocer **8 colores** por defecto que son:

- BLANCO
- NEGRO
- ROJO
- VERDE
- AZUl
- AMARILLO
- NARANJA
- MARRON

Aunque se pueden añadir más, recomendamos utilizar variantes de colores facilmente diferenciables, ya que tonos similares pueden dar lugar a errores.

El sensor de color requiere de una **calibración inicial** para poder clasificar los colores correctamente. 

# [Calibración del sensor de color TCS3200](http://blascarr.com/lessons/tcs3200-color-sensor-calibration/)

### Calibración de intensidades BLANCO - NEGRO
Primero hay que calibrar las lecturas con respecto al color blanco y el color negro que establecen los límites de lectura.

### Calibración de la tabla de colores
Acto seguido se requiere de una calibración de una tabla de 8 colores que se almacenan en la memoria EEPROM.

Esta calibración se puede realizar a través del monitor serie o por Bluetooth a través de una aplicación.

# Formatos de juego a desarrollar.

- Cuando se alcanza un color, se ejecutan las acciones de movimiento hasta llegar al objetivo. Juego de la hormiga. [Hormiga de Langton Simplificado](https://es.wikipedia.org/wiki/Hormiga_de_Langton)
- El **EscornaColor** tiene un número de vidas limitado. Los colores pueden quitar vidas. El objetivo es llegar al objetivo vivo.
- A través de una aplicación se pueden ver los colores y movimientos del Escornabot. Sin ver el tablero ni al EscornaColor hay que dibujar el tablero original.

- Programación libre de escornabot con distintas dinámicas en base a las lecturas del sensor de color.

Esta extensión se basa en el desarrollo de una librería para módulos TCS3200 accesible en el siguiente enlace.

- [Introducción al color en robótica](http://blascarr.com/lessons/introduccion-al-color/)