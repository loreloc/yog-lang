
/*! @file symtable.h */

#pragma once

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "config.h"

/**
 * @brief The possible types of a symbol
 */
enum symbol_type
{
	SYMBOL_UNKNOW,
	SYMBOL_INTEGER
};

/**
 * @brief The symbol data structure
 */
struct symbol
{
	enum symbol_type type; /*!< @brief The type of the symbol */
	char id[ID_STR_SIZE];  /*!< @brief The identifier of the symbol */
	struct symbol *next;   /*!< @brief The next symbol in the bucket */
};

/**
 * @brief The symbol table data structure
 * The symbol table is implemented as an hash table
 */
struct symbol_table
{
	size_t buckets_cnt;      /*!< @brief The number of buckets of the hash table structure */
	struct symbol **buckets; /*!< @brief The symbol lists buckets */
	size_t symbols_cnt;      /*!< @brief The number of symbols in the symbol table */
};

/**
 * @brief Initialize a symbol table
 * @param st A pointer to the symbol table to initialize
 */
void symbol_table_init(struct symbol_table *st);

/**
 * @brief Clear the resources holded by a symbol table
 * @param st A pointer to the symbol table to clear
 */
void symbol_table_clear(struct symbol_table *st);

/**
 * @brief Print a symbol table
 * @param st The symbol table to print
 */
void symbol_table_show(struct symbol_table st);

/**
 * @brief Find a symbol by its name in a symbol table
 * @brief st The symbol table in which to find the symbol
 * @brief id The identifier of the symbol to find
 * @return A pointer to the symbol if it's found, NULL otherwise
 */
struct symbol *symbol_table_find(struct symbol_table st, const char* id);

/**
 * @brief Add a new symbol in a symbol table
 * @brief st A pointer to the symbol table to modify
 * @brief id The identifier of the symbol to add
 * @return A pointer to the new symbol
 */
struct symbol *symbol_table_add(struct symbol_table *st, const char* name);

