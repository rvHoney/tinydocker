#ifndef TINYDOCKER_CLI_H
#define TINYDOCKER_CLI_H

#include "../container/container.h"

/**
 * @brief Parse command line arguments into ContainerArgs structure
 * @param argc Number of arguments
 * @param argv Array of argument strings
 * @param args Pointer to ContainerArgs structure to fill
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on failure
 */
int parse_args(int argc, char *argv[], ContainerArgs *args);

/**
 * @brief Print usage information
 * @param program_name Name of the program (argv[0])
 */
void print_usage(const char *program_name);

#endif // TINYDOCKER_CLI_H