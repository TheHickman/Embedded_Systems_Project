/*
    File     get.c
    Authors  Henry Hickman, Taran Jennison
    Date     14 October 2019
    Brief    C file for getting scores/arrows/inputs etc
*/

#include "system.h"
#include "get.h"
#include "timer.h"
#include "navswitch.h"
#include "ir_uart.h"
#include "tinygl.h"
#include "pacer.h"
#include "draw.h"

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

int get_winner(game_info* game, int ply1_score)
//calculated if ply2
{
    int won;
    if (game->score/256 > ply1_score) {
        won = 15;
    } else if (game->score/256 < ply1_score) {
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

void select_speed(game_info* game)
{
    char speed_char = '1';
    int speed = 1;
    uint16_t counter = 0;
    uint8_t loop_check = 0;
    while (loop_check == 0) {
        pacer_wait();
        tinygl_update ();
        navswitch_update();

        if (navswitch_push_event_p (NAVSWITCH_NORTH)) {
            speed_char++;
            speed++;
            if (speed_char > '5') { //Looping through 1-5
                speed_char = '1';
                speed = 1;
            }
        }
        if (navswitch_push_event_p (NAVSWITCH_SOUTH)) {
            speed_char--;
            speed--;
            if (speed_char < '1') { //Ensuring it loops
                speed_char = '5';
                speed = 5;
            }
        }

        display_char(speed_char);

        if (counter > 300) {    //force wait to confirm speed
            if (navswitch_push_event_p (NAVSWITCH_PUSH)) { //First person to press decides speed
                if (ir_uart_write_ready_p()) {
                    ir_uart_putc(speed);
                    loop_check += 1;
                    game->p1status = 1;
                }
            } else if (ir_uart_read_ready_p()) { //If you didn't decide speed this calls
                speed = ir_uart_getc();
                loop_check += 1;
            }
        }
        counter++;
    }
    tinygl_clear();
    //set time gap and total time loop
    game->time_gap = 75 * (6-speed);
    game->total_time_loop = 3 * game->time_gap;
}