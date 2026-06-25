#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>

#define GAME_WIDTH 40
#define GAME_HEIGHT 20
#define BAR_WIDTH 2
#define BLOCK_ROWS 3

#define EMPTY_C ' '
#define BLOCK_C '#'
#define BALL_C 'O'
#define BAR_C '='
#define MAX_BALL_DELAY 200000
#define MIN_BALL_DELAY 75000

int BAR_POSITION = GAME_WIDTH/2;
int BALL_X, BALL_Y, BALL_DIR; //0-7 counterclockwise from right
int MAX_BLOCKS;
int NUM_BLOCKS = 0;
int SCORE = 0;
bool GAME_OVER;

struct Slot * game[GAME_WIDTH][GAME_HEIGHT];

struct Slot { 
  char c;
  int color;
};

int printSlot(int x, int y, char c) {
  if (game[x][y]->color >= 0) {
    attron(COLOR_PAIR(game[x][y]->color));
  }
  mvaddch(2*y + 2, 2*x + 1, c);
  if (game[x][y]->color >= 0) {
    attroff(COLOR_PAIR(game[x][y]->color));
  }
  return 1;
}

int printGame(struct Slot * arr[GAME_WIDTH][GAME_HEIGHT]) {
  int i, j;
  mvprintw(0, 0, "SCORE: %d", SCORE);
  for (i = 0; i < GAME_HEIGHT; i++) {
    for (j = 0; j < GAME_WIDTH; j++) {
      printSlot(j, i, arr[j][i]->c);
    }
  }
  for (i = 0; i < GAME_HEIGHT*2 + 1; i++) {
    mvaddch(i + 1, 0, '|');
    mvaddch(i + 1, GAME_WIDTH*2, '|');
  }
  for (i = 0; i < GAME_WIDTH*2; i++) {
    mvaddch(1, i, '-');
    mvaddch(GAME_HEIGHT*2 + 1, i, '-');
  }
  mvaddch(1, 0, ACS_ULCORNER);
  mvaddch(GAME_HEIGHT*2 + 1, 0, ACS_LLCORNER);
  mvaddch(1, GAME_WIDTH*2, ACS_URCORNER);
  mvaddch(GAME_HEIGHT*2 + 1, GAME_WIDTH*2, ACS_LRCORNER);
  return 1;
}

int randomColor(int notMe) {
  int color = notMe;
  while (color == notMe) {
    int random = rand() % 5;
    if (random == 0) {
      color = COLOR_RED;
    } else if (random == 1) {
      color = COLOR_GREEN;
    } else if (random == 2) {
      color = COLOR_YELLOW;
    } else if (random == 3) {
      color = COLOR_BLUE;
    } else {
      color = COLOR_CYAN;
    }
  }
  return color;
}

int placeBar(struct Slot * arr[GAME_WIDTH][GAME_HEIGHT]) {
  int y = GAME_HEIGHT - 1;
  int x = BAR_POSITION;
  arr[x][y]->c = BAR_C;
  int i;
  for (i = 0; i < BAR_WIDTH; i++) {
    arr[x+(i+1)][y]->c = BAR_C;
    arr[x-(i+1)][y]->c = BAR_C;
  }
  return 1;
}

int placeBall(struct Slot * arr[GAME_WIDTH][GAME_HEIGHT]) {
  int y = BLOCK_ROWS + 1;
  int x = BAR_POSITION;
  BALL_X = x;
  BALL_Y = y;
  arr[x][y]->c = BALL_C;
  if (rand() % 2 == 0) {
    BALL_DIR = 5;
  } else {
    BALL_DIR = 7;
  }
  return 1;
}

int bounce(char c, int next_x, int next_y) {
  if (c == EMPTY_C) {
    if (next_x < 0) {
      if (BALL_DIR == 3) {BALL_DIR = 1;}
      else if (BALL_DIR == 5) {BALL_DIR = 7;}
      else {BALL_DIR = 0;}
    } else if (next_x > GAME_WIDTH - 1) {
      if (BALL_DIR == 1) {BALL_DIR = 3;}
      else if (BALL_DIR == 7) {BALL_DIR = 5;}
      else {BALL_DIR = 4;}
    } else if (next_y < 0) {
      if (BALL_DIR == 1) {BALL_DIR = 7;}
      else if (BALL_DIR == 3) {BALL_DIR = 5;}
      else {BALL_DIR = 6;}
    }
  }
  if (c == BAR_C) {
    if (next_x - BAR_POSITION >= BAR_WIDTH) {
      BALL_DIR = 1;
    } else if (next_x - BAR_POSITION <= -BAR_WIDTH) {
      BALL_DIR = 3;
    } else
      if (BALL_DIR == 7) {BALL_DIR = 1;}
      else if (BALL_DIR == 5) {BALL_DIR = 3;}
      else {BALL_DIR = 2;}
  }
  if (c == BLOCK_C) {
    game[next_x][next_y]->c = EMPTY_C;
    game[next_x][next_y]->color = -1;
    printSlot(next_x, next_y, EMPTY_C);
    NUM_BLOCKS--;
    SCORE = SCORE + 100;
    mvprintw(0, 0, "SCORE: %d", SCORE);
    if (BALL_DIR == 1) {BALL_DIR = 7;}
    else if (BALL_DIR == 3) {BALL_DIR = 5;}
    else if (BALL_DIR == 7) {BALL_DIR = 1;}
    else if (BALL_DIR == 5) {BALL_DIR = 3;}
    else {BALL_DIR = 6;}
  }
  return 1;
}

int moveBall(struct Slot * arr[GAME_WIDTH][GAME_HEIGHT]) {
  arr[BALL_X][BALL_Y]->c = EMPTY_C;
  int chgX = 0;
  int chgY = 0;
  int next_x, next_y;
  if (BALL_DIR < 2 || BALL_DIR > 6) {
    chgX = 1;
  } else if (BALL_DIR > 2 && BALL_DIR < 6) {
    chgX = -1;
  }
  if (BALL_DIR < 4 && BALL_DIR > 0) {
    chgY = -1;
  } else if (BALL_DIR > 4) {
    chgY = 1;
  }
  next_x = BALL_X + chgX;
  next_y = BALL_Y + chgY;
  if (next_y > GAME_HEIGHT - 1) {
    GAME_OVER = true;
  }
  if (next_x < 0 || next_x > GAME_WIDTH - 1 || next_y < 0) {
    bounce(EMPTY_C, next_x, next_y);
    return moveBall(arr);
  }
  if (game[next_x][next_y]->c == BAR_C) {
    bounce(BAR_C, next_x, next_y);
    return moveBall(arr);
  }
  if (game[next_x][next_y]->c == BLOCK_C) {
    bounce(BLOCK_C, next_x, next_y);
    return moveBall(arr);
  }
  printSlot(BALL_X, BALL_Y, EMPTY_C);
  BALL_X = next_x;
  BALL_Y = next_y;
  printSlot(BALL_X, BALL_Y, BALL_C);
  arr[BALL_X][BALL_Y]->c = BALL_C;
  refresh();
  return 1;
}

int makeGame(struct Slot * arr[GAME_WIDTH][GAME_HEIGHT]) {
  int i, j;
  int color = -1;
  for (i = 0; i < GAME_HEIGHT; i++) {
    color = randomColor(color);
    for (j = 0; j < GAME_WIDTH; j++) {
      arr[j][i] = (struct Slot *) malloc(sizeof(struct Slot));
      if (i < BLOCK_ROWS && (i + j)%2 == GAME_WIDTH % 2) {
	arr[j][i]->c = BLOCK_C;
	MAX_BLOCKS++;
	arr[j][i]->color = color;
      }
      else {
	arr[j][i]->c = EMPTY_C;
	arr[j][i]->color = -1;
      }
    }
  }
  NUM_BLOCKS = MAX_BLOCKS;
  placeBar(arr);
  placeBall(arr);
  return 1;
}

int moveBar(struct Slot * arr[GAME_WIDTH][GAME_HEIGHT], int dir) {
  if (BAR_POSITION + dir - BAR_WIDTH < 0 || BAR_POSITION + dir + BAR_WIDTH >= GAME_WIDTH) {
    return 0;
  }
  arr[BAR_POSITION + (-dir * BAR_WIDTH)][GAME_HEIGHT - 1]->c = EMPTY_C;
  printSlot(BAR_POSITION + (-dir * BAR_WIDTH), GAME_HEIGHT - 1, EMPTY_C);
  arr[BAR_POSITION + dir + (dir * BAR_WIDTH)][GAME_HEIGHT - 1]->c = BAR_C;
  BAR_POSITION = BAR_POSITION + dir;
  printSlot(BAR_POSITION + (dir * BAR_WIDTH), GAME_HEIGHT - 1, BAR_C);
  return 1;
}

void *ballMethod(void *vargp) {
  while (!GAME_OVER && NUM_BLOCKS > 0) {
    moveBall(game);
    usleep(MIN_BALL_DELAY + (int)((MAX_BALL_DELAY - MIN_BALL_DELAY)*((float)NUM_BLOCKS/(float)MAX_BLOCKS)));
  }
  return NULL;
}

void *gameOver(void *vargp) {
  while (1) {
    attron(COLOR_PAIR(COLOR_RED));
    mvprintw(GAME_HEIGHT, GAME_WIDTH - 4, "GAME OVER");
    attroff(COLOR_PAIR(COLOR_RED));
    refresh();
    sleep(1);
    mvprintw(GAME_HEIGHT, GAME_WIDTH - 4, "         ");
    refresh();
    sleep(1);
  }
}

void *youWin(void *vargp) {
  while (1) {
    attron(COLOR_PAIR(COLOR_GREEN));
    mvprintw(GAME_HEIGHT, GAME_WIDTH - 4, "YOU WIN!!");
    attroff(COLOR_PAIR(COLOR_GREEN));
    refresh();
    sleep(1);
    mvprintw(GAME_HEIGHT, GAME_WIDTH - 4, "         ");
    refresh();
    sleep(1);
  }
}

int main() {
  srand(time(NULL));
  
  initscr();
  raw();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);
  start_color();
  init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
  init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
  init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
  init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
  init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);

  makeGame(game);
  GAME_OVER = false;

  pthread_t ball_thread;
  pthread_create(&ball_thread, NULL, ballMethod, NULL);
  
  int input = 0;
  printGame(game);
  timeout(250);
  while (!GAME_OVER && NUM_BLOCKS > 0) {
    input = getch();
    if (input == 'q') {
      GAME_OVER = true;
    }
    if (input == KEY_LEFT) {
      moveBar(game, -1);
    } else if (input == KEY_RIGHT) {
      moveBar(game, 1);
    }
  }
  pthread_t end_thread;
  if (NUM_BLOCKS == 0) {
    pthread_create(&end_thread, NULL, youWin, NULL);
  } else {
    pthread_create(&end_thread, NULL, gameOver, NULL);
  }
  input = 0;
  while (input != 'q') {
    
    input = getch();
  }
  endwin();
  return 1;
}
