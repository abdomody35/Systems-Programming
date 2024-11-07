#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    printf("Creating a new process...\n");

    int pid = fork();

    if (pid < 0)
    {
        perror("Error: Unable to fork\n");
        return 1;
    }

    if (!pid)
    {
        printf("Child is sleeping for 5 seconds...\n");
        sleep(5);
        printf("Child process exiting...\n");
        printf("Child process Finished executing\n");
    }
    else
    {
        printf("Parent process exiting...\n");
        printf("Parent process Finished executing\n");
        printf("Child is now an orphan\n");
        return 0;
    }

    return 0;
}
