#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

enum Constants {
    PERMS = 0666
};

int main(int argc, char** argv) {
    if (argc < 3) {
        _exit(1);
    }

    char* cmd = argv[1];
    char* input_path = argv[2];
    char* output_path = argv[3];

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork error");
        _exit(1);
    } else if (!pid) {
        // ребенок
        int fd = open(input_path, O_RDONLY);
        if (fd < 0) {
            perror("input open error");
            _exit(1);
        }
        // дублируем фд для stdin
        dup2(fd, 0);
        close(fd);

        fd = open(output_path, O_WRONLY | O_CREAT | O_TRUNC, PERMS);
        if (fd < 0) {
            perror("output open error");
            _exit(1);
        }
        // дублируем фд для stdout
        dup2(fd, 1);
        close(fd);

        execlp(cmd, cmd, NULL);
        _exit(1);
    }

    waitpid(pid, NULL, 0);
}