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

#ifndef VERSION
#    define VERSION "?.?.?"
#endif

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    ContainerArgs args = {
        .hostname = "tinydocker",
        .rootfs = "./rootfs",
        .process = (char *[]){ "/bin/sh", NULL },
        .max_cpus = 1, // 100% of one core
        .max_memory = 1024 * 1024 * 1024 // 1 GB
    };

    printf("🐟  tinydocker v%s\n\n", VERSION);
    printf("📦  Container config:\n");
    printf("├─  Hostname: %s\n", args.hostname);
    printf("├─  Rootfs: %s\n", args.rootfs);
    printf("├─  Process: %s\n", args.process[0]);
    printf("├─  Max CPUs: %d\n", args.max_cpus);
    printf("└─  Max Memory: %ldMB\n\n", args.max_memory / (1024 * 1024));

    printf("🚀 Starting container...\n");
    printf("\n");

    pid_t pid =
        clone(init_container, child_stack + STACK_SIZE,
              CLONE_NEWUTS | CLONE_NEWPID | CLONE_NEWNS | SIGCHLD, &args);

    if (pid == -1)
    {
        perror("clone");
        return EXIT_FAILURE;
    }

    printf("✅ Running container with PID %d:\n", pid);

    // Create and setup cgroup
    CGroup *cgroup =
        cgroup_create("tinydocker", args.max_cpus, args.max_memory);
    if (!cgroup)
    {
        perror("cgroup_create");
        kill(pid, SIGKILL);
        return EXIT_FAILURE;
    }

    if (cgroup_apply_limits(cgroup) == EXIT_FAILURE)
    {
        perror("cgroup_apply_limits");
        cgroup_free(cgroup);
        kill(pid, SIGKILL);
        return EXIT_FAILURE;
    }

    if (cgroup_add_process(cgroup, pid) == EXIT_FAILURE)
    {
        perror("cgroup_add_process");
        cgroup_free(cgroup);
        kill(pid, SIGKILL);
        return EXIT_FAILURE;
    }

    int status;
    if (waitpid(pid, &status, 0) == -1)
    {
        perror("waitpid");
        cgroup_free(cgroup);
        return EXIT_FAILURE;
    }

    if (WIFEXITED(status))
    {
        int exit_code = WEXITSTATUS(status);
        if (exit_code != EXIT_SUCCESS)
        {
            fprintf(stderr, "Container process exited with code %d\n",
                    exit_code);
        }
    }
    else if (WIFSIGNALED(status))
    {
        fprintf(stderr, "Container process was killed by signal %d\n",
                WTERMSIG(status));
    }

    if (cgroup_destroy(cgroup) == EXIT_FAILURE)
    {
        perror("cgroup_destroy");
        cgroup_free(cgroup);
        return EXIT_FAILURE;
    }

    cgroup_free(cgroup);

    return EXIT_SUCCESS;
}