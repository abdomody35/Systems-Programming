#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

int main(int argc, char **argv)
{
    printf("Welcome to Number Doubler!\n");

    while (1)
    {
        int fds1[2], fds2[2];
        pipe(fds1);
        pipe(fds2);
        char buffer[1024];

        int pid = fork();

        if (!pid)
        {
            close(fds1[1]);

            read(fds1[0], buffer, 1024);

            int num = atoi(buffer) * 2;

            close(fds1[0]);

            char str[12];
            sprintf(str, "%d", num);

            write(fds2[1], (void *)str, 12);

            close(fds2[1]);

            return 0;
        }

        close(fds1[0]);

        printf("Enter a number or 0 to stop: ");

        int input;
        int scaned = scanf("%d", &input);

        if (scaned < 1 || input == 0)
        {
            break;
        }

        char str[12];
        sprintf(str, "%d", input);

        write(fds1[1], (void *)str, 12);

        close(fds1[1]);

        read(fds2[0], buffer, 1024);

        printf("%d doubled is %s\n", input, buffer);

        close(fds2[0]);
    }
    
    return 0;
}