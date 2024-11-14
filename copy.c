#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 4096

int main(int argc, char **argv)
{

    if (argc > 4 || argc < 3)
    {
        printf("Usage: %s [-f] <source> <destination>\n", argv[0]);
        return 1;
    }

    char *sourceFile = argv[1];
    char *destinationFile = argv[2];

    char *flag = NULL;

    if (argc == 4)
    {
        if (strcmp(argv[1], "-f") == 0)
        {
            flag = argv[1];
            sourceFile = argv[2];
            destinationFile = argv[3];
        }
        else if (strcmp(argv[2], "-f") == 0)
        {
            flag = argv[2];
            sourceFile = argv[1];
            destinationFile = argv[3];
        }
        else if (strcmp(argv[3], "-f") == 0)
        {
            flag = argv[3];
        }
        else
        {
            printf("ERROR 3: Invalid flag. Only -f is allowed.\n");
            return 3;
        }
    }

    int source = open(sourceFile, O_RDONLY);

    if (source == -1)
    {
        printf("ERROR 1: Could not open source file\n");
        return 1;
    }

    if (!flag)
    {
        int fileExists = open(destinationFile, O_RDONLY);

        if (fileExists != -1)
        {
            printf("The file '%s' already exists. Do you want to overwrite it? (y/n): ", destinationFile);
            char choice;
            scanf(" %c", &choice);

            if (choice != 'y' && choice != 'Y')
            {
                close(fileExists);
                return 0;
            }
        }

        close(fileExists);
    }

    int destination = open(destinationFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (destination == -1)
    {
        printf("ERROR 2: Could not open or create destination file\n");
        close(source);
        return 1;
    }

    int size;
    char *buffer = (char *)malloc(BUFFER_SIZE);

    while ((size = read(source, buffer, BUFFER_SIZE)) > 0)
    {
        write(destination, buffer, size);
    }

    free(buffer);

    return 0;
}