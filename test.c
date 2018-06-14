#include <stdio.h>
#include "lib/util/util.h"
#include "lib/types/list.c"
#include "lib/types/tree.c"
#include "lib/goban/goban.c"
// #include "lib/gocl.c"
#include "lib/match/gomatch.c"
#include "lib/SGFReader/SGFReader.c"
#include "lib/nn/nn.c"
#include "lib/assert/assert.c"
#include "lib/goban/goban_test.c"
#include "lib/match/gomatch_test.c"

int main(void) {
	gobanTest();
	gomatchTest();
}