#include "cgroup.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../utils/utils.h"

int init_cgroup(const pid_t pid, const char *group_name, int max_cpus,
                long max_memory)
{
    char *path_to_cgroup =
        malloc(strlen("/sys/fs/cgroup/") + strlen(group_name) + 1);
    if (!path_to_cgroup)
    {
        perror("malloc");
        free(path_to_cgroup);
        return EXIT_FAILURE;
    }

    sprintf(path_to_cgroup, "/sys/fs/cgroup/%s", group_name);

    if (mkdir(path_to_cgroup, 0755) == -1 && errno != EEXIST)
    {
        perror("mkdir");
        free(path_to_cgroup);
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