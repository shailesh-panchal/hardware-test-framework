#ifndef SAFE_STRING_H
#define SAFE_STRING_H

#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>

/**
 * @brief Safely copies a string from source to destination.
 * 
 * This function guarantees that the destination buffer is always null-terminated
 * (if dest_size > 0), preventing buffer overflows. It mimics the POSIX strlcpy behaviour.
 * 
 * @param dest      Pointer to the destination buffer.
 * @param src       Pointer to the source string.
 * @param dest_size Total size of the destination buffer in bytes.
 * @return size_t   The total length of the string it tried to create (length of src).
 *                  If return value >= dest_size, truncation occurred.
 */
static inline size_t safe_string_copy(char *dest, const char *src, size_t dest_size) {
    // 1. Safety check for null pointers or empty sizes
    if (dest == NULL || dest_size == 0) {
        return 0;
    }
    if (src == NULL) {
        dest[0] = '\0';
        return 0;
    }

    size_t src_len = strlen(src);
    size_t copy_len = src_len;

    // 2. Bound check: Ensure space remains for the '\0' terminator
    if (copy_len >= dest_size) {
        copy_len = dest_size - 1;
    }

    // 3. Perform the safe copy
    if (copy_len > 0) {
        memcpy(dest, src, copy_len);
    }

    // 4. Always enforce exact termination
    dest[copy_len] = '\0';

    return src_len;
}

static inline bool safe_string_compare(const char *str1, const char *str2)
{
    if (str1 == NULL || str2 == NULL)
    {
        return false;
    }

    return (strcmp(str1, str2) == 0);
}

/**
 * @brief Safely concatenates the src string to the dst buffer.
 * 
 * @param dst        Pointer to the destination buffer.
 * @param src        Pointer to the source string to append.
 * @param dst_size   The TOTAL size allocated for the dst buffer (including '\0').
 * @return true      If concatenation succeeded completely.
 * @return false     If truncation happened (buffer was too small) or invalid inputs.
 */
static inline bool safe_strcat(char *dst, const char *src, size_t dst_size) {
    // 1. Safety check for null pointers or zero-size buffers
    if (dst == NULL || src == NULL || dst_size == 0) {
        return false;
    }

    // 2. Find the current length of the string already inside dst
    size_t dst_len = strnlen(dst, dst_size);

    // If dst is not null-terminated within the given dst_size, it's an invalid buffer
    if (dst_len >= dst_size) {
        return false; 
    }

    // 3. Calculate exactly how much room is left (accounting for the null terminator space)
    size_t remaining_space = dst_size - dst_len - 1;

    // 4. Safely copy over characters from src using the calculated remaining space limit
    size_t i = 0;
    bool fits = true;

    while (src[i] != '\0') {
        if (i < remaining_space) {
            dst[dst_len + i] = src[i];
        } else {
            // Buffer is full; we must truncate the rest of the src string
            fits = false;
            break;
        }
        i++;
    }

    // 5. Explicitly guarantee null-termination under all conditions
    dst[dst_len + (i < remaining_space ? i : remaining_space)] = '\0';

    return fits;
}

/**
 * @brief Safely formats a string and explicitly checks for truncation.
 * 
 * @param dst       Pointer to destination buffer.
 * @param dst_size  Total size of destination buffer.
 * @param format    Printf-style format string.
 * @return true     If the string formatted perfectly without truncation.
 * @return false    If truncation occurred or an error happened.
 */
static inline bool safe_sprintf(char *dst, size_t dst_size, const char *format, ...) {
    if (dst == NULL || dst_size == 0 || format == NULL) {
        return false;
    }

    va_list args;
    va_start(args, format);
    
    // vsnprintf is the variable-argument version of snprintf
    int result = vsnprintf(dst, dst_size, format, args);
    
    va_end(args);

    // An error occurred during processing
    if (result < 0) {
        dst[0] = '\0'; // Ensure buffer is safely cleared/terminated
        return false;
    }

    // Check if the output fit completely inside the buffer
    if ((size_t)result >= dst_size) {
        return false; // Truncation happened!
    }

    return true; // Successfully formatted completely
}


#endif // SAFE_STRING_H
