// PID: 730710582
// I pledge the COMP211 honor code.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct LineNode {
    char *line;
    struct LineNode *next;
} LineNode;

LineNode* read_input_lines() {
    LineNode *head = NULL;
    LineNode *tail = NULL;
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, stdin)) != -1) {
        if (line[nread - 1] == '\n') {
            line[nread - 1] = '\0';
        }

        LineNode *node = malloc(sizeof(LineNode));
        if (!node) {
            perror("malloc failed");
            exit(1);
        }

        node->line = strdup(line);
        node->next = NULL;

        if (!head)
            head = node;
        else
            tail->next = node;

        tail = node;
    }

    free(line);
    return head;
}

LineNode* insert_sorted(LineNode* sorted, LineNode* new_node) {
    if (!sorted || strcmp(new_node->line, sorted->line) < 0) {
        new_node->next = sorted;
        return new_node;
    }

    LineNode *curr = sorted;
    while (curr->next && strcmp(new_node->line, curr->next->line) >= 0) {
        curr = curr->next;
    }

    new_node->next = curr->next;
    curr->next = new_node;
    return sorted;
}

LineNode* sort_lines(LineNode* head) {
    LineNode *sorted = NULL;
    while (head) {
        LineNode *next = head->next;
        head->next = NULL;
        sorted = insert_sorted(sorted, head);
        head = next;
    }
    return sorted;
}

void print_and_free_lines(LineNode* head) {
    while (head) {
        printf("%s\n", head->line);
        free(head->line);
        LineNode *next = head->next;
        free(head);
        head = next;
    }
}

int main(void) {
    LineNode *lines = read_input_lines();
    lines = sort_lines(lines);
    print_and_free_lines(lines);
    return 0;
}