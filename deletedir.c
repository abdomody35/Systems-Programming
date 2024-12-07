#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv)
{

    if (argc < 2 || argc > 3)
    {
        printf("Usage: %s [-f] <directoryname>\n", argv[0]);
        return 1;
    }

    char *directoryname = argv[1];
    char *flag = NULL;

    if (argc == 3)
    {
        if (strcmp(argv[1], "-f") == 0)
        {
            flag = argv[1];
            directoryname = argv[2];
        }
        else if (strcmp(argv[2], "-f") == 0)
        {
            flag = argv[2];
        }
        else
        {
            printf("ERROR 2: Invalid flag. Only -f if allowed\n");
            return 2;
        }
    }

    if (!flag)
    {
        printf("Are you sure you want to delete '%s'? (y/n): ", directoryname);
        char choice;
        scanf(" %c", &choice);

        if (choice != 'y' && choice != 'Y')
        {
            return 0;
        }
    }

    int result = rmdir(directoryname);

    if (result < 0)
    {
        perror("ERROR 2: Cannot delete directory.\n");
        return 2;
    }

    return 0;
}