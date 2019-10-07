#include "system.h"
#include "tinygl.h"
#include "../fonts/font5x7_1.h"
#include "pacer.h"
#include "navswitch.h"
#include "ledmat.h"
#include "timer.h"

void display_character (char* character)
{
    char buffer[2];

    buffer[0] = character;
    buffer[1] = '\0';
    tinygl_text (buffer);
}

void display_arrow(char direction) 
{
	while(1) {
		if (direction == 'U') {
			ledmat_display_column (0b0001000, 0);
			ledmat_display_column (0b0000100, 1);
			ledmat_display_column (0b0111110, 2);
			ledmat_display_column (0b0000100, 3);
			ledmat_display_column (0b0001000, 4);
		}
		if (direction == 'D') {
			ledmat_display_column (0b0001000, 0);
			ledmat_display_column (0b0010000, 1);
			ledmat_display_column (0b0111110, 2);
			ledmat_display_column (0b0010000, 3);
			ledmat_display_column (0b0001000, 4);
		}
		if (direction == 'L') {
			ledmat_display_column (0b0001000, 0);
			ledmat_display_column (0b0011100, 1);
			ledmat_display_column (0b0101010, 2);
			ledmat_display_column (0b0001000, 3);
			ledmat_display_column (0b0001000, 4);
		}
		if (direction == 'R') {
			ledmat_display_column (0b0001000, 0);
			ledmat_display_column (0b0001000, 1);
			ledmat_display_column (0b0101010, 2);
			ledmat_display_column (0b0011100, 3);
			ledmat_display_column (0b0001000, 4);
		}
	}
}

void choose_arrow(char level_choice)
{
	char arrows[4] = {'R', 'L', 'D', 'U'};
	timer_tick_t timer = timer_get();
	int randomIndex = (timer/2)%4;
	while(1) {
		pacer_wait();
    	tinygl_update ();
    	navswitch_update();
    	display_arrow(arrows[randomIndex]);
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
        if (navswitch_push_event_p (NAVSWITCH_PUSH)) { //if the navswitch has been pressed
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
    		choose_arrow(character);
    	}



    }
}
