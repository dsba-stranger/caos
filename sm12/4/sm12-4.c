#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>

enum Constants {
    TMP_FILE_PERMS = 0700
};

const char *get_tmp_dir() {
    char *env;

    if ((env = getenv("XDG_RUNTIME_DIR"))) {
        return env;
    }

    if ((env = getenv("TMPDIR"))) {
        return env;
    }

    return "/tmp";
}

int main(int argc, char **argv) {
    char tmp_file[PATH_MAX] = {};
    int written = snprintf(tmp_file, sizeof(tmp_file), "%s/apb%d.py", get_tmp_dir(), getpid());

    if (written < 0 || (unsigned)written > sizeof(tmp_file)) {
        return 1;
    }

    int fd = open(tmp_file, O_RDWR | O_CREAT | O_TRUNC | O_EXCL, TMP_FILE_PERMS);

    if (fd < 0) {
        return 1;
    }

    FILE *file = fdopen(fd, "w");

    if (!file) {
        return 1;
    }

    const char *python_program = "#!/usr/bin/python3\n"
                                 "from os import remove\n"
                                 "from sys import argv\n"
                                 "print(";

    fprintf(file, "%s", python_program);

    for (int i = 1; i < argc; ++i) {
        if (i > 1) {
            fprintf(file, "*");
        }
        fprintf(file, "%s", argv[i]);
    }

    fprintf(file, ")\nremove(argv[0])");
    fclose(file);

    execlp(tmp_file, tmp_file, NULL);
    exit(1);
}