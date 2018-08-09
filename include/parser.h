
/*! @file parser.h */

#pragma once

#include "scanner.h"
#include "ast.h"

/*! @brief The parse context data structure */
struct parse_context
{
	/*! @brief The lexical context */
	struct lex_context lex_ctx;

	/*! @brief A pointer to the error list */
	struct error_list *errs;

	/*! @brief The current extracted token */
	struct token tok;
};

/**
 * @brief Initialize a parse context
 * @param ctx A pointer to the parse context to initialize
 * @param source A pointer to the source file
 * @param st A pointer to the symbol table
 * @param errs A pointer to the error list
 */
void parse_context_init(struct parse_context *ctx, FILE *source, struct symbol_table *st, struct error_list *errs);

/**
 * @brief Parse the source code
 * 
 * LL(1) parser for the following grammar (EBNF syntax)
 * ```
 * source = "var" variables "begin" statements "end"
 * 
 * variables = { "identifier" ":" "int" ";" }
 * 
 * statements = { [ assign | input | output | branch | loop ] }
 * 
 * assign = "identifier" "=" expression ";"
 * 
 * input = "read" "identifier" ";"
 * 
 * output = "write" expression ";"
 * 
 * branch = "if" "(" condition ")" "begin" statements "else" statements "end"
 * 
 * loop = "while" "(" condition ")" "begin" statements "end"
 * 
 * expression = term { ( "+" | "-" ) term }
 * 
 * term = factor { ( "*" | "/" ) factor }
 * 
 * factor =   "literal"
 *          | "identifier"
 *          | ( "+" | "-" ) factor
 *          | "(" <expression> ")"
 *
 * condition = expression ( "=" | "<>" | "<" | "<=" | ">" | ">=" ) expression
 *
 * ```
 * @param ctx A pointer to the parse context
 * @return The abstract syntax tree of the source code
 */
struct ast *parse(struct parse_context *ctx);

