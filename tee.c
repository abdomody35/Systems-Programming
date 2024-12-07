#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include "write.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <program> [arg1] [arg2] ...\n", argv[0]);
        return 1;
    }

    int fds[2], size;

    if (pipe(fds) == -1)
    {
        perror("pipe failed");
        return 2;
    }

    int pid = fork();

    if (pid == -1)
    {
        perror("fork failed");
        return 3;
    }

    if (!pid)
    {
        close(fds[1]);

        int fd = open("log.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

        if (fd == -1)
        {
            perror("open failed");
            return 4;
        }

        char *buffer = (char *)malloc(1024);

        while ((size = read(fds[0], buffer, 1024)) > 0)
        {
            if (write_all(1, buffer, size) == -1)
            {
                perror("write failed");
                close(fd);
                return 5;
            }

            if (write_all(fd, buffer, size) == -1)
            {
                perror("write failed");
                close(fd);
                return 5;
            }
        }

        if (size == -1)
        {
            perror("read failed");
            close(fds[0]);
            return 6;
        }

        free(buffer);
        close(fds[0]);
        close(fd);
        return 0;
    }

    close(fds[0]);

    if (dup2(fds[1], 1) == -1)
    {
        perror("dup2 failed");
        close(fds[1]);
        return 7;
    }

    if (dup2(fds[1], 2) == -1)
    {
        perror("dup2 failed");
        close(fds[1]);
        return 7;
    }

    char *args[argc];

    args[0] = argv[1];

    for (int i = 2; i < argc; i++)
    {
        args[i - 1] = argv[i];
    }

    args[argc - 1] = NULL;

    execv(argv[1], args);

    perror("Cannot run your program");
    close(fds[1]);
    return 8;
}