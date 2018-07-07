
/*! @file parser.h */

#pragma once

#include "scanner.h"
#include "instructions.h"

/**
 * @brief The parse context
 */
struct parse_context
{
	struct lex_context lex_ctx; /*!< @brief The lexical context */
	struct symbol_table *st;    /*!< @brief A pointer to the symbol table */
	struct error_list *errs;    /*!< @brief A pointer to the error list */
	struct instr_list instrs;   /*!< @brief The instructions list */
	struct token tok;           /*!< @brief The current extracted token */
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
 * <expression> ::= <term> | <term> "+" <term> | <term> "-" <term>
 * 
 * <term> ::= <factor> | <factor> "*" <factor> | <factor> "/" <factor>
 * 
 * <factor> ::= "literal" | "identifier"
 * ```
 * @param ctx A pointer to the parse context
 */
void parse(struct parse_context *ctx);
