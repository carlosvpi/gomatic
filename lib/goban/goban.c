#ifndef __GOBAN__
#define __GOBAN__

#include <stdlib.h>
#include <string.h>
#include "../util/util.c"

#define GOBAN_SIZE 19

#define EMPTY_MOKU 0
#define WHITE_MOKU 1
#define BLACK_MOKU -1
#define ALIVE_MOKU 2
#define OUT_OF_BOUND_MOKU -2

#define VALID_PLAY 1
#define KO_INVALID_PLAY -1
#define NON_EMPTY_POSITION_PLAY -2
#define SUICIDE_PLAY -3
#define INVALID_STONE_PLAY -4
#define OUT_OF_BOARD_PLAY -5

typedef char PLAY_RESULT;
typedef float MOKU;
typedef MOKU* GOBAN;

GOBAN newGoban();
BOOL clearGoban(GOBAN goban);
BOOL freeGoban(GOBAN goban);
BOOL setMoku(GOBAN goban, int i, int j, MOKU moku);
BOOL getMoku(GOBAN goban, int i, int j, MOKU* moku);
BOOL equalGoban(GOBAN a, GOBAN b);
GOBAN copyGoban(GOBAN source, GOBAN dest);
void markGroup(GOBAN goban, int i, int j, MOKU mark, GOBAN markedBoard);
MOKU getOponent(MOKU side);
PLAY_RESULT play(GOBAN current, int i, int j, MOKU stone, GOBAN previous, GOBAN* next);
BOOL hasBlackWon(GOBAN a);
void print(GOBAN goban);
void printValues(GOBAN goban);
void initGobanC ();

GOBAN _goban_auxGoban;

GOBAN newGoban() {
    return (GOBAN) calloc(GOBAN_SIZE * GOBAN_SIZE, sizeof(MOKU));
}

BOOL clearGoban(GOBAN goban) {
    if (goban == NULL) {
        return FALSE;
    }
    memset(goban, 0, GOBAN_SIZE * GOBAN_SIZE * sizeof(MOKU));
    return TRUE;
}

BOOL freeGoban(GOBAN goban) {
    if (goban == NULL) {
        return FALSE;
    }
    free(goban);
    return TRUE;
}

BOOL setMoku(GOBAN goban, int i, int j, MOKU moku) {
    if (i < 0 || j < 0 || i >= GOBAN_SIZE || j >= GOBAN_SIZE) {
        return FALSE;
    } else {
        goban[i * GOBAN_SIZE + j] = moku;
        return TRUE;
    }
}
BOOL getMoku(GOBAN goban, int i, int j, MOKU* moku) {
    if (i < 0 || j < 0 || i >= GOBAN_SIZE || j >= GOBAN_SIZE) {
        return FALSE;
    } else {
    	*moku = goban[i * GOBAN_SIZE + j];
        return TRUE;
    }
}

BOOL equalGoban(GOBAN a, GOBAN b) {
    for (int i = 0; i < GOBAN_SIZE * GOBAN_SIZE; i++) {
        if (a[i] != b[i]) {
            return FALSE;
        }
    }
    return TRUE;
}

GOBAN copyGoban(GOBAN source, GOBAN dest) {
    GOBAN finalDest = dest == NULL ? newGoban() : dest;
    memcpy(finalDest, source, sizeof(MOKU) * GOBAN_SIZE * GOBAN_SIZE);
    return finalDest;
}

void _markGroup(int i, int j, MOKU side, MOKU mark, GOBAN markedBoard) {
    markedBoard[i * GOBAN_SIZE + j] = mark;
    if (i > 0 && markedBoard[(i - 1) * GOBAN_SIZE + j] == side) {
        _markGroup(i - 1, j, side, mark, markedBoard);
    }
    if (j > 0 && markedBoard[i * GOBAN_SIZE + j - 1] == side) {
        _markGroup(i, j - 1, side, mark, markedBoard);
    }
    if (i < GOBAN_SIZE - 1 && markedBoard[(i + 1) * GOBAN_SIZE + j] == side) {
        _markGroup(i + 1, j, side, mark, markedBoard);
    }
    if (j < GOBAN_SIZE - 1 && markedBoard[i * GOBAN_SIZE + j + 1] == side) {
        _markGroup(i, j + 1, side, mark, markedBoard);
    }
}

void markGroup(GOBAN goban, int i, int j, MOKU mark, GOBAN markedBoard) {
    if (i >= 0 && j >= 0 && i < GOBAN_SIZE && j < GOBAN_SIZE) {
        _markGroup(i, j, goban[i * GOBAN_SIZE + j], mark, markedBoard);
    }
}

BOOL _isStoneDeadAndOwnedBy(GOBAN markedBoard, int i, int j, MOKU side) {
    if (i < 0 || j < 0 || i >= GOBAN_SIZE || j >= GOBAN_SIZE) {
        return FALSE;
    } else if (markedBoard[i * GOBAN_SIZE + j] == side) {
        return TRUE;
    } else {
        return FALSE;
    }
}

MOKU getOponent(MOKU side) {
    return side == WHITE_MOKU ? BLACK_MOKU
        : side == BLACK_MOKU ? WHITE_MOKU
        : EMPTY_MOKU;
}

PLAY_RESULT play(GOBAN current, int i, int j, MOKU stone, GOBAN previous, GOBAN* next) {
    int _i, _j;

    // Some checks
    if (i < 0 || j < 0 || i >= GOBAN_SIZE || j >= GOBAN_SIZE) {
        return OUT_OF_BOARD_PLAY;
    }
    if (current[i * GOBAN_SIZE + j] != EMPTY_MOKU) {
        return NON_EMPTY_POSITION_PLAY;
    }
    if (stone != BLACK_MOKU && stone != WHITE_MOKU) {
        return INVALID_STONE_PLAY;
    }
    GOBAN goban = copyGoban(current, *next);

    // Place the stone
    goban[i * GOBAN_SIZE + j] = stone;

    // Mark as alive the groups connected to any empty moku
    _goban_auxGoban = copyGoban(current, _goban_auxGoban);
    for (_i = 0; _i < GOBAN_SIZE * GOBAN_SIZE; _i++) {
        for (_j = 0; _j < GOBAN_SIZE; _j++) {
            if (goban[_i * GOBAN_SIZE + _j] == EMPTY_MOKU) {
                markGroup(goban, _i - 1, _j, ALIVE_MOKU, _goban_auxGoban);
                markGroup(goban, _i, _j - 1, ALIVE_MOKU, _goban_auxGoban);
                markGroup(goban, _i + 1, _j, ALIVE_MOKU, _goban_auxGoban);
                markGroup(goban, _i, _j + 1, ALIVE_MOKU, _goban_auxGoban);
            }
        }
    }

    MOKU oponent = getOponent(stone);

    // If the just placed stone is not alive
    if (_goban_auxGoban[i * GOBAN_SIZE + j] != ALIVE_MOKU) {
        // Mark as alive if it killed any of the enemy's stones that are neighbour to it
        if (_isStoneDeadAndOwnedBy(_goban_auxGoban, i - 1, j, oponent) ||
            _isStoneDeadAndOwnedBy(_goban_auxGoban, i, j - 1, oponent) ||
            _isStoneDeadAndOwnedBy(_goban_auxGoban, i + 1, j, oponent) ||
            _isStoneDeadAndOwnedBy(_goban_auxGoban, i, j + 1, oponent)) {
            markGroup(goban, i, j, ALIVE_MOKU, _goban_auxGoban);
        } else {
            // Otherwise, this play is suicide and, hence, illegal
            goban[i * GOBAN_SIZE + j] = EMPTY_MOKU;
            return SUICIDE_PLAY;
        }
    }

    // Remove dead groups
    for (_i = 0; _i < GOBAN_SIZE * GOBAN_SIZE; _i++) {
        if (_goban_auxGoban[_i] != ALIVE_MOKU) {
            goban[_i] = EMPTY_MOKU;
        }
    }

    // Check for KO
    if (equalGoban(goban, previous) == TRUE) {
        return KO_INVALID_PLAY;
    }

    *next = goban;
    return VALID_PLAY;
}

BOOL hasBlackWon(GOBAN goban) {
    int blackStones, whiteStones;
    MOKU moku;
    for (int i = 0; i < GOBAN_SIZE; i++) {
        for (int j = 0; j < GOBAN_SIZE; j++) {
            getMoku(goban, i, j, &moku);
            if (moku == BLACK_MOKU) {
                blackStones++;
            } else if (moku == WHITE_MOKU) {
                whiteStones++;
            }
        }
    }
    if (blackStones >= whiteStones) {
        return TRUE;
    } else {
        return FALSE;
    }
}

void print(GOBAN goban) {
    int i, j;
    for (i = 0; i < GOBAN_SIZE; i++) {
        for (j = 0; j < GOBAN_SIZE; j++) {
            switch ((int) goban[i * GOBAN_SIZE + j]) {
                case WHITE_MOKU:
                    printf(KYEL "+ ");
                break;
                case BLACK_MOKU:
                    printf(KBLU "+ ");
                break;
                default:
                    printf(KWHT ". ");
            }
        }
        printf(KWHT "\n");
    }
}

void printValues(GOBAN goban) {
    int i, j;
    MOKU moku;
    for (i = 0; i < GOBAN_SIZE; i++) {
        for (j = 0; j < GOBAN_SIZE; j++) {
            moku = goban[i * GOBAN_SIZE + j];
            if (moku < 0) {
                printf(" %.1f", moku);
            } else {
                printf("  %.1f", moku);
            }
        }
        printf("\n");
    }
    printf("\n");
}

void initGobanC () {
	_goban_auxGoban = newGoban();
}

#endif