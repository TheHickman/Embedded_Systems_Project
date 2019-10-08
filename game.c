#include "system.h"
#include "tinygl.h"
#include "../fonts/font5x7_1.h"
#include "pacer.h"
#include "navswitch.h"
#include "ledmat.h"
#include "timer.h"
#include "task.h"

#define DISP_HZ 500
#define GAME_SIZE 20

typedef struct important
 {
     char arrow;
     int display;
     int count;
     int randomIndex;
     int numarrows;
     int score = 0;
 } game_info;

void call_tinygl_update_to_avoid_warnings(void *data)
{
    data = data;
    tinygl_update();
}

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
        tinygl_draw_line(tinygl_point(2, 1), tinygl_point(2, 5), 1);
        tinygl_draw_point(tinygl_point(1, 2), 1);
        tinygl_draw_point(tinygl_point(3, 2), 1);
        tinygl_draw_point(tinygl_point(0, 3), 1);
        tinygl_draw_point(tinygl_point(4, 3), 1);
    }
    if (direction == 'D') {
        tinygl_draw_line(tinygl_point(2, 1), tinygl_point(2, 5), 1);
        tinygl_draw_point(tinygl_point(1, 4), 1);
        tinygl_draw_point(tinygl_point(3, 4), 1);
        tinygl_draw_point(tinygl_point(0, 3), 1);
        tinygl_draw_point(tinygl_point(4, 3), 1);
    }
    if (direction == 'L') {
        tinygl_draw_line(tinygl_point(0, 3), tinygl_point(5, 3), 1);
        tinygl_draw_point(tinygl_point(1, 2), 1);
        tinygl_draw_point(tinygl_point(1, 4), 1);
        tinygl_draw_point(tinygl_point(2, 1), 1);
        tinygl_draw_point(tinygl_point(2, 5), 1);
    }
    if (direction == 'R') {
        tinygl_draw_line(tinygl_point(0, 3), tinygl_point(5, 3), 1);
        tinygl_draw_point(tinygl_point(3, 2), 1);
        tinygl_draw_point(tinygl_point(3, 4), 1);
        tinygl_draw_point(tinygl_point(2, 1), 1);
        tinygl_draw_point(tinygl_point(2, 5), 1);
    }
    tinygl_update();
}


void display_mess (char* message) {
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

int get_score(int count) {
    int score;
    int pre_score = count - 500;
    if (pre_score <= -250) {
        score = 0;
    } else if (score < 0) {
        score = pre_score + 250;
    } else {
        score = 250 - pre_score
    return score;
}

void switch_arrow(void* data)
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
    system_init ();
    navswitch_init ();
    timer_init();
    tinygl_init(DISP_HZ);
    tinygl_font_set(&font5x7_1);
    tinygl_text_speed_set(20);

    pacer_init(DISP_HZ);
    display_mess("SELECT A SPEED");
    uint8_t counter = 0;
    while (counter == 0)
    {
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
            counter += 1;
        }
    }
    int speed = 2;
    int cycle_spacer = DISP_HZ / speed;
    tinygl_clear();
    game_info game = {0, 0, 0, 0};
    uint16_t counter = 0;
    int correct = 0;

    while 1
    {
        pacer_wait();
        navswitch_update();
        counter++;
        if (counter % cycle_spacer == 0) {
            if (counter == cycle_spacer * 3) {
                switch_arrow();
                correct = 0;
            }
        }
        if (counter == cycle_spacer * 0) {        // first arrow
            display_arrow(direction, 0)
        } else if (counter == cycle_spacer * 1) { // second arrow
            display_arrow(direction, 1)
        } else if (counter == cycle_spacer * 2 and correct == 1) { // input
            display_arrow(direction, 2)
        }

        if (navswitch_push_event_p (NAVSWITCH_NORTH) && game->arrow == 'U') {
            game->score += get_score(counter);
            correct = 1;
        } else if (navswitch_push_event_p (NAVSWITCH_SOUTH) && game->arrow == 'D') {
            game->score += get_score(counter);
            correct = 1;
        } else if (navswitch_push_event_p (NAVSWITCH_EAST) && game->arrow == 'R') {
            game->score += get_score(counter);
            correct = 1;
        } else if (navswitch_push_event_p (NAVSWITCH_WEST) && game->arrow == 'L') {
            game->score += get_score(counter);
            correct = 1;
        }
    }
}
