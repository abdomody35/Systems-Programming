#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <program> [arg1] [arg2] ...\n", argv[0]);
        return 1;
    }

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

    char *args[argc];

    args[0] = argv[1];

    for (int i = 2; i < argc; i++)
    {
        args[i - 1] = argv[i];
    }

    args[argc - 1] = NULL;

    execv(argv[1], args);

    perror("Cannot run your program");
    close(fd);
    return 3;
}