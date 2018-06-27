
/*! @file symtable.h */

#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "config.h"

/*! @brief The number of buckets of the symbol table */
#define ST_BUCKETS_MAX_CNT 256

/*! @brief The weight threshold that causes a reash of the symbol table */
#define ST_WEIGHT_THRESHOLD 2.0f

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
	enum symbol_type type;  /*!< @brief The type of the symbol */
	char name[ID_STR_SIZE]; /*!< @brief The name string of the symbol */
	struct symbol *next;    /*!< @brief The next symbol in the bucket */
};

/**
 * @brief The symbol table data structure
 * The symbol table is implemented as an hash table
 */
struct symbol_table
{
	size_t symbols_cnt;      /*!< @brief The number of symbols in the symbol table */
	float weight;            /*!< @brief The ratio between the number of symbols and the number of buckets */
	size_t buckets_cnt;      /*!< @brief The number of buckets of the hash table structure */
	struct symbol **buckets; /*!< @brief The symbol lists buckets */
};

/**
 * @brief Initialize a symbol table
 * @param st A pointer to the symbol table to initialize
 */
void symbol_table_init(struct symbol_table *st);

/**
 * @brief Clear the resources holded by a symbol table
 * @param st A pointer to the symbol table to clean
 */
void symbol_table_clear(struct symbol_table *st);

/**
 * @brief Find a symbol by its name in a symbol table
 * @brief st The symbol table in which to find the symbol
 * @brief name The name of the symbol to find
 * @return A pointer to the symbol if it's found, NULL otherwise
 */
struct symbol *symbol_table_find(struct symbol_table st, const char* name);

/**
 * @brief Add a new symbol in a symbol table
 * @brief st A pointer to the symbol table to modify
 * @brief name The name of the symbol to add
 */
void symbol_table_add(struct symbol_table *st, const char* name);

/**
 * @brief Reash a symbol table
 * @param st_in The input symbol table
 * @param st_out The output symbol table
 */
void symbol_table_reash(struct symbol_table st_in, struct symbol_table *st_out);

