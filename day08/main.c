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

#define NUM_CONNECTIONS (1000)

typedef struct {
    long x, y, z;
} point;

typedef struct {
    point a, b;
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

long find_next_group(connection connections[NUM_CONNECTIONS], bool *visited, int length) {
    int initial = -1;
    for (int i = 0; i < length; i++) {
        if (!visited[i]) {
            initial = i;
            break;
        }
    }

    if (initial == -1) {
        return 0;
    }

    queue queue = queue_new();
    queue_push(&queue, initial);
    visited[initial] = true;

    long found = 1;

    while (!queue_is_empty(&queue)) {
        int next_idx = queue_pop(&queue);

        for (int i = 0; i < NUM_CONNECTIONS; i++) {
            if (connections[i].a_idx == next_idx && !visited[connections[i].b_idx])  {
                queue_push(&queue, connections[i].b_idx);
                visited[connections[i].b_idx] = true;
                found++;
            } else if (connections[i].b_idx == next_idx && !visited[connections[i].a_idx])  {
                queue_push(&queue, connections[i].a_idx);
                visited[connections[i].a_idx] = true;
                found++;
            }
        }
    }

    return found;
}

bool is_already_connected(connection connections[NUM_CONNECTIONS], int connection_length, bool *visited, int length, int start, int end) {
    memset(visited, 0, sizeof(bool) * length);
    queue queue = queue_new();
    queue_push(&queue, start);
    visited[start] = true;

    while (!queue_is_empty(&queue)) {
        int next_idx = queue_pop(&queue);
        if (next_idx == end) {
            return true;
        }

        for (int i = 0; i < connection_length; i++) {
            if (connections[i].a_idx == next_idx && !visited[connections[i].b_idx])  {
                queue_push(&queue, connections[i].b_idx);
                visited[connections[i].b_idx] = true;
            } else if (connections[i].b_idx == next_idx && !visited[connections[i].a_idx])  {
                queue_push(&queue, connections[i].a_idx);
                visited[connections[i].a_idx] = true;
            }
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

    connection connections[NUM_CONNECTIONS] = {};
    bool *visited = malloc(sizeof(bool) * length);

    for (int i = 0; i < NUM_CONNECTIONS; i++) {
        double min_dist = DBL_MAX;
        for (int j = 0; j < length - 1; j++) {
            for (int k = j + 1; k < length; k++) {
                bool already_used = false;
                for (int l = 0; l < i; l++) {
                    if (j == connections[l].a_idx && k == connections[l].b_idx) {
                        already_used = true;
                        break;
                    }
                }
                if (already_used) {
                    continue;
                }

                double dist = sqrt(pow(points[j].x - points[k].x, 2) +
                                   pow(points[j].y - points[k].y, 2) +
                                   pow(points[j].z - points[k].z, 2));

                if (dist < min_dist) {
                    min_dist = dist;
                    connections[i].a = points[j];
                    connections[i].b = points[k];
                    connections[i].a_idx = j;
                    connections[i].b_idx = k;
                }
            }
        }

        printf("%d - %d to %d dist: %f\n", i, connections[i].a_idx,
               connections[i].b_idx, min_dist);
    }

    memset(visited, 0, sizeof(bool) * length);

    long found;

    long max1 = 0;
    long max2 = 0;
    long max3 = 0;

    do {
        found = find_next_group(connections, visited, length);
        if (found > max1) {
            max3 = max2;
            max2 = max1;
            max1 = found;
        } else if (found > max2) {
            max3 = max2;
            max2 = found;
        } else if (found > max3) {
            max3 = found;
        }
        printf("found: %ld\n", found);
    } while (found != 0);

    printf("product: %ld\n", max1 * max2 * max3);
}
