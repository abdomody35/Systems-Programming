#include <unistd.h>
#include <stdio.h>
#include "write.h"

int main(int argc, char **argv)
{
    int fds[2], size;

    if (pipe(fds) == -1)
    {
        perror("pipe failed");
        return 1;
    }

    int pid = fork();

    if (pid == -1)
    {
        perror("fork failed");
        return 2;
    }

    if (!pid)
    {
        close(fds[1]);

        char buffer[1024];

        while ((size = read(fds[0], buffer, 1024)) > 0)
        {
        }

        if (size == -1)
        {
            perror("read failed");
            close(fds[0]);
            return 3;
        }

        printf("Child: %s\n", buffer);

        close(fds[0]);
        return 0;
    }

    close(fds[0]);

    if (write_all(fds[1], "Hello, world!", 14) == -1)
    {
        perror("write failed");
        close(fds[1]);
        return 4;
    }

    close(fds[1]);
    return 0;
}
