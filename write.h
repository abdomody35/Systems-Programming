#ifndef WRITE_H
#define WRITE_H

#include <unistd.h>
#include <stdio.h>

int write_all(int fd, const char *buf, int count)
{
    int written;
    while ((written = write(fd, buf, count)) > 0)
    {
        count -= written;
        buf += written;
    }

    if (written == -1)
    {
        perror("write failed");
        return -1;
    }

    return 0;
}

#endif