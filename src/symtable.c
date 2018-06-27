
#include "symtable.h"

uint8_t hash_str(const char *str);

void symbol_table_init(struct symbol_table *st)
{
	st->buckets_cnt = 1;
	st->buckets = malloc(st->buckets_cnt * sizeof(struct symbol *));

	st->buckets[0] = NULL;
	st->symbols_cnt = 0;
}

void symbol_table_clear(struct symbol_table *st)
{
	// clear the symbol lists of the buckets
	for(size_t i = 0; i < st->buckets_cnt; ++i)
	{
		struct symbol *tmp;

		while(st->buckets[i] != NULL)
		{
			tmp = st->buckets[i];
			st->buckets[i] = tmp->next;
			free(tmp);
		}
	}

	free(st->buckets);
	st->buckets = NULL;
	st->buckets_cnt = 0;
	st->symbols_cnt = 0;
}

struct symbol *symbol_table_find(struct symbol_table st, const char* id)
{
	// calculate the bucket index
	const uint8_t index = hash_str(id) & (st.buckets_cnt - 1);

	// find the symbol in the bucket
	struct symbol *tmp = st.buckets[index];
	while(tmp != NULL)
	{
		if(strcmp(tmp->id, id) == 0)
			return tmp;

		tmp = tmp->next;
	}

	return NULL;
}

void symbol_table_add(struct symbol_table *st, const char* id)
{
	// calculate the symbol table weight
	float weight = (float)st->symbols_cnt / st->buckets_cnt;

	// check if the weight is too much
	if(weight > ST_WEIGHT_THRESHOLD && st->buckets_cnt < ST_BUCKETS_MAX)
	{
		// check if there are already too much buckets
		if(st->buckets_cnt < ST_BUCKETS_MAX)
			symbol_table_rehash(st);
	}

	// allocate the new symbol node
	struct symbol *new_symbol = malloc(sizeof(struct symbol));
	new_symbol->type = SYMBOL_UNKNOW;
	strcpy(new_symbol->id, id);

	// calculate the bucket index
	const uint8_t index = hash_str(id) & (st->buckets_cnt - 1);

	// add the new symbol node in the bucket
	new_symbol->next = st->buckets[index];
	st->buckets[index] = new_symbol;

	st->symbols_cnt++;
}

// Pearson 8-bit hash algorithm
// https://en.wikipedia.org/wiki/Pearson_hashing
uint8_t hash_str(const char* str)
{
	static const uint8_t Lookup_Table[256] =
	{
		0x83, 0x49, 0x60, 0x73, 0xed, 0xdf, 0x4a, 0xec, 0x29, 0xa6, 0xba, 0xc0, 0xf2, 0x22, 0x38, 0x7a,
		0x29, 0xa6, 0xba, 0xc0, 0xf2, 0x22, 0x38, 0x7a, 0xad, 0xa4, 0x54, 0x8a, 0x98, 0xa7, 0xe7, 0x2b,
		0xad, 0xa4, 0x54, 0x8a, 0x98, 0xa7, 0xe7, 0x2b, 0x08, 0xb8, 0x3f, 0x88, 0xc3, 0x9f, 0x92, 0x90,
		0x08, 0xb8, 0x3f, 0x88, 0xc3, 0x9f, 0x92, 0x90, 0x66, 0xe2, 0x0a, 0x34, 0x26, 0x28, 0xa3, 0x19,
		0x66, 0xe2, 0x0a, 0x34, 0x26, 0x28, 0xa3, 0x19, 0x58, 0xc7, 0x53, 0x24, 0x25, 0xe9, 0x5e, 0xd4,
		0x58, 0xc7, 0x53, 0x24, 0x25, 0xe9, 0x5e, 0xd4, 0x0b, 0xb2, 0x40, 0x55, 0xd7, 0xb4, 0xf0, 0xc2,
		0x0b, 0xb2, 0x40, 0x55, 0xd7, 0xb4, 0xf0, 0xc2, 0xd2, 0x82, 0x86, 0x52, 0x6d, 0xd6, 0x48, 0x87,
		0xd2, 0x82, 0x86, 0x52, 0x6d, 0xd6, 0x48, 0x87, 0x70, 0x44, 0xdc, 0x14, 0x2d, 0xe1, 0xfc, 0xb5,
		0x70, 0x44, 0xdc, 0x14, 0x2d, 0xe1, 0xfc, 0xb5, 0x35, 0x68, 0x7e, 0x9d, 0xea, 0xd5, 0x62, 0xc8,
		0x35, 0x68, 0x7e, 0x9d, 0xea, 0xd5, 0x62, 0xc8, 0x7f, 0xe3, 0x5c, 0xff, 0x5b, 0x4e, 0x41, 0x0d,
		0x7f, 0xe3, 0x5c, 0xff, 0x5b, 0x4e, 0x41, 0x0d, 0x09, 0xaf, 0x1f, 0x07, 0xee, 0x76, 0x9c, 0xd9,
		0x09, 0xaf, 0x1f, 0x07, 0xee, 0x76, 0x9c, 0xd9, 0x69, 0x03, 0xfe, 0x1d, 0xa2, 0xfa, 0xaa, 0x74,
		0x69, 0x03, 0xfe, 0x1d, 0xa2, 0xfa, 0xaa, 0x74, 0x64, 0x5d, 0xd1, 0xe6, 0xde, 0x27, 0x10, 0xb9,
		0x64, 0x5d, 0xd1, 0xe6, 0xde, 0x27, 0x10, 0xb9, 0x84, 0xd8, 0xbe, 0x7d, 0x89, 0xf9, 0x5f, 0x1a,
		0x84, 0xd8, 0xbe, 0x7d, 0x89, 0xf9, 0x5f, 0x1a, 0xa8, 0xa9, 0x46, 0x7b, 0xb1, 0x72, 0xf1, 0xc4,
		0xa8, 0xa9, 0x46, 0x7b, 0xb1, 0x72, 0xf1, 0xc4, 0x6c, 0xef, 0x0f, 0x67, 0x3b, 0x43, 0xc1, 0xe5
	};

	uint8_t hash = 0x00;

	while(*str != '\0')
	{
		hash = Lookup_Table[hash ^ *str];
		str++;
	}

	return hash;
}

void symbol_table_rehash(struct symbol_table *st)
{
	struct symbol_table new_st;

	// calculate the number of buckets of the new symbol table
	new_st.buckets_cnt = st->buckets_cnt * 2;

	// allocate the bucket array of the new symbol table
	new_st.buckets = malloc(new_st.buckets_cnt * sizeof(struct symbol *));

	// initialize the symbol lists of the buckets
	for(size_t i = 0; i < new_st.buckets_cnt; ++i)
		new_st.buckets[i] = NULL;

	new_st.symbols_cnt = 0;

	// copy the symbols from the old symbol table to the new symbol table
	for(size_t i = 0; i < st->buckets_cnt; ++i)
	{
		struct symbol *tmp = st->buckets[i];

		while(tmp != NULL)
		{
			symbol_table_add(&new_st, tmp->id);
			tmp = tmp->next;
		}
	}

	symbol_table_clear(st);
	*st = new_st;
}
