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
	for (int i = 0; i < 10; i++) {
		match = newMatch();
		alpha = 20 / sqrt(i + 10);
		blackWon = trainPlayingMatch(match, nn, alpha);
		printf("match: %d, alpha: %.3f, ", i, alpha);
		if (blackWon) {
			printf(KBLU "Black" KWHT " won, ");
		} else {
			printf(KYEL "White" KWHT " won, ");
		}
		saveMatch(match, "sgf/m1.sgf");
		saveNN(nn, "nns/mk");
		// printf("---------------------------\n\n\n\n\n");
	}
}
