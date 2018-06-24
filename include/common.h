
/*! @file common.h */

#include <stdlib.h>
#include <string.h>

/**
 * @brief Duplicate a string (alternative to 'strdup')
 * @param str The string to duplicate
 * @return NULL if an error occured, the new string otherwise
 */
char *string_duplicate(const char *str);

