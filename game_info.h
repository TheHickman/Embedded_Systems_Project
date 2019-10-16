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

typedef struct game_info {
    char arrow;                 //what arrow the game is currently on
   	int randomIndex;            //"random" number used to "randomise" arrow choice
    int score;                  //score on this UCFK
    uint16_t time_gap;          //number of ticks per task/arrow position
    uint16_t total_time_loop;   //number of ticks per arrow (ie how often arrow is changed)
    uint8_t p1status;           //flag for player number. 0 for p2, 1 for p1
    uint8_t switched;           //flag to prevent scoring multiple times off one arrow
} game_info;

/**Gets the score based on the count and information in the game_info struct
@param uint16_t the count of when the button was pressed
@param game_info pointer the struct containing information for calculation
*/
int get_score(uint16_t count, game_info* game);

/**Switches to next arrow in sequence
@param game_info pointer contains the previous arrow
*/
void switch_arrow(game_info* game);

/**Sends a character to the other player
@param game_info pointer contains your score
@param int contains player1 score
*/
int get_winner(game_info* game, int ply1_score);

/**Checks if input if correct
@param game_info pointer contains the arrow meant to pushed
*/
int check_input(game_info* game);

/**Selects the speed to play at
@param game_info pointer is p1status and time_gap is stored
*/
void select_speed(game_info* game);

#endif