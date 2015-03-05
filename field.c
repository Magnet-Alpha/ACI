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

int out(int i, int j) {return (i < 0 || i > 7 || j < 0 || j > 7);}

t_move *makemove(int sl, int sc, int el, int ec, t_unit *u) {
  t_move *m = malloc(sizeof(struct t_move));
  m->sl = sl;
  m->sc = sc;
  m->el = el;
  m->ec = ec;
  m->eat = u;
  return m;
}

t_move *checkline(t_field *f, t_unit *u, int i, int j, int m, int g,t_move *o) {
  if (out(i, m))
    return o;
  if (f->mat[i][m] == NULL) {
    t_move *mo = makemove(i, j, i, m, NULL);
    mo->next = checkline(f, u, i, j, m+g, g, o);
    return mo;
  }
  if (f->mat[i][m]->team != u->team){
    t_move *mo = makemove(i, j, i, m, f->mat[i][m]);
    mo->next = NULL;
    return mo;
  }
  else
    return o;
}

t_move *checkcolumn(t_field *f, t_unit *u, int i, int j, int m, int g,t_move *o)
{
  if (out(m, j))
    return o;
  if (f->mat[m][j] == NULL) {
    t_move *mo = makemove(i, j, m, j, NULL);
    mo->next = checkcolumn(f, u, i, j, m+g, g, o);
    return mo;
  }
  if (f->mat[m][j]->team != u->team){
    t_move *mo = makemove(i, j, m, j, f->mat[m][j]);
    mo->next = NULL;
    return mo;
  }
  else
    return o;
}

t_move *checkdia(t_field *f,t_unit *u,int i,int j,
		 int a,int b,int c,int d,t_move *o) {
  if (out(i+a, j+b))
    return o;
  if (f->mat[i+a][j+b] == NULL) {
    t_move *mo = makemove(i, j, i+a, j+b, NULL);
    mo->next = checkdia(f, u, i, j, a+c, b+d, c, d, o);
    return mo;
  }
  if (f->mat[i+a][j+b]->team != u->team){
    t_move *mo = makemove(i, j, i+a, j+b, f->mat[i+a][j+b]);
    mo->next = NULL;
    return mo;
  }
  else
    return o;
}

t_move *checkcav(t_field *f, t_unit *u, int i, int j, int a, int b, t_move *o) {
  if (out(i + a, j + b))
    return o;
  if (f->mat[i+a][j+b] == NULL) {
    t_move *mo = makemove(i, j, i+a, j+b, NULL);
    mo->next = NULL;
    return mo;
  }
  if (f->mat[i+a][j+b]->team != u->team){
    t_move *mo = makemove(i, j, i+a, j+b, f->mat[i+a][j+b]);
    mo->next = NULL;
    return mo;
  }
  else
    return o;
}

t_move *checkcase(t_field *f, t_unit *u, int i, int j, int a, int b, t_move *o)
{
  if (out(a, b))
    return o;
  if (f->mat[a][b] == NULL) {
    t_move *mo = makemove(i, j, a, b, NULL);
    mo->next = NULL;
    return mo;
  }
  if (f->mat[a][b]->team != u->team){
    t_move *mo = makemove(i, j, a, b, f->mat[a][b]);
    mo->next = NULL;
    return mo;
  }
  else
    return o;
}

void give_moves(t_field *f, t_unit *u, int i, int j) {
  t_move *m;
  switch(u->type) {
  case 't': u->moves = checkline(f, u, i, j, j-1, -1, NULL);
    u->moves = checkline(f, u, i, j, j+1, 1, u->moves);
    u->moves = checkcolumn(f, u, i, j, i-1, -1, u->moves);
    u->moves = checkcolumn(f, u, i, j, i+1, 1, u->moves);
  case 'c': u->moves = checkcav(f, u, i, j, 1, 2, NULL);
    u->moves = checkcav(f, u, i, j, 2, 1, u->moves);
    u->moves = checkcav(f, u, i, j, -1, 2, u->moves);
    u->moves = checkcav(f, u, i, j, -2, 1, u->moves);
    u->moves = checkcav(f, u, i, j, 1, -2, u->moves);
    u->moves = checkcav(f, u, i, j, 2, -1, u->moves);
    u->moves = checkcav(f, u, i, j, -1, -2, u->moves);
    u->moves = checkcav(f, u, i, j, -2, -1, u->moves);
  case 'f': u->moves = checkdia(f, u, i, j, -1, -1, -1, -1, NULL);
    u->moves = checkdia(f, u, i, j, -1, 1, -1, 1 , u->moves);
    u->moves = checkdia(f, u, i, j, 1, -1, 1, -1, u->moves);
    u->moves = checkdia(f, u, i, j, 1, 1, 1, 1, u->moves);
  case 'd': u->moves = checkline(f, u, i, j, j-1, -1, NULL);
    u->moves = checkline(f, u, i, j, j+1, 1, u->moves);
    u->moves = checkcolumn(f, u, i, j, i-1, -1, u->moves);
    u->moves = checkcolumn(f, u, i, j, i+1, 1, u->moves);
    u->moves = checkdia(f, u, i, j, -1, -1, -1, -1, u->moves);
    u->moves = checkdia(f, u, i, j, -1, 1, -1, 1 , u->moves);
    u->moves = checkdia(f, u, i, j, 1, -1, 1, -1, u->moves);
    u->moves = checkdia(f, u, i, j, 1, 1, 1, 1, u->moves);
  case 'r': u->moves = checkcav(f, u, i, j, i+1, j, NULL);
    u->moves = checkcav(f, u, i, j, i+1, j+1, u->moves);
    u->moves = checkcav(f, u, i, j, i+1, j-1, u->moves);
    u->moves = checkcav(f, u, i, j, i, j+1, u->moves);
    u->moves = checkcav(f, u, i, j, i, j-1, u->moves);
    u->moves = checkcav(f, u, i, j, i-1, j+1, u->moves);
    u->moves = checkcav(f, u, i, j, i-1, j, u->moves);
    u->moves = checkcav(f, u, i, j, i-1, j-1, u->moves);
  default: if(!out(i+1,j)) {
      u->moves = makemove(i, j, i+1, j, NULL);
      u->moves->next = NULL;
    }
    else {u->moves = NULL;};
  }
}

void update_moves(t_field *f) {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (f->mat[i][j] != NULL)
        give_moves(f, f->mat[i][j], i, j);
    }
  }
}

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

t_unit *take(t_field *f, char *s) {
  if (s[0] < 'a' || s[0] > 'h')
    return NULL;
  int col = s[0] - 'a';
  if (s[1] < '1' || s[1] > '8')
    return NULL;
  int lin = s[1] - '1';
  return f->mat[lin][col];
}

t_move *checkmove(t_field *f, t_unit *u, int i, int j) {
  t_move *m = u->moves;
  while (m != NULL && (m->el != i || m->ec != j)) {
    m = m->next;
  }
  return m;
}

void execmove(t_field *f, t_unit *u, t_move *m) {
  if (m->eat != NULL) {
    m->eat->status = 0;
  }
  f->mat[m->el][m->ec] = u;
  f->mat[m->sl][m->sc] = NULL;
}

int moving(t_field *f, char *s) {
  char *start = malloc(sizeof(char)*2);
  char *end = malloc(sizeof(char)*2);
  sscanf("MOVE %s %s", start, end);
  t_unit *u = take(f, start);
  if(u == NULL || u->team != f->team_playing){
    printf("Move not valid");
    return 0;
  }
  if (s[0] < 'a' || s[0] > 'h') {
    printf("Move not valid");
    return 0;
  }
  int col = s[0] - 'a';
  if (s[1] < '1' || s[1] > '8') {
    printf("Move not valid");
    return 0;
  }
  int lin = end[1] - '1';
  t_move *m = checkmove(f, u, lin, col);
  if (m == NULL){
    printf("Move not valid");
    return 0;
  }
  execmove(f, u, m);
  return 1;
}

int main() {
	struct t_field *f = new_field();
	display(f);
	int a;
	scanf("%d", &a);
	return a;
}
