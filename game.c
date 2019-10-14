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

#define DISP_HZ 500
#define GAME_SIZE 20

typedef struct important {
    char arrow;
    int randomIndex;
    uint8_t numarrows;
    int score;
} game_info;


int get_score(int count)
{
    int score;
    int pre_score = count - 500;
    if (pre_score <= -250) {
        score = 0;
    } else if (pre_score < 0) {
        score = pre_score + 250;
    } else {
        score = 250 - pre_score;
    }
    return score;
}

void switch_arrow(game_info* game)
{
    char arrows[4] = {'R', 'L', 'D', 'U'};
	timer_tick_t timer = timer_get();
    game->randomIndex = (game->randomIndex + (timer * 11245 + 12345)) % 4;
    game->numarrows += 1;
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
            }
        }
        if (ir_uart_read_ready_p()) { //If you didn't decide speed this calls
            speed = ir_uart_getc();
            loop_check += 1;
        }

    }

    tinygl_clear();
    game_info game = {'U', 0, 45, 0};
    uint16_t counter = 0;
    int task = 0;
    uint8_t how_many_arrows = 0;

    while (how_many_arrows < 15) {

        pacer_wait();
        counter = (counter + 1) % 750;

        if (counter%250 == 0) {
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
        } else if (task == 2) { // input
            display_arrow(game.arrow, 2);
        }

    }
    display_mess ("Winner");
}
