#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void display_menu()
{
    printf("\nEnvironment Variables Management Menu\n");
    printf("1. Display all environment variables\n");
    printf("2. Set the value of a an environment variable\n");
    printf("3. Get the value of an environment variable\n");
    printf("4. Delete an environment variable\n");
    printf("5. Exit\n");
    printf("Enter your choice: ");
}

void display_env_vars()
{
    extern char **environ;
    char **env = environ;
    while (*env)
    {
        printf("%s\n", *env);
        env++;
    }
}

void set_env_var(char *name, char *value)
{
    char buffer[1024];
    sprintf(buffer, "%s=%s", name, value);
    putenv(buffer);
}

void get_env_var(char *name)
{
    char *value = getenv(name);
    if (value)
    {
        printf("\nValue of %s: %s\n", name, value);
    }
    else
    {
        printf("\nEnvironment variable %s not found\n", name);
    }
}

int main()
{
    int choice;
    char name[128], value[512];
    while (1)
    {
        display_menu();
        scanf(" %d", &choice);
        switch (choice)
        {
        case 1:
            display_env_vars();
            break;
        case 2:
            printf("Enter the name of the environment variable: ");
            scanf(" %s", name);
            printf("Enter the value of the environment variable: ");
            scanf(" %s", value);
            set_env_var(name, value);
            printf("\nEnvironment Variable %s set successfully with value %s\n", name, value);
            break;
        case 3:
            printf("Enter the name of the environment variable: ");
            scanf(" %s", name);
            get_env_var(name);
            break;
        case 4:
            printf("Enter the name of the environment variable to delete: ");
            scanf(" %s", name);
            unsetenv(name);
            printf("\nEnvironment Variable %s was deleted successfully\n", name);
            break;
        case 5:
            printf("Exiting...\n");
            exit(0);
        default:
            printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}