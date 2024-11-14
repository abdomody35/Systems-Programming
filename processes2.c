#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    int pid = fork();

    if (pid == 0)
    {
        printf("pid = %d, Output: ", getpid());
    }
    else
    {
        wait(NULL);
    }

    if (pid && pid % 2)
    {
        printf("\n");
        return 0;
    }

    printf("x");

    if (pid)
    {
        printf("\n");
    }

    return 0;
}
