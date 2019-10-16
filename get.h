/*
    File     get.c
    Authors  Henry Hickman, Taran Jennison
    Date     14 October 2019
    Brief    H file for getting scores/arrows/inputs etc
*/

#ifndef GET_H
#define GET_H

#include "system.h"

typedef struct game_info {
    char arrow;                 //what arrow the game is currently on
    int randomIndex;            //"random" number used to "randomise" arrow choice
    int score;                  //score on this UCFK
    uint16_t time_gap;          //number of ticks per task/arrow position
    uint16_t total_time_loop;   //number of ticks per arrow (ie how often arrow is changed)
    uint8_t p1status;           //flag for player number. 0 for p2, 1 for p1
    uint8_t switched;           //flag to prevent scoring multiple times off one arrow
} game_info;

int get_score(uint16_t count, game_info* game);

void switch_arrow(game_info* game);

int get_winner(game_info* game, int ply1_score);

int check_input(game_info* game);

void select_speed(game_info* game);

#endif