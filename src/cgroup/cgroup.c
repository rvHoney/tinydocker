#define _GNU_SOURCE
#include "cgroup.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../utils/utils.h"

#define CGROUP_BASE_PATH "/sys/fs/cgroup"

CGroup *cgroup_create(const char *name, int max_cpus, long max_memory)
{
    CGroup *cgroup = malloc(sizeof(CGroup));
    if (!cgroup)
    {
        fprintf(stderr, "Error: malloc failed: %s\n", strerror(errno));
        return NULL;
    }

    cgroup->name = strdup(name);
    if (!cgroup->name)
    {
        fprintf(stderr, "Error: strdup failed: %s\n", strerror(errno));
        free(cgroup);
        return NULL;
    }

    size_t path_len = strlen(CGROUP_BASE_PATH) + strlen(name) + 2;
    cgroup->path = malloc(path_len);
    if (!cgroup->path)
    {
        fprintf(stderr, "Error: malloc failed: %s\n", strerror(errno));
        free(cgroup->name);
        free(cgroup);
        return NULL;
    }

    ssize_t ret =
        snprintf(cgroup->path, path_len, "%s/%s", CGROUP_BASE_PATH, name);
    if (ret < 0 || (size_t)ret >= path_len)
    {
        fprintf(stderr, "Path too long\n");
        free(cgroup->name);
        free(cgroup->path);
        free(cgroup);
        return NULL;
    }

    cgroup->max_cpus = max_cpus;
    cgroup->max_memory = max_memory;
    cgroup->pid = 0;

    if (mkdir(cgroup->path, 0755) == -1 && errno != EEXIST)
    {
        fprintf(stderr, "Error: mkdir failed: %s\n", strerror(errno));
        cgroup_free(cgroup);
        return NULL;
    }

    return cgroup;
}

int cgroup_add_process(CGroup *cgroup, pid_t pid)
{
    if (!cgroup)
        return EXIT_FAILURE;

    size_t path_len = strlen(cgroup->path) + strlen("/cgroup.procs") + 1;
    char *path_to_proc = malloc(path_len);
    if (!path_to_proc)
    {
        fprintf(stderr, "Error: malloc failed: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    ssize_t ret =
        snprintf(path_to_proc, path_len, "%s/cgroup.procs", cgroup->path);
    if (ret < 0 || (size_t)ret >= path_len)
    {
        fprintf(stderr, "Path too long\n");
        free(path_to_proc);
        return EXIT_FAILURE;
    }

    ret = write_str_to_file(path_to_proc, "%d\n", pid);
    free(path_to_proc);

    if (ret == EXIT_SUCCESS)
        cgroup->pid = pid;

    return ret;
}

int cgroup_apply_limits(CGroup *cgroup)
{
    if (!cgroup)
        return EXIT_FAILURE;

    size_t path_len = strlen(cgroup->path) + strlen("/cpu.max") + 1;
    char *path_to_cpu_max = malloc(path_len);
    if (!path_to_cpu_max)
    {
        fprintf(stderr, "Error: malloc failed: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    ssize_t ret =
        snprintf(path_to_cpu_max, path_len, "%s/cpu.max", cgroup->path);
    if (ret < 0 || (size_t)ret >= path_len)
    {
        fprintf(stderr, "Path too long\n");
        free(path_to_cpu_max);
        return EXIT_FAILURE;
    }

    path_len = strlen(cgroup->path) + strlen("/memory.max") + 1;
    char *path_to_memory_max = malloc(path_len);
    if (!path_to_memory_max)
    {
        fprintf(stderr, "Error: malloc failed: %s\n", strerror(errno));
        free(path_to_cpu_max);
        return EXIT_FAILURE;
    }

    ret = snprintf(path_to_memory_max, path_len, "%s/memory.max", cgroup->path);
    if (ret < 0 || (size_t)ret >= path_len)
    {
        fprintf(stderr, "Path too long\n");
        free(path_to_cpu_max);
        free(path_to_memory_max);
        return EXIT_FAILURE;
    }

    int status = EXIT_SUCCESS;
    if (cgroup->max_cpus > 0)
    {
        status = write_str_to_file(path_to_cpu_max, "%d %d\n",
                                   cgroup->max_cpus * 100000, 100000);
    }
    else
    {
        status = write_str_to_file(path_to_cpu_max, "max 100000\n");
    }

    if (status == EXIT_SUCCESS)
    {
        status =
            write_str_to_file(path_to_memory_max, "%ld\n", cgroup->max_memory);
    }

    free(path_to_cpu_max);
    free(path_to_memory_max);
    return status;
}

int cgroup_destroy(CGroup *cgroup)
{
    if (!cgroup)
        return EXIT_FAILURE;

    if (rmdir(cgroup->path) == -1 && errno != ENOENT)
    {
        fprintf(stderr, "Error: rmdir failed: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void cgroup_free(CGroup *cgroup)
{
    if (cgroup)
    {
        free(cgroup->name);
        free(cgroup->path);
        free(cgroup);
    }
}