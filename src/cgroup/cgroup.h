#ifndef TINYDOCKER_CGROUP_H
#define TINYDOCKER_CGROUP_H

#include <sys/types.h>

typedef struct
{
    char *name;
    char *path;
    int max_cpus;
    long max_memory;
    pid_t pid;
} CGroup;

/**
 * @brief Create a new cgroup
 * @param name Name of the cgroup
 * @param max_cpus Maximum number of CPUs (0 for unlimited)
 * @param max_memory Maximum memory in bytes
 * @return Pointer to new CGroup structure, NULL on failure
 */
CGroup *cgroup_create(const char *name, int max_cpus, long max_memory);

/**
 * @brief Add a process to a cgroup
 * @param cgroup Pointer to CGroup structure
 * @param pid Process ID to add
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on failure
 */
int cgroup_add_process(CGroup *cgroup, pid_t pid);

/**
 * @brief Apply resource limits to a cgroup
 * @param cgroup Pointer to CGroup structure
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on failure
 */
int cgroup_apply_limits(CGroup *cgroup);

/**
 * @brief Destroy a cgroup
 * @param cgroup Pointer to CGroup structure
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on failure
 */
int cgroup_destroy(CGroup *cgroup);

/**
 * @brief Free a CGroup structure
 * @param cgroup Pointer to CGroup structure
 */
void cgroup_free(CGroup *cgroup);

#endif // TINYDOCKER_CGROUP_H