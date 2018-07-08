
/*! @file location.h */

#pragma once

#include <stddef.h>
#include <stdint.h>

/*! @brief The location specifies a position in the source code */
struct location
{
	/*! @brief The row index in the source code */
	size_t row;

	/*! @brief The column index in the source code */
	size_t col;
};

