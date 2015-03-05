/*
################################################
# field.h                                      #
#----------------------------------------------#
# Header of field.c                            #
# Contains definitions of field and units      #
# for chess.                                   #
################################################
*/

#ifndef FIELD_H_
#define FIELD_H_

typedef struct t_unit  t_unit;

typedef struct t_field t_field;

typedef struct t_move  t_move;

//Represents a possible move for a unit.
struct t_move {
  int sl; // Start line
  int sc; // Start Column
  int el; // End Line
  int ec; // End Column
  t_move *next; // Chain list
  t_unit *eat; // If it can eat a unit, points to it, else == NULL
};

// Represents a unit of the field.
struct t_unit {
  char type;   // Type of unit
  int team;    // White (0) or Black (1)
  int status;  // 0 if KO'd, 1 otherwise
  int moved;   // 0 if this unit has never moved
  t_move *moves; // Possible moves for this unit.
};
/*
# ABOUT t_unit->type --------------------------#
# 'r' # EN: King       | FR: Roi               #
# 'd' # EN: Queen      | FR: Dame              #
# 't' # EN: Rook       | FR: Tour              #
# 'f' # EN: Bishop     | FR: Fou               #
# 'c' # EN: Knight     | FR: Cavalier          #
# 'p' # EN: Pawn       | FR: Pion              #
#----------------------------------------------#
*/

// Represents the field.
struct t_field {
  int             turn;           // "It's turn no. ..."
  int             team_playing;   // 0 if White is playing, 1 if Black
  struct t_unit  *mat[8][8];      // A matrix of 8x8 squares, aka the field itself.
  };
/*
# ABOUT t_field->mat --------------------------#
# Each square may                              #
# - be NULL if there is no unit on it          #
# - contain the pointer to the unit on it      #
#----------------------------------------------#
*/



struct t_unit *new_unit(char c, int team);

struct t_field *new_field();

struct t_field *move(struct t_field *f, int sx, int sy, int dx, int dy);

#endif // FIELD_H_
