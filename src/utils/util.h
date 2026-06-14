/**
 * @file util.h
 * @brief Common utility functions.
 *
 * This module provides helper functions used across the
 * Hardware Test Framework.
 *
 * Current utilities include:
 * - File loading helpers
 *
 * These utilities are intended to provide reusable,
 * platform-independent functionality for framework modules.
 */

#ifndef UTIL_H
#define UTIL_H

/**
 * @brief Read an entire file into a null-terminated string.
 *
 * Opens the specified file, reads its complete contents into
 * dynamically allocated memory, and returns a pointer to the
 * resulting string.
 *
 * The returned buffer is guaranteed to be null-terminated.
 *
 * The caller is responsible for releasing the allocated memory
 * using free().
 *
 * Example:
 *
 * @code
 * char *content = read_file_to_string("device.jsonc");
 *
 * if (content != NULL)
 * {
 *     printf("%s\n", content);
 *     free(content);
 * }
 * @endcode
 *
 * @param[in] filename Path to the file to be read.
 *
 * @return Pointer to a dynamically allocated string containing
 *         the file contents.
 *
 * @retval NULL File could not be opened, memory allocation failed,
 *              or an I/O error occurred.
 */
char* read_file_to_string(const char *filename);

#endif /* UTIL_H */