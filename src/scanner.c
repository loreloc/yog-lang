
#include "scanner.h"

#define TEXT_SIZE ID_STR_SIZE

// the states of the finite state automata
enum fsa_state
{
	FSA_START,
	FSA_LITERAL,
	FSA_WORD,
	FSA_OPERATOR,
	FSA_COLON,
	FSA_SEMICOLON,
	FSA_ERROR,
	FSA_ACCEPT
};

// the classes of the characters in the source code
enum char_class
{
	CHAR_WHITESPACE,
	CHAR_DIGIT,
	CHAR_ALPHABETICAL,
	CHAR_OPERATOR,
	CHAR_COLON,
	CHAR_SEMICOLON,
	CHAR_UNKNOW
};

void update_cursor(struct location *loc, char c);
enum char_class identify_char(char c);
enum fsa_state next_state(enum fsa_state state, enum char_class class);
struct token make_token_word(char *text, struct symbol_table *st, struct location loc);
struct token make_token_operator(char *text, struct location loc);
bool lex_step(struct token *tok, struct lex_context *ctx, struct symbol_table *st, struct error_handler *err_hnd);

void lex_context_init(struct lex_context *ctx, FILE *source)
{
	ctx->source = source;
	ctx->lookahead = '\0';
	ctx->loc.row = 1;
	ctx->loc.col = 1;
}

bool lex(struct token *tok, struct lex_context *ctx, struct symbol_table *st, struct error_handler *err_hnd)
{
	bool result = false;

	// execute a lexical step until a valid token is found or source EOF is reached
	while(!result && !feof(ctx->source))
		result = lex_step(tok, ctx, st, err_hnd);

	return result;
}

void update_cursor(struct location *loc, char c)
{
	if(c == '\n')
	{
		loc->row++;
		loc->col = 1;
	}
	else
	{
		loc->col++;
	}
}

enum char_class identify_char(char c)
{
	// identify the character class of a character
	if(c == ' ' || c == '\t' || c == '\n' || c == '\r')
		return CHAR_WHITESPACE;
	else if((c >= 'A' && c <= 'z') || (c >= 'a' && c <= 'z'))
		return CHAR_ALPHABETICAL;
	else if(c >= '0' && c <= '9')
		return CHAR_DIGIT;
	else if(c == '+' || c == '-' || c == '*' || c == '/' || c == '=')
		return CHAR_OPERATOR;
	else if(c == ':')
		return CHAR_COLON;
	else if(c == ';')
		return CHAR_SEMICOLON;
	else
		return CHAR_UNKNOW;
}

enum fsa_state next_state(enum fsa_state state, enum char_class class)
{
	static const enum fsa_state Transition_Table[7][7] =
	{
		             /*   WHITESPACE  DIGIT        ALPHABETICAL OPERATOR      COLON       SEMICOLON      UNKNOW    */
		/* START     */ { FSA_START,  FSA_LITERAL, FSA_WORD,    FSA_OPERATOR, FSA_COLON,  FSA_SEMICOLON, FSA_ERROR  },
		/* LITERAL   */ { FSA_ACCEPT, FSA_LITERAL, FSA_ACCEPT,  FSA_ACCEPT,   FSA_ACCEPT, FSA_ACCEPT,    FSA_ACCEPT },
		/* WORD      */ { FSA_ACCEPT, FSA_WORD,    FSA_WORD,    FSA_ACCEPT,   FSA_ACCEPT, FSA_ACCEPT,    FSA_ACCEPT },
		/* OPERATOR  */ { FSA_ACCEPT, FSA_ACCEPT,  FSA_ACCEPT,  FSA_ACCEPT,   FSA_ACCEPT, FSA_ACCEPT,    FSA_ACCEPT },
		/* COLON     */ { FSA_ACCEPT, FSA_ACCEPT,  FSA_ACCEPT,  FSA_ACCEPT,   FSA_ACCEPT, FSA_ACCEPT,    FSA_ACCEPT },
		/* SEMICOLON */ { FSA_ACCEPT, FSA_ACCEPT,  FSA_ACCEPT,  FSA_ACCEPT,   FSA_ACCEPT, FSA_ACCEPT,    FSA_ACCEPT },
		/* ERROR     */ { FSA_ACCEPT, FSA_ACCEPT,  FSA_ACCEPT,  FSA_ACCEPT,   FSA_ACCEPT, FSA_ACCEPT,    FSA_ERROR  }
	};

	return Transition_Table[(size_t)state][(size_t)class];
}

struct token make_token_word(char *text, struct symbol_table *st, struct location loc)
{
	struct token tok;

	if(strcmp(text, "var") == 0)
		tok.type = TOKEN_VAR;
	else if(strcmp(text, "begin") == 0)
		tok.type = TOKEN_BEGIN;
	else if(strcmp(text, "end") == 0)
		tok.type = TOKEN_END;
	else if(strcmp(text, "int") == 0)
		tok.type = TOKEN_INT;
	else if(strcmp(text, "read") == 0)
		tok.type = TOKEN_READ;
	else if(strcmp(text, "write") == 0)
		tok.type = TOKEN_WRITE;
	else
	{
		tok.type = TOKEN_IDENTIFIER;
		
		// find the identifier in the symbol table
		struct symbol *sym = symbol_table_find(*st, text);

		// if the symbol isn't found add it to the symbol table
		if(!sym)
			sym = symbol_table_add(st, text);

		tok.sym = sym;
	}

	// initialize the token location
	tok.loc = loc;

	return tok;
}

struct token make_token_operator(char *text, struct location loc)
{
	struct token tok;

	switch(text[0])
	{
		case '+':
			tok.type = TOKEN_PLUS;
			break;
		case '-':
			tok.type = TOKEN_MINUS;
			break;
		case '*':
			tok.type = TOKEN_MUL;
			break;
		case '/':
			tok.type = TOKEN_DIV;
			break;
		default: // case '=':
			tok.type = TOKEN_EQUAL;
			break;
	}

	// initialize the token location
	tok.loc = loc;

	return tok;
}

bool lex_step(struct token *tok, struct lex_context *ctx, struct symbol_table *st, struct error_handler *err_hnd)
{
	char text[TEXT_SIZE] = { '\0' };
	size_t text_len = 0;
	struct location text_loc;

	enum fsa_state state = FSA_START;
	char character;

	// ignore whitespaces
	while(state == FSA_START)
	{
		// get the next character
		if(ctx->lookahead == '\0')
		{
			character = fgetc(ctx->source);
		}
		else
		{
			character = ctx->lookahead;
			ctx->lookahead = '\0';
		}

		if(feof(ctx->source))
			return false;

		state = next_state(state, identify_char(character));

		update_cursor(&ctx->loc, character);
	}

	// initialize the result token location
	text_loc.row = ctx->loc.row;
	text_loc.col = ctx->loc.col - 1;

	// add the read character to the text
	text[text_len++] = character;

	while(true)
	{
		character = fgetc(ctx->source);

		if(feof(ctx->source))
			break;

		enum fsa_state new_state = next_state(state, identify_char(character));

		// check if the new state is final
		if(new_state == FSA_ACCEPT)
		{
			ctx->lookahead = character;
			break;
		}

		state = new_state;

		if(text_len < TEXT_SIZE)
			text[text_len++] = character;

		update_cursor(&ctx->loc, character);
	}

	// check if the text string is too long
	if(text_len == TEXT_SIZE)
	{
		error_handler_add(err_hnd, text_loc, ERROR_LEXICAL, "TEXT OVERFLOW");
		return false;
	}

	// construct the result token
	switch(state)
	{
		case FSA_LITERAL:
			tok->type = TOKEN_LITERAL;
			tok->lit  = atoi(text);
			tok->loc = text_loc;
			break;
		case FSA_WORD:
			*tok = make_token_word(text, st, text_loc);
			break;
		case FSA_OPERATOR:
			*tok = make_token_operator(text, text_loc);
			break;
		case FSA_COLON:
			tok->type = TOKEN_COLON;
			tok->loc = text_loc;
			break;
		case FSA_SEMICOLON:
			tok->type = TOKEN_SEMICOLON;
			tok->loc = text_loc;
			break;
		default:
			error_handler_add(err_hnd, text_loc, ERROR_LEXICAL, text);
			return false;
	}

	return true;
}

