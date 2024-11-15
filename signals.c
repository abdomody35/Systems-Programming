#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void SIGINT_handler(int signum)
{
    printf("\nCaught SIGINT signal\n");
    exit(0);
}

void SIGTERM_handler(int signum)
{
    printf("\nCaught SIGTERM signal\n");
    exit(0);
}

void SIGKILL_handler(int signum)
{
    printf("\nCaught SIGKILL signal\n");
    exit(0);
}

void SIGFPE_handler(int signum)
{
    printf("\nCaught SIGFPE signal\n");
    exit(0);
}

void SIGSEGV_handler(int signum)
{
    printf("\nCaught SIGSEGV signal\n");
    exit(0);
}

void SIGCHLD_handler(int signum)
{
    int status;
    wait(&status);
    printf("Child process terminated with status %d\n", WEXITSTATUS(status));
}

int main()
{
    signal(SIGINT, SIGINT_handler);
    signal(SIGTERM, SIGTERM_handler);
    signal(SIGKILL, SIGKILL_handler);
    signal(SIGFPE, SIGFPE_handler);
    signal(SIGSEGV, SIGSEGV_handler);
    signal(SIGCHLD, SIGCHLD_handler);

    printf("Which signal would you like to send?\n");
    printf("1. SIGINT\n");
    printf("2. SIGTERM\n");
    printf("3. SIGKILL\n");
    printf("4. SIGFPE\n");
    printf("5. SIGSEGV\n");
    printf("6. SIGCHLD\n");
    printf("7. Kill the process using kill system call.\n");
    printf("Enter your choice :");

    int choice;

    scanf(" %d", &choice);

    switch (choice)
    {
    case 1:
        raise(SIGINT);
        break;
    case 2:
        raise(SIGTERM);
        break;
    case 3:
        raise(SIGKILL);
        break;
    case 4:
        raise(SIGFPE);
        break;
    case 5:
        raise(SIGSEGV);
        break;
    case 6:
        raise(SIGCHLD);
        break;
    case 7:
        kill(getpid(), SIGKILL);
        break;
    default:
        printf("Invalid choice. Exiting...\n");
    }

    return 0;
}