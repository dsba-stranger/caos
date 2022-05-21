#include <unistd.h>

enum {
    STATIC_BUF_SIZE = 4096 // 1KB
};

static unsigned char static_buf[STATIC_BUF_SIZE]; // статический буфер

struct FileWriteState
{
    int fd;               // "файловый дескриптор", для вывода на стандартный поток вывода - 1
    unsigned char *buf;   // указатель на буфер
    int bufsize;          // размер буфера
    size_t written_bytes; // сколько уже записали
};

static struct FileWriteState dummy = {STDOUT_FILENO, static_buf, STATIC_BUF_SIZE, 0};
struct FileWriteState *stout = &dummy;

void flush(struct FileWriteState *out) {
    if (!out) {
        return;
    }

    write(out->fd, out->buf, out->written_bytes);
    out->written_bytes = 0;
}

void writechar(int c, struct FileWriteState *out) {
    if (!out) {
        return;
    }

    // вывод, если буфер заполнен
    if (out->written_bytes >= out->bufsize) {
        flush(out);
    }
    out->buf[out->written_bytes++] = c;
}


#ifdef TESTING
#include <stdio.h>
#include <string.h>

int main() {
    writechar('a', stout);
    writechar('b', stout);
    writechar('c', stout);

    return 0;
}

#endif