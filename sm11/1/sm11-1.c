#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <fcntl.h>

int main(int argc, char **argv) {
    enum { PERMS = 0600 };
    int fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, PERMS);

    if (fd == -1) {
        return 1;
    }

    size_t rows = strtol(argv[2], NULL, 10);
    size_t cols = strtol(argv[3], NULL, 10);

    size_t matrix_size = rows * cols;

    int *buffer;
    size_t size = sizeof(*buffer) * matrix_size;
    if (ftruncate(fd, size) < 0) {
        close(fd);
        return 1;
    }
    void *data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (data == MAP_FAILED) {
        return 1;
    }

    buffer = data;
    size_t max_l = 0, max_r = cols - 1, max_up = 0, max_down = rows - 1;
    int x = 1;

    for (;;) {

        if (max_l > max_r)
            break;

        for (size_t i = max_l; i <= max_r; ++i) {
            buffer[max_up * cols + i] = x++;
        }

        ++max_up;

        if (max_up > max_down)
            break;

        for (size_t i = max_up; i <= max_down; ++i) {
            buffer[i * cols + max_r] = x++;
        }

        if (max_r-- == 0 || max_r < max_l)
            break;

        for (size_t i = max_r + 1; i > max_l; --i) {
            buffer[max_down * cols + i - 1] = x++;
        }

        if (max_down-- == 0 || max_down < max_up)
            break;

        for (size_t i = max_down + 1; i > max_up; --i) {
            buffer[(i - 1) * cols + max_l] = x++;
        }

        ++max_l;
    }

    munmap(buffer, size);
    close(fd);
}