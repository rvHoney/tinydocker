#define _GNU_SOURCE
#include <errno.h>
#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "cgroup/cgroup.h"
#include "container/container.h"

#define STACK_SIZE (1024 * 1024)

extern char child_stack[STACK_SIZE];

int main(int argc, char *argv[])
{
    ContainerArgs args = {
        .hostname = "tinydocker",
        .rootfs = "./rootfs",
        .command = (char *[]){ "/bin/sh", NULL },
        .max_cpus = 1, // 100% of one core
        .max_memory = 1024 * 1024 * 1024 // 1 GB
    };

    printf("🚀 Starting container...\n");

    pid_t pid =
        clone(init_container, child_stack + STACK_SIZE,
              CLONE_NEWUTS | CLONE_NEWPID | CLONE_NEWNS | SIGCHLD, &args);

    if (pid == -1)
    {
        perror("clone");
        return EXIT_FAILURE;
    }

    printf("📦 Container started with PID %d\n", pid);

    if (init_cgroup(pid, "tinydocker", args.max_cpus, args.max_memory)
        == EXIT_FAILURE)
    {
        perror("init_cgroup");
        kill(pid, SIGKILL);
        return EXIT_FAILURE;
    }

    waitpid(pid, NULL, 0);

    if (destroy_cgroup("tinydocker") == EXIT_FAILURE)
    {
        perror("destroy_cgroup");
        return EXIT_FAILURE;
    }

    printf("✨ Container finished\n");

    return EXIT_SUCCESS;
}