/*
    File     draw.h
    Authors  Henry Hickman, Taran Jennison
    Date     15 October 2017
    Brief    Utilities for the PSR game.
*/




#ifndef DRAW_H
#define DRAW_H

#include "system.h"
#include "tinygl.h"
#include "../fonts/font5x7_1.h"


/**Displays one character on the screen
@param character to display
*/
void display_char(char c);

/**Displays a message and waits for a button press
@param pointer to a message to display
*/
void display_mess (char* message);

/**Displays a message and waits for a button press
@param char letter of arrow correspondnig with direciton to display
@param int is how far up the board to diplay the arrow.
*/
void display_arrow(char direction, int count);

#endif