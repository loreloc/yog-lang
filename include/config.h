
/*! @file config.h */

#pragma once

#include <stdio.h>
#include <stdlib.h>

/*! @brief The identifier string size */
#define ID_STR_SIZE 32

/**
 * @brief Custom succeed-or-die memory allocation function
 * @param bytes The number of bytes to allocate
 * @return A pointer to the allocated memory block of size at least 1 byte
 */
void *ymalloc(size_t bytes);

/**
 * @brief Custom memory free function
 * @param ptr A pointer to the memory block to free
 */
void yfree(void *ptr);

