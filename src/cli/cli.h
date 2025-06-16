/**
 * @file cli.h
 * @brief Command-line interface functionality
 */

#ifndef TINYDOCKER_CLI_H
#define TINYDOCKER_CLI_H

#include "../container/container.h"

/**
 * @brief Parse command-line arguments
 *
 * Parses the command-line arguments and fills the ContainerArgs structure
 * with the configuration. If no command is specified, defaults to /bin/sh.
 *
 * @param argc Number of command-line arguments
 * @param argv Array of command-line argument strings
 * @param args Pointer to ContainerArgs structure to fill
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on failure
 */
int parse_args(int argc, char *argv[], ContainerArgs *args);

#endif // TINYDOCKER_CLI_H