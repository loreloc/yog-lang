
/*! @file symtable.h */

#pragma once

#include "defines.h"
#include "common.h"
#include "location.h"


/*! @brief The types of a symbol node */
enum symbol_type
{
	SYMBOL_UNKNOW,
	SYMBOL_INTEGER
};

/*! @brief The symbol node */
struct symbol
{
	/*! @brief The type of the symbol node */
	enum symbol_type type;

	/*! @brief The location in the source code where the symbol is defined */
	struct location loc;

	/*! @brief The identifier string of the symbol */
	char id[ID_STR_SIZE];

	/*! @brief The integral value of the symbol */
	int64_t value;

	/*! @brief The next symbol in the symbol table bucket */
	struct symbol *next;
};

/*! @brief The symbol table data structure (hash table) */
struct symbol_table
{
	/*! @brief The number of buckets of the hash table */
	size_t buckets_cnt;

	/*! @brief The buckets of symbol lists */
	struct symbol **buckets;

	/*! @brief The number of symbols in the symbol table */
	size_t symbols_cnt;
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
 * @return A pointer to the symbol if it is found, NULL otherwise
 */
struct symbol *symbol_table_find(struct symbol_table st, const char* id);

/**
 * @brief Add a new symbol in a symbol table
 * @brief st A pointer to the symbol table to modify
 * @brief id The identifier of the symbol to add
 * @return A pointer to the new symbol added
 */
struct symbol *symbol_table_add(struct symbol_table *st, const char* name);

