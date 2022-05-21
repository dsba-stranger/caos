#include <stdlib.h>
#include <stdio.h>

typedef struct Node {
    int value;
    int count;
    struct Node *prev;
    struct Node *next;
} Node;

Node *find_node(Node *head, int value) {
    while (head) {
        if (head->value == value) {
            return head;
        }
        head = head->next;
    }
    return NULL;
}

Node *create_node(int value) {
    Node *node = calloc(1, sizeof(*node));
    if (!node) {
        return NULL;
    }
    node->value = value;
    node->count = 1;
    node->prev = node->next = NULL;
    return node;
}

void move_front(Node *node, Node **head, Node **tail) {
    if (*head) {
        (*head)->prev = node;
    }

    if (node->prev) {
        node->prev->next = node->next;
    }

    if (node->next) {
        node->next->prev = node->prev;
    } else if (node->prev) {
        *tail = node->prev;
    }

    node->prev = NULL;
    node->next = *head;
    *head = node;

    if (!*tail) {
        *tail = *head;
    }
}

void free_nodes(Node *head) {
    while (head) {
        Node *next = head->next;
        free(head);
        head = next;
    }
}

int main() {
    Node *head = NULL;
    Node *tail = NULL;
    int x = 0;
    int ret_code = 0;

    while ((ret_code = scanf("%d", &x)) != EOF) {
        if (ret_code != 1) {
            free_nodes(head);
            return 1;
        }

        Node *node = find_node(head, x);

        if (!node) {
            node = create_node(x);

            if (!node) {
                free_nodes(head);
                return 1;
            }
        } else {
            ++node->count;
        }

        if (node != head) {
            move_front(node, &head, &tail);
        }
    }

    while (tail) {
        printf("%d %d\n", tail->value, tail->count);
        tail = tail->prev;
    }

    free_nodes(head);
}