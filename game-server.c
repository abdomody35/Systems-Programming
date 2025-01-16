#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>

#define BUFFER_SIZE 4096

int serverSocket = 0;
int clientCount = 0;

void handleSIGINT();

void sendMenu(int socket);

void invalidChoice(int socket);

char *input(int socket, char *message);

void mainActivity(int socket);

char upper(char c);

int main(int argc, char **argv)
{
    signal(SIGINT, &handleSIGINT);

    serverSocket = socket(AF_UNIX, SOCK_STREAM, 0);

    if (serverSocket < 0)
    {
        perror("socket");
        return 1;
    }

    struct sockaddr_un serverAddress = {AF_UNIX, "server.temp"};

    unlink(serverAddress.sun_path);

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        perror("bind");
        return 2;
    }

    if (listen(serverSocket, 5) < 0)
    {
        perror("listen");
        return 3;
    }

    struct sockaddr_un clientAddress;

    int clientAddressSize, communicationSocket;

    while ((communicationSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressSize)) > 0)
    {
        clientCount++;

        int pid = fork();

        if (pid == -1)
        {
            perror("fork");
            return 8;
        }

        if (!pid)
        {
            int sent = send(communicationSocket, "Welcome to Guess The Number Server!\n", 36, 0);

            if (sent < 0)
            {
                perror("send");
                return 4;
            }

        menu:
            sendMenu(communicationSocket);

            char buffer[BUFFER_SIZE];

            int size = recv(communicationSocket, buffer, BUFFER_SIZE - 1, 0);

            if (size < 0)
            {
                perror("recv");
                return 5;
            }

            buffer[size] = '\0';

            switch (atoi(buffer))
            {
            case 1:
                mainActivity(communicationSocket);
                goto menu;
                break;
            case 2:
                close(communicationSocket);
                break;
            default:
                invalidChoice(communicationSocket);
                goto menu;
                break;
            }
        }

        close(communicationSocket);
    }

    close(serverSocket);

    unlink("server.temp");

    printf("\n");

    return 0;
}

void handleSIGINT()
{
    while (clientCount)
    {
        wait(NULL);
        clientCount--;
    }

    close(serverSocket);
}

char *input(int socket, char *message)
{
    if (send(socket, message, strlen(message), 0) == -1)
    {
        perror("send");
        exit(4);
    }

    if (send(socket, "/-|", 3, 0) == -1)
    {
        perror("send");
        exit(4);
    }

    char buffer[BUFFER_SIZE];
    int size;

    if ((size = recv(socket, buffer, BUFFER_SIZE - 1, 0)) < 0)
    {
        perror("recv");
        exit(4);
    }

    buffer[size] = '\0';

    return strdup(buffer);
}

void invalidChoice(int socket)
{
    if (send(socket, "\nInvalid choice!\n", 17, 0) == -1)
    {
        perror("send");
        exit(4);
    }
}

void sendMenu(int socket)
{
    if (send(socket, "\n1. Play\n2. exit\n\nEnter Your Choice : /-|", 41, 0) == -1)
    {
        perror("send");
        exit(4);
    }
}

void mainActivity(int socket)
{
    while (1)
    {
        srand(time(NULL));

        int num = rand() % 21 + rand() % 21;

        char *buffer = input(socket, "\nEnter Your Guess (0-40): "), choice;

        int correct = (atoi(buffer) == num);

        switch (correct)
        {
        case 1:
            if (send(socket, "\nCorrect!\n", 10, 0) == -1)
            {
                perror("send");
                exit(4);
            }
            break;
        case 0:
            char message[40];

            sprintf(message, "\nIncorrect! The number was %d\n", num);

            if (send(socket, message, strlen(message), 0) == -1)
            {
                perror("send");
                exit(4);
            }
            break;
        default:
            invalidChoice(socket);
            break;
        }

        free(buffer);

        while (1)
        {
            buffer = input(socket, "\nWould you like to continue (Y/N) ? ");

            choice = upper(*buffer);

            if (choice != 'Y' && choice != 'N')
            {
                invalidChoice(socket);
            }
            else
            {
                break;
            }
        }

        if (choice == 'N')
        {
            break;
        }

        free(buffer);
    }
}

char upper(char c)
{
    return c >= 'A' && c <= 'Z' ? c : c - 32;
}