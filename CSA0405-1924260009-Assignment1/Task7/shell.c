#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    pid_t pid;
    int status;

    printf("shell: running job 1\n");
    fflush(stdout);                       /* empty the buffer before fork() */

    /* TODO 1: fork a child, exec expr in it, wait for it in the parent */
    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    }
    if (pid == 0) {
        /* child: replace the program with expr 6 * 7 */
        execlp("expr", "expr", "6", "*", "7", (char *)NULL);
        _exit(127);                       /* only reached if exec fails */
    }
    /* parent: wait for the child and report its exit status */
    waitpid(pid, &status, 0);
    printf("job 1 exit status %d\n", WEXITSTATUS(status));

    printf("shell: running job 2\n");
    fflush(stdout);
    pid = fork();
    if (pid == 0) _exit(3);               /* a child that fails with code 3 */
    waitpid(pid, &status, 0);
    printf("job 2 exit status %d\n", WEXITSTATUS(status));
    return 0;
}
