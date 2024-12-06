#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "write.h"

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
            printf("ERROR 2: Invalid flag. Only -f is allowed.\n");
            return 2;
        }
    }

    int source = open(sourceFile, O_RDONLY);

    if (source == -1)
    {
        printf("ERROR 3: Could not open source file\n");
        return 3;
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
        printf("ERROR 4: Could not open or create destination file\n");
        close(source);
        return 4;
    }

    int size;
    char *buffer = (char *)malloc(BUFFER_SIZE);

    while ((size = read(source, buffer, BUFFER_SIZE)) > 0)
    {
        if (write_all(destination, buffer, size) == -1)
        {
            perror("write failed");
            close(destination);
            close(source);
            return 4;
        }
    }

    if (size == -1)
    {
        perror("read failed");
        free(buffer);
        close(destination);
        close(source);
        return 3;
    }

    free(buffer);
    close(destination);
    close(source);
    return 0;
}