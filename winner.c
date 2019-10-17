

#include "system.h"
#include "communication.h"
#include "game_info.h"
#include <stdlib.h>

int get_winner(int score, int p1_score)
//only called by p0
{
    int won;
    if (score > p1_score) {
        won = 15;
    } else if (score < p1_score) {
        won = 0;
    } else {
        won = 8;
    }
    return won;
}
uint8_t pl0_get_winner(int score)
{
    uint8_t winner_flag = 8;
    //get p1 score
    int p1score = recv();
    //set winner logic val
    winner_flag = get_winner(score, p1score);
    //send winner logic val
    send(winner_flag);
    return winner_flag;
}

uint8_t pl1_get_winner(int score)
{
    uint8_t winner_flag = 8;
    send(score);
    //wait 100 ticks to give p2 time to win logic val
    wait(100);
    //while not recvd
    winner_flag = recv();
    return winner_flag;
}