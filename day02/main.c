#include <math.h>
#include <stdio.h>
#include <stdbool.h>

int get_num_digits(long num) {
    return ceil(log10((double) num + 0.5));
}

bool is_invalid(long num) {
    int num_digits = get_num_digits(num);

    for (int i = 1; i <= num_digits / 2; i++) {
        if (num_digits % i != 0) {
            continue;
        }

        long temp_num = num;
        long mask = pow(10, i);

        long segment = num % mask;
        temp_num /= mask;

        bool invalid = true;

        for (int j = 1; j < num_digits / i; j++) {
            if (temp_num % mask != segment) {
                invalid = false;
                break;
            }

            temp_num /= mask;
        }

        if (invalid) {
            return true;
        }
    }

    return false;
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
