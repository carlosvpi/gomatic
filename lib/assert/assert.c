#ifndef __ASSERT__

#define __ASSERT__

#include "../types/list.c"
#include "../util/util.c"

typedef struct _testBed* TESTBED;

int assertNotNull(TESTBED testBed, char* test, void** a);
int assertEqualPtr(TESTBED testBed, char* test, void* a, void* b);
int assertEqualInt(TESTBED testBed, char* test, int a, int b);
int assertEqualChar(TESTBED testBed, char* test, char a, char b);
int assertEqualFloat(TESTBED testBed, char* test, float a, float b);
int assertDifferentPtr(TESTBED testBed, char* test, void* a, void* b);
int assertDifferentInt(TESTBED testBed, char* test, int a, int b);
int assertDifferentChar(TESTBED testBed, char* test, char a, char b);
int assertDifferentFloat(TESTBED testBed, char* test, float a, float b);

void summarizeTest(TESTBED testBed);

struct _testBed {
	char* describing;
	int numTests;
	LIST didntPass;
};

TESTBED newTestBed(char* describing) {
	TESTBED testBed = (TESTBED) malloc(sizeof(struct _testBed));
	testBed->describing = describing;
	testBed->numTests = 0;
	testBed->didntPass = newList();
	return testBed;
}

int assertNotNull(TESTBED testBed, char* test, void** a) {
	testBed->numTests++;
	if (a != NULL && *a != NULL) {
		return 1;
	} else {
		push(testBed->didntPass, test);
		printf(KRED "    %s" KWHT "\n", test);
	}	
	return 0;
}
int assertEqualPtr(TESTBED testBed, char* test, void* a, void* b) {
	testBed->numTests++;
	if (a != NULL && b != NULL) {
		int d = a - b;
		if (d == 0) {
			return 1;
		}
	}
	push(testBed->didntPass, test);
	printf(KRED "    %s" KWHT ": %d != %d\n", test, (int) a, (int) b);
	return 0;
}
int assertEqualInt(TESTBED testBed, char* test, int a, int b) {
	testBed->numTests++;
	if (a == b) {
		return 1;
	}
	push(testBed->didntPass, test);
	printf(KRED "    %s" KWHT ": %d != %d\n", test, a, b);
	return 0;
}
int assertEqualChar(TESTBED testBed, char* test, char a, char b) {
	testBed->numTests++;
	if (a == b) {
		return 1;
	}
	push(testBed->didntPass, test);
	printf(KRED "    %s" KWHT ": %d != %d\n", test, a, b);
	return 0;
}
int assertEqualFloat(TESTBED testBed, char* test, float a, float b) {
	testBed->numTests++;
	if (a == b) {
		return 1;
	}
	push(testBed->didntPass, test);
	printf(KRED "    %s" KWHT ": %f != %f\n", test, a, b);
	return 0;
}
int assertDifferentPtr(TESTBED testBed, char* test, void* a, void* b) {
	testBed->numTests++;
	if (a != NULL && b != NULL) {
		int d = a - b;
		if (d != 0) {
			return 1;
		}
	}
	push(testBed->didntPass, test);
	printf(KRED "    %s" KWHT ": %d == %d\n", test, (int) a, (int) b);
	return 0;
}
int assertDifferentInt(TESTBED testBed, char* test, int a, int b) {
	testBed->numTests++;
	if (a != b) {
		return 1;
	}
	push(testBed->didntPass, test);
	printf(KRED "    %s" KWHT ": %d == %d\n", test, a, b);
	return 0;
}
int assertDifferentChar(TESTBED testBed, char* test, char a, char b) {
	testBed->numTests++;
	if (a != b) {
		return 1;
	}
	push(testBed->didntPass, test);
	printf(KRED "    %s" KWHT ": %d == %d\n", test, a, b);
	return 0;
}
int assertDifferentFloat(TESTBED testBed, char* test, float a, float b) {
	testBed->numTests++;
	if (a != b) {
		return 1;
	}
	push(testBed->didntPass, test);
	printf(KRED "    %s" KWHT ": %f == %f\n", test, a, b);
	return 0;
}

void summarizeTest(TESTBED testBed) {
	int didntPass = length(testBed->didntPass);
	int passed = testBed->numTests - didntPass;
	if (didntPass == 0) {
		printf(KGRN "[SUCC] " KWHT "%s; " KGRN "%d/%d" KWHT " tests\n", testBed->describing, testBed->numTests, testBed->numTests);
	} else {
		printf(KRED "[FAIL] " KWHT "%s; %d/%d tests passed; " KRED "%d" KWHT "/%d failed\n", testBed->describing, passed, testBed->numTests, didntPass, testBed->numTests);
		for (int i = 0; i < didntPass; i++) {
			printf(KRED "    %s\n", (char*) get(testBed->didntPass, i));
		}
		printf("\n");
	}
}

#endif