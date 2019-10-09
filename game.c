#include "system.h"
#include "tinygl.h"
#include "../fonts/font5x7_1.h"
#include "pacer.h"
#include "navswitch.h"
#include "ledmat.h"
#include "timer.h"
#include "task.h"
#include "ir_uart.h"

#define DISP_HZ 500
#define GAME_SIZE 20

typedef struct important {
    char arrow;
    int display;
    int count;
    int randomIndex;
    int numarrows;
    int score;
} game_info;


void display_character (char character)
{
    char buffer[2];
    buffer[0] = character;
    buffer[1] = '\0';
    tinygl_text (buffer);
}


void display_arrow(char direction, int count)
{
    tinygl_clear();
    if (direction == 'U') {
        tinygl_draw_line(tinygl_point(2, 2-(count%3)), tinygl_point(2, 6-(count%3)), 1);
        tinygl_draw_point(tinygl_point(1, 3-(count%3)), 1);
        tinygl_draw_point(tinygl_point(3, 3-(count%3)), 1);
        tinygl_draw_point(tinygl_point(0, 4-(count%3)), 1);
        tinygl_draw_point(tinygl_point(4, 4-(count%3)), 1);
    }
    if (direction == 'D') {
        tinygl_draw_line(tinygl_point(2, 2-(count%3)), tinygl_point(2, 6-(count%3)), 1);
        tinygl_draw_point(tinygl_point(1, 5-(count%3)), 1);
        tinygl_draw_point(tinygl_point(3, 5-(count%3)), 1);
        tinygl_draw_point(tinygl_point(0, 4-(count%3)), 1);
        tinygl_draw_point(tinygl_point(4, 4-(count%3)), 1);
    }
    if (direction == 'L') {
        tinygl_draw_line(tinygl_point(0, 4-(count%3)), tinygl_point(5, 4-(count%3)), 1);
        tinygl_draw_point(tinygl_point(1, 3-(count%3)), 1);
        tinygl_draw_point(tinygl_point(1, 5-(count%3)), 1);
        tinygl_draw_point(tinygl_point(2, 2-(count%3)), 1);
        tinygl_draw_point(tinygl_point(2, 6-(count%3)), 1);
    }
    if (direction == 'R') {
        tinygl_draw_line(tinygl_point(0, 4-(count%3)), tinygl_point(5, 4-(count%3)), 1);
        tinygl_draw_point(tinygl_point(3, 3-(count%3)), 1);
        tinygl_draw_point(tinygl_point(3, 5-(count%3)), 1);
        tinygl_draw_point(tinygl_point(2, 2-(count%3)), 1);
        tinygl_draw_point(tinygl_point(2, 6-(count%3)), 1);
    }
    tinygl_update();
}


void display_mess (char* message)
{
    uint8_t counter = 0;
    tinygl_text_mode_set (TINYGL_TEXT_MODE_SCROLL);
    tinygl_text(message);
    while(counter == 0) {
        pacer_wait();
        navswitch_update ();
        tinygl_update();
        if (navswitch_push_event_p (NAVSWITCH_PUSH)) {
            counter++;
        }
    }
}

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
    game->randomIndex = ((timer*1103515245 + 12345)/65536)%4;
    game->numarrows += 1;
    game->arrow = arrows[game->randomIndex];
}

int main (void)
{
    char character = '1';
    //char speed = '1';
    system_init ();
    //ir_uart_init ();
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
            character += 1;
            if (character > '5') {
                character = '1';
            }
        }
        if (navswitch_push_event_p (NAVSWITCH_SOUTH)) {
            character -= 1;
            if (character < '1') {
                character = '5';
            }
        }
        display_character (character);
        if (navswitch_push_event_p (NAVSWITCH_PUSH)) {
            loop_check += 1;
        }
    }

    tinygl_clear();
    game_info game = {'U', 0, 0, 0, 45, 0};
    uint16_t counter = 0;
    int task = 0;
    int how_many_arrows = 0;

    while (how_many_arrows < 15) {

        pacer_wait();
        counter = (counter + 1) % 750;

        if (counter%250 == 0) {
            task = (task + 1) % 3;
        }

        if (counter == 0) {
            switch_arrow(&game);
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
