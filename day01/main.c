#include <stdio.h>

int main() {
    char direction;
    int amount;

    int current_pos = 50;

    int count = 0;

    while (scanf("%c%d\n", &direction, &amount) == 2) {
        if (direction == 'L') {
            current_pos -= amount;
            current_pos += 100;
            current_pos %= 100;
        } else {
            current_pos += amount;
            current_pos %= 100;
        }

        if (current_pos == 0) {
            count++;
        }
    }

    printf("%d\n", count);
}
