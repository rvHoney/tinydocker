/**
 * @file cgroup.h
 * @brief Control groups (cgroups) management functionality
 */

#ifndef TINYDOCKER_CGROUP_H
#define TINYDOCKER_CGROUP_H

#include <sys/types.h>

/**
 * @brief Control group structure
 */
typedef struct
{
    char *name; /**< Name of the control group */
    char *path; /**< Path to the control group in the filesystem */
    int max_cpus; /**< Maximum number of CPUs allowed */
    long max_memory; /**< Maximum memory allowed in bytes */
    pid_t pid; /**< Process ID of the container */
} CGroup;

/**
 * @brief Create a new control group
 *
 * Creates a new control group with the specified name and resource limits.
 * The control group is created in the cgroup filesystem.
 *
 * @param name Name of the control group
 * @param max_cpus Maximum number of CPUs allowed
 * @param max_memory Maximum memory allowed in bytes
 * @return Pointer to the created CGroup structure, or NULL on failure
 */
CGroup *cgroup_create(const char *name, int max_cpus, long max_memory);

/**
 * @brief Add a process to a control group
 *
 * Adds the specified process to the control group, applying the resource
 * limits.
 *
 * @param cgroup Pointer to the CGroup structure
 * @param pid Process ID to add to the control group
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on failure
 */
int cgroup_add_process(CGroup *cgroup, pid_t pid);

/**
 * @brief Apply resource limits to a control group
 *
 * Sets the CPU and memory limits for the control group.
 *
 * @param cgroup Pointer to the CGroup structure
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on failure
 */
int cgroup_apply_limits(CGroup *cgroup);

/**
 * @brief Destroy a control group
 *
 * Removes the control group from the filesystem.
 *
 * @param cgroup Pointer to the CGroup structure
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on failure
 */
int cgroup_destroy(CGroup *cgroup);

/**
 * @brief Free resources associated with a control group
 *
 * Frees all memory allocated for the control group structure.
 *
 * @param cgroup Pointer to the CGroup structure
 */
void cgroup_free(CGroup *cgroup);

#endif // TINYDOCKER_CGROUP_H