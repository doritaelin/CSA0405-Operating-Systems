// plate.c   compile: gcc -O2 -pthread plate.c -o plate
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define CAMERAS 64
#define WORK    60000000L        // work per frame

int next_camera = 0;             // shared: next camera to process
int plates_found = 0;            // shared: total plates detected
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// simulates plate recognition on one frame
int process_frame(int cam)
{
    volatile long x = cam;
    for (long i = 0; i < WORK; i++)
        x = (x * 31 + i) % 1000003;
    return (x % 3 == 0);         // 1 if a plate was "found"
}

void *worker(void *arg)
{
    while (1) {
        pthread_mutex_lock(&lock);
        int cam = next_camera++;           // get next camera safely
        pthread_mutex_unlock(&lock);
        if (cam >= CAMERAS) break;

        int found = process_frame(cam);

        pthread_mutex_lock(&lock);         // protected counter update
        plates_found += found;
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

double now(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
}

int main(int argc, char *argv[])
{
    int nthreads = (argc > 1) ? atoi(argv[1]) : 1;
    pthread_t tid[CAMERAS];

    double start = now();
    for (int i = 0; i < nthreads; i++)
        pthread_create(&tid[i], NULL, worker, NULL);
    for (int i = 0; i < nthreads; i++)
        pthread_join(tid[i], NULL);
    double elapsed = now() - start;

    printf("Threads = %2d  Time = %.2f s  Plates found = %d\n",
           nthreads, elapsed, plates_found);
    return 0;
}
