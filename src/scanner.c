
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

void lex_context_init(struct lex_context *ctx, FILE *source)
{
	ctx->source = source;
	ctx->lookahead = '\0';
	ctx->loc.row = 1;
	ctx->loc.col = 1;
}

bool lex(struct token *tok, struct lex_context *ctx, struct symbol_table *st, struct error_handler *err_hnd)
{
	char text[TEXT_SIZE] = { '\0' };
	size_t text_len = 0;
	struct location text_loc;
	enum fsa_state state = FSA_START;

	while(true)
	{
		char character;

		// get the next character
		if(ctx->lookahead =='\0')
		{
			character = fgetc(ctx->source);
		}
		else
		{
			character = ctx->lookahead;
			ctx->lookahead = '\0';
		}

		// check if source EOF has been reached
		if(feof(ctx->source))
			break;

		// get the next automata state
		enum fsa_state new_state = next_state(state, identify_char(character));

		// check if the new state is final
		if(new_state == FSA_ACCEPT)
		{
			// check for text buffer overflow
			if(text_len == TEXT_SIZE)
			{
				state = FSA_ERROR;
				new_state = FSA_START;
			}
			else
			{
				ctx->lookahead = character;
				break;
			}
		}

		// handle different state changes
		if(state == FSA_START)
		{
			if(new_state != FSA_START)
				text_loc = ctx->loc;
		}
		else if(state == FSA_ERROR)
		{
			if(new_state != FSA_ERROR)
			{
				const char *err_msg = (text_len < TEXT_SIZE) ? text : "TOKEN TOO LONG";
				error_handler_add(err_hnd, text_loc, ERROR_LEXICAL, err_msg);
				memset(text, '\0', TEXT_SIZE);
				text_len = 0;
			}
		}

		state = new_state;

		// add the read character into the text buffer
		if(state != FSA_START && text_len < TEXT_SIZE)
			text[text_len++] = character;

		// update the lexical context cursor location
		update_cursor(&ctx->loc, character);
	}

	// check for text buffer overflow
	if(text_len == TEXT_SIZE)
	{
		error_handler_add(err_hnd, text_loc, ERROR_LEXICAL, "TOKEN TOO LONG");
		return false;
	}

	// construct the result token
	switch(state)
	{
		case FSA_START:
			return false;
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
		default: // case FSA_ERROR:
			error_handler_add(err_hnd, text_loc, ERROR_LEXICAL, text);
			return false;
	}

	return true;
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
	else if((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
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
		/* LITERAL   */ { FSA_ACCEPT, FSA_LITERAL, FSA_ERROR,   FSA_ACCEPT,   FSA_ACCEPT, FSA_ACCEPT,    FSA_ACCEPT },
		/* WORD      */ { FSA_ACCEPT, FSA_WORD,    FSA_WORD,    FSA_ACCEPT,   FSA_ACCEPT, FSA_ACCEPT,    FSA_ACCEPT },
		/* OPERATOR  */ { FSA_ACCEPT, FSA_ACCEPT,  FSA_ACCEPT,  FSA_ACCEPT,   FSA_ACCEPT, FSA_ACCEPT,    FSA_ACCEPT },
		/* COLON     */ { FSA_ACCEPT, FSA_ACCEPT,  FSA_ACCEPT,  FSA_ACCEPT,   FSA_ACCEPT, FSA_ACCEPT,    FSA_ACCEPT },
		/* SEMICOLON */ { FSA_ACCEPT, FSA_ACCEPT,  FSA_ACCEPT,  FSA_ACCEPT,   FSA_ACCEPT, FSA_ACCEPT,    FSA_ACCEPT },
		/* ERROR     */ { FSA_START,  FSA_ERROR,   FSA_ERROR,   FSA_ERROR,    FSA_ERROR,  FSA_ERROR,     FSA_ERROR  }
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

