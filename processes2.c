#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

void reverse_string(char *str)
{
    int len = strlen(str);
    char *start = str;
    char *end = str + len - 1;

    while (start < end)
    {
        char temp = *start;
        *start = *end;
        *end = temp;
        start++;
        end--;
    }
}

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
        char input_string[100];
        while (1)
        {
            printf("\nI am the child process\n");
            printf("Enter a string (or 0 to quit): ");
            fgets(input_string, sizeof(input_string), stdin);
            input_string[strcspn(input_string, "\n")] = '\0';

            if (strcmp(input_string, "0") == 0)
            {
                break;
            }

            reverse_string(input_string);
            printf("Reversed string: %s\n", input_string);
        }
        printf("\nChild process exiting...\n");
        return 0;
    }
    else
    {
        printf("I am the parent process\n");
        printf("Parent process waiting for child process...\n");
        int status;
        wait(&status);
        if (WIFEXITED(status))
        {
            if (WEXITSTATUS(status))
            {
                printf("Child process exited with error status: %d\n", WEXITSTATUS(status));
            }
            else
            {
                printf("Child process exited with no errors.\n");
            }
        }
        else
        {
            printf("Child has crashed.\n");
        }
        printf("Parent process finished\n");
    }
}