

#ifndef WINNER_H
#define WINNER_H

#include "system.h"
#include "communication.h"
#include "game_info.h"
#include <stdlib.h>

/**Sends a character to the other player
@param int score contains p0 score
@param int p1_score contains p1 score
*/
int get_winner(int score, int p1_score);
/**Handles score comunication and winner calculation for player 0
@param int score for player 0 to use in winner calculation
*/
uint8_t pl0_get_winner(int score);

/**Handles score comunication and winner calculation for player 1
@param int score for player 1 to send
*/
uint8_t pl1_get_winner(int score);


#endif