#ifndef TINYDOCKER_CLI_H
#define TINYDOCKER_CLI_H

#include "../container/container.h"

/**
 * Parse command line arguments into a ContainerArgs structure
 * @param argc Number of arguments
 * @param argv Array of argument strings
 * @param args Pointer to ContainerArgs structure to fill
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on error
 */
int parse_args(int argc, char *argv[], ContainerArgs *args);

#endif // TINYDOCKER_CLI_H