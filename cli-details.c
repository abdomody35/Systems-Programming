#include <stdio.h>

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		printf("Usage: %s <arg1> <arg2> ...\n", argv[0]);
		return 1;
	}

	printf("The program is named %s on your computer.\n\n", argv[0]);
	printf("You entered %d arguments.\n\n", argc - 1);
	printf("The arguments are:\n");

	for (int i = 1; i < argc; i++)
	{
		printf("%s\n", argv[i]);
	}

	return 0;
}
