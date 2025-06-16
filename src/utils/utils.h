/**
 * @file utils.h
 * @brief Utility functions
 */

#ifndef TINYDOCKER_UTILS_H
#define TINYDOCKER_UTILS_H

/**
 * @brief Write a formatted string to a file
 *
 * Opens a file in write mode and writes a formatted string to it.
 * The format string and arguments work like printf.
 *
 * @param path Path to the file to write
 * @param fmt Format string
 * @param ... Variable arguments for the format string
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on failure
 */
int write_str_to_file(const char *path, const char *fmt, ...);

#endif // TINYDOCKER_UTILS_H