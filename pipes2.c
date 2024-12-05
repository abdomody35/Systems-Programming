#include <unistd.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    int fds1[2], fds2[2];
    pipe(fds1);
    pipe(fds2);
    char buffer[1024];
    if (!fork())
    {
        close(fds1[1]);
        read(fds1[0], buffer, 1024);
        printf("%s\n", buffer);
        close(fds1[0]);
        write(fds2[1], "Child Sending to Parent!", 25);
        close(fds2[1]);
        return 0;
    }
    close(fds1[0]);
    write(fds1[1], "Parent Sending to Child!", 25);
    close(fds1[1]);
    read(fds2[0], buffer, 1024);
    printf("%s\n", buffer);
    close(fds2[0]);
    return 0;
}