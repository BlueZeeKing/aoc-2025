#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int find_reachable(bool* occupied, bool *next_occupied, int width, int height) {
    int count = 0;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int num_neighbors = 0;
            for (int y_offset = -1; y_offset <= 1; y_offset++) {
                for (int x_offset = -1; x_offset <= 1; x_offset++) {
                    if (y_offset == 0 && x_offset == 0) {
                        continue;
                    }

                    if (y + y_offset >= height || y + y_offset < 0 ||
                        x + x_offset >= width || x + x_offset < 0) {
                        continue;
                    }

                    if (occupied[(y + y_offset) * width + (x + x_offset)]) {
                        num_neighbors++;
                    }
                }
            }
            if (num_neighbors < 4 && occupied[y * width + x]) {
                next_occupied[y * width + x] = false;
                count++;
            }
        }
    }

    return count;
}

int main() {
    char c;
    int width = -1;
    int height = 0;

    int capacity = 1;
    int length = 0;
    bool *occupied = malloc(sizeof(bool) * capacity);

    while ((c = getc(stdin)) != EOF) {
        ungetc(c, stdin);
        int current_width = 0;
        while ((c = getc(stdin)) != '\n' && c != EOF) {
            current_width++;

            if (length == capacity) {
                capacity *= 2;
                occupied = realloc(occupied, sizeof(bool) * capacity);
            }

            occupied[length++] = c == '@';
        }

        if (current_width == 0) {
            break;
        }

        if (width == -1) {
            width = current_width;
        }
        height++;
        assert(current_width == width);
    }

    assert(length == width * height);

    bool *next_occupied = malloc(sizeof(bool) * length);
    memcpy(next_occupied, occupied, sizeof(bool) * length);

    int sum = 0;
    int count = 1;
    while ((count = find_reachable(occupied, next_occupied, width, height)) > 0) {
        sum += count;

        bool *temp;
        temp = occupied;
        occupied = next_occupied;
        next_occupied = temp;

        memcpy(next_occupied, occupied, sizeof(bool) * length);
    }

    printf("%d\n", sum);
}
