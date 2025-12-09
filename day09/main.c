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
    long x, y;
} point;

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
        length++;
    }

    long max_area = LONG_MIN;

    for (int i = 0; i < length - 1; i++) {
        for (int j = i + 1; j < length; j++) {
            long area = labs(points[i].x - points[j].x + 1) * labs(points[i].y - points[j].y + 1);
            if (area > max_area) {
                max_area = area;
            }
        }
    }

    printf("%ld\n", max_area);
}
