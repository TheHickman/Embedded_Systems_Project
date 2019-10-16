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

int recv(void)
{
    uint8_t recieved = 0;
    int val = 0;
    while (recieved < 1) {
        if (ir_uart_read_ready_p()) {
            val = ir_uart_getc();
            recieved = 1;
        }
    }
    return val;
}

void send(char sender)
{
    uint8_t sent = 0;
    while (sent < 1) {
        if (ir_uart_write_ready_p()) {
            ir_uart_putc(sender);
            sent = 1;
        }
    }
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

        if (counter++ > 300) {    //force wait to confirm speed
            if (navswitch_push_event_p (NAVSWITCH_PUSH)) { //First person to press decides speed
                send(speed);
                loop_check += 1;
                game->p1status = 1;
            }
        } else if (ir_uart_read_ready_p()) { //If you didn't decide speed this calls
            speed = recv();
            loop_check += 1;
        }
        //counter++;
    }
    tinygl_clear();
    //set time gap and total time loop
    game->time_gap = 75 * (6-speed);
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

        //display arrow for current task
        display_arrow(game.arrow, task);

        //check score when in task 1 & 2
        if (task > 0) {
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
        send(game.score/250);
    }

    //init values for send/recv of score

    uint8_t winner_flag = 0;
    uint8_t p1score = 0;
    //if p2
    if (game.p1status == 0) {
        //get p1 score
        p1score = recv();

        //set winner logic val
        winner_flag = get_winner(&game, p1score);

        //send winner logic val
        send(winner_flag);
    }

    //if p1
    if (game.p1status == 1) {
        //wait 100 ticks to give p2 time to win logic val
        wait(100);
        //while not recvd
        winner_flag = recv();

        //Display win result
        if (winner_flag == 15) {
            display_mess("Loser");
        } else if (winner_flag == 0) {
            display_mess("Winner");
        } else if (winner_flag == 8) {
            display_mess("Draw");
        } else {
            display_mess("Error");
        }
    }
    if (game.p1status == 0) {   //if p2
        //Display win result
        if (winner_flag == 15) {
            display_mess("Winner");
        } else if (winner_flag == 0) {
            display_mess("Loser");
        } else if (winner_flag == 8) {
            display_mess("Draw");
        } else {
            display_mess("Error");
        }
    }

}
