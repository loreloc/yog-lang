
/*! @file parser.h */

#pragma once

#include "scanner.h"
#include "ast.h"

/*! @brief The parse context data structure */
struct parse_context
{
	/*! @brief The lexical context */
	struct lex_context lex_ctx;

	/*! @brief A pointer to the symbol table */
	struct symbol_table *st;

	/*! @brief A pointer to the error list */
	struct error_list *errs;

	/*! @brief The previous extracted token */
	struct token prev_tok;

	/*! @brief The current extracted token */
	struct token curr_tok;
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
 * @see https://en.wikipedia.org/wiki/Recursive_descent_parser
 * 
 * Descendent recursive LL(1) parser for the following grammar
 * ```
 * <source> ::= "var" <variables> "begin" <statements> "end"
 * 
 * <variables> ::= "identifier" ":" "int" ";" <variables> | ""
 * 
 * <statements> ::=   <assignment> ";" <statements> | ""
 *                  | <input>      ";" <statements> | ""
 *                  | <output>     ";" <statements> | ""
 * 
 * <assignment> ::= "identifier" "=" <expression>
 * 
 * <input> ::= "read" "identifier"
 * 
 * <output> ::= "write" <expression>
 * 
 * <expression> ::= <term> | <term> "+" <expression> | <term> "-" <expression>
 * 
 * <term> ::= <factor> | <factor> "*" <term> | <factor> "/" <term>
 * 
 * <factor> ::=   "literal" | "identifier"
 *              | "+" <expression> | "-" <expression>
 *              | "(" <expression> ")"
 * ```
 * @param ctx A pointer to the parse context
 * @return The abstract syntax tree of the source code
 */
struct ast *parse(struct parse_context *ctx);

