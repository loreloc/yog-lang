
/*! @file common.h */

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/*! @brief Custom assert macro */
#define yassert(val, msg) yassert_fail(val, msg, __FILE__, __LINE__)

/**
 * @brief Custom assert function
 * @param val The boolean value to check
 * @param msg The error message to print
 * @param file The file where the check failed
 * @param line The line number where the check failed
 */
void yassert_fail(bool val, const char *msg, const char *file, size_t line);

/**
 * @brief Custom succeed-or-die memory allocation function
 * @param size The number of bytes to allocate
 * @return A pointer to the allocated memory block of size at least 1 byte
 */
void *ymalloc(size_t size);

/**
 * @brief Custom succeed-or-die zero initialized memory allocation function
 * @param num The number of blocks to allocate
 * @param size The size of each block
 * @return A pointer to the allocated memory block of size at least 1 byte
 */
void *ycalloc(size_t num, size_t size);

/**
 * @brief Custom succeed-or-die memory reallocation function
 * @param ptr A pointer to the memory block to reallocate
 * @param size The number of bytes to reallocate
 * @return A pointer to the reallocated memory block of size at least 1 byte
 */
void *yrealloc(void *ptr, size_t size);

/**
 * @brief Custom memory free function
 * @param ptr A pointer to the memory block to free
 */
void yfree(void *ptr);

/**
 * @brief Custom succeed-or-die string duplicate function
 * @param str A pointer to the string to duplicate
 * @return A pointer to a new string
 */
char *ystrdup(char *str);

