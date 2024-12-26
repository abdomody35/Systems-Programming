#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

#define BUFFER_SIZE 4096

int serverSocket = 0;
int clientCount = 0;
int loggedIn = 0;

void handleSIGINT();

void sendMenu(int socket);

void invalidChoice(int socket);

char *input(int socket, char *message);

unsigned long hash(char *str);

void loginUser(int socket);

void registerUser(int socket);

void logoutUser(int socket);

void mainActivity(int socket);

int main(int argc, char **argv)
{
    signal(SIGINT, &handleSIGINT);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket < 0)
    {
        perror("socket");
        return 1;
    }

    struct sockaddr_in serverAddress = {AF_INET, htons(3000), INADDR_ANY};

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

    struct sockaddr_in clientAddress;

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
            loggedIn = 0;

            int sent = send(communicationSocket, "Welcome to The Calculator Server!\n", 34, 0);

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

            if (!loggedIn)
            {
                switch (atoi(buffer))
                {
                case 1:
                    loginUser(communicationSocket);
                    goto menu;
                    break;
                case 2:
                    registerUser(communicationSocket);
                    goto menu;
                    break;
                case 3:
                    close(communicationSocket);
                    break;
                default:
                    invalidChoice(communicationSocket);
                    goto menu;
                    break;
                }
            }
            else
            {
                switch (atoi(buffer))
                {
                case 1:
                    mainActivity(communicationSocket);
                    goto menu;
                    break;
                case 2:
                    logoutUser(communicationSocket);
                    goto menu;
                    break;
                case 3:
                    close(communicationSocket);
                    break;
                default:
                    invalidChoice(communicationSocket);
                    goto menu;
                    break;
                }
            }
        }

        close(communicationSocket);
    }

    close(serverSocket);

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

unsigned long hash(char *str)
{
    unsigned long hash = 5381;

    int c;

    while ((c = *str++))
    {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

void invalidChoice(int socket)
{
    if (send(socket, "\nInvalid choice!\n", 17, 0) == -1)
    {
        perror("send");
        exit(4);
    }
}

void logoutUser(int socket)
{
    loggedIn = 0;

    if (send(socket, "\nLogged Out\n", 12, 0) == -1)
    {
        perror("send");
        exit(4);
    }
}

void loginUser(int socket)
{
    char *username = input(socket, "\nEnter Your username : ");
    char *password = input(socket, "Enter Your password : ");

    unsigned long hashedPassword = hash(password);

    free(password);

    int fd = open("userdata.csv", O_RDONLY | O_CREAT | O_APPEND, 0644);

    if (fd == -1)
    {
        perror("open");
        exit(6);
    }

    char userInfo[strlen(username) + sizeof(hashedPassword) + 3];

    sprintf(userInfo, "%s,%ld\n", username, hashedPassword);

    char buffer[BUFFER_SIZE];

    int size;

    while ((size = read(fd, buffer, BUFFER_SIZE - 1)))
    {
        if (strstr(buffer, userInfo))
        {
            loggedIn = 1;

            if (send(socket, "\nLogged In Successfully\n", 24, 0) == -1)
            {
                perror("send");
                exit(4);
            }

            break;
        }
    }

    if (!loggedIn)
    {
        if (send(socket, "\nInvalid username or password\n", 30, 0) == -1)
        {
            perror("send");
            exit(4);
        }
    }

    close(fd);
    free(username);
}

void registerUser(int socket)
{
    char *username = input(socket, "\nEnter Your username : ");
    char *password = input(socket, "Enter Your password : ");
    char *confirm = input(socket, "Confirm Your password : ");

    if (strcmp(password, confirm))
    {
        send(socket, "\nPasswords do not match!\n", 25, 0);
        free(username);
        free(password);
        free(confirm);
        return;
    }

    if (strlen(password) < 8)
    {
        send(socket, "\nPassword must be at least 8 characters long!\n", 46, 0);
        free(username);
        free(password);
        free(confirm);
        return;
    }

    unsigned long hashedPassword = hash(password);

    free(password);

    int fd = open("userdata.csv", O_RDWR | O_CREAT | O_APPEND, 0644);

    if (fd == -1)
    {
        perror("open");
        exit(6);
    }

    struct stat fileStat;

    if (fstat(fd, &fileStat) == 0 && fileStat.st_size == 0)
    {
        if (write(fd, "\n", 1) == -1)
        {
            perror("write");
            exit(7);
        }
    }
    else
    {
        char usernameInDatabase[strlen(username) + 2];

        sprintf(usernameInDatabase, "\n%s,", username);

        char buffer[BUFFER_SIZE];

        int size, userExists = 0;

        while ((size = read(fd, buffer, BUFFER_SIZE - 1)))
        {
            if (strstr(buffer, usernameInDatabase))
            {
                userExists = 1;
                break;
            }
        }

        if (userExists)
        {
            if (send(socket, "\nUser already exists!\n", 22, 0) < 0)
            {
                perror("send");
                exit(4);
            }

            return;
        }
    }

    char userInfo[strlen(username) + sizeof(hashedPassword) + 3];

    sprintf(userInfo, "%s,%ld\n", username, hashedPassword);

    if (write(fd, userInfo, strlen(userInfo)) == -1)
    {
        perror("write");
        exit(7);
    }

    if (send(socket, "\nUser created successfully\n", 27, 0) < 0)
    {
        perror("send");
        exit(4);
    }

    close(fd);
    free(username);
    free(confirm);
}

void sendMenu(int socket)
{
    if (loggedIn)
    {
        if (send(socket, "\n1. Perform an Operation\n2. Logout\n3. exit\n\nEnter Your Choice : /-|", 67, 0) == -1)
        {
            perror("send");
            exit(4);
        }

        return;
    }

    if (send(socket, "\n1. Login\n2. Register\n3. exit\n\nEnter Your Choice : /-|", 54, 0) == -1)
    {
        perror("send");
        exit(4);
    }
}

void sumAndAverageOfAllInputs(int socket, int printSum, int printAvg)
{
    long long sum = 0, n = 0;

    while (1)
    {
        char *buffer = input(socket, "Enter a Number : ");

        int num = atoi(buffer);

        free(buffer);

        sum += num;

        if (!num)
        {
            break;
        }

        n++;
    }

    if (printSum)
    {
        char message[22 + 8];

        snprintf(message, 22 + 8, "\nSum of all inputs : %lld\n", sum);

        if (send(socket, message, strlen(message), 0) == -1)
        {
            perror("send");
            exit(4);
        }
    }

    if (printAvg)
    {
        long double average = sum / (float)n;

        char message[26 + 16];

        snprintf(message, 26 + 16, "\nAverage of all inputs : %Lf\n", average);

        if (send(socket, message, strlen(message), 0) == -1)
        {
            perror("send");
            exit(4);
        }
    }
}

void sumOfAllInputs(int socket)
{
    sumAndAverageOfAllInputs(socket, 1, 0);
}

void averageOfAllInputs(int socket)
{
    sumAndAverageOfAllInputs(socket, 0, 1);
}

void mainActivity(int socket)
{
    int choice = 0;

    while (choice != 4)
    {
        char *buffer = input(socket, "\n1. Sum of all inputs\n2. Average of all inputs\n3. Both\n4. Go back\n\nEnter Your Choice : ");

        choice = atoi(buffer);

        switch (choice)
        {
        case 1:
            sumOfAllInputs(socket);
            break;
        case 2:
            averageOfAllInputs(socket);
            break;
        case 3:
            sumAndAverageOfAllInputs(socket, 1, 1);
            break;
        case 4:
            break;
        default:
            invalidChoice(socket);
            break;
        }

        free(buffer);
    }
}
