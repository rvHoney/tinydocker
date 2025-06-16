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

#define STACK_SIZE (1024 * 1024)
char child_stack[STACK_SIZE];

int init_container(void *arg)
{
    ContainerArgs *args = (ContainerArgs *)arg;

    // Set hostname
    if (sethostname(args->hostname, strlen(args->hostname)) != 0)
    {
        perror("sethostname");
        return EXIT_FAILURE;
    }

    // Change root directory
    if (chroot(args->rootfs) != 0)
    {
        fprintf(stderr, "chroot failed: %s\n", strerror(errno));
        fprintf(stderr,
                "Make sure the root filesystem exists and contains necessary "
                "files\n");
        return EXIT_FAILURE;
    }

    // Change to root directory
    if (chdir("/") != 0)
    {
        perror("chdir");
        return EXIT_FAILURE;
    }

    // Mount /proc
    if (mount("proc", "/proc", "proc", 0, NULL) != 0)
    {
        perror("mount /proc");
        return EXIT_FAILURE;
    }

    // Fork to handle unmounting
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        if (umount2("/proc", MNT_DETACH) != 0)
        {
            perror("umount2 /proc");
        }
        return EXIT_FAILURE;
    }

    if (pid == 0)
    {
        // Child process - execute the command
        if (execvp(args->process[0], args->process) != 0)
        {
            fprintf(stderr, "Failed to execute %s: %s\n", args->process[0],
                    strerror(errno));
            if (umount2("/proc", MNT_DETACH) != 0)
            {
                perror("umount2 /proc");
            }
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    else
    {
        // Parent process - wait for child and unmount
        int status;
        waitpid(pid, &status, 0);

        // Unmount /proc after child process ends
        if (umount2("/proc", MNT_DETACH) != 0)
        {
            perror("umount2 /proc");
        }

        return WIFEXITED(status) ? WEXITSTATUS(status) : EXIT_FAILURE;
    }
}