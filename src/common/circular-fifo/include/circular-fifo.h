
/**
 * @file circular-fifo.h
 * @brief Generic fixed-size circular FIFO implementation.
 *
 * This module provides a generic circular FIFO that supports any data type
 * through caller-provided storage. The implementation is hardware independent,
 * uses fixed memory, performs O(1) push/pop operations, and does not provide
 * internal thread synchronization.
 *
 * Thread Safety:
 *     Not thread-safe. Callers must synchronize concurrent access.
 */


#ifndef CIRCULAR_FIFO_H
#define CIRCULAR_FIFO_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    void     *buffer;
    uint32_t  capacity;
    uint32_t  element_size;

    uint32_t  head;
    uint32_t  tail;
    uint32_t  count;

} circular_fifo_t;

/* Return Codes */

#define CIRCULAR_FIFO_SUCCESS        (0)
#define CIRCULAR_FIFO_ERROR         (-1)
#define CIRCULAR_FIFO_FULL          (-2)
#define CIRCULAR_FIFO_EMPTY         (-3)
#define CIRCULAR_FIFO_INVALID_ARG   (-4)

/**
 * @brief Initialize circular FIFO.
 */
int32_t circular_fifo_init(
    circular_fifo_t *fifo,
    void *buffer,
    uint32_t capacity,
    uint32_t element_size);

/**
 * @brief Push an element into FIFO.
 */
int32_t circular_fifo_push(
    circular_fifo_t *fifo,
    const void *data);

/**
 * @brief Pop an element from FIFO.
 */
int32_t circular_fifo_pop(
    circular_fifo_t *fifo,
    void *data);

/**
 * @brief Returns true if FIFO is empty.
 */
bool circular_fifo_is_empty(
    const circular_fifo_t *fifo);

/**
 * @brief Returns true if FIFO is full.
 */
bool circular_fifo_is_full(
    const circular_fifo_t *fifo);

/**
 * @brief Returns number of elements stored.
 */
uint32_t circular_fifo_size(
    const circular_fifo_t *fifo);

/**
 * @brief Returns FIFO capacity.
 */
uint32_t circular_fifo_capacity(
    const circular_fifo_t *fifo);

/**
 * @brief Clears FIFO contents.
 */
void circular_fifo_clear(
    circular_fifo_t *fifo);

#ifdef __cplusplus
}
#endif

#endif