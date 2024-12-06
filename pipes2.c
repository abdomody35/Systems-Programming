#include <unistd.h>
#include <stdio.h>
#include "write.h"

int main(int argc, char **argv)
{
    int fds1[2], fds2[2], size;

    if (pipe(fds1) == -1)
    {
        perror("pipe failed");
        return 1;
    }

    if (pipe(fds2) == -1)
    {
        perror("pipe failed");
        return 1;
    }

    char buffer[1024];

    int pid = fork();

    if (pid == -1)
    {
        perror("fork failed");
        return 2;
    }

    if (!pid)
    {
        close(fds1[1]);
        close(fds2[0]);

        while ((size = read(fds1[0], buffer, 1024)) > 0)
        {
        }

        if (size == -1)
        {
            perror("read failed");
            close(fds1[0]);
            close(fds2[1]);
            return 3;
        }

        printf("%s\n", buffer);

        close(fds1[0]);

        if (write_all(fds2[1], "Child Sending to Parent!", 25) == -1)
        {
            perror("write failed");
            close(fds1[0]);
            return 4;
        }

        close(fds2[1]);
        return 0;
    }

    close(fds1[0]);
    close(fds2[1]);

    if (write_all(fds1[1], "Parent Sending to Child!", 25) == -1)
    {
        perror("write failed");
        close(fds1[0]);
        return 4;
    }

    close(fds1[1]);

    while ((size = read(fds2[0], buffer, 1024)) > 0)
    {
    }

    if (size == -1)
    {
        perror("read failed");
        close(fds1[0]);
        close(fds2[1]);
        return 3;
    }

    printf("%s\n", buffer);

    close(fds2[0]);
    return 0;
}