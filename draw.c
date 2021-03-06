/*
    File     draw.c
    Authors  Henry Hickman, Taran Jennison
    Date     14 October 2019
    Brief    C file for displaying on LED Matrix
*/

#include "tinygl.h"
#include "../fonts/font5x7_1.h"
#include "pacer.h"
#include "navswitch.h"
#include "system.h"
#include "draw.h"

void display_char (char character)
{
    char buffer[2];
    buffer[0] = character;
    buffer[1] = '\0';
    tinygl_text (buffer);
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

void display_arrow(char direction, int count)
{
    tinygl_clear();
    if (direction == 'U') {
        tinygl_draw_line(tinygl_point(2, 2-(count)), tinygl_point(2, 6-(count)), 1);
        tinygl_draw_point(tinygl_point(1, 3-(count)), 1);
        tinygl_draw_point(tinygl_point(3, 3-(count)), 1);
        tinygl_draw_point(tinygl_point(0, 4-(count)), 1);
        tinygl_draw_point(tinygl_point(4, 4-(count)), 1);
    }
    if (direction == 'D') {
        tinygl_draw_line(tinygl_point(2, 2-(count)), tinygl_point(2, 6-(count%3)), 1);
        tinygl_draw_point(tinygl_point(1, 5-(count)), 1);
        tinygl_draw_point(tinygl_point(3, 5-(count)), 1);
        tinygl_draw_point(tinygl_point(0, 4-(count)), 1);
        tinygl_draw_point(tinygl_point(4, 4-(count)), 1);
    }
    if (direction == 'L') {
        tinygl_draw_line(tinygl_point(0, 4-(count)), tinygl_point(5, 4-(count%3)), 1);
        tinygl_draw_point(tinygl_point(1, 3-(count)), 1);
        tinygl_draw_point(tinygl_point(1, 5-(count)), 1);
        tinygl_draw_point(tinygl_point(2, 2-(count)), 1);
        tinygl_draw_point(tinygl_point(2, 6-(count)), 1);
    }
    if (direction == 'R') {
        tinygl_draw_line(tinygl_point(0, 4-(count)), tinygl_point(5, 4-(count)), 1);
        tinygl_draw_point(tinygl_point(3, 3-(count)), 1);
        tinygl_draw_point(tinygl_point(3, 5-(count)), 1);
        tinygl_draw_point(tinygl_point(2, 2-(count)), 1);
        tinygl_draw_point(tinygl_point(2, 6-(count)), 1);
    }
    tinygl_update();
}


void display_mess (char* message)
{
    uint8_t counter = 0;
    tinygl_text_mode_set (TINYGL_TEXT_MODE_SCROLL);
    tinygl_text(message);
    while(counter == 0) { //WIll not leave until button is pressed
        pacer_wait();
        navswitch_update ();
        tinygl_update();
        if (navswitch_push_event_p (NAVSWITCH_PUSH)) {
            counter++;
        }
    }
}
