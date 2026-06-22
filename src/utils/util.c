#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "util.h"

#define LOG_MODULE "UTILS_MANAGER"
#include "logger.h"

// Helper function to read the entire file into a dynamically allocated string
char* read_file_to_string(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        // printf("Error: Could not open file %s\n", filename);
        LOG_ERROR("Could not open file %s", filename);
        return NULL;
    }

    // Find file size
    fseek(file, 0, SEEK_END);
    uint32_t length = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate memory for the buffer
    char *buffer = (char*)malloc(length + 1);
    if (buffer == NULL) {
        // printf("Error: Memory allocation failed\n");
        LOG_ERROR("Memory allocation failed");
        fclose(file);
        return NULL;
    }

    // Read the file and null-terminate the string
    size_t read_bytes = fread(buffer, 1, length, file);
    buffer[read_bytes] = '\0';

    fclose(file);
    return buffer;
}
