#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>

#define PLANES 50
#define N 30
#define K 10

static volatile int landing = 0;
static volatile int on_carrier = 0;
static pthread_t planes[PLANES];
static pthread_mutex_t airway = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mux = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t less_then_K = PTHREAD_COND_INITIALIZER, full_carrier = PTHREAD_COND_INITIALIZER;

void* airplane(void* p)
{
    srand(pthread_self());
    while(1)
    {
        usleep(rand() % 100);

        pthread_mutex_lock(&mux);
        landing++;
        pthread_mutex_unlock(&mux);

        pthread_mutex_lock(&airway);
        while(on_carrier == N) 
            pthread_cond_wait(&full_carrier, &airway);
        usleep(2);
        pthread_mutex_unlock(&airway);

        pthread_mutex_lock(&mux);
        landing--;
        on_carrier++;
        pthread_mutex_unlock(&mux);
        printf("Landed\nOn board: %d\nIn air: %d\n", on_carrier, PLANES - on_carrier);

        pthread_cond_signal(&less_then_K);
        usleep(rand() % 100);

        pthread_mutex_lock(&airway);
        while(on_carrier < K && landing != 0) 
            pthread_cond_wait(&less_then_K, &airway);
        usleep(2);
        pthread_mutex_unlock(&airway);

        pthread_mutex_lock(&mux);
        on_carrier--;
        pthread_mutex_unlock(&mux);    
        printf("Started\nOn board: %d\nIn air: %d\n", on_carrier, PLANES - on_carrier);

        pthread_cond_signal(&full_carrier);
    }
}

int main()
{
    for(int i = 0;i < PLANES;i++)
        pthread_create(&planes[i], NULL, airplane, NULL);
    pthread_join(planes[0], NULL);
    return 0;
}