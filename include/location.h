
/*! @file location.h */

#pragma once

#include <stddef.h>
#include <stdint.h>

/**
 * @brief A location specify a position in the source code
 */
struct location
{
	size_t row; /*!< @brief The row index in the source code    */
	size_t col; /*!< @brief The column index in the source code */
};

