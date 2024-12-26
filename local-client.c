#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>

#define BUFFER_SIZE 4096

int serverNeedsInput(char *buffer, int size)
{
    return buffer[size - 1] == '|' && buffer[size - 2] == '-' && buffer[size - 3] == '/';
}

int main(int argc, char **argv)
{
    int serverSocket = socket(AF_UNIX, SOCK_STREAM, 0);

    if (serverSocket < 0)
    {
        perror("socket");
        return 1;
    }

    struct sockaddr_un serverAddress = {AF_UNIX, "server.temp"};

    if (connect(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        perror("connect");
        return 2;
    }

    char buffer[BUFFER_SIZE];

    int size;

    while (serverSocket)
    {
        size = recv(serverSocket, buffer, BUFFER_SIZE - 1, 0);

        if (size < 0)
        {
            perror("recv");
            return 3;
        }

        if (!size)
        {
            break;
        }

        if (serverNeedsInput(buffer, size))
        {
            buffer[size - 3] = '\0';

            printf("%s", buffer);

            do
            {
                if (!fgets(buffer, BUFFER_SIZE - 1, stdin))
                {
                    perror("fgets");
                    return 4;
                }

                size = strlen(buffer);

            } while (isspace(buffer[0]));

            int sent = send(serverSocket, buffer, size - 1, 0);

            if (sent < 0)
            {
                perror("send");
                return 5;
            }

            continue;
        }

        buffer[size] = '\0';

        printf("%s", buffer);
    }

    close(serverSocket);

    return 0;
}
