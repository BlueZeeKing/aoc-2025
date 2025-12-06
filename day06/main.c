#include <ctype.h>
#include <stddef.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

str_ref readline() {
    int capacity = 1;
    str_ref str = { malloc(capacity), 0 };
    char c;
    while ((c = getc(stdin)) != '\n' && c != EOF) {
        if (capacity == str.length) {
            capacity *= 2;
            str.data = realloc(str.data, capacity);
        }
        str.data[str.length++] = c;
    }
    return str;
}

int main() {
    str_ref line = readline();
    assert(!strisempty(line));

    int width = -1;

    int height_capacity = 1;
    int height = 0;
    int64_t **problems = malloc(sizeof(int64_t *) * height_capacity);

    char *ops = NULL;

    while (!strisempty(line)) {
        split_iter iter = split_iter_new(line, ' ');

        int width_capacity = 1;
        int current_width = 0;
        if (width != -1) {
            width_capacity = width;
        }
        int64_t *parsed_line = malloc(sizeof(int64_t) * width_capacity);

        bool is_ops = false;

        str_ref part = split_iter_next(&iter);
        while (!strisempty(part)) {
            if (current_width == width_capacity) {
                width_capacity *= 2;
                parsed_line = realloc(parsed_line, sizeof(int64_t) * width_capacity);
            }
            if (isdigit(part.data[0])) {
                parsed_line[current_width++] = strtonum(part);
            } else {
                is_ops = true;
                if (ops == NULL) {
                    ops = malloc(sizeof(char) * width);
                }
                ops[current_width++] = part.data[0];
            }

            part = split_iter_next(&iter);
        }

        if (is_ops) {
            break;
        }

        if (width == -1) {
            width = current_width;
        }
        assert(width == current_width);

        if (height == height_capacity) {
            height_capacity *= 2;
            problems = realloc(problems, sizeof(int64_t *) * height_capacity);
        }
        problems[height++] = parsed_line;

        free(line.data);
        line = readline();
    }
    free(line.data);

    int64_t sum = 0;

    for (int x = 0; x < width; x++) {
        char op = ops[x];
        int64_t value = 0;
        if (op == '*') {
            value = 1;
        }
        for (int y = 0; y < height; y++) {
            if (op == '*') {
                value *= problems[y][x];
            } else {
                value += problems[y][x];
            }
        }
        sum += value;
    }

    printf("sum: %ld\n", sum);
}
