#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include "write.h"

int main()
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

        int fd = open("log.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

        if (fd == -1)
        {
            perror("open failed");
            return 3;
        }

        char *buffer = (char *)malloc(1024);

        while ((size = read(fds[0], buffer, 1024)) > 0)
        {
            if (write_all(1, buffer, size) == -1)
            {
                perror("write failed");
                close(fd);
                return 4;
            }

            if (write_all(fd, buffer, size) == -1)
            {
                perror("write failed");
                close(fd);
                return 4;
            }
        }

        if (size == -1)
        {
            perror("read failed");
            close(fds[0]);
            return 5;
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
        return 6;
    }

    if (dup2(fds[1], 2) == -1)
    {
        perror("dup2 failed");
        close(fds[1]);
        return 6;
    }

    printf("Creating log file...\n\n");
    printf("Log file created successfully!\n\n");
    printf("Logging data to file...\n\n");
    printf("Logged to file successfully!\n\n");
    printf("Terminating...\n");

    close(fds[1]);

    return 0;
}