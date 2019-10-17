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
#include "timer.h"
#include "task.h"
#include "ir_uart.h"
#include "display.h"
#include "draw.h"
#include "string.h"
#include "stdlib.h"
#include "game_info.h"
#include "communication.h"

#define DISP_HZ 600
#define GAME_SIZE 20





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

    uint8_t winner_flag = 0;
    //if p1
    if (game.p1status == 1) {
        winner_flag = pl1_get_winner(game.score/256);
        if (winner_flag == 8) {
            winner_flag = pl1_get_winner(game.score%256);
        }


        display_score(game.score);
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

    //if p0
    if (game.p1status == 0) {
        winner_flag = pl0_get_winner(game.score/256);
        if (winner_flag == 8) {
            winner_flag = pl0_get_winner(game.score%256);
        }

        display_score(game.score);
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

    display_mess("Press mid to play again");
    if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
        main();
    }

}
