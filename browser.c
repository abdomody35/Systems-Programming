#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include "write.h"

#define BUFFER_SIZE 4096

void enable_non_canonical_mode();

void disable_non_canonical_mode();

void print_option_bar();

void update_choice(char *choice);

int is_valid_choice(char choice);

void clear_terminal();

void connect_to_server();

char *collect_data(int serverSocket);

void print_parsed(const char *str);

int main(int argc, char **argv)
{
    char choice;

    printf("Welcome to the terminal browser!\n");

    while (1)
    {
        print_option_bar();

        update_choice(&choice);

        if (choice == 'q')
        {
            break;
        }

        clear_terminal();

        connect_to_server();
    }

    printf("\n");

    return 0;
}

void enable_non_canonical_mode()
{
    struct termios terminalSettings;

    tcgetattr(STDIN_FILENO, &terminalSettings);

    terminalSettings.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(STDIN_FILENO, TCSANOW, &terminalSettings);
}

void disable_non_canonical_mode()
{
    struct termios terminalSettings;

    tcgetattr(STDIN_FILENO, &terminalSettings);

    terminalSettings.c_lflag |= (ICANON | ECHO);

    tcsetattr(STDIN_FILENO, TCSANOW, &terminalSettings);
}

void print_option_bar()
{
    printf("\n\noptinos: q. quit - n. new connection");
}

int is_valid_choice(char choice)
{
    return choice == 'q' || choice == 'n';
}

void update_choice(char *choice)
{
    enable_non_canonical_mode();

    do
    {
        scanf(" %c", choice);
    } while (!is_valid_choice(*choice));

    disable_non_canonical_mode();
}

void clear_terminal()
{
    system("clear");
}

void connect_to_server()
{
    char url[1025];

    printf("Enter the url : ");
    scanf(" %1024s", url);

    int serverSocket = -1;

    struct addrinfo hints = {}, *serverAddress, *rp;

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    char *host = strtok(url, "/");
    char *route = strtok(NULL, " ");

    int error = getaddrinfo(host, "http", &hints, &serverAddress);

    if (error)
    {
        fprintf(stderr, "\nFailed to reach the specified url (%s)", gai_strerror(error));
        return;
    }

    for (rp = serverAddress; rp != NULL; rp = rp->ai_next)
    {
        serverSocket = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

        if (serverSocket == -1)
        {
            continue;
        }

        if (connect(serverSocket, rp->ai_addr, rp->ai_addrlen) == 0)
        {
            break;
        }

        close(serverSocket);
    }

    if (rp == NULL)
    {
        fprintf(stderr, "Could not connect\n");
        freeaddrinfo(serverAddress);
        return;
    }

    freeaddrinfo(serverAddress);

    char buffer[BUFFER_SIZE];

    sprintf(buffer, "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", route ? route : "/", host);

    if (send(serverSocket, buffer, strlen(buffer), 0) < 0)
    {
        perror("send");
        exit(2);
    }

    char *data = collect_data(serverSocket);

    print_parsed(data);

    free(data);

    close(serverSocket);
}

char *collect_data(int serverSocket)
{
    char buffer[BUFFER_SIZE], *data = malloc(1);

    int size, length = 0;

    if (!data)
    {
        perror("malloc");
        exit(4);
    }

    data[0] = '\0';

    while ((size = recv(serverSocket, buffer, BUFFER_SIZE - 1, 0)) > 0)
    {
        if (size < 0)
        {
            perror("recv");
            free(data);
            exit(3);
        }

        buffer[size] = '\0';

        length += size;

        char *new_data = realloc(data, length + 1);

        if (!new_data)
        {
            perror("realloc");
            free(data);
            exit(4);
        }

        data = new_data;

        strcat(data, buffer);
    }

    return data;
}

// simple html parser that assumes html is not malformed
void print_parsed(const char *str)
{
    putchar('\n');

    char *html = strstr(str, "<html");

    if (!html)
    {
        html = strstr(str, "<HTML");

        if (!html)
        {
            printf("\nNot HTML Content!\n\n%s", str);
            return;
        }
    }

    while (*html)
    {
        if (*html == '<')
        {
            if (!strncmp(html, "<style", 6) || !strncmp(html, "<script", 7) || !strncmp(html, "<STYLE", 6) || !strncmp(html, "<SCRIPT", 7))
            {
                while (*html != '>')
                {
                    html++;
                }

                while (*html != '<')
                {
                    html++;
                }
            }
            else if (!strncmp(html, "<a", 2) || !strncmp(html, "<A", 2))
            {
                char *href = strstr(html, "href");

                if (!href)
                {
                    href = strstr(html, "HREF");
                }

                if (href)
                {
                    while (*href != '"')
                    {
                        href++;
                    }

                    href++;

                    putchar('(');

                    while (*href != '"')
                    {
                        putchar(*href);
                        href++;
                    }

                    putchar(')');
                }

                while (*html != '>')
                {
                    html++;
                }

                html++;

                while (*html != '<')
                {
                    putchar(*html);
                    html++;
                }
            }

            while (*html != '>')
            {
                html++;
            }
        }
        else
        {
            putchar(*html);
        }

        html++;
    }
}
