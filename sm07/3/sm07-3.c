#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

typedef struct Node {
    int value;
    struct Node *left;
    struct Node *right;
} Node;

typedef struct Tree {
    Node *root;
} Tree;

typedef enum Result {
    ERROR, REPEATED, OK
} Result;

Result tree_insert(Tree *tree, int value) {
    Node *cur = tree->root;
    Node *parent = NULL;

    while (cur) {
        parent = cur;

        if (cur->value > value) {
            cur = cur->left;
        } else if (cur->value < value) {
            cur = cur->right;
        } else {
            return REPEATED;
        }
    }

    Node *node = calloc(1, sizeof(*node));

    if (!node) {
        return ERROR;
    }

    node->value = value;

    if (!parent) {
        tree->root = node;
    } else if (parent->value > value) {
        parent->left = node;
    } else {
        parent->right = node;
    }

    return OK;
}

void node_print(Node *node) {
    if (node) {
        node_print(node->right);
        printf("%d ", node->value);
        node_print(node->left);
    }
}

void tree_print(Tree *tree) {
    node_print(tree->root);
    printf("0\n");
}

void node_free(Node *node) {
    if (node) {
        node_free(node->left);
        node_free(node->right);
        free(node);
    }
}

void tree_free(Tree *tree) {
    node_free(tree->root);
    tree->root = NULL;
}

int main() {
    int value = 0;
    int ret_code = 0;

    Tree tree = {};

    while ((ret_code = scanf("%d", &value)) != EOF) {
        if (ret_code != 1) {
            tree_free(&tree);
            return 1;
        }

        if (value != 0) {
            if (tree_insert(&tree, value) == ERROR) {
                tree_free(&tree);
                return 1;
            }
        } else {
            tree_print(&tree);
            tree_free(&tree);
        }
    }

    if (value != 0) {
        tree_print(&tree);
    }

    tree_free(&tree);

    return 0;
}