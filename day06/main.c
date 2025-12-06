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

int main() {
    str_ref file = readfile(stdin);
    assert(!strisempty(file));

    split_iter lines = split_iter_new(file, '\n');

    int64_t height = split_iter_count(lines) - 1;

    split_iter ops_iter = split_iter_new(split_iter_get(lines, height), ' ');
    int line_width = ops_iter.str.length;
    char* initial_ptr = ops_iter.str.data;
    int64_t width = split_iter_count(ops_iter);
    char *ops = malloc(width);
    int64_t *offsets = malloc(width * sizeof(int64_t));
    int64_t *lengths = malloc(width * sizeof(int64_t));

    for (int i = 0; i < width; i++) {
        str_ref op = split_iter_next(&ops_iter);
        ops[i] = op.data[0];
        offsets[i] = op.data - initial_ptr;
        if (i != 0) {
            lengths[i - 1] = offsets[i] - offsets[i - 1] - 1;
        }
    }
    lengths[width - 1] = line_width - offsets[width - 1];

    int64_t sum = 0;

    for (int x = 0; x < width; x++) {
        char op = ops[x];
        int64_t result = 0;
        if (op == '*') {
            result = 1;
        }

        for (int x_inner = 0; x_inner < lengths[x]; x_inner++) {
            int64_t value = 0;
            for (int y = 0; y < height; y++) {
                char digit = split_iter_next(&lines).data[x_inner + offsets[x]];
                if (digit == ' ') {
                    continue;
                }
                value *= 10;
                value += digit - '0';
            }
            lines = split_iter_new(file, '\n');

            if (op == '*') {
                result *= value;
            } else {
                result += value;
            }
        }


        sum += result;
    }

    free(file.data);

    printf("sum: %ld\n", sum);
}
