#include <stdlib.h>
#include <unistd.h>

enum {
    BUF_SIZE = 4096 // 1KB
};

ssize_t copy_file(int in_fd, int out_fd) {
    unsigned char buf[BUF_SIZE];
    ssize_t res = 0;
    ssize_t bytes_read = 0;

    while ((bytes_read = read(in_fd, buf, BUF_SIZE)) > 0) {
        ssize_t bytes_written = 0;
        ssize_t shift = 0;
        while (bytes_read > 0 && (bytes_written = write(out_fd, buf + shift, bytes_read)) > 0) {
            shift += bytes_written;
            res += bytes_written;
            bytes_read -= bytes_written;
        }
        if (bytes_written == -1) {
            return -1;
        }
    }

    return bytes_read == 0 ? res : -1;
}

#ifdef TESTING
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

int main() {
    int in_fd = open("input.txt", O_RDWR);
    int out_fd = open("output.txt", O_RDWR);
    printf("%lu\n", copy_file(in_fd, out_fd));
    close(in_fd);
    close(out_fd);
    return 0;
}

#endif