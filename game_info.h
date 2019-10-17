/*
    File     game_info.c
    Authors  Henry Hickman, Taran Jennison
    Date     15 October 2019
    Brief    H file for all functions that have game_info passed into them
*/

#ifndef GAME_INFO_H
#define GAME_INFO_H

#include "system.h"
#include "timer.h"
#include "navswitch.h"
#include "ir_uart.h"
#include "tinygl.h"
#include "pacer.h"
#include "draw.h"
#include <stdlib.h>

typedef struct game_info {
    char arrow;                 //what arrow the game is currently on
    int randomIndex;            //"random" number used to "randomise" arrow choice
    int score;                  //score on this UCFK
    uint16_t time_gap;          //number of ticks per task/arrow position
    uint16_t total_time_loop;   //number of ticks per arrow (ie how often arrow is changed)
    uint8_t p1status;           //flag for player number. 0 for p2, 1 for p1
    uint8_t switched;           //flag to prevent scoring multiple times off one arrow
    int speed;                  //speed is selected by user from 1 to 5
} game_info;

/**Waits for given number of ticks (on a 600hz cycle)
@param int ticks, the number of ticks to wait
*/
void wait(int ticks);

/**Gets the score based on the count and information in the game_info struct
@param uint16_t the count of when the button was pressed
@param game_info pointer the struct containing information for calculation
*/
int get_score(uint16_t count, game_info* game);

/**Switches to next arrow in sequence
@param game_info pointer contains the previous arrow
*/
void switch_arrow(game_info* game);

/**Checks if input if correct
@param game_info pointer contains the arrow meant to pushed
*/
int check_input(game_info* game);

/**Updates the speed displayed
@param game info pointer that containts the current speed
*/
void update_speed_display(game_info* game);

/**Confirms the speed is correct and waits for transmissions
@param game info pointer that contains speed
@param uint8_t that is for the waiting
*/
uint8_t confirm_speed(game_info* game, uint8_t loop_check);

/**Selects the speed to play at
@param game_info pointer is p1status and time_gap is stored
*/
void select_speed(game_info* game);

#endif
