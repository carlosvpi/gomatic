#ifndef __TREE__

#define __TREE__
#define CHILDREN_EXPANSION_LENGTH 10

#include <stdlib.h>
#include "list.c"

typedef struct _tree* TREE;

struct _tree;

TREE newTree(void* root);
BOOL freeTree(TREE tree);
BOOL treePush(TREE tree, TREE child);
void* treeRoot(TREE tree);
TREE treeGet(TREE tree, int index);

struct _tree {
	void* root;
	LIST children;
};

TREE newTree(void* root) {
	TREE tree = (TREE) malloc(sizeof(struct _tree));
	tree->root = root;
	tree->children = newList();
	return tree;
}

BOOL freeTree(TREE tree) {
	if (tree == NULL) {
		return FALSE;
	}
	free(tree->root);
	freeList(tree->children);
	free(tree);
	return TRUE;
}

BOOL treePush(TREE tree, TREE child) {
	return tree == NULL ? FALSE : push(tree->children, (void*) child);
}

void* treeRoot(TREE tree) {
	return tree == NULL ? NULL : tree->root;
}

TREE treeGet(TREE tree, int index) {
	return tree == NULL ? NULL : (TREE) get(tree->children, index);
}

#endif