#include <stdio.h>
#include <time.h>

enum Constants {
    BUF_SIZE = 32,
    SECONDS_IN_DAY = 86400
};

const char *new_date(time_t current_time, int num_days, const char *buffer, size_t len) {
    const char *overflow_str = "OVERFLOW";
    time_t seconds;

    if (__builtin_mul_overflow(num_days, SECONDS_IN_DAY, &seconds)) {
        return overflow_str;
    }

    time_t new_time;

    if (__builtin_add_overflow(current_time, seconds, &new_time)) {
        return overflow_str;
    }

    struct tm timeinfo;
    localtime_r(&new_time, &timeinfo);
    strftime((char*)buffer, sizeof(*buffer) * len, "%Y-%m-%d", &timeinfo);
    return buffer;
}

int main() {
    time_t current_time = time(NULL);

    int value = 0;
    int ret_code = 0;

    while ((ret_code = scanf("%d", &value)) != EOF) {
        if (ret_code != 1) {
            return 1;
        }
        char date[BUF_SIZE] = {};
        printf("%s\n", new_date(current_time, value, date, sizeof(date)));
    }
}