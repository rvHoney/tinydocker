#define _GNU_SOURCE
#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define STACK_SIZE (1024 * 1024)

static char child_stack[STACK_SIZE];

int child_func(void *arg)
{
    (void)arg;
    printf("Child process: PID=%ld\n", (long)getpid());
    execlp("sh", "sh", NULL);
    perror("execlp failed");
    return 1;
}

int main(void)
{
    pid_t pid = clone(child_func, child_stack + STACK_SIZE,
                      CLONE_NEWUTS | CLONE_NEWNS | CLONE_NEWPID | CLONE_NEWIPC
                          | CLONE_NEWNET | SIGCHLD,
                      NULL);

    if (pid == -1)
    {
        perror("clone failed");
        exit(EXIT_FAILURE);
    }

    printf("Child process created with PID: %d\n", pid);

    if (waitpid(pid, NULL, 0) == -1)
    {
        perror("waitpid failed");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}