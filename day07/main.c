#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

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

int main() {
    int width = -1;
    int height = 0;

    int capacity = 1;
    int length = 0;
    bool *is_splitter = malloc(sizeof(bool) * capacity);

    int start_x, start_y;

    str_ref file_contents = readfile(stdin);
    split_iter lines = split_iter_new(file_contents, '\n');

    str_ref line = split_iter_next(&lines);
    while (!strisempty(line)) {
        int current_width = 0;
        for (int i = 0; i < line.length; i++) {
            if (length == capacity) {
                capacity *= 2;
                is_splitter = realloc(is_splitter, sizeof(bool) * capacity);
            }

            is_splitter[length++] = line.data[i] == '^';

            if (line.data[i] == 'S') {
                start_y = height;
                start_x = current_width;
            }

            current_width++;
        }

        if (current_width == 0) {
            break;
        }

        if (width == -1) {
            width = current_width;
        }
        height++;
        assert(current_width == width);
        line = split_iter_next(&lines);
    }

    assert(length == width * height);

    bool *has_beam = malloc(sizeof(bool) * width);
    memset(has_beam, 0, sizeof(bool) * width);
    has_beam[start_x] = true;
    bool *next_has_beam = malloc(sizeof(bool) * width);

    int num_splits = 0;

    for (int y = 0; y < height; y++) {
        memset(next_has_beam, 0, sizeof(bool) * width);
        for (int x = 0; x < width; x++) {
            if (has_beam[x] && !is_splitter[x + y * width]) {
                next_has_beam[x] = true;
            } else if (has_beam[x] && is_splitter[x + y * width]) {
                num_splits++;
                if (x > 0) {
                    next_has_beam[x - 1] = true;
                }
                if (x < width - 1) {
                    next_has_beam[x + 1] = true;
                }
            }
        }

        bool *temp = has_beam;
        has_beam = next_has_beam;
        next_has_beam = temp;
    }

    printf("%d\n", num_splits);
}
