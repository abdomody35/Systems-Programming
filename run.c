#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        printf("Usage: %s <program name> <num of args> <arg1> <arg2> ...\n", argv[0]);
        return 1;
    }

    int numOfArgs = 0;

    if (argc > 2)
    {
        numOfArgs = atoi(argv[2]);
    }

    if (!numOfArgs)
    {
        execl(argv[1], argv[1], NULL);
        return 0;
    }

    if (argc - 3 != numOfArgs)
    {
        printf("Error 2: Enter as many arguments as the count you specified\n");
        return 2;
    }

    char *args[numOfArgs + 2];

    args[0] = (char *)malloc(sizeof(argv[1]));
    strcpy(args[0], argv[1]);

    for (int i = 3; i < argc; i++)
    {
        args[i - 2] = (char *)malloc(sizeof(argv[i]));
        strcpy(args[i - 2], argv[i]);
    }

    args[numOfArgs + 1] = NULL;

    execv(argv[1], args);

    return 0;
}
