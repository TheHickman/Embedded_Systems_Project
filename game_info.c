/*
    File     game_info.c
    Authors  Henry Hickman, Taran Jennison
    Date     15 October 2019
    Brief    C file for all functions that have game_info passed into them
*/

#include "system.h"
#include "game_info.h"
#include "timer.h"
#include "navswitch.h"
#include "ir_uart.h"
#include "tinygl.h"
#include "pacer.h"
#include "draw.h"
#include "communication.h"
#include <stdlib.h>


void wait(int ticks)
{
    int count_recv = 0;
    while (count_recv < ticks) {
        pacer_wait();
        count_recv++;
    }
}

int get_score(uint16_t count, game_info* game)
{
    int gap = game->time_gap;
    int score;
    int pre_score = count - 2*gap;
    if (pre_score <= -gap) {
        score = 0;
    } else if (pre_score < 0) {
        score = pre_score + gap;
    } else {
        score = gap - pre_score; //scale score by gap so scores are consistant across speeds?
    }
    score = score * (1000/gap);
    return score;
}

void switch_arrow(game_info* game)
{
    char arrows[4] = {'R', 'L', 'D', 'U'};
    game->randomIndex = ((game->randomIndex + 2103) * 13 ) % 4;
    game->arrow = arrows[game->randomIndex];
}

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


int check_input(game_info* game)
{
    navswitch_update();
    int out = 0;
    if (navswitch_push_event_p (NAVSWITCH_NORTH) && game->arrow == 'U') {
        game->switched = 1;
        out = 1;
    } else if (navswitch_push_event_p (NAVSWITCH_SOUTH) && game->arrow == 'D') {
        game->switched = 1;
        out = 1;
    } else if (navswitch_push_event_p (NAVSWITCH_EAST) && game->arrow == 'R') {
        game->switched = 1;
        out = 1;
    } else if (navswitch_push_event_p (NAVSWITCH_WEST) && game->arrow == 'L') {
        game->switched = 1;
        out = 1;
    }
    return out;
}

void display_score(int score)
{
    if (score == 0) {
        display_mess (" zero");
    } else {
        char charScore[10];
        itoa(score, charScore, 10);
        charScore[9] = '\0';
        display_mess (charScore);
        tinygl_clear();
    }
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

void update_speed_display(game_info* game)
{
    char speed_char = '0';
    if (navswitch_push_event_p (NAVSWITCH_NORTH)) {
            game->speed++;
            if (game->speed > 5) { //Looping through 1-5
                game->speed = 1;
            }
        }
        if (navswitch_push_event_p (NAVSWITCH_SOUTH)) {
            game->speed--;
            if (game->speed < 1) { //Ensuring it loops
                game->speed = 5;
            }
        }
    display_char(speed_char + game->speed);
}

uint8_t confirm_speed(game_info* game, uint8_t loop_check)
{
    if (navswitch_push_event_p (NAVSWITCH_PUSH)) { //First person to press decides speed
        send(game->speed);
        loop_check += 1;
        game->p1status = 1;
    } else if (ir_uart_read_ready_p()) { //If you didn't decide speed this calls
        game->speed = recv();
        loop_check += 1;
    }
    return loop_check;
}

void select_speed(game_info* game)
{
    navswitch_update();
    uint16_t counter = 0;
    uint8_t loop_check = 0;
    while (loop_check == 0) {
        pacer_wait();
        tinygl_update ();
        navswitch_update();

        update_speed_display(game);

        if (counter++ > 300) {    //force wait before confirm speed
            loop_check = confirm_speed(game, loop_check);
        }
    }
    tinygl_clear();
    //set time gap and total time loop
    game->time_gap = 75 * (7-(game->speed));
    game->total_time_loop = 3 * game->time_gap;
    display_score(game->time_gap);
}
