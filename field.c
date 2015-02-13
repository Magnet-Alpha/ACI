/*
################################################
# field.c                                      #
#----------------------------------------------#
# Source code                                  #
################################################
*/

#include <stdlib.h>
#include <stdio.h>
#include "field.h"

#define color(param) printf("\033[%sm",param)


// Generates a new unit and returns a pointer to it.
struct t_unit *new_unit(char c, int team) {
	struct t_unit *u = malloc(sizeof(struct t_unit));
	u->type = c;            // What kind of unit is this?
	u->team = team;         // Who owns this unit?
	u->status = 1;          // This unit is alive! (...for now.)
	u->moved = 0;           // This unit has never moved
	return u;
}

// Generates a new field to begin the game with.
struct t_field *new_field() {
	struct t_field *f = malloc(sizeof(struct t_field));
	f->turn = 0;
	f->team_playing = 0;                     // White team plays first

	for (int i = 0; i < 8; ++i) {            // Placing pawns and specifying empty squares
		f->mat[1][i]= new_unit('p', 0);
		f->mat[2][i] = NULL;
		f->mat[3][i] = NULL;
		f->mat[4][i] = NULL;
		f->mat[5][i] = NULL;
		f->mat[6][i] = new_unit('p', 1);
	}
	f->mat[0][0] = new_unit('t', 0);         // Placing main units
	f->mat[0][1] = new_unit('c', 0);
	f->mat[0][2] = new_unit('f', 0);
	f->mat[0][3] = new_unit('d', 0);
	f->mat[0][4] = new_unit('r', 0);
	f->mat[0][5] = new_unit('f', 0);
	f->mat[0][6] = new_unit('c', 0);
	f->mat[0][7] = new_unit('t', 0);
	f->mat[7][0] = new_unit('t', 1);
	f->mat[7][1] = new_unit('c', 1);
	f->mat[7][2] = new_unit('f', 1);
	f->mat[7][3] = new_unit('d', 1);
	f->mat[7][4] = new_unit('r', 1);
	f->mat[7][5] = new_unit('f', 1);
	f->mat[7][6] = new_unit('c', 1);
	f->mat[7][7] = new_unit('t', 1);
	return f;
}
// Moves a unit from [sX, sY] to [dX, dY], if possible.
struct t_field* move(struct t_field *f, int sx, int sy, int dx, int dy) {
	if (f->mat[sx][sy] == NULL)
		return f;
	if (f->mat[dx][dy] != NULL)
		free(f->mat[dx][dy]);
	f->mat[dx][dy] = f->mat[sx][sy];
	f->mat[sx][sy] = NULL;
	return f;
}


// Displays the field (console mode)
int display(struct t_field *f) {
	printf("#+ABCDEFGH#\n");
	printf("#+--------|\n");
	char s[13];
	for (int i = 7; i >= 0; --i) {
		s[0] = 48+i+1;
		s[1] = '|';
		for (int j = 2; j <= 9; j++) {
			if (f->mat[i][j-2] == NULL)
				s[j] = ' ';
			else {
				f->mat[i][j-2]->team ? color("36") : color("31");
				s[j] = f->mat[i][j-2]->type - 32;
				color("37");
			}
		}
		s[10] = '#';
		s[11] = '\n';
		s[12] = '\0';
		printf("%s", s);
	}
	printf("#+--------|\n");
	return 0;
}



int main() {
	struct t_field *f = new_field();
	display(f);
	int a;
	scanf("%d", &a);
	return a;
}
