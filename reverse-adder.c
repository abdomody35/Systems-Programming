#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

void *perform(void *data)
{
    printf("Enter the number of elements in the array : ");

    int n;

    scanf("%d", &n);

    int arr1[n], arr2[n];

    printf("Enter the elements of the array : \n");

    for (int i = 0; i < n; i++)
    {
        printf("Element %d : ", i + 1);

        scanf("%d", &arr1[i]);

        arr2[n - i - 1] = arr1[i];
    }

    printf("The array is : ");

    for (int i = 0; i < n; i++)
    {
        printf("%d ", arr1[i] + arr2[i]);
    }

    printf("\n");

    return NULL;
}

int main()
{
    pthread_t thread;

    pthread_create(&thread, NULL, &perform, NULL);

    pthread_join(thread, NULL);

    return 0;
}