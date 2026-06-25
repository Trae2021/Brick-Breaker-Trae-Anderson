File name: brickbreaker.cpp
Makefile creates file named brickbreaker
For this "Choose Your Own Assignment", I chose to make a version of the classic brick breaker game. I was inspired to do this when my coworker, looking at my pokemon game I had been working on, mistook it for a brick breaker game. I believe it conforms to specifications by being of similar work as the previous assignments this year because it took me a similar amount of time to complete. If you are unfamiliar with this type of game, there is a ball ('O') that will bounce around the playing area and the user can move the bar and the bottom to help bounce it. The goal is to make the ball hit and break all the bricks ('#') at the top of the screen (each brick that is broken adds to your score).  If the ball ever reaches the bottom of the playing area without being bounced back, the game immediatelely ends. I also chose to implement a mechanic that causes the ball to move faster the less bricks that remain, so it will start slower and slowly gain speed.


**GRADING NOTE**
In past assignments, there have been issues with the makefile not working on Pyrite, despite working on the VM provided in the syllabus. If this occurs, please try running it on the VM!


**CONTROLS**
LEFT ARROW - moves the bar left
RIGHT ARROW - moves the bar right
'q' - if the game is still going, forces it into a GAME OVER screen. If you are already on a GAME OVER or YOU WIN!! screen, quits the program. So, to quit the program mid-game, press q twice.


**TIP**
If the ball hits the very corner of the bar, it will bounce back the way it came!
