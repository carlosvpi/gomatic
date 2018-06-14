#ifndef _NN_H_

#define _NN_H_
#define FILTER_SIZE 3
#define NN_SIZE 19
#define ALPHA 0.1
#define MAX_POSITIONS 500
#define MIN_WEIGHT -9999999
#define MAX_WEIGHT 9999999
#include "../goban/goban.c"

typedef struct _layer* LAYER;
typedef struct _nn* NN;

NN newNN(int size);
BOOL freeNN(NN nn);
NN loadNN(char* filename);
BOOL saveNN(NN nn, char* filename);
BOOL trainPlayingMatch(MATCH match, NN nn, float alpha);
void initNNC ();

GOBAN _nn_workBoard1, _nn_workBoard2, _nn_auxGoban;
GOBAN _nn_next, _nn_current, _nn_previous, _nn_deltaBoard1, _nn_deltaBoard2;

struct _layer {
    float weights[GOBAN_SIZE][GOBAN_SIZE][FILTER_SIZE][FILTER_SIZE];
    float bases[GOBAN_SIZE][GOBAN_SIZE];
    GOBAN goban;
};

struct _nn {
    LAYER* layers;
    int size;
};

NN newNN(int size) {
    NN nn = (NN) malloc(sizeof(struct _nn));
    nn->layers = (LAYER*) malloc(size * sizeof(LAYER));
    nn->size = size;
    for (int k = 0; k < size; k++) {
        nn->layers[k] = (LAYER) malloc(sizeof(struct _layer));
        for (int i = 0; i < GOBAN_SIZE * GOBAN_SIZE * FILTER_SIZE * FILTER_SIZE; i++) {
            ((float*) nn->layers[k]->weights)[i] = (2 * (float) rand()) / ((float) RAND_MAX) - 1;
        }
        for (int i = 0; i < GOBAN_SIZE * GOBAN_SIZE; i++) {
            ((float*) nn->layers[k]->bases)[i] = (2 * (float) rand()) / ((float) RAND_MAX) - 1;
        }
    }
    return nn;
}

BOOL freeNN(NN nn) {
    if (nn == NULL) {
        return FALSE;
    }
    free(nn->layers);
    free(nn);
    return TRUE;
}

// void printNN(NN nn) {
//  int value;
//  for (int s = 0; s < nn->size; s++) {
//      printValues(nn->layers[s]->bases);
//      for (int i = 0; i < GOBAN_SIZE; i++) {
//          for (int j = 0; j < GOBAN_SIZE; j++) {
//              value = 0;
//              for (int fi = 0; fi < 3; fi++) {
//                  for (int fj = 0; fj < 3; fj++) {
//                      value += nn->layers[s]->weights[i][j][fi][fj];
//                  }
//              }
//              if (value == nan) {
//                  printf("  nan");
//              } else if (value < 0) {
//                  printf(" %.1f", value);
//              } else {
//                  printf("  %.1f", value);
//              }
//          }
//          printf("\n");
//      }
//      printf("\n\n");
//  }
// }

GOBAN getValueGoban(GOBAN current, NN nn, BOOL fillValues) {
    if (nn == NULL) {
        return NULL;
    }
    int size = nn->size;
    // Not using OpenCL
    MOKU value;
    copyGoban(current, _nn_workBoard1);
    MOKU multiplier;
    GOBAN workboardA, workboardB;
    int s, i, j, fi, fj;
    for (s = 0; s < nn->size; s++) {
        workboardA = s % 2 == 0 ? _nn_workBoard1 : _nn_workBoard2;
        workboardB = s % 2 == 0 ? _nn_workBoard2 : _nn_workBoard1;
        for (i = 0; i < GOBAN_SIZE; i++) {
            for (j = 0; j < GOBAN_SIZE; j++) {
                value = nn->layers[s]->bases[i][j];
                for (fi = -1; fi < 2; fi++) {
                    for (fj = -1; fj < 2; fj++) {
                        if (!getMoku(workboardA, i + fi, j + fj, &multiplier)) {
                            multiplier = 0;
                        }
                        value += multiplier * nn->layers[s]->weights[i][j][fi + 1][fj + 1];
                    }
                }
                value = 1 / (1 + exp(-value));
                setMoku(workboardB, i, j, value);
            }
        }
        if (fillValues == TRUE) {
            nn->layers[s]->goban = newGoban();
            copyGoban(workboardB, nn->layers[s]->goban);
        }
    }
    return nn->size % 2 == 0 ? _nn_workBoard2 : _nn_workBoard1;
}
BOOL getPlay(GOBAN current, int* i, int* j, MOKU stone, GOBAN previous, GOBAN *next, NN nn) {
    copyGoban(getValueGoban(current, nn, FALSE), _nn_auxGoban);
    *i = -1;
    *j = -1;
    MOKU max = 0, moku;
    PLAY_RESULT playResult = 1;
    for (int fi = 0; fi < GOBAN_SIZE; fi++) {
        for (int fj = 0; fj < GOBAN_SIZE; fj++) {
            getMoku(_nn_auxGoban, fi, fj, &moku);
            moku = (2 * moku - 1) * stone;
            if (moku > max) {
                playResult = play(current, fi, fj, stone, previous, next);
                if (playResult == VALID_PLAY) {
                    max = moku;
                    *i = fi;
                    *j = fj;
                }
            }
        }
    }
    if (*i == -1 || *j == -1) {
        return FALSE;
    }
    play(current, *i, *j, stone, previous, next);
    return TRUE;
}

GOBAN backpropagation(GOBAN position, NN nn, BOOL hasWon, float alpha, GOBAN deltaBoard1, GOBAN deltaBoard2) {
    MOKU value, aux = 0, aux2, delta;
    MOKU nextValue, nextDelta;
    int layer, i, j, fi, fj;
    GOBAN deltaBoardA, deltaBoardB;
    getValueGoban(position, nn, TRUE);
    clearGoban(deltaBoard1);
    clearGoban(deltaBoard2);

    for (i = 0; i < GOBAN_SIZE; i++) {
        for (j = 0; j < GOBAN_SIZE; j++) {
            setMoku(deltaBoard1, i, j, hasWon ? 1 : -1);
            setMoku(deltaBoard2, i, j, hasWon ? 1 : -1);
        }
    }

    for (layer = nn->size - 1; layer >= -1; layer--) {
        deltaBoardA = layer % 2 == 0 ? deltaBoard1 : deltaBoard2;
        deltaBoardB = layer % 2 == 0 ? deltaBoard2 : deltaBoard1;
        printValues(nn->layers[layer]->goban);
        for (i = 0; i < GOBAN_SIZE; i++) {
            for (j = 0; j < GOBAN_SIZE; j++) {
                delta = 0;
                getMoku(layer >= 0 ? nn->layers[layer]->goban : position, i, j, &nextValue);
                if (layer >= 0) {
                    nextValue = 2 * nextValue - 1;
                }
                if (nextValue != nextValue) {
                    exit(0);
                }

                if (layer < nn->size - 1) {
                    for (fi = -1; fi < 2; fi++) {
                        for (fj = -1; fj < 2; fj++) {
                            getMoku(deltaBoardA, i + fi, j + fj, &nextDelta);
                            delta += nextDelta * nn->layers[layer + 1]->weights[i][j][fi + 1][fj + 1];
                        }
                    }
                    setMoku(deltaBoardB, i, j, delta * nextValue * (1 - nextValue));
                    for (fi = -1; fi < 2; fi++) {
                        for (fj = -1; fj < 2; fj++) {
                            getMoku(deltaBoardA, i + fi, j + fj, &nextDelta);
                            nn->layers[layer + 1]->weights[i][j][fi + 1][fj + 1] -= alpha * nextDelta * nextValue;
                        }
                    }
                    nn->layers[layer + 1]->bases[i][j] -= alpha * nextDelta;
                } else {
                    getMoku(deltaBoardA, i, j, &delta);
                    setMoku(deltaBoardB, i, j, delta * nextValue * (1 - nextValue));
                }
            }
        }
    }

    return deltaBoardB;
}

BOOL trainPlayingMatch(MATCH match, NN nn, float alpha) {
    GOBAN positions[MAX_POSITIONS];
    int numberOfPositions = 0;

    for (int i = 0; i < MAX_POSITIONS; i++) {
        positions[i] = newGoban();
    }

    MOKU player = BLACK_MOKU;
    int i, j;
    _nn_current = newGoban();
    _nn_previous = newGoban();

    while (getPlay(_nn_current, &i, &j, player, _nn_previous, &_nn_next, nn)) {
        annotate(match, player, i, j);
        player = getOponent(player);
        _nn_auxGoban = _nn_previous;
        _nn_previous = _nn_current;
        _nn_current = _nn_next;
        _nn_next = _nn_auxGoban;
        copyGoban(_nn_current, positions[++numberOfPositions]);
    }
    LIST nodes = treeRoot(match->gameTree);
    ANNOTATION annotation;
    BOOL blackWon = hasBlackWon(_nn_current);
    clearGoban(_nn_current);
    clearGoban(_nn_previous);
    if (!length(nodes)) {
        // printf("%.4f\n[%d, %d]\n", alpha, i, j);
        // printValues(_nn_current);
        exit(0);
    }
    for (int i = 0; i < length(nodes); i++) {
        annotation = (ANNOTATION) get(nodes, i);
        backpropagation(positions[numberOfPositions - i], nn, annotation->player == -1 ? blackWon : blackWon == TRUE ? FALSE : TRUE, alpha, _nn_deltaBoard1, _nn_deltaBoard2);
        play(_nn_current, annotation->i, annotation->j, annotation->player, _nn_previous, &_nn_next);
        _nn_auxGoban = _nn_previous;
        _nn_previous = _nn_current;
        _nn_current = _nn_next;
        _nn_next = _nn_auxGoban;
    }
    return blackWon; 
}

NN loadNN(char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        return newNN(NN_SIZE);
    }
    int size;
    fread(&size, 1, sizeof(char), file);
    NN nn = newNN(size);
    int basesSize = GOBAN_SIZE * GOBAN_SIZE;
    int weightsSize = basesSize * FILTER_SIZE * FILTER_SIZE;
    for (int i = 0; i < size; i++) {
        fread(nn->layers[i]->weights, sizeof(float), weightsSize, file);
        fread(nn->layers[i]->bases, sizeof(float), basesSize, file);
    }
    fclose(file);
    return nn;
}

BOOL saveNN(NN nn, char* filename) {
    if (nn == NULL) {
        return FALSE;
    }
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        return FALSE;
    }
    fwrite(&nn->size, 1, sizeof(char), file);
    int basesSize = GOBAN_SIZE * GOBAN_SIZE;
    int weightsSize = basesSize * FILTER_SIZE * FILTER_SIZE;
    for (int i = 0; i < nn->size; i++) {
        fwrite(nn->layers[i]->weights, sizeof(float), weightsSize, file);
        fwrite(nn->layers[i]->bases, sizeof(float), basesSize, file);
    }
    fclose(file);
    return TRUE;
}

void initNNC () {
    _nn_auxGoban = newGoban();
    _nn_workBoard1 = newGoban();
    _nn_workBoard2 = newGoban();
    _nn_next = newGoban();
    _nn_current = newGoban();
    _nn_previous = newGoban();
    _nn_deltaBoard1 = newGoban();
    _nn_deltaBoard2 = newGoban();
}

#endif