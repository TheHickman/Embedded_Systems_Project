Henry Hickman (hjh60) and Taran Jennison (tmj49) ENCE260 Project.

Step 1: 

    Open terminal in the group422 directory
        -OR-
    Open terminal and navigate to the group442 directory
        
Step 2: run "make clean"

Step 3: run "make program"

Step 4: Both players must adavance past the select speed menu before selecting a speed

Step 5: One player picks a speed

Step 6: Press the  joystick in the right direciton on the third arrow flash (arrow at top of screen)

Step 7: Highest score wins

Step 8: Press the mid button to play again. Repeat for fun. 


-- Starting the game --

Once the game has been flashed onto both UCFKs a scroll message prompting the user to select the speed will be displayed.
The user must push the joystick in/down to proceed to speed selection.
Speed is selected on a scale of 1-5 and this can be cycled by pushing the joystick up or down (1 is the slowest and 5 is the fastest.)
Once the user has selected the speed they want they must confirm this by pushing the joystick down/in.
This will start the game for both users.

-- Playing the game --

The game follows a basic game play loop for a set count (15 by default).
1) A random arrow direction is selected.
2) The arrow will be displayed on the LED matrix moving from the bottom to the top.
3) The user must then try to push the joystick in the correct direction as closse to when the top arrow is show as they can.
4) A score is then calculated based on how close the users timming was and this is added to their total.
5) The loop the repeats.

This loop is reapeated until the arrow count reaches the pre determined game length value (default 15).
Once This count has been reached the scores are compaired and a winner is calculated.
Each UCFK then displays if it is the winner, loser or if it was a draw.

The game can then be played again by pushing the joystick in/down.

-- Technical tips for scoring and speed --

Scoring is calculated based on how close the users input is to the time when the arrow reaches the top.
A perfect score for an arrow would be to push the joystick in the correct direction at the same frame that the arrow apears at the top of the display.
A score can only be registered when the arrow is in the middle or the top positions.
    This means that a score can be registered at any time when the arrow is in the middle or top positions.
    Note: The a score can only be registered once per arrow so don't spam or try to get in too early.
    
Speed chart:

1 = 0.625 s per position for 1.875 s per arrow cycle

2 = 0.500 s per position for 1.500 s per arrow cycle

3 = 0.375 s per position for 1.255 s per arrow cycle

4 = 0.250 s per position for 0.750 s per arrow cycle

5 = 0.125 s per position for 0.375 s per arrow cycle