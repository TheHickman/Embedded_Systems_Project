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
    char arrow;                 //what arrow the game is currently on
    int randomIndex;            //"random" number used to "randomise" arrow choice
    int score;                  //score on this UCFK
    uint16_t time_gap;          //number of ticks per task/arrow position
    uint16_t total_time_loop;   //number of ticks per arrow (ie how often arrow is changed)
    uint8_t p1status;           //flag for player number. 0 for p2, 1 for p1
    uint8_t switched;           //flag to prevent scoring multiple times off one arrow
} game_info;

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

void wait(int ticks)
{
    int count_recv = 0;
    while (count_recv < ticks) {
        pacer_wait();
        count_recv++;
    }
}

void select_speed(game_info* game)
{
    char speed = '1';
    uint8_t loop_check = 0;
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
                game->p1status = 1;
            }
        }
        if (ir_uart_read_ready_p()) { //If you didn't decide speed this calls
            speed = ir_uart_getc();
            loop_check += 1;
        }

    }
    tinygl_clear();
    //set time gap and total time loop
    game->time_gap = 75 * (6-(speed-49));
    game->total_time_loop = 3 * game->time_gap;
}

int main (void)
{
    //init system stuff
    system_init ();
    ir_uart_init ();
    navswitch_init ();
    timer_init();
    tinygl_init(DISP_HZ);
    tinygl_font_set(&font5x7_1);
    tinygl_text_speed_set(20);

    //init game_info
    game_info game = {'U', 0, 0, 0, 0, 0, 0};

    pacer_init(DISP_HZ);
    display_mess("SELECT A SPEED");
    tinygl_clear();
    wait(50);
    navswitch_update();

    //get speed
    select_speed(&game);
    wait(50);

    //Init stuff for game
    uint16_t counter = 0;
    int task = 0;
    uint8_t how_many_arrows = 0;


    //main gameplay loop (maybe seperate to somewhere else?)
    while (how_many_arrows < 15) {

        pacer_wait();

        //counter logic
        counter = (counter + 1) % game.total_time_loop;
        //resultant counter logic
        if (counter == 0) {
            switch_arrow(&game);
            how_many_arrows += 1;
            game.switched = 0;
        }

        //task logic
        if (counter%game.time_gap == 0) {
            task = (task + 1) % 3;
        }

        //task 0 (first arrow)
        if (task == 0) {
            display_arrow(game.arrow, 0);
        } else if (task == 1) { //task 1 (second arrow + input)
            display_arrow(game.arrow, 1);
            if (game.switched == 0) {
                if (check_input(&game)) {
                    game.score += get_score(counter, &game);
                }
            }
        } else if (task == 2) { //task 2 (third arrow + input)
            if (game.switched == 0) {
                if (check_input(&game)) {
                    game.score += get_score(counter, &game);
                }
            }
        }
        //while loop end
    }
    //end of main game loop

    tinygl_clear();

    //send score if p1
    if (game.p1status == 1) {
        if (ir_uart_write_ready_p()) {
            ir_uart_putc(game.score/256);
        }
    }

    //init values for send/recv of score
    uint8_t recieved = 0;
    uint8_t won = 0;
    uint8_t p1score = 0;

    //if p2
    if (game.p1status == 0) {
        //while not recvd
        while (recieved < 1) {
            if (ir_uart_read_ready_p()) {
                p1score = ir_uart_getc();
                recieved += 1;
            }
        }
        //set winner logic val
        won = get_winner(&game, p1score);

        //send winner logic val
        if (ir_uart_write_ready_p()) {
            ir_uart_putc(won);
        }
    }

    //if p1
    if (game.p1status == 1) {
        //wait 100 ticks to give p2 time to win logic val
        wait(100);
        recieved = 0;
        //while not recvd
        while (recieved < 1) {
            if (ir_uart_read_ready_p()) {
                won = ir_uart_getc();
                recieved += 1;
            }
        }
        //Display win result
        if (won == 15) {
            display_mess("Loser");
        }
        else if (won == 0) {
            display_mess("Winner");
        }
        else if (won == 8) {
            display_mess("Draw");
        } else {
            display_mess("Error");
        }
    }
    if (game.p1status == 0) {   //if p2
        //Display win result
        if (won == 15) {
            display_mess("Winner");
        }
        else if (won == 0) {
            display_mess("Loser");
        }
        else if (won == 8) {
            display_mess("Draw");
        } else {
            display_mess("Error");
        }
    }

}
