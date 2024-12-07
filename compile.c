#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "string.h"

char *program, *output, *compiler;

void print_usage()
{
    fprintf(stderr, "Usage: %s [filename.c [-o outputname]] [-c gcc|clang]\n", program);
}

int update_output_and_compiler(const char *flag, const char *arg)
{
    if (!arg)
    {
        fprintf(stderr, "An argument must be specified after the flag.\n");
        return 8;
    }

    if (!string_compare(flag, "-o"))
    {
        output = arg;
        return 0;
    }

    if (!string_compare(flag, "-c"))
    {
        if (string_compare(arg, "gcc") && string_compare(arg, "clang"))
        {
            fprintf(stderr, "Invalid compiler. only gcc and clang are supported.\n");
            return 6;
        }

        compiler = arg;
        return 0;
    }

    fprintf(stderr, "Invalid flag. use -o for the name of the output file or -c for the compiler\n");
    return 5;
}

int main(int argc, char **argv)
{
    program = argv[0];

    if (!(argc < 5 || argc == 6))
    {
        print_usage();
        return 1;
    }

    if (argc == 3)
    {
        if (!string_compare(argv[1], "-o"))
        {
            fprintf(stderr, "Cannot specify an output without specifiying a file to compile\n");
            print_usage();
            return 4;
        }

        if (!string_compare(argv[1], "-c"))
        {
            int error = update_output_and_compiler(argv[1], argv[2]);

            if (error)
            {
                print_usage();
                return error;
            }
        }
    }

    if (argc == 1 || compiler)
    {
        if (!compiler)
        {
            char *temp = "gcc";
            compiler = temp;
        }

        int fds[2];

        if (pipe(fds) == -1)
        {
            perror("pipe failed");
            return 10;
        }

        int pid = fork();

        if (pid == -1)
        {
            perror("fork failed");
            return 11;
        }

        if (!pid)
        {
            close(fds[0]);

            if (dup2(fds[1], 1) == -1)
            {
                perror("dup2 failed");
                close(fds[1]);
                return 12;
            }

            if (dup2(fds[1], 2) == -1)
            {
                perror("dup2 failed");
                close(fds[1]);
                return 12;
            }

            execlp("ls", "ls", NULL);

            perror("execlp failed");
            close(fds[1]);
            return 13;
        }

        close(fds[1]);

        if (dup2(fds[0], 0) == -1)
        {
            perror("dup2 failed");
            close(fds[0]);
            return 13;
        }

        char buffer[1024];

        while (1)
        {
            char filename[255] = {};

            scanf("%s", filename);

            int length = string_length(filename);

            if (!length)
            {
                break;
            }

            if (!(filename[length - 2] == '.' && filename[length - 1] == 'c'))
            {
                continue;
            }

            printf("\ncompiling %s...\n\n", filename);

            int pid = fork();

            if (pid == -1)
            {
                perror("fork failed");
                return 11;
            }

            if (!pid)
            {
                if (!compiler)
                {
                    char *temp = "gcc";
                    compiler = temp;
                }

                char *temp = string_copy(filename);
                temp[string_length(temp) - 2] = '\0';
                output = temp;

                execlp(compiler, compiler, "-o", output, filename, NULL);

                perror("Compiler failed");
                return 9;
            }

            int status;
            wait(&status);

            if (WIFEXITED(status))
            {
                if (WEXITSTATUS(status) == 9)
                {
                    perror("Compiler failed");
                    return 9;
                }
            }
        }

        close(fds[0]);
        return 0;
    }

    char *filename = argv[1];
    int flag1, flag2;

    if (argc > 2 && (!string_compare(argv[1], "-o") || !string_compare(argv[1], "-c")))
    {
        filename = argv[3];
        flag1 = 1;
    }

    if (argc > 4 && (!string_compare(argv[3], "-o") || !string_compare(argv[3], "-c")))
    {
        filename = argv[5];
        flag2 = 3;
    }

    int length = string_length(filename);

    if (!(filename[length - 2] == '.' && filename[length - 1] == 'c'))
    {
        fprintf(stderr, "Has to be a c file\n");
        print_usage();
        return 2;
    }

    int fd = open(filename, O_RDONLY);

    if (fd == -1)
    {
        fprintf(stderr, "File doesn't exist.\n");
        print_usage();
        return 3;
    }

    close(fd);

    if (argc > 2)
    {
        if (!flag1)
        {
            flag1 = 2;
        }

        int error = update_output_and_compiler(argv[flag1], argv[flag1 + 1]);

        if (error)
        {
            return error;
        }
    }

    if (argc == 6)
    {
        if (!flag2)
        {
            flag2 = 4;
        }

        if (!string_compare(argv[flag1], argv[flag2]))
        {
            fprintf(stderr, "Invalid usage of the same flag\n");
            print_usage();
            return 7;
        }

        int error = update_output_and_compiler(argv[flag2], argv[flag2 + 1]);

        if (error)
        {
            return error;
        }
    }

    if (!compiler)
    {
        char *temp = "gcc";
        compiler = temp;
    }

    if (!output)
    {
        char *temp = string_copy(filename);
        temp[string_length(temp) - 2] = '\0';
        output = temp;
    }

    execlp(compiler, compiler, "-o", output, filename, NULL);

    perror("Compiler failed");

    return 9;
}