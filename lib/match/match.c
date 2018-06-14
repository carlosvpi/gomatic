#ifndef __MATCH__
#define __MATCH__

typedef struct _match* MATCH;
typedef struct _annotation* ANNOTATION;
typedef struct _property* PROPERTY;

struct _match;
struct _annotation;
struct _property;

#include <stdlib.h>
// #include "../SGFReader/SGFReader.c"

MATCH newMatch();
BOOL saveMatch(MATCH match, char* filename);

ANNOTATION newAnnotation(MOKU player, int i, int j);
char* annotationToString(ANNOTATION annotation);
BOOL annotate(MATCH match, MOKU player, int i, int j);

PROPERTY newProperty(char* propIdent);
BOOL propertyPush(PROPERTY property, char* propValue);
BOOL freeProperty(PROPERTY property);

char* getIdent(PROPERTY property);
LIST getValueList(PROPERTY property);

struct _property {
    char* propIdent;
    LIST propValueList;
};

PROPERTY newProperty(char* propIdent) {
    PROPERTY property = (PROPERTY) malloc(sizeof(struct _property));
    property->propIdent = propIdent;
    property->propValueList = newList();
    return property;
}

BOOL propertyPush(PROPERTY property, char* propValue) {
    if (property == NULL) {
        return FALSE;
    }
    return push(property->propValueList, propValue);
}

BOOL freeProperty(PROPERTY property) {
    if (property == NULL) {
        return FALSE;
    }
    free(property->propIdent);
    freeList(property->propValueList);
    free(property);
    return TRUE;
}
char* getIdent(PROPERTY property) {
	return property == NULL ? NULL : property->propIdent;
}
LIST getValueList(PROPERTY property) {
	return property == NULL ? NULL : property->propValueList;
}

struct _result {
	char winner;
	float diff; // diff = 0 means 'by resign'
};

struct _match {
	char* ff;
	char* pb;
	char* pw;
	char* br;
	char* wr;
	char* bt;
	char* wt;
	struct _result result;
	float komi;
	int handicap;
	int gameTime;
	char* date;
	char* eventName;
	int tournamentRound;
	char* place;
	char* rules;
	char* gameName;
	char* opening;
	char* gameComment;
	char* source;
	char* nameOfRecorder;
	char* nameOfAnnotator;
	char* copyright;
	TREE gameTree;
};

MATCH newMatch() {
	MATCH match = (MATCH) malloc(sizeof(struct _match));
	match->ff = "4";
	match->pb = "Black";
	match->pw = "White";
	match->br = "30k";
	match->wr = "30k";
	match->bt = "";
	match->wt = "";
	match->result.winner = '0';
	match->result.diff = 0;
	match->komi = 0.5;
	match->handicap = 0;
	match->gameTime = 0;
	match->date = "";
	match->eventName = "Gomatic Learning";
	match->tournamentRound = 1;
	match->place = "London";
	match->rules = "Custom";
	match->gameName = "";
	match->opening = "";
	match->gameComment = "";
	match->source = "";
	match->nameOfRecorder = "";
	match->nameOfAnnotator = "";
	match->copyright = "";
	match->gameTree = newTree(newList());
	return match;
}
BOOL freeMatch(MATCH match) {
	if (match == NULL) {
		return FALSE;
	}
	free(match->pb);
	free(match->pw);
	free(match->br);
	free(match->wr);
	free(match->bt);
	free(match->wt);
	free(match->date);
	free(match->eventName);
	free(match->place);
	free(match->rules);
	free(match->gameName);
	free(match->opening);
	free(match->gameComment);
	free(match->source);
	free(match->nameOfRecorder);
	free(match->nameOfAnnotator);
	free(match->copyright);
	free(match);
	return TRUE;
}

BOOL saveMatch(MATCH match, char* filename) {
	FILE* file = fopen(filename, "w");
	if (file == NULL) {
		return FALSE;
	}
	fprintf(file, "(;FF[%s]PB[%s]PW[%s]BR[%s]WR[%s]BT[%s]WT[%s]KM[%.1f]HA[%d]TM[%d]DT[%s]EV[%s]RO[%d]PC[%s]RU[%s]GN[%s]ON[%s]GC[%s]SO[%s]US[%s]AN[%s]CP[%s]\n",
		match->ff,
		match->pb,
		match->pw,
		match->br,
		match->wr,
		match->bt,
		match->wt,
		match->komi,
		match->handicap,
		match->gameTime,
		match->date,
		match->eventName,
		match->tournamentRound,
		match->place,
		match->rules,
		match->gameName,
		match->opening,
		match->gameComment,
		match->source,
		match->nameOfRecorder,
		match->nameOfAnnotator,
		match->copyright);
	fprintf(file, "RE[%c+%.1f]", match->result.winner, match->result.diff);
	LIST nodes = treeRoot(match->gameTree);
	for (int i = 0; i < length(nodes); i++) {
		fprintf(file, ";%s", annotationToString(get(nodes, i)));
	}

	fprintf(file, ")");
	fclose(file);
	printf(KGRN "Saved\n" KWHT);
	return TRUE;
}

struct _annotation {
	MOKU player;
	int i;
	int j;
};

ANNOTATION newAnnotation(MOKU player, int i, int j) {
	ANNOTATION annotation = (ANNOTATION) malloc(sizeof(struct _annotation));
	annotation->player = player;
	annotation->i = i;
	annotation->j = j;
	return annotation;
}

BOOL annotate(MATCH match, MOKU player, int i, int j) {
	return push(treeRoot(match->gameTree), newAnnotation(player, i, j));
}

char* annotationToString(ANNOTATION annotation) {
	char* string = (char*) malloc(6 * sizeof(char));
	string[0] = annotation->player == 1 ? 'W' : 'B';
	string[1] = '[';
	string[2] = annotation->i + 'a';
	string[3] = annotation->j + 'a';
	string[4] = ']';
	string[5] = '\0';
	return string;
}

#endif