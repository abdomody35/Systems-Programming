#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t mutex;

int x = 0;

void *perform(void *arg)
{
    pthread_mutex_lock(&mutex);
    x++;
    pthread_mutex_unlock(&mutex);
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: %s <number-of-threads>.\n", argv[0]);
        return 1;
    }

    int threadCount = atoi(argv[1]);

    pthread_t threads[threadCount];

    pthread_mutex_init(&mutex, 0);

    for (int i = 0; i < threadCount; i++)
    {
        pthread_create(&threads[i], NULL, &perform, NULL);
    }

    for (int i = 0; i < threadCount; i++)
    {
        pthread_join(threads[i], NULL);
    }

    printf("x is %d\n", x);

    return 0;
}