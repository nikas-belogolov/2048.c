// 2048.cpp : Defines the entry point for the application.
//


#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <math.h>

//Regular text
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define YEL "\e[0;33m"
#define BLU "\e[0;34m"
#define MAG "\e[0;35m"
#define CYN "\e[0;36m"
#define WHT "\e[0;37m"

//Regular bold text
#define BRED "\e[1;31m"
#define BGRN "\e[1;32m"
#define BYEL "\e[1;33m"
#define BBLU "\e[1;34m"
#define BMAG "\e[1;35m"
#define BCYN "\e[1;36m"
#define BWHT "\e[1;37m"

#define COLOR_RESET "\e[0m"

#ifdef _WIN32
#include <conio.h>
#else
#include <unistd.h>
#include <termios.h>
#endif

char get_single_char(void) {
#ifdef _WIN32
	return _getch();
#else
	char ch;
	struct termios oldt, newt;

	// Get terminal attributes
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;

	// Disable canonical mode and echo
	newt.c_lflag &= ~(ICANON | ECHO);

	// Set terminal attributes
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);

	// Read a single character
	ch = getchar();

	// Restore original terminal attributes
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

	return ch;
#endif
}


void game_print_tiles(Game* game) {

	printf("Score: %d\n", game->score);

	for (int row = 0; row < 4; row++) {
		printf("\n-------------------------------------\n");
		for (int col = 0; col < 4; col++) {
			printf("|% *s", 8, " ");
		}
		printf("|\n");
		for (int col = 0; col < 4; col++) {

			const char* tile_color;
			const char *tile_colors[14] = { WHT, CYN, MAG, BLU, YEL, GRN, RED, BWHT, BCYN, BMAG, BBLU, BYEL, BGRN, BRED };

			int tile_index = game->tiles[row][col];
			tile_index = tile_index == 0 ? 0 : log2(tile_index);
			tile_color = tile_colors[tile_index];

			printf("| %s%-6d%s ", tile_color, game->tiles[row][col], COLOR_RESET);
		}
		printf("|\n");

		for (int col = 0; col < 4; col++) {
			printf("|% *s", 8, " ");
		}
		printf("|");
	}
	printf("\n-------------------------------------\n");
}

void game_print_commands(void) {
	printf("(W)Up (S)Down (A)Left (D)Right (R)Restart (Q)Quit\n\n");
}

int random_tile() {
	return (rand() % 10) == 9 ? 4 : 2;
}

Game* game_init(void) {
	Game* game = malloc(sizeof(Game));

	if (game == NULL) return NULL;

	srand(time(NULL));

	memset(game, 0, sizeof(Game)); // Initialize all struct fields to 0

	// generate initial random two tiles
	int first_tile_pos = rand() % 16;
	int second_tile_pos;

	do {
		second_tile_pos = rand() % 16;
	} while (second_tile_pos == first_tile_pos);

	int first_tile = random_tile();
	int second_tile = random_tile();

	game->tiles[first_tile_pos / 4][first_tile_pos % 4] = first_tile;
	game->tiles[second_tile_pos / 4][second_tile_pos % 4] = second_tile;
	game->largest_tile = first_tile > second_tile ? first_tile : second_tile;

	return game;
}





bool move_tiles_up(Game* game) {
	bool tile_moved = false;

	for (int row = 1; row < 4; row++) {
		for (int col = 0; col < 4; col++) {

			if (game->tiles[row][col] == 0) continue;

			int tile_row = row;

			uint32_t* tile_dest = &game->tiles[tile_row - 1][col];
			uint32_t* tile = &game->tiles[tile_row][col];

			for (; tile_row - 1 >= 0; tile_row--) {
				tile_dest = &game->tiles[tile_row - 1][col];
				tile = &game->tiles[tile_row][col];

				if (*tile_dest == 0) {
					*tile_dest = *tile;
					*tile = 0;
					tile_moved = true;
				}
				else if (*tile_dest == *tile) {
					game->score += *tile_dest * 2;
					*tile_dest += *tile;
					*tile = 0;
					tile_moved = true;
					if (*tile_dest > game->largest_tile) game->largest_tile = *tile_dest;

					break;
				}
				else break;
			}
		}
	}
	return tile_moved;
}

bool move_tiles_down(Game* game) {
	bool tile_moved = false;

	for (int row = 2; row >= 0; row--) {
		for (int col = 0; col < 4; col++) {

			// Empty tile
			if (game->tiles[row][col] == 0) continue;

			int tile_row = row;

			uint32_t* tile_dest = &game->tiles[tile_row + 1][col];
			uint32_t* tile = &game->tiles[tile_row][col];

			for (; tile_row + 1 < 4; tile_row++) {
				tile_dest = &game->tiles[tile_row + 1][col];
				tile = &game->tiles[tile_row][col];

				if (*tile_dest == 0) {
					*tile_dest = *tile;
					*tile = 0;
					tile_moved = true;
				}
				else if (*tile_dest == *tile) {
					game->score += *tile_dest * 2;
					*tile_dest += *tile;
					*tile = 0;
					tile_moved = true;
					if (*tile_dest > game->largest_tile) game->largest_tile = *tile_dest;

					break;
				}
				else break;
			}
		}
	}
	return tile_moved;

}

bool move_tiles_left(Game* game) {
	bool tile_moved = false;

	for (int row = 0; row < 4; row++) {
		for (int col = 1; col < 4; col++) {

			// Empty tile
			if (game->tiles[row][col] == 0) continue;

			int tile_col = col;

			uint32_t* tile_dest = &game->tiles[row][tile_col - 1];
			uint32_t* tile = &game->tiles[row][tile_col];

			for (; tile_col - 1 >= 0; tile_col--) {
				tile_dest = &game->tiles[row][tile_col - 1];
				tile = &game->tiles[row][tile_col];

				if (*tile_dest == 0) {
					*tile_dest = *tile;
					*tile = 0;
					tile_moved = true;
				}
				else if (*tile_dest == *tile) {
					game->score += *tile_dest * 2;
					*tile_dest += *tile;
					*tile = 0;
					tile_moved = true;
					if (*tile_dest > game->largest_tile) game->largest_tile = *tile_dest;

					break;
				}
				else break;
			}
		}
	}
	return tile_moved;

}

bool move_tiles_right(Game* game) {
	bool tile_moved = false;
	for (int row = 0; row < 4; row++) {
		for (int col = 2; col >= 0; col--) {

			// Empty tile
			if (game->tiles[row][col] == 0) continue;

			int tile_col = col;

			uint32_t* tile_dest = &game->tiles[row][tile_col + 1];
			uint32_t* tile = &game->tiles[row][tile_col];

			for (; tile_col + 1 < 4; tile_col++) {
				tile_dest = &game->tiles[row][tile_col + 1];
				tile = &game->tiles[row][tile_col];

				if (*tile_dest == 0) {
					*tile_dest = *tile;
					*tile = 0;
					tile_moved = true;
				}
				else if (*tile_dest == *tile) {
					game->score += *tile_dest * 2;
					*tile_dest += *tile;
					*tile = 0;
					tile_moved = true;
					if (*tile_dest > game->largest_tile) game->largest_tile = *tile_dest;
					break;
				}
				else break;
			}
		}
	}
	return tile_moved;
}

bool game_has_legal_move(Game* game) {

	Game *game_copy = malloc(sizeof(Game));
	memcpy(game_copy, game, sizeof(Game));

	if (move_tiles_up(game_copy) ||
		move_tiles_down(game_copy) ||
		move_tiles_left(game_copy) ||
		move_tiles_right(game_copy)) return true;

	return false;
}

bool tiles_equal(uint32_t *arr1, uint32_t *arr2) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			
			if (arr1[i * 4 + j] != arr2[i * 4 + j]) { 
				return false;
			}
		}
	}

	return true;
}

void game_start(void) {

	Game* game;

game_loop:
	game = game_init();

	while (game_has_legal_move(game)) {
	get_move:
		printf("\033[H\033[J");

		game_print_tiles(game);
		game_print_commands();

		char move;
		move = tolower(get_single_char());

		uint32_t *prev_tiles = malloc(sizeof(uint32_t) * 4 * 4);
		memcpy(prev_tiles, game->tiles, sizeof(uint32_t) * 4 * 4);

		bool tile_moved;

		switch (move) {
			case 'q':
				return;
			case 'r':
				goto game_loop;
			case 'w':
				tile_moved = move_tiles_up(game);
				break;
			case 's':
				tile_moved = move_tiles_down(game);
				break;
			case 'a':
				tile_moved = move_tiles_left(game);
				break;
			case 'd':
				tile_moved = move_tiles_right(game);
				break;
			default:
				goto get_move;
				break;
		}

		if (!tile_moved) goto get_move;

		game->moves++;

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (game->tiles[i][j] == 0) {
					goto gen_new_tile;
				}
			}
		}

		goto get_move;

	gen_new_tile:
		int new_tile_pos;

		do {
			new_tile_pos = rand() % 16;
		} while (game->tiles[new_tile_pos / 4][new_tile_pos % 4] != 0);

		game->tiles[new_tile_pos / 4][new_tile_pos % 4] = random_tile();
	}

	if (game->won) {
		printf("%sGame over! you won.%s", BGRN, COLOR_RESET);
	} else {
		printf("%sGame over! you lost.%s", BRED, COLOR_RESET);
	}

	printf("\nGame statistics:\n");
	printf("Score: %d\n", game->score);
	printf("Moves: %d\n", game->moves);
	printf("Largest tile: %d\n", game->largest_tile);
}