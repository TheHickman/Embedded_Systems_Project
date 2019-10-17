/*
    File     draw.h
    Authors  Henry Hickman, Taran Jennison
    Date     14 October 2019
    Brief    H file for drawing on the ciruct board
*/




#ifndef DRAW_H
#define DRAW_H

#include "system.h"
#include "tinygl.h"
#include "../fonts/font5x7_1.h"
#include <stdlib.h>



/**Displays one character on the screen
@param character to display
*/
void display_char(char c);

/**Displays a message and waits for a button press
@param char letter of arrow correspondnig with direciton to display
@param int is how far up the board to diplay the arrow.
*/
void display_arrow(char direction, int count);

/**Displays a message and waits for a button press
@param pointer to a message to display
*/
void display_mess (char* message);

/**Displays the score
@param int score (the score to be displayed)
*/
void display_score(int score);

#endif
