#define _GNU_SOURCE
#include <errno.h>
#include <sched.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define STACK_SIZE (1024 * 1024)

static char child_stack[STACK_SIZE];

typedef struct
{
    char *hostname;
    char *rootfs;
    char **command;
    int max_cpus;
    long max_memory; // in bytes
} ContainerArgs;

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
    mkdir("/proc", 0755);
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

int write_str_to_file(const char *path, const char *fmt, ...)
{
    FILE *f = fopen(path, "w");
    if (!f)
    {
        perror("fopen");
        return EXIT_FAILURE;
    }

    va_list args;
    va_start(args, fmt);
    if (vfprintf(f, fmt, args) < 0)
    {
        perror("vfprintf");
        va_end(args);
        fclose(f);
        return EXIT_FAILURE;
    }
    va_end(args);
    fclose(f);
    return EXIT_SUCCESS;
}

int init_cgroup(const pid_t pid, const char *group_name, int max_cpus,
                long max_memory)
{
    char *path_to_cgroup =
        malloc(strlen("/sys/fs/cgroup/") + strlen(group_name) + 1);
    if (!path_to_cgroup)
    {
        perror("malloc");
        return EXIT_FAILURE;
    }

    sprintf(path_to_cgroup, "/sys/fs/cgroup/%s", group_name);

    if (mkdir(path_to_cgroup, 0755) == -1 && errno != EEXIST)
    {
        perror("mkdir");
        return EXIT_FAILURE;
    }

    char *path_to_cpu_max =
        malloc(strlen(path_to_cgroup) + strlen("/cpu.max") + 1);
    char *path_to_memory_max =
        malloc(strlen(path_to_cgroup) + strlen("/memory.max") + 1);
    char *path_to_proc =
        malloc(strlen(path_to_cgroup) + strlen("/cgroup.procs") + 1);

    if (!path_to_cpu_max || !path_to_memory_max || !path_to_proc)
    {
        perror("malloc");
        free(path_to_cpu_max);
        free(path_to_memory_max);
        free(path_to_proc);
        free(path_to_cgroup);
        return EXIT_FAILURE;
    }

    sprintf(path_to_cpu_max, "%s/cpu.max", path_to_cgroup);
    sprintf(path_to_memory_max, "%s/memory.max", path_to_cgroup);
    sprintf(path_to_proc, "%s/cgroup.procs", path_to_cgroup);

    printf("📊 Setting up cgroup at '%s'\n", path_to_cgroup);

    printf("🔳 Setting CPU limit to %d CPUs\n", max_cpus);
    if (max_cpus > 0)
    {
        if (write_str_to_file(path_to_cpu_max, "%d %d\n", max_cpus * 100000,
                              100000)
            == EXIT_FAILURE)
        {
            perror("write_str_to_file");
            free(path_to_cpu_max);
            free(path_to_memory_max);
            free(path_to_proc);
            free(path_to_cgroup);
            return EXIT_FAILURE;
        }
    }
    else
    {
        if (write_str_to_file(path_to_cpu_max, "max 100000\n") == EXIT_FAILURE)
        {
            perror("write_str_to_file");
            free(path_to_cpu_max);
            free(path_to_memory_max);
            free(path_to_proc);
            free(path_to_cgroup);
            return EXIT_FAILURE;
        }
    }

    printf("💾 Setting memory limit to %ld bytes\n", max_memory);
    if (write_str_to_file(path_to_memory_max, "%ld\n", max_memory)
        == EXIT_FAILURE)
    {
        perror("write_str_to_file");
        free(path_to_cpu_max);
        free(path_to_memory_max);
        free(path_to_proc);
        free(path_to_cgroup);
        return EXIT_FAILURE;
    }

    printf("👥 Adding PID %d to cgroup\n\n", pid);
    if (write_str_to_file(path_to_proc, "%d\n", pid) == EXIT_FAILURE)
    {
        perror("write_str_to_file");
        free(path_to_cpu_max);
        free(path_to_memory_max);
        free(path_to_proc);
        free(path_to_cgroup);
        return EXIT_FAILURE;
    }

    free(path_to_cpu_max);
    free(path_to_memory_max);
    free(path_to_proc);
    free(path_to_cgroup);

    return EXIT_SUCCESS;
}

int destroy_cgroup(const char *group_name)
{
    char *path_to_cgroup =
        malloc(strlen("/sys/fs/cgroup/") + strlen(group_name) + 1);
    if (!path_to_cgroup)
    {
        perror("malloc");
        return EXIT_FAILURE;
    }

    sprintf(path_to_cgroup, "/sys/fs/cgroup/%s", group_name);

    if (rmdir(path_to_cgroup) == -1 && errno != ENOENT)
    {
        perror("rmdir");
        free(path_to_cgroup);
        return EXIT_FAILURE;
    }

    free(path_to_cgroup);
    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    ContainerArgs args = {
        .hostname = "tinydocker",
        .rootfs = "./rootfs",
        .command = (char *[]){ "/bin/sh", NULL },
        .max_cpus = 1, // 100% of one core
        .max_memory = 1024 * 1024 * 1024 // 1 GB
    };

    int current_arg = 1;
    int command_offset = 0;
    while (current_arg < argc)
    {
        if (strcmp(argv[current_arg], "--") == 0)
        {
            command_offset = current_arg + 1;
            break;
        }
        else if (strcmp(argv[current_arg], "-h") == 0 && current_arg + 1 < argc)
        {
            args.hostname = argv[++current_arg];
        }
        else if (strcmp(argv[current_arg], "-r") == 0 && current_arg + 1 < argc)
        {
            args.rootfs = argv[++current_arg];
        }
        else if (strcmp(argv[current_arg], "--cpus") == 0
                 && current_arg + 1 < argc)
        {
            args.max_cpus = atoi(argv[++current_arg]);
        }
        else if (strcmp(argv[current_arg], "--memory") == 0
                 && current_arg + 1 < argc)
        {
            args.max_memory = atol(argv[++current_arg]);
        }
        else
        {
            fprintf(stderr,
                    "Usage: %s [-h hostname] [-r rootfs] [--cpus num] "
                    "[--memory size] [-- command [args...]]\n",
                    argv[0]);
            exit(EXIT_FAILURE);
        }
        current_arg++;
    }

    if (command_offset > 0)
    {
        int cmd_argc = argc - command_offset;
        args.command = malloc((cmd_argc + 1) * sizeof(char *));
        if (!args.command)
        {
            perror("malloc failed");
            exit(EXIT_FAILURE);
        }
        for (int j = 0; j < cmd_argc; j++)
        {
            args.command[j] = argv[command_offset + j];
        }
        args.command[cmd_argc] = NULL;
    }

    printf("🐟 Welcome to TinyDocker!\n\n");

    pid_t pid = clone(init_container, child_stack + STACK_SIZE,
                      CLONE_NEWUTS | CLONE_NEWNS | CLONE_NEWPID | CLONE_NEWIPC
                          | CLONE_NEWNET | SIGCHLD,
                      &args);

    if (pid == -1)
    {
        perror("clone failed");
        exit(EXIT_FAILURE);
    }

    printf("📦 Container created with PID: %ld \n\n", (long)pid);

    char *group_name =
        malloc(strlen("tinydocker-") + strlen(args.hostname) + 1);
    if (!group_name)
    {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    snprintf(group_name, strlen("tinydocker-") + strlen(args.hostname) + 1,
             "tinydocker-%s", args.hostname);

    if (init_cgroup(pid, group_name, args.max_cpus, args.max_memory)
        == EXIT_FAILURE)
    {
        fprintf(stderr, "Failed to set up cgroup for PID %ld\n", (long)pid);
        exit(EXIT_FAILURE);
    }

    if (waitpid(pid, NULL, 0) == -1)
    {
        perror("waitpid failed");
        exit(EXIT_FAILURE);
    }

    destroy_cgroup(group_name);
    free(group_name);

    if (command_offset > 0)
        free(args.command);

    return EXIT_SUCCESS;
}