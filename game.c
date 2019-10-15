/*
    File     draw.h
    Authors  Henry Hickman, Taran Jennison
    Date     14 October 2019
    Brief    Main c file for the game
*/

#include "system.h"
#include "tinygl.h"
#include "../fonts/font5x7_1.h"
#include "pacer.h"
#include "navswitch.h"
#include "ledmat.h"
#include "timer.h"
#include "task.h"
#include "ir_uart.h"
#include "display.h"
#include "draw.h"
#include "string.h"
#include "stdlib.h"

#define DISP_HZ 600
#define GAME_SIZE 20

typedef struct important {
    char arrow;
    int randomIndex;
    int score;
    uint16_t time_gap;
    uint16_t total_time_loop;
} game_info;

int check_input(game_info* game)
{
    navswitch_update();
    int out = 0;
    if (navswitch_push_event_p (NAVSWITCH_NORTH) && game->arrow == 'U') {
        out = 1;
    } else if (navswitch_push_event_p (NAVSWITCH_SOUTH) && game->arrow == 'D') {
        out = 1;
    } else if (navswitch_push_event_p (NAVSWITCH_EAST) && game->arrow == 'R') {
        out = 1;
    } else if (navswitch_push_event_p (NAVSWITCH_WEST) && game->arrow == 'L') {
        out = 1;
    }
    return out;
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
    timer_tick_t timer = timer_get();
    game->randomIndex = (game->randomIndex + (timer * 11245 + 12345)) % 4;
    game->arrow = arrows[game->randomIndex];
}

void send_byte(int byte)
{
    uint8_t sent = 0;
    uint8_t ACK = 0;
    while (sent == 0) {
        if (ir_uart_write_ready_p()) {
            ir_uart_putc(byte);
            while (ACK == 0) {
                if (ir_uart_read_ready_p()) {
                    ACK = ir_uart_getc();
                }
            }
            sent++;
        }
    }
}

void send_score(game_info* game)
{
    uint8_t sent = 0;
    while (sent < 2) {
        send_byte(game->score);
        game->score = game->score >> 8;
        sent++;
    }
}

int recv_byte()
{
    uint8_t recv = 0;
    int recv_byte = 0;
    while (recv == 0) {
        if (ir_uart_read_ready_p()) {
            recv_byte = ir_uart_getc();
            recv++;
        }
    }
    uint8_t sent_ACK = 0;
    uint8_t ACK = 15;   //15 to reduce possibility of bit errors causing problems
    while (sent_ACK == 0) {
        if (ir_uart_write_ready_p()) {
            ir_uart_putc(ACK);
            sent_ACK++;
        }
    }
    return recv_byte;
}

int recv_score()
{
    uint8_t count = 0;
    int recvd_score = 0;
    int ply2_score = 0;
    while (count < 2) {
        recvd_score = recv_byte();
        ply2_score = (recvd_score << 8*count) + ply2_score;
        count++;
    }
    return ply2_score;
}

int get_winner(game_info* game, int ply1_score)
{
    int won;
    if (game->score > ply1_score) {
        won = 1;
    } else if (game->score < ply1_score) {
        won = 0;
    } else {
        won = 5;
    }
    return won;
}

int main (void)
{
    char speed = '1';
    system_init ();
    ir_uart_init ();
    navswitch_init ();
    timer_init();
    tinygl_init(DISP_HZ);
    tinygl_font_set(&font5x7_1);
    tinygl_text_speed_set(20);

    pacer_init(DISP_HZ);
    display_mess("SELECT A SPEED");
    uint8_t loop_check = 0;
    uint8_t p1status = 0;
    while (loop_check == 0) {
        pacer_wait();
        tinygl_update ();
        navswitch_update();

        if (navswitch_push_event_p (NAVSWITCH_NORTH)) {
            speed += 1;
            if (speed > '5') { //Looping through 1-5
                speed = '1';
            }
        }
        if (navswitch_push_event_p (NAVSWITCH_SOUTH)) {
            speed -= 1;
            if (speed < '1') { //Ensuring it loops
                speed = '5';
            }
        }
        display_char(speed);
        if (navswitch_push_event_p (NAVSWITCH_PUSH)) { //First person to press decides speed
            if (ir_uart_write_ready_p()) {
                ir_uart_putc(speed);
                loop_check += 1;
                p1status = 1;
            }
        }
        if (ir_uart_read_ready_p()) { //If you didn't decide speed this calls
            speed = ir_uart_getc();
            loop_check += 1;
        }

    }
    tinygl_clear();

    //Init stuff for game
    game_info game = {'U', 0, 0, 0, 0};
    uint16_t counter = 0;
    game.time_gap = 75 * (6-(speed-49));
    game.total_time_loop = 3 * game.time_gap;
    int task = 0;
    uint8_t how_many_arrows = 0;


    //main gameplay loop (maybe seperate to somewhere else?)
    while (how_many_arrows < 15) {

        pacer_wait();
        counter = (counter + 1) % game.total_time_loop;

        if (counter%game.time_gap == 0) {
            task = (task + 1) % 3;
        }

        if (counter == 0) {
            switch_arrow(&game);
            how_many_arrows += 1;
        }

        if (task == 0) {        // first arrow
            display_arrow(game.arrow, 0);
        } else if (task == 1) { // second arrow
            display_arrow(game.arrow, 1);
            if (check_input(&game)) {
                game.score += get_score(counter, &game);
            }
        } else if (task == 2) { // input
            display_arrow(game.arrow, 2);
            if (check_input(&game)) {
                game.score += get_score(counter, &game);
            }
        }

    }

    //Display score
    if (game.score == 0) {
        display_mess ("zero");
    } else {
        char sNum[10];
        itoa(game.score, sNum, 10);
        sNum[9] = '\0';
        display_mess (sNum);
        tinygl_clear();
    }

    uint8_t won = 0;
    // Send score from ply1 to ply2
    if (p1status == 1) {    //if p1
        send_score(&game);
        won = recv_byte();
        // invert won to reflect result
        if (won == 1) {
            won = 0;
        } else if (won == 0) {
            won == 1;
        }
    } else {                //if p2
        int ply1_score = 0;
        ply1_score = recv_score();
        won = get_winner(&game, ply1_score);
        send_byte(won);
    }
    //disp win stat
    if (won == 1) {
        display_mess("Loser");
    } else if (won == 0) {
        display_mess("Winner");
    } else {
        display_mess("Draw");
    }
}
