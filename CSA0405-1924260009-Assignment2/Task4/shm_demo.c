// shm_demo.c   compile: gcc shm_demo.c -o shm_demo -pthread
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/wait.h>

#define DONE -1                        /* end-of-data marker */

struct shared {
    sem_t empty;                       /* 1 = slot free, writer may write */
    sem_t full;                        /* 1 = data ready, reader may read */
    int value;                         /* the shared slot */
};

int main(void)
{
    struct shared *s = mmap(NULL, sizeof *s, PROT_READ | PROT_WRITE,
                            MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (s == MAP_FAILED) { perror("mmap"); return 1; }
    sem_init(&s->empty, 1, 1);         /* pshared = 1: shared between processes */
    sem_init(&s->full,  1, 0);

    pid_t pid = fork();
    if (pid == 0) {                    /* analyser (consumer) */
        int count = 0, sum = 0, max = 0;
        while (1) {
            sem_wait(&s->full);        /* wait until data is ready */
            int r = s->value;
            sem_post(&s->empty);       /* slot is free again */
            if (r == DONE) break;
            count++; sum += r;
            if (count == 1 || r > max) max = r;
        }
        printf("analyser: %d readings, average %.1f, max %d\n", count,
               count ? (double)sum / count : 0.0, max);
        fflush(stdout);
        _exit(0);
    }
    int readings[] = {31, 33, 36, 34, 41, 38};   /* reader (producer) */
    for (int i = 0; i < 6; i++) {
        sem_wait(&s->empty);           /* wait until slot is free */
        s->value = readings[i];
        sem_post(&s->full);            /* tell the analyser */
    }
    sem_wait(&s->empty);
    s->value = DONE;                   /* send end-of-data marker */
    sem_post(&s->full);

    waitpid(pid, NULL, 0);
    printf("reader: done\n");
    return 0;
}
