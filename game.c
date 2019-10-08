#include "system.h"
#include "tinygl.h"
#include "../fonts/font5x7_1.h"
#include "pacer.h"
#include "navswitch.h"
#include "ledmat.h"
#include "timer.h"
#include "task.h"
#include "ir_uart.h"


typedef struct important
 {
     int display;
     int count;
     int randomIndex;
     int numarrows;
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

void switching(void* data)
{
	game_info* game = (game_info*) data;
	char arrows[4] = {'R', 'L', 'D', 'U'};
	if (game->numarrows == 3) {
        tinygl_clear();
		display_mess("Points: ");
        tinygl_clear();
		display_mess("Winner!");
	}
	else if (game->display == 0) {
		display_arrow(arrows[game->randomIndex], game->count);
		game->display += 1;
		game->count += 1;
	}
	else {
		tinygl_clear();
		game->display = 0;
	}
	if (game->count%3 == 0) {
		timer_tick_t timer = timer_get();
		game->randomIndex = ((timer*1103515245 + 12345)/65536)%4;
		game->numarrows += 1;
	}
}

int main (void)
{
	char speed = '1';
    system_init ();
    ir_uart_init ();
    navswitch_init ();
    timer_init();
    tinygl_init(500);
    tinygl_font_set(&font5x7_1);
    tinygl_text_speed_set(20);

    pacer_init(500);
    display_mess("SELECT A SPEED");
    uint8_t counter = 0;
    while (counter == 0)
    {
    	pacer_wait();
    	tinygl_update ();
    	navswitch_update();
    	if (navswitch_push_event_p (NAVSWITCH_NORTH)) {
    		speed += 1;
    		if (speed > '5') {
    			speed = '1';
    		}
    	}
    	if (navswitch_push_event_p (NAVSWITCH_SOUTH)) {
    		speed -= 1;
    		if (speed < '1') {
    			speed = '5';
    		}
    	}
    	display_character (speed);
    	if (navswitch_push_event_p (NAVSWITCH_PUSH)) {
            ir_uart_putc(speed);
    		counter += 1;
    	}
        if (ir_uart_read_ready_p()) {
            speed = ir_uart_getc();
            counter += 1;
        }
	}
	tinygl_clear();
	game_info game = {0, 0, 0, 0};
	task_t tasks[] = 
	{
		{.func = call_tinygl_update_to_avoid_warnings, .period = TASK_RATE/2200, .data = 0},
		{.func = switching, .period = TASK_RATE/4, .data = &game}
	};
	task_schedule(tasks, 2);
}
