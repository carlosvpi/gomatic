#ifndef __SGFREADER__

#define __SGFREADER__
#define STRING_RESERVED_LENGTH 10

#include <string.h>
#include "../match/match.c"

MATCH readSGF(char* fileName);

void printFromFile(FILE* file, int n) {
    char* rest = (char*) malloc(n * sizeof(char));
    char c;
    for(int i = 0; i < n && c != '\0'; i++) {
        c = getc(file);
        rest[i] = c;
    }
    printf(KYEL "%s" KWHT "...\n", rest);
}

void ws(FILE* file) {
	char c = getc(file);
	while (c == ' ' || c == 13 || c == 10) {
		c = getc(file);
	}
	ungetc(c, file);
}

char* SimpleText(FILE* file) {
    char c;
    char* text = (char*) malloc(STRING_RESERVED_LENGTH * sizeof(char));
    int length = 0;
    int rsvLength = STRING_RESERVED_LENGTH;
    while ((c = getc(file)) != '\0' /* && c != ':' */ && c != ']') {
        if (c == '\\') {
            c = getc(file);
        }
        text[length] = c;
        length++;
        if (length >= rsvLength - 1) {
            rsvLength += STRING_RESERVED_LENGTH;
            text = (char*) realloc(text, rsvLength * sizeof(char));
        }
    }
    ungetc(c, file);
    text = (char*) realloc(text, (length + 1) * sizeof(char));
    text[length] = '\0';
    return text;
}

int Int(char* s, int* index) {
    int i = 0;
    index = index == NULL ? &i : index;
    int n = 0;
    char c = s[(*index)++];
    while (c >= '0' && c <= '9') {
        n *= 10;
        n += c - '0';
        c = s[(*index)++];
    }
    return n;
}

float NumberPart(char* s, int* index) {
    int i = 0;
    index = index == NULL ? &i : index;
    char c = s[(*index)++];
    float n = 0;
    float e = 1;
    while (c >= '0' && c <= '9') {
        e /= 10;
        n += e * (c - '0');
        c = s[(*index)++];
    }
    return n;
}

float Real(char* s, int* index) {
    int i = 0;
    index = index == NULL ? &i : index;
    char c = s[(*index)++];
    float n = 0;
    float sign = 1;
    if (c == '-') {
        sign = -1;
    } else if (c != '+') {
        (*index)--;
    }
    n = sign * Int(s, index);
    c = s[(*index)++];
    if (c == '.') {
        n += NumberPart(s, index);
    } else {
        (*index)--;
    }
    return n;
}

struct _result Result(char* s) {
    struct _result result;
    result.winner = s[0];
    int i = 2;
    result.diff = Int(s, &i);
    return result;
}

char* ValueType(FILE *file) {
    ws(file);
    return SimpleText(file);
}

int strLength(char* string) {
    int i = 0;
    while(string[i] != '\0') {
        i++;
    }
    return i;
}

char* CValueType(FILE *file) {
    // char* value1 = ValueType(file);
    // char c;
    // ws(file);
    // if ((c = getc(file)) == ':') {
    //     VALUE_TYPE value2 = ValueType(file);
    //     void** value = (void**) malloc(2 * sizeof(VALUE_TYPE));
    //     value[0] = value1;
    //     value[1] = value2;
    //     return newValueType(COMPOSE_TYPE, (void*) value);
    // } else {
    //     ungetc(c, file);
    // }
    // return value1;
    return ValueType(file);
}

char* PropValue(FILE* file) {
    char c;
	ws(file);
    if ((c = getc(file)) != '[') {
        ungetc(c, file);
        return NULL;
    }
    char* value = CValueType(file);
    ws(file);
    if ((c = getc(file)) != ']') {
        printf(KRED "[FORMAT ERROR] Expecting ']', " KWHT "%c" KRED " found\n" KWHT, c);
        printFromFile(file, 20);
        exit(-1);
    }
    return value;
}

char* PropIdent(FILE* file) {
    char c;
    char* propIdent = (char*) malloc(STRING_RESERVED_LENGTH * sizeof(char));
    int length = 0;
    int rsvLength = STRING_RESERVED_LENGTH;
	ws(file);
    while (TRUE) {
        c = getc(file);
        if (c >= 'A' && c <= 'Z') {
            propIdent[length] = c;
            length++;
            if (length >= rsvLength - 1) {
                rsvLength += STRING_RESERVED_LENGTH;
                propIdent = (char*) realloc(propIdent, rsvLength * sizeof(char));
            }
        } else {
            ungetc(c, file);
            if (length == 0) {
                return NULL;
            }
            propIdent = (char*) realloc(propIdent, (length + 1) * sizeof(char));
            propIdent[length] = '\0';
            return propIdent;
        }
    }
}

PROPERTY Property(FILE* file) {
    char* propIdent = PropIdent(file);
    if (propIdent == NULL) {
        return NULL;
    }
	PROPERTY property = newProperty(propIdent);
	char* propValue;
	while ((propValue = PropValue(file)) != NULL) {        
		propertyPush(property, propValue);
	}
    if (length(property->propValueList) == 0) {
        printf(KRED "[FORMAT ERROR] Expecting a property value between '[]'" KWHT);
        printFromFile(file, 20);
        exit(-1);
    }
	return property;
}

// Node = LIST Property
LIST Node(FILE* file) {
	LIST node = newList();
	char c;
	ws(file);
	if ((c = getc(file)) != ';') {
        ungetc(c, file);
        return NULL;
	}
	PROPERTY property;
	while ((property = Property(file)) != NULL) {
		push(node, property);
	}
	return node;
}

// Sequence = LIST node
LIST Sequence(FILE* file) {
	LIST sequence = newList();
	LIST node;
	while ((node = Node(file)) != NULL) {
		push(sequence, (void*) node);
	}
	return sequence;
}

// GameTree = TREE {LIST sequence, LIST [GameTree, ...]}
TREE GameTree(FILE* file) {
	char c;
	ws(file);
	if ((c = getc(file)) != '(') {
        ungetc(c, file);
        return NULL;
    }
	TREE gameTree = newTree((void*) Sequence(file));
	TREE gameTree2;
	while ((gameTree2 = GameTree(file)) != NULL) {        
		treePush(gameTree, gameTree2);
	}
	ws(file);
	if ((c = getc(file)) != ')') {
        printf(KRED "[FORMAT ERROR] Expecting ')', " KWHT "%c" KRED " found\n" KWHT, c);
        exit(-1);
	}
	return gameTree;
}

// collection = LIST [GameTree, GameTree, ...]
LIST Collection(FILE* file) {
	LIST collection = newList();
	TREE gameTree;
	while ((gameTree = GameTree(file)) != NULL) {
		push(collection, gameTree);
	}
	return collection;
}

MATCH _populateMatch(LIST node, TREE gameTree) {
    MATCH match = newMatch();
    char* propIdent;
    PROPERTY property;
    for (int i = 0; i < length(node); i++) {
        property = (PROPERTY) get(node, i);
        char* propValue = (char*) get((LIST) getValueList(property), 0);
        propIdent = getIdent(property);
        if (strcmp(propIdent, "FF") == 0) {
            match->ff = propValue;
        } else if (strcmp(propIdent, "PB") == 0) {
            match->pb = propValue;
        } else if (strcmp(propIdent, "PW") == 0) {
            match->pw = propValue;
        } else if (strcmp(propIdent, "BR") == 0) {
            match->br = propValue;
        } else if (strcmp(propIdent, "WR") == 0) {
            match->wr = propValue;
        } else if (strcmp(propIdent, "BT") == 0) {
            match->bt = propValue;
        } else if (strcmp(propIdent, "WT") == 0) {
            match->wt = propValue;
        } else if (strcmp(propIdent, "RE") == 0) {
            match->result = Result(propValue);
        } else if (strcmp(propIdent, "KM") == 0) {
            match->komi = Real(propValue, NULL);
        } else if (strcmp(propIdent, "HA") == 0) {
            match->handicap = Int(propValue, NULL);
        } else if (strcmp(propIdent, "TM") == 0) {
            match->gameTime = Int(propValue, NULL);
        } else if (strcmp(propIdent, "DT") == 0) {
            match->date = propValue;
        } else if (strcmp(propIdent, "EV") == 0) {
            match->eventName = propValue;
        } else if (strcmp(propIdent, "RO") == 0) {
            match->tournamentRound = Int(propValue, NULL);
        } else if (strcmp(propIdent, "PC") == 0) {
            match->place = propValue;
        } else if (strcmp(propIdent, "RU") == 0) {
            match->rules = propValue;
        } else if (strcmp(propIdent, "GN") == 0) {
            match->gameName = propValue;
        } else if (strcmp(propIdent, "ON") == 0) {
            match->opening = propValue;
        } else if (strcmp(propIdent, "GC") == 0) {
            match->gameComment = propValue;
        } else if (strcmp(propIdent, "SO") == 0) {
            match->source = propValue;
        } else if (strcmp(propIdent, "US") == 0) {
            match->nameOfRecorder = propValue;
        } else if (strcmp(propIdent, "AN") == 0) {
            match->nameOfAnnotator = propValue;
        } else if (strcmp(propIdent, "CP") == 0) {
            match->copyright = propValue;
        }
    }
    match->gameTree = gameTree;
    return match;
}
MATCH populateMatch(LIST sgf) {
    TREE firstGame = (TREE) get(sgf, 0);
    LIST firstSequence = (LIST) treeRoot(firstGame);
    LIST firstNode = (LIST) get(firstSequence, 0);
    return _populateMatch(firstNode, firstGame);
}

MATCH readSGF(char* fileName) {    
    FILE* file = fopen(fileName, "r");
    if (file == NULL) {
        printf(KRED "[FILE ERROR] Could not open program " KWHT "%s\n", fileName);
        exit(-1);
    }
    MATCH match = populateMatch(Collection(file));
    fclose(file);
    return match;
}

#endif