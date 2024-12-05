#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

int main()
{
    int fds[2];
    pipe(fds);

    int pid = fork();

    if (!pid)
    {
        close(fds[1]);

        int fd = open("log.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

        if (fd == -1)
        {
            perror("open failed");
            return 1;
        }

        int size;
        char *buffer = (char *)malloc(1024);

        while ((size = read(fds[0], buffer, 1024)) > 0)
        {
            write(1, buffer, size);
            write(fd, buffer, size);
        }

        free(buffer);

        close(fds[0]);

        close(fd);

        return 0;
    }

    close(fds[0]);

    dup2(fds[1], 1);
    dup2(fds[1], 2);

    printf("Creating log file...\n\n");

    printf("Log file created successfully!\n\n");

    printf("Logging data to file...\n\n");

    printf("Logged to file successfully!\n\n");

    printf("Terminating...\n");

    close(fds[1]);

    return 0;
}