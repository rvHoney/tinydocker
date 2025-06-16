#define _GNU_SOURCE
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

#include "cgroup/cgroup.h"
#include "cli/cli.h"
#include "container/container.h"

#ifndef VERSION
#    define VERSION "?.?.?"
#endif

#define TTY_PG_FATAL_ERROR 2

int main(int argc, char *argv[])
{
    ContainerArgs args;

    if (parse_args(argc, argv, &args) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    printf("🐟  tinydocker v%s\n\n", VERSION);
    printf("📦  Container config:\n");
    printf("├─  Hostname: %s\n", args.hostname);
    printf("├─  Rootfs: %s\n", args.rootfs);
    printf("├─  Process: %s\n", args.process[0]);
    printf("├─  Max CPUs: %d\n", args.max_cpus);
    printf("└─  Max Memory: %ldMB\n\n", args.max_memory / (1024 * 1024));

    // Check if rootfs exists and is a directory
    struct stat st;
    if (stat(args.rootfs, &st) != 0)
    {
        fprintf(stderr, "Error: Root filesystem '%s' not found: %s\n",
                args.rootfs, strerror(errno));
        fprintf(stderr,
                "Please create a root filesystem directory or specify a "
                "different path with -r\n");
        return EXIT_FAILURE;
    }
    if (!S_ISDIR(st.st_mode))
    {
        fprintf(stderr, "Error: '%s' is not a directory\n", args.rootfs);
        return EXIT_FAILURE;
    }

    printf("🚀 Starting container...\n");
    printf("\n");

    pid_t pid =
        clone(init_container, child_stack + STACK_SIZE,
              CLONE_NEWUTS | CLONE_NEWPID | CLONE_NEWNS | SIGCHLD, &args);

    if (pid == -1)
    {
        if (errno == EPERM)
        {
            fprintf(stderr,
                    "Error: Operation not permitted. This program requires "
                    "root privileges.\n");
            fprintf(stderr,
                    "Please run with sudo: sudo %s [OPTIONS] -- COMMAND "
                    "[ARGS...]\n",
                    argv[0]);
        }
        else
        {
            fprintf(stderr, "Error: clone failed: %s\n", strerror(errno));
        }
        return EXIT_FAILURE;
    }

    printf("✅ Running container with PID %d:\n", pid);

    // Create and setup cgroup
    CGroup *cgroup =
        cgroup_create("tinydocker", args.max_cpus, args.max_memory);
    if (!cgroup)
    {
        fprintf(stderr, "Error: cgroup creation failed: %s\n", strerror(errno));
        kill(pid, SIGKILL);
        return EXIT_FAILURE;
    }

    if (cgroup_apply_limits(cgroup) == EXIT_FAILURE)
    {
        fprintf(stderr, "Error: cgroup limits application failed: %s\n",
                strerror(errno));
        cgroup_free(cgroup);
        kill(pid, SIGKILL);
        return EXIT_FAILURE;
    }

    if (cgroup_add_process(cgroup, pid) == EXIT_FAILURE)
    {
        fprintf(stderr, "Error: cgroup process addition failed: %s\n",
                strerror(errno));
        cgroup_free(cgroup);
        kill(pid, SIGKILL);
        return EXIT_FAILURE;
    }

    int status;
    if (waitpid(pid, &status, 0) == -1)
    {
        fprintf(stderr, "Error: container process wait failed: %s\n",
                strerror(errno));
        cgroup_free(cgroup);
        return EXIT_FAILURE;
    }

    if (WIFEXITED(status))
    {
        int exit_code = WEXITSTATUS(status);
        // Ignore the tty process group error (exit code 2)
        if (exit_code != EXIT_SUCCESS && exit_code != TTY_PG_FATAL_ERROR)
        {
            fprintf(stderr, "Error: Container process exited with code %d\n",
                    exit_code);
        }
    }
    else if (WIFSIGNALED(status))
    {
        fprintf(stderr, "Error: Container process was killed by signal %d\n",
                WTERMSIG(status));
    }

    if (cgroup_destroy(cgroup) == EXIT_FAILURE)
    {
        fprintf(stderr, "Error: cgroup destruction failed: %s\n",
                strerror(errno));
        cgroup_free(cgroup);
        return EXIT_FAILURE;
    }

    cgroup_free(cgroup);

    return EXIT_SUCCESS;
}