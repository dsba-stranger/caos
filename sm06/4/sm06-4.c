#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct Node {
    int value;
    struct Node *next;
} Node;

typedef struct List {
    Node *head;
} List;

bool PushBack(List *list, int value) {
    Node *node = calloc(1, sizeof(Node));
    if (!node) {
        return false;
    }
    node->value = value;
    node->next = list->head;
    list->head = node;
    return true;
}

void Free(List *list) {
    Node *cur = list->head;

    while (cur) {
        Node *next = cur->next;
        free(cur);
        cur = next;
    }

    list->head = NULL;
}

int main() {
    int x = 0;
    List list = {};
    int ret_code = 0;

    while ((ret_code = scanf("%d", &x)) != EOF) {
        if (ret_code != 1) {
            Free(&list);
            return 1;
        }
        if (!PushBack(&list, x)) {
            Free(&list);
            return 1;
        }
    }

    Node *cur = list.head;

    while (cur) {
        Node *next = cur->next;
        printf("%d\n", cur->value);
        free(cur);
        cur = next;
    }

}