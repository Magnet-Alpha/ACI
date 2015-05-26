/*
  ################################################
  # field.c                                      #
  #----------------------------------------------#
  # Source code                                  #
  ################################################
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <SDL/SDL.h>
#include "field.h"

#define color(param) printf("\033[%sm",param)

// Prevents out-of-range
int out(int i, int j) {return (i < 0 || i > 7 || j < 0 || j > 7);}

void freemoves(t_move *head);

int* turns(int t) {
    int *tu = calloc(sizeof(int), 6);
    int nb = 1;
    while (t > 9) {
        tu[nb] = t%10;
        t = (int)t/10;
        nb++;
    }
    tu[nb] = t;
    tu[0] = nb;
}

t_move *makemove(int sl, int sc, int el, int ec, t_unit *u) {
  t_move *m = malloc(sizeof(struct t_move));
  m->sl = sl;
  m->sc = sc;
  m->el = el;
  m->ec = ec;
  m->eat = u;
  return m;
}

// Get accessible cells in the given line
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
    mo->next = o;
    return mo;
  }
  else
    return o;
}

// Get accessible cells in the given column
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
    mo->next = o;
    return mo;
  }
  else
    return o;
}

// Get accessible cells in the diagonals from a unit's position
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
    mo->next = o;
    return mo;
  }
  else
    return o;
}

// Get accessible cells from a knight's position
t_move *checkcav(t_field *f, t_unit *u, int i, int j, int a, int b, t_move *o) {
  if (out(i + a, j + b))
    return o;
  if (f->mat[i+a][j+b] == NULL) {
    t_move *mo = makemove(i, j, i+a, j+b, NULL);
    mo->next = o;
    return mo;
  }
  if (f->mat[i+a][j+b]->team != u->team){
    t_move *mo = makemove(i, j, i+a, j+b, f->mat[i+a][j+b]);
    mo->next = o;
    return mo;
  }
  else
    return o;
}

// Check if a specific cell is accessible
t_move *checkcase(t_field *f, t_unit *u, int i, int j, int a, int b, t_move *o)
{
  if (out(a, b))
    return o;
  if (f->mat[a][b] == NULL) {
    t_move *mo = makemove(i, j, a, b, NULL);
    mo->next = o;
    return mo;
  }
  if (f->mat[a][b]->team != u->team){
    t_move *mo = makemove(i, j, a, b, f->mat[a][b]);
    mo->next = o;
    return mo;
  }
  else
    return o;
}

// Check if a specific case is occupied by an enemy (useful for the Pawn)
t_move *checkcaseenemy(t_field *f, t_unit *u, int i, int j, int a, int b, t_move *o)
{
  if (out(a, b))
    return o;
  if (f->mat[a][b] != NULL && f->mat[a][b]->team != u->team){
    t_move *mo = makemove(i, j, a, b, f->mat[a][b]);
    mo->next = o;
    return mo;
  }
  else
    return o;
}

// Check if a specific case is empty (useful for the Pawn)
t_move *checkcaseempty(t_field *f, t_unit *u, int i, int j, int a, int b, t_move *o)
{
  if (out(a, b))
    return o;
  if (f->mat[a][b] == NULL && u->type){
    t_move *mo = makemove(i, j, a, b, f->mat[a][b]);
    mo->next = o;
    return mo;
  }
  // Note : testing u->type silences the "unused parameter" warning
  else
    return o;
}

void give_moves(t_field *f, t_unit *u, int i, int j) {
  switch(u->type) {
  case 't':
    u->moves = checkline(f, u, i, j, j-1, -1, NULL);
    u->moves = checkline(f, u, i, j, j+1, 1, u->moves);
    u->moves = checkcolumn(f, u, i, j, i-1, -1, u->moves);
    u->moves = checkcolumn(f, u, i, j, i+1, 1, u->moves);
    return;
  case 'c':
    u->moves = checkcav(f, u, i, j, 1, 2, NULL);
    u->moves = checkcav(f, u, i, j, 2, 1, u->moves);
    u->moves = checkcav(f, u, i, j, -1, 2, u->moves);
    u->moves = checkcav(f, u, i, j, -2, 1, u->moves);
    u->moves = checkcav(f, u, i, j, 1, -2, u->moves);
    u->moves = checkcav(f, u, i, j, 2, -1, u->moves);
    u->moves = checkcav(f, u, i, j, -1, -2, u->moves);
    u->moves = checkcav(f, u, i, j, -2, -1, u->moves);
    return;
  case 'f':
    u->moves = checkdia(f, u, i, j, -1, -1, -1, -1, NULL);
    u->moves = checkdia(f, u, i, j, -1, 1, -1, 1 , u->moves);
    u->moves = checkdia(f, u, i, j, 1, -1, 1, -1, u->moves);
    u->moves = checkdia(f, u, i, j, 1, 1, 1, 1, u->moves);
    return;
  case 'd':
    u->moves = checkline(f, u, i, j, j-1, -1, NULL);
    u->moves = checkline(f, u, i, j, j+1, 1, u->moves);
    u->moves = checkcolumn(f, u, i, j, i-1, -1, u->moves);
    u->moves = checkcolumn(f, u, i, j, i+1, 1, u->moves);
    u->moves = checkdia(f, u, i, j, -1, -1, -1, -1, u->moves);
    u->moves = checkdia(f, u, i, j, -1, 1, -1, 1 , u->moves);
    u->moves = checkdia(f, u, i, j, 1, -1, 1, -1, u->moves);
    u->moves = checkdia(f, u, i, j, 1, 1, 1, 1, u->moves);
    return;
  case 'r':
    u->moves = checkcase(f, u, i, j, i+1, j, NULL);
    u->moves = checkcase(f, u, i, j, i+1, j+1, u->moves);
    u->moves = checkcase(f, u, i, j, i+1, j-1, u->moves);
    u->moves = checkcase(f, u, i, j, i, j+1, u->moves);
    u->moves = checkcase(f, u, i, j, i, j-1, u->moves);
    u->moves = checkcase(f, u, i, j, i-1, j+1, u->moves);
    u->moves = checkcase(f, u, i, j, i-1, j, u->moves);
    u->moves = checkcase(f, u, i, j, i-1, j-1, u->moves);
    return;
  case 'p':
    u->moves = NULL;
    if (out(i-(u->team *2 -1),j))
      return;
    if (u->moved == 0)  // Sp. move : 2 squares ahead from the starting pos.
      u->moves = checkcaseempty(f, u, i, j, i-(2*(u->team *2 -1)), j, u->moves);
    u->moves = checkcaseempty(f, u, i, j, i-(u->team *2 -1), j, u->moves);
    u->moves = checkcaseenemy(f, u, i, j, i-(u->team *2 -1), j+1, u->moves);
    u->moves = checkcaseenemy(f, u, i, j, i-(u->team *2 -1), j-1, u->moves);
    return;
  }
}

// Find the X-coord. of u (line)
int find_x(t_field *f, t_unit *u) {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (f->mat[i][j] == u)
        return i;
    }
  }
  return -1;
}

// Find the Y-coord. of u (column)
int find_y(t_field *f, t_unit *u) {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (f->mat[i][j] == u)
        return j;
    }
  }
  return -1;
}

// Check if the cell [x,y] may be attacked by the opposite team
int is_in_danger(t_field *f, int team, int x, int y) {
  struct t_move *m = NULL;
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (f->mat[i][j] != NULL) {
        if (f->mat[i][j]->team != team) {
          m = f->mat[i][j]->moves;
          while (m != NULL) {
            if (m->eat == f->mat[x][y])
              return 1;       // Danger!
            m = m->next;
          }
        }
      }
    }
  }
  return 0;                   // Safe.
}

// Is a king in check?
int is_in_check(t_field *f, int team) {
  // Looking for the king
  int x = 0;
  int y = 0;
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (f->mat[i][j] != NULL && f->mat[i][j]->type == 'r' && f->mat[i][j]->team == team) {
        x = i;
        y = j;
      }
    }
  }
  // Checking
  return is_in_danger(f, team, x, y);
}

void display_check(t_field *f, int team) {
  char* col = team == 0 ? "White" : "Black";
  if (is_in_check(f, team))
    printf("%s king in check!\n", col);
  return;
}


// Checks if castling (FR: Roque) with these two units is permitted
// A castling is permitted if :
// - Neither the Rook nor the King have ever moved since the beginning
// - The King is not in check
// - There are no units between the King and the Rook
// - The King doesn't travel cells in which it could be in check
int check_castling(t_field *f, t_unit *k, t_unit *r) {
  int x = find_x(f, k);
  if (k->moved || r->moved || is_in_check(f, k->team))
    return 0;
  if (find_y(f, r) == 0) {
    if (f->mat[x][1] || f->mat[x][2] || f->mat[x][3] ||
        is_in_danger(f, k->team, x, 3) ||
        is_in_danger(f, k->team, x, 2))
      return 0;
  }
  else {
    if (f->mat[x][6] || f->mat[x][5] ||
        is_in_danger(f, k->team, x, 6) ||
        is_in_danger(f, k->team, x, 5))
      return 0;
  }
  return 1;
}

// Execute the castling
// type = 0 : King moves -> 2 + Rook moves <- 2 ["Petit roque"]
// type = 1 : King moves <- 2 + Rook moves -> 3 ["Grand roque"]
void exec_castling(t_field *f, t_unit *k, t_unit *r, int type) {
  int kx = find_x(f, k);
  int ky = find_y(f, k);
  int rx = find_x(f, r);
  int ry = find_y(f, r);
  if (type) {      // "Grand roque"
    f->mat[kx][ky - 2] = f->mat[kx][ky];
    f->mat[kx][ky] = NULL;
    f->mat[rx][ry + 3] = f->mat[rx][ry];
    f->mat[rx][ry] = NULL;
  }
  else {           // "Petit roque"
    f->mat[kx][ky + 2] = f->mat[kx][ky];
    f->mat[kx][ky] = NULL;
    f->mat[rx][ry - 2] = f->mat[rx][ry];
    f->mat[rx][ry] = NULL;
  }
}

// Manages the castling move (returns 1 if success, 0 if not possible)
int castling(t_field *f, int team, int type) {
  t_unit *k;
  t_unit *r;
  if (team) {
    k = f->mat[7][4];
    if (type)
      r = f->mat[7][0];
    else
      r = f->mat[7][7];
  }
  else {
    k = f->mat[0][4];
    if (type)
      r = f->mat[0][0];
    else
      r = f->mat[0][7];
  }
  if (check_castling(f, k, r)) {
    exec_castling(f, k, r, type);
    return 1;
  }
  else {
    printf("Can't do castling.\n");
    return 0;
  }
}

// Promotes the pawn u to something else.
// ai = 0 if a human player owns the pawn
// ai = 1 if the AI owns the pawn (=> Queen)
void promote(t_unit *u, int ai) {
  if (ai) {
    u->type = 'd';
  }
  else {
    char *c = malloc(sizeof(char)*1024);
    printf("Which type of unit do you want to promote your pawn to?\n");
    printf("Q (Queen) / R (Rook) / B (Bishop) / K (Knight)\n");
    do {
      read(STDIN_FILENO, c, 10);
      if (c[0] != 'Q' && c[0]!= 'R' && c[0]!= 'B' && c[0]!= 'K')
        printf("You stupid\n");
    } while (c[0] != 'Q' && c[0]!= 'R' && c[0]!= 'B' && c[0]!= 'K');
    if (c[0] == 'Q') { u->type = 'd'; }
    if (c[0] == 'R') { u->type = 't'; }
    if (c[0] == 'B') { u->type = 'f'; }
    if (c[0] == 'K') { u->type = 'c'; }
  }
}

// Is there a pawn to be promoted?
// Normally, there can be max. 1 pawn in that situation.
void check_promotion(t_field *f, int ai) {
  t_unit *u = NULL;
  for (int i = 0; i <8; i++) {
    if (f->mat[0][i] && f->mat[0][i]->type == 'p')
      u = f->mat[0][i];
    if (f->mat[7][i] && f->mat[7][i]->type == 'p')
      u = f->mat[7][i];
  }
  if (u)
    promote(u, ai);
  return;
}

// Update valid moves
void update_moves(t_field *f) {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (f->mat[i][j] != NULL) {
        freemoves(f->mat[i][j]->moves);
        give_moves(f, f->mat[i][j], i, j);
      }
    }
  }
}

// Generates a new unit and returns a pointer to it.
struct t_unit *new_unit(char c, int team) {
  struct t_unit *u = calloc(sizeof(struct t_unit), 1);
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
int display(struct t_field *f, SDL_Surface *ecran, int cons) {
    if (cons) {
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
	//f->mat[i][j-2]->team ? color("36") : color("31");
	s[j] = f->mat[i][j-2]->type - 32;
	//color("37");
      }
    }
    s[10] = '#';
    s[11] = '\n';
    s[12] = '\0';
    printf("%s", s);
  }
  printf("#+--------|\n");
    }
    else {
  SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 0, 0, 0));

  SDL_Surface turn;
  SDL_Surface save;
  turn = *SDL_LoadBMP("turn.bmp");
  save = *SDL_LoadBMP("save.bmp");
  SDL_Rect positiont;
  positiont.x = 500;
  positiont.y = 50;

  SDL_Rect positions;
  positions.x = 560;
  positions.y = 400;

  SDL_SetColorKey(&turn,SDL_SRCCOLORKEY,SDL_MapRGB(turn.format,255,0,0));
  SDL_BlitSurface(&turn, NULL, ecran, &positiont);
  SDL_SetColorKey(&save,SDL_SRCCOLORKEY,SDL_MapRGB(save.format,255,0,0));
  SDL_BlitSurface(&save, NULL, ecran, &positions);

  int *t = turns(f->turn);
  SDL_Surface *tu = calloc(sizeof(SDL_Surface), t[0]);
  SDL_Rect positiontu;

  positiontu.x = 580;
  positiontu.y = 50;
  int n = 0;
  char *c = calloc(sizeof(char), 5);
  c[1] = '.';
  c[2] = 'b';
  c[3] = 'm';
  c[4] = 'p';
  while(t[0] > 0) {
    c[0] = t[t[0]] + '0';
    tu[n] = *SDL_LoadBMP(c);
    SDL_SetColorKey(tu+n,SDL_SRCCOLORKEY,SDL_MapRGB(tu[n].format,255,0,0));
    SDL_BlitSurface(tu+n, NULL, ecran, &positiontu);
    positiontu.x = positiontu.x + 30;
    n++;
    t[0]--;
  }

  SDL_Surface *squares = calloc(sizeof(SDL_Surface), 32);
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 8; j++) {
      squares[i*8+j] = *SDL_CreateRGBSurface(SDL_HWSURFACE,60,60,32,0,0,0,0);
      SDL_FillRect(squares+(i*8+j), NULL,SDL_MapRGB(ecran->format,255,255,255));
      SDL_Rect position;

      position.x = (i*2 + j%2)*60;
      position.y = j*60;

      SDL_BlitSurface(squares+(i*8+j), NULL, ecran, &position);
    }
  }

  int k = 0;
  char *buf = calloc(sizeof(char), 6);
  buf[2] = '.';
  buf[3] = 'b';
  buf[4] = 'm';
  buf[5] = 'p';

  SDL_Surface *pieces = calloc(sizeof(SDL_Surface), 32);
  for(int i = 0; i < 8; i++) {
    for(int j = 0; j < 8; j++) {
      if (f->mat[i][j] != NULL) {
	buf[0] = f->mat[i][j]->type;
	if(f->mat[i][j]->team)
	  buf[1] = 'b';
	else
	  buf[1] = 'w';
	pieces[k] = *SDL_LoadBMP(buf);
	SDL_Rect position;

	position.x = j*60;
	position.y = (7-i)*60;

	SDL_SetColorKey(pieces+k,SDL_SRCCOLORKEY,SDL_MapRGB(pieces[k].format,255,0,0));
	SDL_BlitSurface(pieces+k, NULL, ecran, &position);
	k++;
      }
    }
  }
  SDL_Flip(ecran);
    }

  return 0;
}

// IA: Values of unit types
int match_cost(char c) {
  switch (c) {
  case 'r': return 1000;
  case 'd': return 10;
  case 'f': return 3;
  case 'c': return 2;
  case 't': return 5;
  default: return 1;
  }
}

state make_state(t_move *t, int c, int v) {
  state s = malloc(sizeof(struct stat));
  s->mov = t;
  s->cost = c;
  if (t!=NULL && t->eat != NULL) {
      s->cost += match_cost(t->eat->type) * v;
  }
  s->side = NULL;
  s->next = NULL;
  return s;
}

void freemoves(t_move *head) {
  t_move *all = head;
  while(all != NULL) {
    t_move *p = all;
    all = all->next;
    free(p);
  }
  head = NULL;
}

void execmove(t_field *f, t_unit *u, t_move *m) {
  if (m->eat != NULL) {
    m->eat->status = 0;
    freemoves(m->eat->moves);
    free(m->eat);
  }
  u->moved = 1;
  f->mat[m->el][m->ec] = u;
  f->mat[m->sl][m->sc] = NULL;
}

t_field *cam(t_field *f, t_move *m) {
  t_field *co = calloc(sizeof(struct t_field), 1);
  co->turn = f->turn;
  co->team_playing = f->team_playing;
  for(int i = 0; i < 8; i++) {
    for(int j = 0; j < 8; j++) {
      if(f->mat[i][j] != NULL) {
	co->mat[i][j] = new_unit(f->mat[i][j]->type, f->mat[i][j]->team);
      }
    }
  }
  if (m!=NULL)
    execmove(co, co->mat[m->sl][m->sc], m);
  update_moves(co);
  return co;
}

void freestates(state head, state not){
  state s = head;
  while(s != NULL && s != not) {
    state p = s;
    s = s->side;
    free(p);
  }
  if(s != NULL) {
    if(s->next != NULL) {
      free(s->next->mov);
      free(s->next);
    }
    s = s->side;
  }
  while(s != NULL) {
    state p = s;
    s = s->side;
    free(p);
  }
  head = NULL;
}

void freefield(t_field *f) {
  for(int i = 0; i < 8; i++) {
    for(int j = 0; j < 8; j++) {
      if(f->mat[i][j] != NULL) {
        freemoves(f->mat[i][j]->moves);
        free(f->mat[i][j]);
      }
    }
    //free(f->mat[i]);
  }
  //free(f->mat);
  free(f);
}

void calculating(t_field *f, state actual, int t, int a_b, int first) {
  if (t < 0)
    return;
  state n = actual->next;
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (f->mat[i][j]!=NULL&&f->mat[i][j]->team==((f->team_playing+(t%2))%2)) {
	t_move *m = f->mat[i][j]->moves;
	while (m != NULL) {
	  n->side = make_state(m, actual->cost, (((t%2)*2)-1)*(-1));
	  n->side->next = make_state(NULL, 0, 1);
	  calculating(cam(f, n->side->mov), n->side, t-1, actual->cost, first);
	  if (((t%2 == 0) && (n->side->cost > actual->cost)) || ((t%2 != 0) && (n->side->cost < actual->cost))) {
        actual->next = n->side;
        actual->cost = n->side->cost;
	  }
	  if (((t%2 == 0) && (actual->cost > a_b)) || ((t%2 != 0) && (actual->cost < a_b))) {
        if (n != actual->next && n != NULL)
            free(n);
        if (t != first)
            freefield(f);
        return;
	  }
      a_b = n->side->cost;
      state fr = n;
	  n = n->side;
	  if (fr != actual->next)
        free(fr);
	  m = m->next;
	}
      }
    }
  }
  if (n != actual->next && n != NULL)
    free(n);
  if (t != first)
    freefield(f);
}

void IAplay(t_field *f, int t) {
  printf("CPU playing...\n");
  state actual = make_state(NULL, 0, 1);
  actual->next = make_state(NULL, 0, 1);
  calculating(f, actual, t*2, -1000000, t*2);
  state n = actual->next;
  printf("CPU : MOVE %c%c %c%c\n", n->mov->sc+'a', n->mov->sl+'1',
	 n->mov->ec+'a', n->mov->el+'1');
  execmove(f, f->mat[n->mov->sl][n->mov->sc], n->mov);
  /*free(n->mov);
  free(n);
  free(actual);*/
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

// Check if moving the unit u to [i,j] is permitted
t_move *checkmove(t_field *f, t_unit *u, int i, int j) {
  if (!f)     // Note : this test silences the "unused parameter" warning
    return NULL;
  t_move *m = u->moves;
  while (m != NULL && (m->el != i || m->ec != j)) {
    m = m->next;
  }
  return m;
}

int moving(t_field *f, char *s) {
  char *start = malloc(sizeof(char)*2);
  char *end = malloc(sizeof(char)*2);
  sscanf(s, "MOVE %s %s", start, end);
  t_unit *u = take(f, start);
  if(u == NULL || u->team != f->team_playing){
    printf("Move not valid\n");
    return 0;
  }
  if (end[0] < 'a' || end[0] > 'h') {
    printf("Move not valid\n");
    return 0;
  }
  int col = end[0] - 'a';
  if (end[1] < '1' || end[1] > '8') {
    printf("lol\n");
    printf("Move not valid\n");
    return 0;
  }
  int lin = end[1] - '1';
  t_move *m = checkmove(f, u, lin, col);
  free(start);
  free(end);
  if (m == NULL){
    printf("Move not valid\n");
    return 0;
  }
  execmove(f, u, m);
  return 1;
}

t_unit *match_char(char c) {
  switch(c) {
  case 't':return new_unit('t', 0);
    break;
  case 'c':return new_unit('c', 0);
    break;
  case 'f':return new_unit('f', 0);
    break;
  case 'd':return new_unit('d', 0);
    break;
  case 'r':return new_unit('r', 0);
    break;
  case 'p':return new_unit('p', 0);
    break;
  case 'T':return new_unit('t', 1);
    break;
  case 'C':return new_unit('c', 1);
    break;
  case 'F':return new_unit('f', 1);
    break;
  case 'D':return new_unit('d', 1);
    break;
  case 'R':return new_unit('r', 1);
    break;
  case 'P':return new_unit('p', 1);
    break;
  default:return NULL;
    break;
  }
}

/*------------------------------------------------------------------------*/

int save(t_field *f, char *s) {
    FILE *file = fopen(s, "w+");
    for (int i = 7; i >= 0; i--) {
        char *buf = calloc(sizeof(char), 8);
        for (int j = 0; j < 8; j++) {
            if (f->mat[i][j] != NULL)
                buf[j] = f->mat[i][j]->type + 32 * (char)f->mat[i][j]->team;
            else
                buf[j] = '#';
        }
        fprintf(file, "%s/n", buf);
    }
    fprintf(file, "TURN %d", f->turn);
    fclose(file);
    return 1;
}

t_field* load(char *s) {
  FILE *file = fopen(s, "r");
  if (file == NULL)
    return NULL;
  t_field *f = calloc(sizeof(struct t_field), 1);
  f->team_playing = 0;
  f->turn = 0;
  //f->mat = calloc(sizeof(t_unit), 64);
  char *buf = calloc(sizeof(char), 10);
  for(int i = 7; i >=0; i--) {
    fgets(buf, 10, file);
    for(int j = 0; j < 8; j++) {
      f->mat[i][j] = match_char(buf[j]);
    }
  }
  int t;
  fscanf(file, "TURN %d", &t);
  f->turn = t;
  return f;
}

//Main with SDL
int mainsdl(t_field *f, SDL_Surface *ecran, int multi, int difficulty) {
    int cont = 1;
    t_unit *choice = NULL;
    SDL_Event event;
    while (cont)
    {
        SDL_WaitEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT:
                cont = 0;
                break;
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    int x = (int)event.button.x / 60;
                    int y = (int)event.button.y / 60;
                    if (!out(x, y)) {
                        if (choice == NULL && f->mat[7-y][x] != NULL) {
                            choice = f->mat[7-y][x];
                            t_move *m = choice->moves;
                            while (m != NULL) {
                                SDL_Surface *mo = SDL_LoadBMP("selec.bmp");
                                SDL_Rect position;
                                position.x = x * 60;
                                position.y = y * 60;
                                SDL_SetColorKey(mo,SDL_SRCCOLORKEY,SDL_MapRGB(mo->format,255,0,0));
                                SDL_BlitSurface(mo, NULL, ecran, &position);
                            }
                            SDL_Flip(ecran);
                        }
                        else if (choice != NULL) {
                            char *c = calloc(sizeof(char), 10);
                            c[0] = 'M';
                            c[1] = 'O';
                            c[2] = 'V';
                            c[3] = 'E';
                            c[4] = ' ';
                            c[7] = ' ';
                            if(choice->moves != NULL) {
                                c[5] = choice->moves->sc + 'a';
                                c[6] = choice->moves->sl + '1';
                            }
                            c[8] = x + 'a';
                            c[9] = y + '1';
                            if(moving(f, c)) {
                                check_promotion(f, 0);
                                display(f, ecran, 0);
                                update_moves(f);
                                display_check(f,0);
                                display_check(f,1);
                                f->team_playing = (f->team_playing + 1) % 2;
                                if (multi) {
                                    //FIXME, Multi-stuff
                                }
                                else {
                                    IAplay(f, difficulty);
                                    f->team_playing = (f->team_playing + 1) % 2;
                                    check_promotion(f, 1);
                                    display(f, ecran, 0);
                                    update_moves(f);
                                    display_check(f,0);
                                    display_check(f,1);
                                }
                            }
                            else {
                                display(f, ecran, 0);
                            }
                            free(c);
                        }
                    }
                    else if (event.button.x >= 600 && event.button.x <= 675 && event.button.y >= 400 && event.button.y <= 440) {
                        char *c = calloc(sizeof(char), 100);
                        printf("Enter file name.\n");
                        while (!read(STDIN_FILENO, c, 100)) {
                            printf("invalid name.\n");
                        }
                        if (save(f, c))
                            printf("Game saved.\n");
                    }
                }
                break;
        }
    }
    return 0;
}

/*----------------------------------------------------------------------------*/
// MAIN PROCESS
int main(int argc, char*argv[]) {
  struct t_field *f;
  int white = 1;
  int multi = 0;
  int difficulty = 1;
  int cons = 1;
  char *d = malloc(sizeof(char)*2);
  char *e = malloc(sizeof(char)*2);
  char *c = malloc(sizeof(char)*1024);
  char *path = calloc(sizeof(char), 1024);
  SDL_Surface *ecran;

  if (argc > 1) {
    char *lvalue = NULL;
    int l;

    while ((l = getopt (argc, argv, "bml:")) != -1)
    switch (l)
      {
      case 'b':
        white = 0;
        break;
      case 'm':
        multi = 1;
        break;
      case 'l':
        lvalue = optarg;
        break;
      default:
        abort();
      }
    f = load(lvalue);
    if (f != NULL) {
      printf("Loading succesful.\n");
      SDL_Init(SDL_INIT_VIDEO);
      ecran = SDL_SetVideoMode(800, 480, 32, SDL_HWSURFACE);
      SDL_WM_SetCaption("ACI", NULL);
      display(f,ecran, 1);
    }
  }
  else {
    f = new_field();
    update_moves(f);
    printf("Do you wanna use shell as field ? Y/N\n");
    do {
        read(STDIN_FILENO, c, 10);
        if(c[0] != 'Y' && c[0]!= 'N')
            printf("You stupid\n");
    } while(c[0] != 'Y' && c[0]!= 'N');
    if(c[0] == 'N') {
        cons = 0;
        SDL_Init(SDL_INIT_VIDEO);
        ecran = SDL_SetVideoMode(800, 480, 32, SDL_HWSURFACE);
        SDL_WM_SetCaption("ACI", NULL);
    }
    display(f, ecran, cons);
    printf("Want to play multi ? Y/N\n");
    do {
        read(STDIN_FILENO, c, 10);
        if(c[0] != 'Y' && c[0]!= 'N')
            printf("You stupid\n");
    } while(c[0] != 'Y' && c[0]!= 'N');
    if(c[0] == 'Y') {
        multi = 1;
        //FIXME, Multi-stuff preparation.
    }
    else {
        printf("Which difficulty ? 1 easy - 2 medium - 3 NOTIMPLEMENTED\n");
        do {
            read(STDIN_FILENO, c, 10);
            if(c[0] != '1' && c[0]!= '2')
                printf("You stupid\n");
        } while(c[0] != '1' && c[0]!= '2');
        difficulty = (int)c[0]-(int)'0';
    }
    printf("Which team player 1 ? W for White, B for Black\n"); //Faire attention au multi
    do {
        read(STDIN_FILENO, c, 10);
        if(c[0] != 'B' && c[0]!= 'W')
            printf("You stupid\n");
    } while(c[0] != 'B' && c[0]!= 'W');
    if(c[0] == 'B' && !multi) {
        IAplay(f, difficulty);
        f->team_playing = (f->team_playing + 1) % 2;
        display(f, ecran, cons);
        update_moves(f);
        display_check(f,0);
        display_check(f,1);
    }
  }
  if (!cons)
    return mainsdl(f, ecran, multi, difficulty);
  while(1) {
    while(fgets(c, 1024, stdin) == 0) {
      if (getc(stdin) == 0)
	return 0;
    }
    if (sscanf(c, "MOVE %s %s", d, e)) {
      if(moving(f, c)) {
        check_promotion(f, 0);
        display(f, ecran, cons);
        update_moves(f);
        display_check(f,0);
        display_check(f,1);
        f->team_playing = (f->team_playing + 1) % 2;
        if (multi) {
	  //FIXME, Multi-stuff
        }
        else {
	  IAplay(f, difficulty);
	  f->team_playing = (f->team_playing + 1) % 2;
	  check_promotion(f, 1);
	  display(f, ecran, cons);
	  update_moves(f);
	  display_check(f,0);
	  display_check(f,1);
        }
      }
    }
    else if (sscanf(c, "SAVE %s", path)) {
      if (save(f, path));
        printf("Game saved\n");
    }
    else if (!strncmp(c, "STOP", 4)) {
      //freeSDL();
      //SDL_QUIT();
      return 0;
    }
    else
      printf("Please enter a move with : MOVE start end\n");
  }
  return 0;
}
