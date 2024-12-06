#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t mutex;

int x = 0;

void *perform(void *arg)
{
    int error = pthread_mutex_lock(&mutex);

    if (error)
    {
        fprintf(stderr, "Error locking mutex\n");
        exit(6);
    }

    x++;

    error = pthread_mutex_unlock(&mutex);

    if (error)
    {
        fprintf(stderr, "Error unlocking mutex\n");
        exit(7);
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <number-of-threads>.\n", argv[0]);
        return 1;
    }

    char *endptr;
    int threadCount = strtol(argv[1], &endptr, 10), error;

    if (*endptr != '\0' || threadCount < 1)
    {
        fprintf(stderr, "Invalid number of threads: %s\n", argv[1]);
        return 2;
    }

    pthread_t threads[threadCount];

    error = pthread_mutex_init(&mutex, 0);

    if (error)
    {
        fprintf(stderr, "pthread_mutex_init failed with error %d.\n", error);
        return 3;
    }

    for (int i = 0; i < threadCount; i++)
    {
        int error = pthread_create(&threads[i], NULL, &perform, NULL);

        if (error)
        {
            fprintf(stderr, "Error creating thread %d: %d.\n", i, error);
            return 4;
        }
    }

    for (int i = 0; i < threadCount; i++)
    {
        int error = pthread_join(threads[i], NULL);

        if (error)
        {
            fprintf(stderr, "Error joining thread %d: %d.\n", i, error);
            return 5;
        }
    }

    error = pthread_mutex_destroy(&mutex);

    if (error)
    {
        fprintf(stderr, "pthread_mutex_destroy failed with error %d.\n", error);
    }

    printf("x is %d\n", x);

    return 0;
}