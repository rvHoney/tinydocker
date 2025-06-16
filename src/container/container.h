#ifndef TINYDOCKER_CONTAINER_H
#define TINYDOCKER_CONTAINER_H

#include <sys/types.h>

#define STACK_SIZE (1024 * 1024)

extern char child_stack[STACK_SIZE];

typedef struct
{
    const char *hostname;
    const char *rootfs;
    int max_cpus;
    long max_memory;
    char **process;
    char **process_args;
} ContainerArgs;

/**
 * @brief Initialize the container environment
 * @param arg Pointer to ContainerArgs structure
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on failure
 */
int init_container(void *arg);

#endif // TINYDOCKER_CONTAINER_H