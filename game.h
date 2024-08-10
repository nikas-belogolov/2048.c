
#ifndef GAME_H
#define GAME_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
	uint32_t tiles[4][4];
	uint32_t score;
	uint32_t moves;
	uint32_t largest_tile;
	bool won;
} Game;

typedef enum {
	Up = 'w',
	Down = 's',
	Left = 'a',
	Right = 'd',
	Restart = 'r',
	Quit = 'q'
} Move;

void game_start(void);

#endif