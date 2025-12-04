#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    long sum = 0;

    char *line;
    while (scanf("%m[0-9]\n", &line) == 1) {
        int len = strlen(line);
        int *line_parsed = malloc(len * sizeof(int));
        for (int i = 0; i < len; i++) {
            line_parsed[i] = line[i] - '0';
        }

        long jolts = 0;

        int current_pos = 0;

        for (int i = 11; i >= 0; i--) {
            long max = -1;
            int max_idx = -1;
            for (; current_pos < len - i; current_pos++) {
                if (line_parsed[current_pos] > max) {
                    max = line_parsed[current_pos];
                    max_idx = current_pos;
                }
            }

            jolts *= 10;
            jolts += max;
            current_pos = max_idx + 1;

            assert(max_idx >= 0);
        }

        sum += jolts;
    }

    printf("sum: %ld\n", sum);
}
