#define _GNU_SOURCE
#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/wait.h>
#include <unistd.h>

#define STACK_SIZE (1024 * 1024)

static char child_stack[STACK_SIZE];

int child_func(void *arg)
{
    (void)arg;

    const char *name = "tinydocker";
    printf("🔧 Setting hostname to '%s'\n", name);
    if (sethostname(name, strlen(name)) == -1)
    {
        perror("sethostname failed");
        return 1;
    }

    printf("🔒 Setting up namespaces...\n");
    if (chroot("./rootfs") == -1)
    {
        perror("chroot failed");
        return 1;
    }

    printf("📂 Changing root directory to './rootfs'\n");
    if (chdir("/") == -1)
    {
        perror("chdir failed");
        return 1;
    }

    printf("🔗 Mounting proc filesystem...\n");
    if (mount("proc", "/proc", "proc", 0, NULL) == -1)
    {
        perror("mount proc failed");
        return 1;
    }

    printf("🖥️ Starting shell in the container...\n");
    execlp("/bin/sh", "sh", NULL);
    perror("execlp failed");
    return 1;
}

int main(void)
{
    printf("🐚 Starting TinyDocker...\n");
    pid_t pid = clone(child_func, child_stack + STACK_SIZE,
                      CLONE_NEWUTS | CLONE_NEWNS | CLONE_NEWPID | CLONE_NEWIPC
                          | CLONE_NEWNET | SIGCHLD,
                      NULL);

    if (pid == -1)
    {
        perror("clone failed");
        exit(EXIT_FAILURE);
    }

    printf("📦 Container created with PID: %ld\n", (long)pid);

    if (waitpid(pid, NULL, 0) == -1)
    {
        perror("waitpid failed");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}