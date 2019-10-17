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

/**Sends a character to the other player
@param int score contains p0 score
@param int p1_score contains p1 score
*/
int get_winner(int score, int p1_score);

/**Checks if input if correct
@param game_info pointer contains the arrow meant to pushed
*/
int check_input(game_info* game);

/**Displays the score
@param int score (the score to be displayed)
*/
void display_score(int score);

/**Handles score comunication and winner calculation for player 0
@param int score for player 0 to use in winner calculation
*/
uint8_t pl0_get_winner(int score);

/**Handles score comunication and winner calculation for player 1
@param int score for player 1 to send
*/
uint8_t pl1_get_winner(int score);

/**Selects the speed to play at
@param game_info pointer is p1status and time_gap is stored
*/
void select_speed(game_info* game);

#endif
