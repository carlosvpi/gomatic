#include <stdio.h>
#include <math.h>
#include "lib/util/util.c"
#include "lib/types/list.c"
#include "lib/types/tree.c"
#include "lib/goban/goban.c"
#include "lib/SGFReader/SGFReader.c"
#include "lib/match/match.c"
#include "lib/nn/nn.c"

int main(void) {
	initGobanC();
	initNNC();

	NN nn = loadNN("nns/mk");
	MATCH match = newMatch();
	BOOL blackWon;
	float alpha;
	for (int i = 0; i < 2; i++) {
		match = newMatch();
		alpha = 1 / sqrt(i + 1);
		blackWon = trainPlayingMatch(match, nn, alpha);
		printf("match: %d, alpha: %.3f, ", i, alpha);
		if (blackWon) {
			printf(KBLU "Black" KWHT " won, ");
		} else {
			printf(KYEL "White" KWHT " won, ");
		}
		saveMatch(match, "sgf/m.sgf");
		saveNN(nn, "nns/mk");
	}
}
