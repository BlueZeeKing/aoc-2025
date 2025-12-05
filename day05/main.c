#include <stddef.h>
#include <stdbool.h>
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
        num *= 10;
        num += str.data[i] - '0';
    }
    return num;
}

bool strisempty(str_ref str) {
    return str.length == 0;
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

typedef struct {
    long low;
    long high;
} range;

int main() {
    int ranges_capacity = 1;
    int ranges_length = 0;
    range *ranges = malloc(sizeof(range) * ranges_capacity);

    str_ref line = readline();
    while (line.length != 0) {
        split_iter line_iter = {
            .str = line,
            .split = '-',
        };

        if (ranges_length == ranges_capacity) {
            ranges_capacity *= 2;
            ranges = realloc(ranges, sizeof(range) * ranges_capacity);
        }
        ranges[ranges_length].low = strtonum(split_iter_next(&line_iter));
        ranges[ranges_length++].high = strtonum(split_iter_next(&line_iter));

        free(line.data);
        line = readline();
    }

    int ingredients_capacity = 1;
    int ingredients_length = 0;
    long *ingredients = malloc(sizeof(long) * ingredients_capacity);

    free(line.data);
    line = readline();
    while (line.length != 0) {
        if (ingredients_length == ingredients_capacity) {
            ingredients_capacity *= 2;
            ingredients = realloc(ingredients, sizeof(long) * ingredients_capacity);
        }
        ingredients[ingredients_length++] = strtonum(line);

        free(line.data);
        line = readline();
    }

    free(line.data);

    int num_fresh = 0;

    for (int i = 0; i < ingredients_length; i++) {
        for (int j = 0; j < ranges_length; j++) {
            if (ranges[j].low <= ingredients[i] && ranges[j].high >= ingredients[i]) {
                num_fresh++;
                break;
            }
        }
    }

    printf("%d\n", num_fresh);

    free(ranges);
    free(ingredients);
}
