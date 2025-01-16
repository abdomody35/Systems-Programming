#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>

#define BUFFER_SIZE 4096

int serverSocket, running = 1;

void *perform(void *data)
{
    char buffer[BUFFER_SIZE];

    int size;

    while (running)
    {
        size = recv(serverSocket, buffer, BUFFER_SIZE - 1, MSG_DONTWAIT);

        if (size < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                sleep(1);
                continue;
            }

            perror("recv");
            exit(3);
        }

        if (size)
        {
            buffer[size] = '\0';

            printf("%s", buffer);
        }
    }
}

int main(int argc, char **argv)
{
    serverSocket = socket(AF_INET, SOCK_DGRAM, 0);

    if (serverSocket < 0)
    {
        perror("socket");
        return 1;
    }

    struct sockaddr_in serverAddress = {AF_INET, htons(4001), inet_addr("127.0.0.1")};
    struct sockaddr_in addr = {AF_INET, htons(4000), inet_addr("127.0.0.1")};

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        perror("bind");
        return 2;
    }

    if (connect(serverSocket, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("connect");
        return 3;
    }

    pthread_t thread;

    pthread_create(&thread, NULL, &perform, NULL);

    char buffer[BUFFER_SIZE];

    while (running)
    {
        fgets(buffer, BUFFER_SIZE, stdin);

        if (!strcmp(buffer, "exit\n"))
        {
            running = 0;
            break;
        }

        if (send(serverSocket, buffer, strlen(buffer), 0) == -1)
        {
            perror("send");
            return 4;
        }
    }

    pthread_join(thread, NULL);

    close(serverSocket);

    return 0;
}
