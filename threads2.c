#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

const int numCount = 1000000;

long sum = 0;

pthread_mutex_t mutex;

void *perform(void *arg)
{
    int count = *((int *)arg);
    for (int i = 0; i < count; i++)
    {
        int num = rand();
        pthread_mutex_lock(&mutex);
        sum += num;
        pthread_mutex_unlock(&mutex);
    }
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

    int performCount = numCount / threadCount;

    for (int i = 0; i < threadCount; i++)
    {
        pthread_create(&threads[i], NULL, &perform, (void *)&performCount);
    }

    for (int i = 0; i < threadCount; i++)
    {
        pthread_join(threads[i], NULL);
    }

    int reminder = numCount - threadCount * performCount;

    perform((void *)&reminder);

    printf("Sum : %ld\n", sum);
    printf("Average : %f\n", (float)sum / numCount);
    return 0;
}
