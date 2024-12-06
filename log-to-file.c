#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

int main()
{
    int fd = open("log.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd == -1)
    {
        perror("open failed");
        return 1;
    }

    if (dup2(fd, 1) == -1)
    {
        perror("dup2 failed");
        close(fd);
        return 2;
    }

    if (dup2(fd, 2) == -1)
    {
        perror("dup2 failed");
        close(fd);
        return 2;
    }

    printf("Creating log file...\n\n");
    printf("Log file created successfully!\n\n");
    printf("Logging data to file...\n\n");
    printf("Logged to file successfully!\n\n");
    printf("Terminating...\n");

    close(fd);
    return 0;
}