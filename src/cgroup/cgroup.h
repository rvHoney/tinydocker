#ifndef TINYDOCKER_CGROUP_H
#define TINYDOCKER_CGROUP_H

#include <sys/types.h>

/**
 * @brief Initialize cgroup with CPU and memory limits
 * @param pid Process ID to add to cgroup
 * @param group_name Name of the cgroup
 * @param max_cpus Maximum number of CPUs (0 for unlimited)
 * @param max_memory Maximum memory in bytes
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on failure
 */
int init_cgroup(const pid_t pid, const char *group_name, int max_cpus,
                long max_memory);

/**
 * @brief Destroy a cgroup
 * @param group_name Name of the cgroup to destroy
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on failure
 */
int destroy_cgroup(const char *group_name);

#endif // TINYDOCKER_CGROUP_H