#define _GNU_SOURCE
#include "container.h"

#include <errno.h>
#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

char child_stack[STACK_SIZE];

int init_container(void *arg)
{
    ContainerArgs *args = (ContainerArgs *)arg;

    printf("🔧 Setting hostname to '%s'\n", args->hostname);
    if (sethostname(args->hostname, strlen(args->hostname)) == -1)
    {
        perror("sethostname failed");
        return EXIT_FAILURE;
    }

    printf("🔒 Setting up namespaces...\n");
    if (chdir(args->rootfs) == -1 || chroot(".") == -1)
    {
        perror("chroot failed");
        return EXIT_FAILURE;
    }

    printf("📂 Changing root directory to '%s'\n", args->rootfs);
    if (chdir("/") == -1)
    {
        perror("chdir failed");
        return EXIT_FAILURE;
    }

    printf("🔗 Mounting proc filesystem...\n\n");
    if (mkdir("/proc", 0755) == -1 && errno != EEXIST)
    {
        perror("mkdir /proc failed");
        return EXIT_FAILURE;
    }

    if (mount("proc", "/proc", "proc", 0, NULL) == -1)
    {
        perror("mount proc failed");
        return EXIT_FAILURE;
    }

    pid_t shell_pid = fork();
    if (shell_pid == -1)
    {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (shell_pid == 0)
    {
        printf("🐚 Starting command: %s\n", args->command[0]);
        execvp(args->command[0], args->command);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }
    else
    {
        waitpid(shell_pid, NULL, 0);
        printf("🧹 Unmounting /proc...\n");
        if (umount2("/proc", MNT_DETACH) == -1)
        {
            perror("umount /proc");
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
}