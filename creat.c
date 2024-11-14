#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Usage: %s <filename> [mode]\n", argv[0]);
        return 1;
    }

    int mode = 0644; // default mode : owner can read and write, group can read, and everyone else can read

    if (argc > 2)
    {
        char *endptr;
        mode = strtol(argv[2], &endptr, 8);

        if (*endptr != '\0')
        {
            printf("ERROR 2: Invalid mode. Mode must be an octal numer.\n");
            return 2;
        }

        if (mode < 0 || mode > 0777)
        {
            printf("ERROR 3: Invalid mode. Mode must be between 0 and 777.\n");
            return 3;
        }
    }

    int fileExists = open(argv[1], O_RDONLY);

    if (fileExists != -1)
    {
        printf("The file '%s' already exists. Do you want to overwrite it? (y/n): ", argv[1]);
        char choice;
        scanf(" %c", &choice);

        if (choice != 'y' && choice != 'Y')
        {
            close(fileExists);
            return 0;
        }

        close(fileExists);
    }

    int fd = creat(argv[1], mode);

    if (fd == -1)
    {
        perror("ERROR 4: Cannot create file.\n");
        return 4;
    }

    close(fd);

    return 0;
}