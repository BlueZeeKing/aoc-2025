#include <math.h>
#include <stdio.h>
#include <stdbool.h>

int get_num_digits(long num) {
    return ceil(log10(num));
}

bool is_invalid(long num) {
    int num_digits = get_num_digits(num);

    if (num_digits % 2 == 1) {
        return false;
    }

    return num / (long) pow(10, num_digits / 2) == num % (long) pow(10, num_digits / 2);
}

int main() {
    long sum = 0;
    long start, end;
    while (scanf("%ld-%ld,", &start, &end) == 2) {
        for (long i = start; i <= end; i++) {
            if (is_invalid(i)) {
                sum += i;
            }
        }
    }

    printf("sum: %ld\n", sum);
}
