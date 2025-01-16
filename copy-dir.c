#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include "write.h"

#define BUFFER_SIZE 4096

int copy_directory(char *src, char *dest);
int copy_file(char *src, char *dest);

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <source-dir> <destination-dir>\n", argv[0]);
        return 1;
    }

    int error = copy_directory(argv[1], argv[2]);

    if (error)
    {
        return error;
    }

    return 0;
}

int copy_directory(char *src, char *dest)
{
    DIR *src_dir;

    struct dirent *ent;

    if ((src_dir = opendir(src)) == NULL)
    {
        perror(src);
        return 2;
    }

    struct stat st = {0};

    if (stat(dest, &st) == -1)
    {
        if (mkdir(dest, 0755) == -1)
        {
            perror(dest);
            return 3;
        }
    }

    do
    {
        if ((ent = readdir(src_dir)) != NULL)
        {
            char src_path[1024];
            char dst_path[1024];
            sprintf(src_path, "%s/%s", src, ent->d_name);
            sprintf(dst_path, "%s/%s", dest, ent->d_name);

            if (ent->d_type == DT_REG)
            {
                if (copy_file(src_path, dst_path) == -1)
                {
                    perror(dst_path);
                    return 4;
                }
            }
            else if (ent->d_type == DT_DIR)
            {
                if (strcmp(ent->d_name, ".") && strcmp(ent->d_name, "..") && copy_directory(src_path, dst_path) == -1)
                {
                    perror(dst_path);
                    return 5;
                }
            }
        }
    } while (ent != NULL);

    closedir(src_dir);
}

int copy_file(char *src, char *dest)
{
    int source = open(src, O_RDONLY);

    if (source == -1)
    {
        perror(src);
        return 6;
    }

    int destination = open(dest, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (destination == -1)
    {
        perror(dest);
        return 7;
    }

    int size;
    char buffer[BUFFER_SIZE];

    while ((size = read(source, buffer, BUFFER_SIZE - 1)) > 0)
    {
        buffer[size] = '\0';

        if (write_all(destination, buffer, size) == -1)
        {
            perror("write failed");
            close(destination);
            close(source);
            return 8;
        }
    }

    if (size == -1)
    {
        perror("read failed");
        close(destination);
        close(source);
        return 9;
    }

    close(destination);
    close(source);
    return 0;
}
