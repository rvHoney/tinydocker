/**
 * @file container.h
 * @brief Container management functionality
 */

#ifndef TINYDOCKER_CONTAINER_H
#define TINYDOCKER_CONTAINER_H

#include <sys/types.h>

/** @brief Size of the stack for the container process */
#define STACK_SIZE (1024 * 1024)

/** @brief Stack for the container process */
extern char child_stack[STACK_SIZE];

/**
 * @brief Container configuration arguments
 */
typedef struct
{
    const char *hostname; /**< Hostname for the container */
    const char *rootfs; /**< Path to the root filesystem */
    int max_cpus; /**< Maximum number of CPUs allowed */
    long max_memory; /**< Maximum memory allowed in bytes */
    char **process; /**< Command and arguments to execute */
} ContainerArgs;

/**
 * @brief Initialize the container environment
 *
 * This function sets up the container environment by:
 * - Setting the hostname
 * - Changing the root directory
 * - Mounting /proc
 * - Executing the specified command
 *
 * @param arg Pointer to ContainerArgs structure containing container
 * configuration
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on failure
 */
int init_container(void *arg);

#endif // TINYDOCKER_CONTAINER_H