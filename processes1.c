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

	printf("Process created sucessfully\n");

	if (!pid)
	{
		printf("\nI am the child process\n");
		printf("My Process id is: %d\n", getpid());
		printf("The process id of my parent is: %d\n", getppid());
		printf("The id of my user is: %d\n", getuid());
		printf("The value fork returned to me is: %d\n", pid);
	}
	else
	{
		printf("\nI am the parent process\n");
		printf("My Process id is: %d\n", getpid());
                printf("The process id of my parent is: %d\n", getppid());
                printf("The id of my user is: %d\n", getuid());
		printf("The value fork returned to me is: %d\n", pid);
	}

	return 0;
}
