
/**
 * @file circular-fifo.c
 * @brief Implementation of generic circular FIFO.
 */

#include "circular-fifo.h"

#include <string.h>

int32_t circular_fifo_init(
    circular_fifo_t *fifo,
    void *buffer,
    uint32_t capacity,
    uint32_t element_size)
{
    if ((fifo == NULL) ||
        (buffer == NULL) ||
        (capacity == 0U) ||
        (element_size == 0U))
    {
        return CIRCULAR_FIFO_INVALID_ARG;
    }

    fifo->buffer = buffer;
    fifo->capacity = capacity;
    fifo->element_size = element_size;

    fifo->head = 0U;
    fifo->tail = 0U;
    fifo->count = 0U;

    return CIRCULAR_FIFO_SUCCESS;
}

int32_t circular_fifo_push(
    circular_fifo_t *fifo,
    const void *data)
{
    uint8_t *buffer;

    if ((fifo == NULL) || (data == NULL))
    {
        return CIRCULAR_FIFO_INVALID_ARG;
    }

    if (fifo->count == fifo->capacity)
    {
        return CIRCULAR_FIFO_FULL;
    }

    buffer = (uint8_t *)fifo->buffer;

    memcpy(
        buffer + (fifo->tail * fifo->element_size),
        data,
        fifo->element_size);

    fifo->tail++;

    if (fifo->tail >= fifo->capacity)
    {
        fifo->tail = 0U;
    }

    fifo->count++;

    return CIRCULAR_FIFO_SUCCESS;
}

int32_t circular_fifo_pop(
    circular_fifo_t *fifo,
    void *data)
{
    uint8_t *buffer;

    if ((fifo == NULL) || (data == NULL))
    {
        return CIRCULAR_FIFO_INVALID_ARG;
    }

    if (fifo->count == 0U)
    {
        return CIRCULAR_FIFO_EMPTY;
    }

    buffer = (uint8_t *)fifo->buffer;

    memcpy(
        data,
        buffer + (fifo->head * fifo->element_size),
        fifo->element_size);

    fifo->head++;

    if (fifo->head >= fifo->capacity)
    {
        fifo->head = 0U;
    }

    fifo->count--;

    return CIRCULAR_FIFO_SUCCESS;
}

bool circular_fifo_is_empty(
    const circular_fifo_t *fifo)
{
    return (fifo == NULL) ? true : (fifo->count == 0U);
}

bool circular_fifo_is_full(
    const circular_fifo_t *fifo)
{
    return (fifo == NULL) ? false : (fifo->count == fifo->capacity);
}

uint32_t circular_fifo_size(
    const circular_fifo_t *fifo)
{
    return (fifo == NULL) ? 0U : fifo->count;
}

uint32_t circular_fifo_capacity(
    const circular_fifo_t *fifo)
{
    return (fifo == NULL) ? 0U : fifo->capacity;
}

void circular_fifo_clear(
    circular_fifo_t *fifo)
{
    if (fifo == NULL)
    {
        return;
    }

    fifo->head = 0U;
    fifo->tail = 0U;
    fifo->count = 0U;
}