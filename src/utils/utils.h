#ifndef TINYDOCKER_UTILS_H
#define TINYDOCKER_UTILS_H

/**
 * @brief Write a formatted string to a file
 * @param path Path to the file
 * @param fmt Format string
 * @param ... Variable arguments for the format string
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on failure
 */
int write_str_to_file(const char *path, const char *fmt, ...);

#endif // TINYDOCKER_UTILS_H