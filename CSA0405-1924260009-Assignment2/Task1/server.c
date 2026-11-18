#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define NUM_CHILDREN 5

int main() {
    printf("Parent (server) PID = %d\n", getpid());

    for (int i = 0; i < NUM_CHILDREN; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork failed");
            exit(1);
        }
        else if (pid == 0) {
            /* Child: simulates one student session */
            printf("Student session %d: PID = %d, Parent PID = %d\n",
                   i + 1, getpid(), getppid());
            sleep(30);          /* simulates waiting for I/O */
            exit(0);
        }
    }

    /* Parent waits for all children */
    for (int i = 0; i < NUM_CHILDREN; i++)
        wait(NULL);

    printf("All sessions finished.\n");
    return 0;
}
