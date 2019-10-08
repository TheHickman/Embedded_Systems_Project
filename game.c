#include "system.h"
#include "tinygl.h"
#include "../fonts/font5x7_1.h"
#include "pacer.h"
#include "navswitch.h"
#include "ledmat.h"
#include "timer.h"
#include "task.h"

int num = 0;

void switching(void)
{
	char arrows[4] = {'R', 'L', 'D', 'U'};
	timer_tick_t timer = timer_get();
	int randomIndex = ((timer*1103515245 + 12345)/65536)%4;
	if (num == 0) {
		display_arrow(arrows[randomIndex]);
		num += 1;
	}
	else {
		tinygl_clear();
		num = 0;
	}
}

void display_character (char* character)
{
    char buffer[2];

    buffer[0] = character;
    buffer[1] = '\0';
    tinygl_text (buffer);
}


void display_arrow(char direction) 
{
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
}


void display_it (char* message) {
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

int main (void)
{
	char character = '1';
    system_init ();
    navswitch_init ();
    timer_init();
    tinygl_init(500);
    tinygl_font_set(&font5x7_1);
    tinygl_text_speed_set(20);

    pacer_init(500);
    display_it("SELECT A SPEED");
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
	tinygl_clear();
	task_t tasks[] = 
	{
		{.func = tinygl_update, .period = TASK_RATE/2200},
		{.func = switching, .period = TASK_RATE}
	};
	task_schedule(tasks, 2);
}