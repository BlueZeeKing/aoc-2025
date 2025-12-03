#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main() {
    int sum = 0;

    char *line;
    while (scanf("%m[0-9]\n", &line) == 1) {
        int len = strlen(line);
        int *line_parsed = malloc(len * sizeof(int));
        for (int i = 0; i < len; i++) {
            line_parsed[i] = line[i] - '0';
        }

        int max = 0;

        for (int i = 0; i < len - 1; i++) {
            for (int j = i + 1; j < len; j++) {
                int num = line_parsed[i] * 10 + line_parsed[j];
                if (num > max) {
                    max = num;
                }
            }
        }
        
        sum += max;

        free(line_parsed);
        free(line);
    }

    printf("sum: %d\n", sum);
}
