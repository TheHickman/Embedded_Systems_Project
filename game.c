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

    return score;
}

void switch_arrow(game_info* game)
{
    char arrows[4] = {'R', 'L', 'D', 'U'};
    timer_tick_t timer = timer_get();
    game->randomIndex = (game->randomIndex + (timer * 11245 + 12345)) % 4;
    game->arrow = arrows[game->randomIndex];
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
    if (p1status == 1) {
    	if (ir_uart_write_ready_p()) {
        	ir_uart_putc(game.score);
    	}
    }
    uint8_t recieved = 0;
    uint8_t won = 0;
    uint8_t p1score = 0;
    if (p1status == 0) {
    	while (recieved < 1) {
    		if (ir_uart_read_ready_p()) {
        		p1score = ir_uart_getc();
        		recieved += 1;
    		}
    	}	
		if (game.score > p1score) {
    		won = 1;
    	}
   		else if (game.score < p1score) { 
   			won = 0;
   		}
   		else {
   			won = 5;
   		}
   		if (ir_uart_write_ready_p()) {
   			ir_uart_putc(won);
   		}
   	}
   	if (p1status == 1) {
   		recieved = 0;
   		while (recieved == 0) {
   			if (ir_uart_read_ready_p()) {
   				won = ir_uart_getc();
   				recieved += 1;
   			}
   		}
   		if (won == 1) {
   			display_mess("Loser");
   		}
   		else if (won == 0) {
   			display_mess("Winner");
   		}
   		else {
   			display_mess("Draw");
   		}
   	}
   	if (p1status == 0) {
   		if (won == 1) {
   			display_mess("Winner");
   		}
   		else if (won == 0) {
   			display_mess("Loser");
   		}
   		else {
   			display_mess("Draw");
   		}
   	}
}
