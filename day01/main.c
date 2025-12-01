#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

int main() {
    char direction;
    int amount;

    int current_pos = 50;
    bool prev_was_0 = false;

    int count = 0;

    while (scanf("%c%d\n", &direction, &amount) == 2) {
        if (direction == 'L') {
            current_pos -= amount;
        } else {
            current_pos += amount;
        }

        count += abs(current_pos / 100);

        if (current_pos <= 0 && !prev_was_0) {
            count++;
        }

        current_pos %= 100;
        if (current_pos < 0) {
            current_pos += 100;
        }
        prev_was_0 = current_pos == 0;
    }

    printf("%d\n", count);
}
