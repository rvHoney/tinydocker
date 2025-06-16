#ifndef TINYDOCKER_CONTAINER_H
#define TINYDOCKER_CONTAINER_H

#include <sys/types.h>

#define STACK_SIZE (1024 * 1024)

extern char child_stack[STACK_SIZE];

typedef struct
{
    char *hostname;
    char *rootfs;
    char **command;
    int max_cpus;
    long max_memory; // in bytes
} ContainerArgs;

/**
 * @brief Initialize the container environment
 * @param arg Pointer to ContainerArgs structure
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on failure
 */
int init_container(void *arg);

#endif // TINYDOCKER_CONTAINER_H