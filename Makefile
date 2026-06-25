brickbreaker: brickbreaker.cpp
	g++ -Wall -Werror -g brickbreaker.cpp -o brickbreaker -lm -lncurses -lpthread
clean:
	rm -f *.o brickbreaker
