#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

enum {
    BASE_YEAR = 1900
};

int main(int argc, char **argv) {
    if (argc < 2) {
        return 1;
    }

    FILE *file = fopen(argv[1], "r");

    if (!file) {
        return 1;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    time_t last_time = 0;
    time_t diff = 0;
    size_t line_num = 0;

    while ((read = getline(&line, &len, file)) != -1) {
        struct tm timeinfo;
        memset(&timeinfo, 0, sizeof(timeinfo));
        sscanf(line, "%04d/%02d/%02d %02d:%02d:%02d", &timeinfo.tm_year, &timeinfo.tm_mon,
               &timeinfo.tm_mday, &timeinfo.tm_hour, &timeinfo.tm_min, &timeinfo.tm_sec);
        timeinfo.tm_year -= BASE_YEAR;
        --timeinfo.tm_mon;
        timeinfo.tm_isdst = -1;

        time_t current_time = mktime(&timeinfo);
        diff = current_time - last_time;
        last_time = current_time;

        if (line_num++ > 0) {
            printf("%ld\n", diff);
        }
    }

    free(line);
    fclose(file);
}