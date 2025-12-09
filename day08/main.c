#include <ctype.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

typedef struct {
    char *data;
    size_t length;
} str_ref;

typedef struct {
    str_ref str;
    char split;
} split_iter;

str_ref strempty() {
    return (str_ref) { .data = NULL, .length = 0 };
}

str_ref strfromcstr(char *cstr) {
    return (str_ref) { .data = cstr, .length = strlen(cstr) };
}

long strtonum(str_ref str) {
    long num = 0;
    for (int i = 0; i < str.length; i++) {
        assert(isdigit(str.data[i]));
        num *= 10;
        num += str.data[i] - '0';
    }
    return num;
}

bool strisempty(str_ref str) {
    return str.length == 0;
}

split_iter split_iter_new(str_ref str, char split) {
    return (split_iter) { str, split };
}

str_ref split_iter_next(split_iter *iter) {
    if (strisempty(iter->str)) {
        return strempty();
    }

    while (iter->str.data[0] == iter->split) {
        iter->str.data++;
        iter->str.length--;

        if (strisempty(iter->str)) {
            return strempty();
        }
    }

    str_ref return_value = iter->str;
    return_value.length = 0;

    while (iter->str.data[0] != iter->split) {
        iter->str.data++;
        iter->str.length--;
        return_value.length++;

        if (strisempty(iter->str)) {
            return return_value;
        }
    }

    return return_value;
}

bool split_iter_next_loop(split_iter *iter, str_ref *part) {
    *part = split_iter_next(iter);
    return !strisempty(*part);
}

str_ref readfile(FILE *fp) {
    int capacity = 1024;
    str_ref str = { malloc(capacity), 0 };
    char buffer[1024];
    int amount_read;
    while ((amount_read = fread(&buffer, 1, 1024, fp)) != 0) {
        if (str.length == capacity) {
            capacity *= 2;
            str.data = realloc(str.data, capacity);
        }
        memcpy(str.data + str.length, &buffer, amount_read);
        str.length += amount_read;
    }

    return str;
}

int64_t split_iter_count(split_iter iter) {
    int64_t count = 0;
    while (!strisempty(split_iter_next(&iter))) {
        count++;
    }
    return count;
}

str_ref split_iter_get(split_iter iter, int64_t idx) {
    while (idx-- > 0) {
        split_iter_next(&iter);
    }
    return split_iter_next(&iter);
}

typedef struct {
    long x, y, z;
} point;

typedef struct {
    int a_idx, b_idx;
} connection;

typedef struct queue_item {
    int next_idx;
    struct queue_item *next;
    struct queue_item *prev;
} queue_item;

typedef struct {
    queue_item *head;
    queue_item *tail;
} queue;

queue queue_new() {
    return (queue) { NULL, NULL };
}

void queue_push(queue *queue, int next_idx) {
    if (!queue->tail) {
        queue->tail = malloc(sizeof(queue_item));
        queue->tail->next_idx = next_idx;
        queue->tail->next = NULL;
        queue->tail->prev = NULL;
        queue->head = queue->tail;
    } else {
        queue_item *new_tail = malloc(sizeof(queue_item));
        new_tail->prev = queue->tail;
        new_tail->next = NULL;
        new_tail->next_idx = next_idx;
        queue->tail->next = new_tail;
        queue->tail = new_tail;
    }
}

int queue_pop(queue *queue) {
    assert(queue->head);

    if (queue->head == queue->tail) {
        queue->tail = NULL;
    }

    int value = queue->head->next_idx;
    queue_item *prev_head = queue->head;
    queue->head = queue->head->next;
    free(prev_head);
    if (queue->head) {
        queue->head->prev = NULL;
    }

    return value;
}

bool queue_is_empty(queue *queue) {
    return queue->head == NULL;
}

bool is_connected(connection *connections, int connections_length, bool *visited, int length) {
    memset(visited, 0, sizeof(bool) * length);
    queue queue = queue_new();
    queue_push(&queue, 0);
    visited[0] = true;

    while (!queue_is_empty(&queue)) {
        int next_idx = queue_pop(&queue);

        for (int i = 0; i < connections_length; i++) {
            if (connections[i].a_idx == next_idx && !visited[connections[i].b_idx])  {
                queue_push(&queue, connections[i].b_idx);
                visited[connections[i].b_idx] = true;
            } else if (connections[i].b_idx == next_idx && !visited[connections[i].a_idx])  {
                queue_push(&queue, connections[i].a_idx);
                visited[connections[i].a_idx] = true;
            }
        }
    }

    for (int i = 0; i < length; i++) {
        if (!visited[i]) {
            return false;
        }
    }

    return true;
}

bool is_already_used(connection *connections, int i, int j, int k) {
    for (int l = 0; l < i; l++) {
        if (j == connections[l].a_idx && k == connections[l].b_idx) {
            return true;
        }
    }

    return false;
}

int main() {
    str_ref file_contents = readfile(stdin);
    split_iter lines = split_iter_new(file_contents, '\n');
    str_ref line;

    int length = 0, capacity = 1;
    point *points = malloc(sizeof(point) * capacity);

    while (split_iter_next_loop(&lines, &line)) {
        split_iter parts = split_iter_new(line, ',');
        if (length == capacity) {
            capacity *= 2;
            points = realloc(points, sizeof(point) * capacity);
        }
        points[length].x = strtonum(split_iter_next(&parts));
        points[length].y = strtonum(split_iter_next(&parts));
        points[length].z = strtonum(split_iter_next(&parts));
        length++;
    }

    int connections_capacity = 1, connections_length = 0;
    connection *connections = malloc(sizeof(connection) * connections_capacity);
    bool *visited = malloc(sizeof(bool) * length);

    bool *connection_set = malloc(sizeof(bool) * length * length);

    int prev_x1 = 0;
    int prev_x2 = 0;

    while (connections_length == 0 || !is_connected(connections, connections_length, visited, length)) {
        if (connections_length == connections_capacity) {
            connections_capacity *= 2;
            connections = realloc(connections, connections_capacity * sizeof(connection));
        }
        double min_dist = DBL_MAX;
        for (int j = 0; j < length - 1; j++) {
            for (int k = j + 1; k < length; k++) {
                if (connection_set[j + k * length]) {
                    continue;
                }

                double dist = sqrt(pow(points[j].x - points[k].x, 2) +
                                   pow(points[j].y - points[k].y, 2) +
                                   pow(points[j].z - points[k].z, 2));

                if (dist < min_dist) {
                    min_dist = dist;
                    connections[connections_length].a_idx = j;
                    connections[connections_length].b_idx = k;
                    prev_x1 = points[j].x;
                    prev_x2 = points[k].x;
                }
            }
        }
        connection_set[connections[connections_length].a_idx + connections[connections_length].b_idx * length] = true;

        connections_length++;
    }

    printf("product: %d\n", prev_x1 * prev_x2);
}
