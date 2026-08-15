#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "circular-fifo.h"

/*--------------------------------------------------------------------
 * User Types
 *-------------------------------------------------------------------*/

typedef enum
{
    STATE_IDLE,
    STATE_RUNNING,
    STATE_DONE
} state_t;

typedef struct
{
    uint32_t id;
    uint8_t status;
    float value;
} message_t;

typedef struct
{
    char text[16];
} string_t;

typedef struct
{
    uint32_t values[5];
} array_t;

/*--------------------------------------------------------------------
 * char
 *-------------------------------------------------------------------*/

static void test_char_type(void)
{
    circular_fifo_t fifo;
    char storage[4];

    char tx = 'A';
    char rx = 0;

    circular_fifo_init(&fifo, storage, 4, sizeof(char));

    assert(circular_fifo_push(&fifo, &tx) == CIRCULAR_FIFO_SUCCESS);
    assert(circular_fifo_pop(&fifo, &rx) == CIRCULAR_FIFO_SUCCESS);

    assert(rx == tx);
}

/*--------------------------------------------------------------------
 * uint8_t
 *-------------------------------------------------------------------*/

static void test_uint8_type(void)
{
    circular_fifo_t fifo;
    uint8_t storage[4];

    uint8_t tx = 0xAB;
    uint8_t rx = 0;

    circular_fifo_init(&fifo, storage, 4, sizeof(uint8_t));

    assert(circular_fifo_push(&fifo, &tx) == CIRCULAR_FIFO_SUCCESS);
    assert(circular_fifo_pop(&fifo, &rx) == CIRCULAR_FIFO_SUCCESS);

    assert(rx == tx);
}

/*--------------------------------------------------------------------
 * uint16_t
 *-------------------------------------------------------------------*/

static void test_uint16_type(void)
{
    circular_fifo_t fifo;
    uint16_t storage[4];

    uint16_t tx = 0x1234;
    uint16_t rx = 0;

    circular_fifo_init(&fifo, storage, 4, sizeof(uint16_t));

    assert(circular_fifo_push(&fifo, &tx) == CIRCULAR_FIFO_SUCCESS);
    assert(circular_fifo_pop(&fifo, &rx) == CIRCULAR_FIFO_SUCCESS);

    assert(rx == tx);
}

/*--------------------------------------------------------------------
 * uint32_t
 *-------------------------------------------------------------------*/

static void test_uint32_type(void)
{
    circular_fifo_t fifo;
    uint32_t storage[4];

    uint32_t tx = 0x12345678;
    uint32_t rx = 0;

    circular_fifo_init(&fifo, storage, 4, sizeof(uint32_t));

    assert(circular_fifo_push(&fifo, &tx) == CIRCULAR_FIFO_SUCCESS);
    assert(circular_fifo_pop(&fifo, &rx) == CIRCULAR_FIFO_SUCCESS);

    assert(rx == tx);
}

/*--------------------------------------------------------------------
 * uint64_t
 *-------------------------------------------------------------------*/

static void test_uint64_type(void)
{
    circular_fifo_t fifo;
    uint64_t storage[4];

    uint64_t tx = 0x123456789ABCDEF0ULL;
    uint64_t rx = 0;

    circular_fifo_init(&fifo, storage, 4, sizeof(uint64_t));

    assert(circular_fifo_push(&fifo, &tx) == CIRCULAR_FIFO_SUCCESS);
    assert(circular_fifo_pop(&fifo, &rx) == CIRCULAR_FIFO_SUCCESS);

    assert(rx == tx);
}

/*--------------------------------------------------------------------
 * float
 *-------------------------------------------------------------------*/

static void test_float_type(void)
{
    circular_fifo_t fifo;
    float storage[4];

    float tx = 3.14159f;
    float rx = 0.0f;

    circular_fifo_init(&fifo, storage, 4, sizeof(float));

    assert(circular_fifo_push(&fifo, &tx) == CIRCULAR_FIFO_SUCCESS);
    assert(circular_fifo_pop(&fifo, &rx) == CIRCULAR_FIFO_SUCCESS);

    assert(rx == tx);
}

/*--------------------------------------------------------------------
 * double
 *-------------------------------------------------------------------*/

static void test_double_type(void)
{
    circular_fifo_t fifo;
    double storage[4];

    double tx = 12345.6789;
    double rx = 0.0;

    circular_fifo_init(&fifo, storage, 4, sizeof(double));

    assert(circular_fifo_push(&fifo, &tx) == CIRCULAR_FIFO_SUCCESS);
    assert(circular_fifo_pop(&fifo, &rx) == CIRCULAR_FIFO_SUCCESS);

    assert(rx == tx);
}

/*--------------------------------------------------------------------
 * enum
 *-------------------------------------------------------------------*/

static void test_enum_type(void)
{
    circular_fifo_t fifo;
    state_t storage[4];

    state_t tx = STATE_RUNNING;
    state_t rx = STATE_IDLE;

    circular_fifo_init(&fifo, storage, 4, sizeof(state_t));

    assert(circular_fifo_push(&fifo, &tx) == CIRCULAR_FIFO_SUCCESS);
    assert(circular_fifo_pop(&fifo, &rx) == CIRCULAR_FIFO_SUCCESS);

    assert(rx == tx);
}

/*--------------------------------------------------------------------
 * struct
 *-------------------------------------------------------------------*/

static void test_struct_type(void)
{
    circular_fifo_t fifo;
    message_t storage[4];

    message_t tx =
    {
        .id = 100,
        .status = 2,
        .value = 25.5f
    };

    message_t rx;

    circular_fifo_init(&fifo, storage, 4, sizeof(message_t));

    assert(circular_fifo_push(&fifo, &tx) == CIRCULAR_FIFO_SUCCESS);
    assert(circular_fifo_pop(&fifo, &rx) == CIRCULAR_FIFO_SUCCESS);

    assert(rx.id == tx.id);
    assert(rx.status == tx.status);
    assert(rx.value == tx.value);
}

/*--------------------------------------------------------------------
 * Pointer
 *-------------------------------------------------------------------*/

static void test_pointer_type(void)
{
    circular_fifo_t fifo;
    int *storage[4];

    int value = 999;

    int *tx = &value;
    int *rx = NULL;

    circular_fifo_init(&fifo, storage, 4, sizeof(int *));

    assert(circular_fifo_push(&fifo, &tx) == CIRCULAR_FIFO_SUCCESS);
    assert(circular_fifo_pop(&fifo, &rx) == CIRCULAR_FIFO_SUCCESS);

    assert(rx == &value);
    assert(*rx == value);
}

/*--------------------------------------------------------------------
 * Character Array
 *-------------------------------------------------------------------*/

static void test_char_array_type(void)
{
    circular_fifo_t fifo;
    string_t storage[4];

    string_t tx = {"Circular FIFO"};
    string_t rx;

    circular_fifo_init(&fifo, storage, 4, sizeof(string_t));

    assert(circular_fifo_push(&fifo, &tx) == CIRCULAR_FIFO_SUCCESS);
    assert(circular_fifo_pop(&fifo, &rx) == CIRCULAR_FIFO_SUCCESS);

    assert(strcmp(tx.text, rx.text) == 0);
}

/*--------------------------------------------------------------------
 * Structure containing Array
 *-------------------------------------------------------------------*/

static void test_array_type(void)
{
    circular_fifo_t fifo;
    array_t storage[2];

    array_t tx = {{1,2,3,4,5}};
    array_t rx;

    circular_fifo_init(&fifo, storage, 2, sizeof(array_t));

    assert(circular_fifo_push(&fifo, &tx) == CIRCULAR_FIFO_SUCCESS);
    assert(circular_fifo_pop(&fifo, &rx) == CIRCULAR_FIFO_SUCCESS);

    assert(memcmp(&tx, &rx, sizeof(array_t)) == 0);
}

/*--------------------------------------------------------------------
 * Main
 *-------------------------------------------------------------------*/

// int main(void)
// {
//     printf("Running Circular FIFO Generic Type Tests...\n");

//     test_char_type();
//     test_uint8_type();
//     test_uint16_type();
//     test_uint32_type();
//     test_uint64_type();
//     test_float_type();
//     test_double_type();
//     test_enum_type();
//     test_struct_type();
//     test_pointer_type();
//     test_char_array_type();
//     test_array_type();

//     printf("------------------------------------------\n");
//     printf("All Generic Data Type Tests Passed\n");
//     printf("------------------------------------------\n");

//     return 0;
// }