#include <unistd.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    int fds[2];
    pipe(fds);
    if (!fork())
    {
        close(fds[1]);
        char buffer[1024];
        read(fds[0], buffer, 1024);
        printf("Child: %s\n", buffer);
        close(fds[0]);
        return 0;
    }
    close(fds[0]);
    write(fds[1], "Hello, world!", 14);
    close(fds[1]);
    return 0;
}